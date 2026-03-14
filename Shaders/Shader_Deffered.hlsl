#include "Struct_Defines.hlsl"
#include "Light_Defines.hlsl"

float GetRimMask(float2 vUV)
{
    uint packed = LoadObjectInfo(vUV, OutlineParam.vInvSize);
    uint flags = UnpackFlags8(packed);
    return HasRim(flags) ? 1.0f : 0.0f;
}

float ComputeToonDiffuse(float3 vNormal, float3 vLightDir)
{
    // 기본 Lambert의 NdotL을 구함
    float NdotL = saturate(dot(vNormal, vLightDir));
    
    // 곡면에서 끊기지않게 Wrap Lighting을 적용
    float fWrapped = saturate((NdotL + toonParam.fWrap) / (1.0f + toonParam.fWrap));

    // 밝은 면과 그림자 면의 경계를 부드럽게 만듦
    float fToonLight = smoothstep(
        toonParam.fShadowMid - toonParam.fShadowSoftness,
        toonParam.fShadowMid + toonParam.fShadowSoftness, fWrapped);
    
    // 그림자 최소 밝기와 밝은면 1.0 사이를 보간
    return lerp(toonParam.fShadowStrength, 1.0f, fToonLight);
}

float ComputeToonRim(float3 vNormal, float3 vViewDir, float3 vLightDir)
{
    // 카메라 기준 외곽 성분을 구함
    float fRimBase = 1.0f - saturate(dot(vNormal, vViewDir));
    
    // threshold와 softness로 얇은 rim 밴드 형성
    float fRim = smoothstep(
        toonParam.fRimThreshold - toonParam.fRimSoftness,
        toonParam.fRimThreshold + toonParam.fRimSoftness, fRimBase);
    
    // Rim이 밝은 면 전체에 퍼지지 않게 어두운 쪽에서만 조금 더 살림
    float fShadowSide = 1.0f - saturate(dot(vNormal, vLightDir));
    float fShadowMask = smoothstep(0.25f, 0.85f, fShadowSide);
    
    return fRim * fShadowMask * toonParam.fRimStrength;
}

float Luma(float3 vC)
{
    return dot(vC, float3(0.216, 0.7152, 0.0722));
}

// 16^3 LUT (256 x 16)
float3 ApplyLUT_16(float3 vC)
{
    const float fSize = 16.0f;
    const float fInvSize = 1.0f / fSize;
    
    vC = saturate(vC);
    
    // Blue slice selection
    float fB = vC.b * (fSize - 1.0f);
    float fSlice0 = floor(fB);
    float fSlice1 = min(fSlice0 + 1.0f, fSize - 1.0f);
    float fT = frac(fB);
    
    float2 vRG = (vC.rg * (fSize - 1.0f) + 0.5f) * fInvSize;

    float2 vUV0 = float2((fSlice0 + vRG.x) * fInvSize, vRG.y);
    float2 vUV1 = float2((fSlice1 + vRG.x) * fInvSize, vRG.y);
    
    float3 vColor0 = g_LUT_Stand.SampleLevel(LinearClampSampler, vUV0, 0).rgb;
    float3 vColor1 = g_LUT_Stand.SampleLevel(LinearClampSampler, vUV1, 0).rgb;
    
    return lerp(vColor0, vColor1, fT);
}

float3 Fresnel_Schlick(float cosTheta, float3 F0)
{
    return F0 + (1.0f - F0) * pow(1.0f - cosTheta, 5.0f);
}

float D_GGX(float fNDotH, float fAlpha)
{
    float fA2 = fAlpha * fAlpha;
    float fDenom = (fNDotH * fNDotH) * (fA2 - 1.0f) + 1.0f;
    return fA2 / max(PI * fDenom * fDenom, EPSILON);
}

float G_SchlickGGX(float fNdotX, float fK)
{
    return fNdotX / max(fNdotX * (1.0f - fK) + fK, EPSILON);
}

float G_Smith(float fNdotV, float fNdotL, float fAlpha)
{
    float fK = fAlpha + 1.0f;
    fK = (fK * fK) / 8.0f;
    return G_SchlickGGX(fNdotV, fK) * G_SchlickGGX(fNdotL, fK);
}

// Soft Threshold
float3 BloomPrefilter(float3 vColor, float fThreshold, float fKnee)
{
    float fBrightness = max(max(vColor.r, vColor.g), vColor.b);
    float fSoft = fBrightness - fThreshold + fKnee;
    fSoft = clamp(fSoft, 0.0, 2.0 * fKnee);
    fSoft = (fSoft * fSoft) / max(4.0 * fKnee, EPSILON);
    
    float fHard = max(fBrightness - fThreshold, 0.0);
    float fContribution = max(fHard, fSoft);
    return vColor * (fContribution / max(fBrightness, EPSILON));
}

float3 Downsample2x2_SceneHDR(float2 vUV, float2 vHalfInvSize)
{
    //float2 vO = vHalfInvSize;
    //float3 c0 = g_RenderTargetSceneHDRTexture.Sample(LinearSampler, vUV + float2(-vO.x, -vO.y)).rgb;
    //float3 c1 = g_RenderTargetSceneHDRTexture.Sample(LinearSampler, vUV + float2(vO.x, -vO.y)).rgb;
    //float3 c2 = g_RenderTargetSceneHDRTexture.Sample(LinearSampler, vUV + float2(-vO.x, vO.y)).rgb;
    //float3 c3 = g_RenderTargetSceneHDRTexture.Sample(LinearSampler, vUV + float2(vO.x, vO.y)).rgb;
    //return (c0 + c1 + c2 + c3) * 0.25;
    
    return g_RenderTargetSceneHDRTexture.Sample(LinearClampSampler, vUV).rgb;
}

float3 Blur9(Texture2D vTexture, float2 vUV, float2 vDir, float2 vInvSize)
{
    float2 stepUV = vDir * vInvSize;
    float w0 = 0.227027;
    float w1 = 0.1945946;
    float w2 = 0.1216216;
    float w3 = 0.054054;
    float w4 = 0.016216;

    // 모든 Sample 호출에 LinearClampSampler를 명시적으로 사용
    float3 c = vTexture.Sample(LinearClampSampler, vUV).rgb * w0;

    c += vTexture.Sample(LinearClampSampler, vUV + stepUV * 1).rgb * w1;
    c += vTexture.Sample(LinearClampSampler, vUV - stepUV * 1).rgb * w1;
    
    c += vTexture.Sample(LinearClampSampler, vUV + stepUV * 2).rgb * w2;
    c += vTexture.Sample(LinearClampSampler, vUV - stepUV * 2).rgb * w2;
    
    c += vTexture.Sample(LinearClampSampler, vUV + stepUV * 3).rgb * w3;
    c += vTexture.Sample(LinearClampSampler, vUV - stepUV * 3).rgb * w3;
    
    c += vTexture.Sample(LinearClampSampler, vUV + stepUV * 4).rgb * w4;
    c += vTexture.Sample(LinearClampSampler, vUV - stepUV * 4).rgb * w4;
    return c;
}

float3 ToneMap_ACES(float3 x)
{
    const float a = 2.51f;
    const float b = 0.03f;
    const float c = 2.43f;
    const float d = 0.59f;
    const float e = 0.14f;
    return saturate((x * (a * x + b)) / (x * (c * x + d) + e));
}

float SilhouetteEdge(float2 vUV)
{
    float2 vInvSize = OutlineParam.vInvSize;
    
    uint iPacked = LoadObjectInfo(vUV, vInvSize);
    uint iFlags8 = UnpackFlags8(iPacked);
    
    if (HasOutline(iFlags8) == false)
        return 0.0f;

    uint iID_Center = UnpackID24(iPacked);
    
    int iStep = max(1, (int) round(OutlineParam.fThicknessPx));
    float2 vO = vInvSize * iStep;

    uint pL = LoadObjectInfo(vUV + float2(-vO.x, 0), vInvSize);
    uint pR = LoadObjectInfo(vUV + float2(vO.x, 0), vInvSize);
    uint pU = LoadObjectInfo(vUV + float2(0, -vO.y), vInvSize);
    uint pD = LoadObjectInfo(vUV + float2(0, vO.y), vInvSize);
    
    uint fL = UnpackFlags8(pL); uint idL = UnpackID24(pL);
    uint fR = UnpackFlags8(pR); uint idR = UnpackID24(pR);
    uint fU = UnpackFlags8(pU); uint idU = UnpackID24(pU);
    uint fD = UnpackFlags8(pD); uint idD = UnpackID24(pD);

    bool bEdgeL = ((HasOutline(fL) == false) || (HasOutline(fL) && (idL != iID_Center)));
    bool bEdgeR = ((HasOutline(fR) == false) || (HasOutline(fR) && (idR != iID_Center)));
    bool bEdgeU = ((HasOutline(fU) == false) || (HasOutline(fU) && (idU != iID_Center)));
    bool bEdgeD = ((HasOutline(fD) == false) || (HasOutline(fD) && (idD != iID_Center)));
    
    bool bSil = (bEdgeL || bEdgeR || bEdgeU || bEdgeD);
    return bSil ? 1.0f : 0.0f;
}

float ComputeFade(float fViewZ)
{
    if (OutlineParam.fFadeEnd <= OutlineParam.fFadeStart + EPSILON)
        return 1.0;
    
    float fZ = abs(fViewZ);
    return saturate((OutlineParam.fFadeEnd - fZ) / max(OutlineParam.fFadeEnd - OutlineParam.fFadeStart, EPSILON));
}

float EdgeFromDepth(float fCenterViewZ, float fNeighViewZ)
{
    // 상대값으로 안정화(거리 커질수록 dz가 커지는 현상 완화)
    float fDenom = max(abs(fCenterViewZ), 1.f);
    float fRel = abs(fNeighViewZ - fCenterViewZ) / fDenom;

    // threshold 넘어간 정도를 strength로 키움
    return saturate((fRel - OutlineParam.fDepthThreshold) * OutlineParam.fDepthStrength);
}

float EdgeFromNormal(float3 vCenterNormal, float3 vNeighNormal)
{
     // 0이면 동일, 클수록 에지
    float fD = 1.0f - saturate(dot(vCenterNormal, vNeighNormal));
    return saturate((fD - OutlineParam.fNormalThreshold) * OutlineParam.fNormalStrength);
}

float Blur5Tap(float2 vUV, float2 vDir, float2 vInvHalf)
{
    // 5-tap separable gaussian (단순/안정)
    const float w0 = 0.227027f;
    const float w1 = 0.1945946f;
    const float w2 = 0.1216216f;
    
    float2 vOff1 = vDir * 1.f * vInvHalf;
    float2 vOff2 = vDir * 2.f * vInvHalf;
    
    float c0 = g_RenderTargetAOTexture.SampleLevel(PointClampSampler, vUV, 0).r;
    float c1 = g_RenderTargetAOTexture.SampleLevel(PointClampSampler, vUV + vOff1, 0).r;
    float c2 = g_RenderTargetAOTexture.SampleLevel(PointClampSampler, vUV - vOff1, 0).r;
    float c3 = g_RenderTargetAOTexture.SampleLevel(PointClampSampler, vUV + vOff2, 0).r;
    float c4 = g_RenderTargetAOTexture.SampleLevel(PointClampSampler, vUV - vOff2, 0).r;

    return c0 * w0 + (c1 + c2) * w1 + (c3 + c4) * w2;
}

VS_OUT_POS_TEX VS_MAIN(VS_IN_POS_TEX input)
{
    VS_OUT_POS_TEX output;
    output.vPosition = mul(float4(input.vPosition, 1.f), W);
    output.vPosition = mul(output.vPosition, VP);
    output.vUV = saturate(input.vUV);

    return output;
}

PS_OUT_BACKBUFFER PS_MAIN_DEBUG(PS_IN_POS_TEX input)
{
    PS_OUT_BACKBUFFER output;
    output.vColor = g_RenderTargetTexture.Sample(LinearSampler, input.vUV);
    return output;
}

PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN_POS_TEX input)
{
    PS_OUT_LIGHT output;

    // 기본 출력은 검정으로 시작한다.
    output.vShade = float4(0.0f, 0.0f, 0.0f, 1.0f);
    output.vSpecular = float4(0.0f, 0.0f, 0.0f, 1.0f);

    // 현재 픽셀의 NDC Z와 View Z를 읽는다.
    float fNDCZ = 0.0f;
    float fViewZ = 0.0f;
    DecodeDepth(input.vUV, fNDCZ, fViewZ);

    // AO / Roughness / Metal 값을 읽는다.
    float fAO = 1.0f;
    float fRough = 0.0f;
    float fMetal = 0.0f;
    DecodeSpecularMask(input.vUV, fAO, fRough, fMetal);

    // SSAO 결과를 읽는다.
    float fSSAO = g_RenderTargetAOTexture.Sample(LinearSampler, input.vUV).r;

    // Material AO와 SSAO를 곱해서 최종 AO를 만든다.
    float fOcc = saturate(fAO * fSSAO);

    // BaseColor를 GBuffer에서 읽는다.
    float3 vBaseColor = g_RenderTargetDiffuseTexture.Sample(LinearSampler, input.vUV).rgb;

    // World normal을 읽고 정규화한다.
    float3 vNormal = normalize(DecodeWorldNormal(input.vUV));

    // Directional light 방향을 정규화한다.
    float3 vLightDir = normalize(Light.vDirection * -1.0f);

    // 현재 픽셀의 월드 위치를 재구성한다.
    float4 vProjPos = ReconstructProjPosition(input.vUV, fNDCZ, fViewZ);
    float4 vViewPos = mul(vProjPos, InvP);
    vViewPos.xyz /= max(EPSILON, vViewPos.w);
    vViewPos.w = 1.0f;
    float4 vWorldPos = mul(vViewPos, InvV);

    // 카메라에서 현재 픽셀을 향하는 view 방향을 구한다.
    float3 vViewDir = normalize(CameraPosition() - vWorldPos.xyz);

    // Half vector를 구한다.
    float3 vHalfDir = normalize(vViewDir + vLightDir);

    // PBR specular 계산에 필요한 각도를 구한다.
    float fNdotL = saturate(dot(vNormal, vLightDir));
    float fNdotV = saturate(dot(vNormal, vViewDir));
    float fNdotH = saturate(dot(vNormal, vHalfDir));
    float fVdotH = saturate(dot(vViewDir, vHalfDir));

    // Roughness를 alpha로 변환한다.
    float fClampedRough = saturate(fRough);
    float fAlpha = max(0.045f, fClampedRough * fClampedRough);

    // Metal 값을 정리한다.
    float fClampedMetal = saturate(fMetal);

    // 금속이면 BaseColor를 F0로 쓰고, 아니면 0.04를 사용한다.
    float3 vF0 = lerp(float3(0.04f, 0.04f, 0.04f), vBaseColor, fClampedMetal);

    // Fresnel term을 계산한다.
    float3 vF = Fresnel_Schlick(fVdotH, vF0);

    // GGX NDF를 계산한다.
    float fD = D_GGX(fNdotH, fAlpha);

    // Smith geometry term을 계산한다.
    float fG = G_Smith(fNdotV, fNdotL, fAlpha);

    // 최종 PBR specular BRDF를 계산한다.
    float3 vSpecularBRDF = (fD * fG) * vF / max(4.0f * fNdotV * fNdotL, 1e-6f);

    // 에너지 보존을 위해 specular 비율을 계산한다.
    float3 vKS = vF;

    // diffuse 비율은 metal이 높을수록 줄인다.
    float3 vKD = (1.0f - vKS) * (1.0f - fClampedMetal);

    // toon 명암 값을 계산한다.
    float fToonDiffuse = ComputeToonDiffuse(vNormal, vLightDir);

    // 기존 PBR diffuse와 같은 의미로 1/PI를 유지한다.
    float3 vDiffuseBRDF = vKD / PI;

    // Ambient는 AO를 그대로 반영한다.
    float3 vAmbient = Light.vAmbient.rgb * fOcc;

    // Direct diffuse는 AO를 약하게 반영한다.
    const float fDirectOccStrength = 0.35f;
    float fDirectOcc = lerp(1.0f, fOcc, fDirectOccStrength);

    // 광원 색을 가져온다.
    float3 vRadiance = Light.vDiffuse.rgb;

    // 최종 Toon diffuse를 만든다.
    float3 vShade = vAmbient + (vRadiance * (fToonDiffuse * fDirectOcc)) * vDiffuseBRDF;

    // Specular는 예전 PBR 공식을 그대로 유지한다.
    float fSpecOcc = lerp(1.0f, fOcc, 0.2f);
    float3 vSpecular = (vSpecularBRDF * vRadiance) * (fNdotL * fSpecOcc);

    // Rim은 지정된 오브젝트에만 적용한다.
    float fRimMask = GetRimMask(input.vUV);

    // Rim 기본값을 계산한다.
    float fRim = ComputeToonRim(vNormal, vViewDir, vLightDir) * fRimMask;

    // Rim은 diffuse에 섞지 않고 specular 쪽에 얹어서 더 얇고 선명하게 보이게 한다.
    float3 vRim = vRadiance * fRim * 0.25f;

    // 최종 shade를 기록한다.
    output.vShade = float4(vShade, 1.0f);

    // 최종 specular + rim을 기록한다.
    output.vSpecular = float4(vSpecular + vRim, 1.0f);

    return output;
}

PS_OUT_LIGHT PS_MAIN_POINT(PS_IN_POS_TEX input)
{
    PS_OUT_LIGHT output;

    // 기본 출력은 검정으로 시작한다.
    output.vShade = float4(0.0f, 0.0f, 0.0f, 1.0f);
    output.vSpecular = float4(0.0f, 0.0f, 0.0f, 1.0f);

    // 현재 픽셀의 NDC Z와 View Z를 읽는다.
    float fNDCZ = 0.0f;
    float fViewZ = 0.0f;
    DecodeDepth(input.vUV, fNDCZ, fViewZ);

    // AO / Roughness / Metal 값을 읽는다.
    float fAO = 1.0f;
    float fRough = 0.0f;
    float fMetal = 0.0f;
    DecodeSpecularMask(input.vUV, fAO, fRough, fMetal);

    // SSAO 결과를 읽는다.
    float fSSAO = g_RenderTargetAOTexture.Sample(LinearSampler, input.vUV).r;

    // Material AO와 SSAO를 곱해서 최종 AO를 만든다.
    float fOcc = saturate(fAO * fSSAO);

    // BaseColor를 GBuffer에서 읽는다.
    float3 vBaseColor = g_RenderTargetDiffuseTexture.Sample(LinearSampler, input.vUV).rgb;

    // World normal을 읽고 정규화한다.
    float3 vNormal = normalize(DecodeWorldNormal(input.vUV));

    // 현재 픽셀의 월드 위치를 재구성한다.
    float4 vProjPos = ReconstructProjPosition(input.vUV, fNDCZ, fViewZ);
    float4 vViewPos = mul(vProjPos, InvP);
    vViewPos.xyz /= max(EPSILON, vViewPos.w);
    vViewPos.w = 1.0f;
    float4 vWorldPos = mul(vViewPos, InvV);

    // 카메라에서 현재 픽셀을 향하는 view 방향을 구한다.
    float3 vViewDir = normalize(CameraPosition() - vWorldPos.xyz);

    // Point light 방향과 거리를 구한다.
    float3 vToLight = Light.vPosition.xyz - vWorldPos.xyz;
    float fDistance = length(vToLight);
    float3 vLightDir = vToLight / max(fDistance, 1e-6f);

    // Point light 감쇠를 계산한다.
    float fAtt = saturate((Light.fRange - fDistance) / max(Light.fRange, 1e-6f));

    // Half vector를 구한다.
    float3 vHalfDir = normalize(vViewDir + vLightDir);

    // PBR specular 계산에 필요한 각도를 구한다.
    float fNdotL = saturate(dot(vNormal, vLightDir));
    float fNdotV = saturate(dot(vNormal, vViewDir));
    float fNdotH = saturate(dot(vNormal, vHalfDir));
    float fVdotH = saturate(dot(vViewDir, vHalfDir));

    // Roughness를 alpha로 변환한다.
    float fClampedRough = saturate(fRough);
    float fAlpha = max(0.045f, fClampedRough * fClampedRough);

    // Metal 값을 정리한다.
    float fClampedMetal = saturate(fMetal);

    // 금속이면 BaseColor를 F0로 쓰고, 아니면 0.04를 사용한다.
    float3 vF0 = lerp(float3(0.04f, 0.04f, 0.04f), vBaseColor, fClampedMetal);

    // Fresnel term을 계산한다.
    float3 vF = Fresnel_Schlick(fVdotH, vF0);

    // GGX NDF를 계산한다.
    float fD = D_GGX(fNdotH, fAlpha);

    // Smith geometry term을 계산한다.
    float fG = G_Smith(fNdotV, fNdotL, fAlpha);

    // 최종 PBR specular BRDF를 계산한다.
    float3 vSpecularBRDF = (fD * fG) * vF / max(4.0f * fNdotV * fNdotL, 1e-6f);

    // 에너지 보존을 위해 specular 비율을 계산한다.
    float3 vKS = vF;

    // diffuse 비율은 metal이 높을수록 줄인다.
    float3 vKD = (1.0f - vKS) * (1.0f - fClampedMetal);

    // toon 명암 값을 계산한다.
    float fToonDiffuse = ComputeToonDiffuse(vNormal, vLightDir);

    // 기존 PBR diffuse와 같은 의미로 1/PI를 유지한다.
    float3 vDiffuseBRDF = vKD / PI;

    // Point light radiance를 만든다.
    float3 vRadiance = Light.vDiffuse.rgb * fAtt;

    // Direct diffuse는 AO를 약하게 반영한다.
    const float fDirectOccStrength = 0.35f;
    float fDirectOcc = lerp(1.0f, fOcc, fDirectOccStrength);

    // 최종 Toon diffuse를 만든다.
    float3 vShade = (vRadiance * (fToonDiffuse * fDirectOcc)) * vDiffuseBRDF;

    // Specular는 예전 PBR 공식을 그대로 유지한다.
    float fSpecOcc = lerp(1.0f, fOcc, 0.2f);
    float3 vSpecular = (vSpecularBRDF * vRadiance) * (fNdotL * fSpecOcc);

    // Rim은 지정된 오브젝트에만 적용한다.
    float fRimMask = GetRimMask(input.vUV);

    // Rim 기본값을 계산한다.
    float fRim = ComputeToonRim(vNormal, vViewDir, vLightDir) * fRimMask;

    // Rim은 diffuse에 섞지 않고 specular 쪽에 얹는다.
    float3 vRim = vRadiance * fRim * 0.25f;

    // 최종 shade를 기록한다.
    output.vShade = float4(vShade, 1.0f);

    // 최종 specular + rim을 기록한다.
    output.vSpecular = float4(vSpecular + vRim, 1.0f);

    return output;
}

PS_OUT_HDR PS_MAIN_OUTLINE(PS_IN_POS_TEX input)
{
    PS_OUT_HDR output;
    float2 vUV = input.vUV;

    float fEdge = SilhouetteEdge(vUV);
    if (fEdge <= 0.0f)
    {
        output.vColor = float4(0, 0, 0, 0);
        return output;
    }

    // 필요한 경우에만 Depth 1회 (fade용)
    float fNdcZ = 0.0f;
    float fViewZ = 0.0f;
    DecodeDepth(vUV, fNdcZ, fViewZ);

    if (fNdcZ >= 0.9999f)
    {
        output.vColor = float4(0, 0, 0, 0);
        return output;
    }

    fEdge *= OutlineParam.fOpacity;
    fEdge *= ComputeFade(fViewZ);

    output.vColor = float4(OutlineParam.vColor.rgb, OutlineParam.vColor.a * fEdge);
    return output;
}

PS_OUT_AO PS_MAIN_SSAOGEN(PS_IN_POS_TEX input)
{
    PS_OUT_AO output;
    
    float2 invHalf = SSAOparam.vInvAOSize;
    float2 invFull = SSAOparam.vInvAOSize * 0.5f;
    
    //============================
    // Depth Decode (NDC Z, View Z)
    //============================
    float fNDCZ = 0.f;
    float fViewZ = 0.f;
    DecodeDepth(input.vUV, fNDCZ, fViewZ);
    // Clear일 때 AO = 1 처리
    if(fViewZ <= EPSILON || fViewZ >= SSAOparam.fFadeEnd)
    {
        output.vAO = float4(1.f, 1.f, 0.f, 1.f);
        return output;
    }
    
    //============================
    // ViewPos / Normal(View)
    //============================
    float4 vProjPos = ReconstructProjPosition(input.vUV, fNDCZ, fViewZ);
    float4 vViewPos = mul(vProjPos, InvP);
    vViewPos.xyz /= max(EPSILON, vViewPos.w);
    vViewPos.w = 1.f;
    float3 vWorldNormal = DecodeWorldNormal(input.vUV);
    float3 vViewNormal = WorldToViewNormal(vWorldNormal);
    
    //============================
    // Noise -> 랜덤 회전 벡터(View)
    //============================
    float2 vNoiseUV = input.vUV * SSAOkernel.vNoiseScale;
    float3 vRand = g_SSAONoiseTexture.Sample(PointSampler, vNoiseUV).xyz;
    vRand.z = 0.f;
    vRand = normalize(vRand);
    
    //============================
    // TBN (View)
    //============================
    float3 vTangent = normalize(vRand - vViewNormal * dot(vRand, vViewNormal));
    float3 vBitangent = cross(vViewNormal, vTangent);
    float3x3 matTBN = float3x3(vTangent, vBitangent, vViewNormal);
    
    //============================
    // SSAO 계산
    //============================
    float fOcc = 0.f;
    [unroll]
    for (int i = 0; i < SSAO_KERNEL_COUNT; ++i)
    {
        // Tangent ==> View
        float3 vSampleDir = mul(SSAOkernel.vKernel[i].xyz, matTBN);
        float3 vSamplePos = vViewPos.xyz + vSampleDir * SSAOparam.fRadius;
        
        // View공간의 SamplePos를 Clip으로 투영
        float4 vClip = mul(float4(vSamplePos, 1.f), CamP);
        float2 vSampleNDC = vClip.xy / max(EPSILON, vClip.w);
        // NDC ==> UV
        float2 vSampleUV = NDC_ToUV(vSampleNDC);
        
        // 화면밖 SampleUV 스킵
        if (vSampleUV.x < 0.f || vSampleUV.x > 1.f || vSampleUV.y < 0.f || vSampleUV.y > 1.f)
            continue;

        float fSampleNDCZ = 0.f;
        float fSampleViewZ = 0.f;
        DecodeDepth(vSampleUV, fSampleNDCZ, fSampleViewZ);
        
        // Clear배경이면 스킵
        if(fSampleViewZ <= EPSILON)
            continue;
        
        // 샘플 포인트보다 카메라에 더 가까운 지오메트리가 있으면 가림
        float fRange = smoothstep(0.f, 1.f, SSAOparam.fRadius / max(EPSILON, abs(vViewPos.z - fSampleViewZ)));
        float fHit = (fSampleViewZ <= (vSamplePos.z - SSAOparam.fBias)) ? 1.f : 0.f;

        fOcc += fHit * fRange;
    }
    
    float fAO = 1.f - (fOcc / (float) SSAO_KERNEL_COUNT);
    
    fAO = pow(saturate(fAO), max(0.0001f, SSAOparam.fPower));
    fAO = lerp(1.f, fAO, SSAOparam.fIntensity);
    
    float fFade = saturate((SSAOparam.fFadeEnd - fViewZ) / max(EPSILON, (SSAOparam.fFadeEnd - SSAOparam.fFadeStart)));
    fAO = lerp(1.f, fAO, fFade);
    
    output.vAO = float4(fAO, fAO, fAO, 1.f);
    return output;
}

PS_OUT_AO PS_MAIN_SSAOBLURH(PS_IN_POS_TEX input)
{
    PS_OUT_AO output;
    float fAO = Blur5Tap(input.vUV, float2(1.f, 0.f), SSAOparam.vInvAOSize);
    output.vAO = float4(fAO, fAO, fAO, 1.f);
    return output;
}

PS_OUT_AO PS_MAIN_SSAOBLURV(PS_IN_POS_TEX input)
{
    PS_OUT_AO output;
    float fAO = Blur5Tap(input.vUV, float2(0.f, 1.f), SSAOparam.vInvAOSize);
    output.vAO = float4(fAO, fAO, fAO, 1.f);
    return output;
}

PS_OUT_AO PS_MAIN_SSAO_UPSAMPLE(PS_IN_POS_TEX input)
{
    PS_OUT_AO output;

    float2 vInvHalf = SSAOparam.vInvAOSize; // 1/halfW, 1/halfH
    float2 vUVFull = input.vUV;

    //============================
    // Center Depth (ViewZ)
    //============================
    float fCenterViewZ = GetViewZ(vUVFull);
    if (fCenterViewZ <= EPSILON)
    {
        output.vAO = float4(1.f, 1.f, 0.f, 1.f);
        return output;
    }

    //============================
    // Full UV -> Half texel space
    // halfCoord = uv * halfSize - 0.5
    // (uv / invHalf == uv * halfSize)
    //============================
    float2 vHalfCoord = (vUVFull / vInvHalf) - 0.5f.xx;
    float2 vBase = floor(vHalfCoord);
    float2 vFrac = frac(vHalfCoord);

    // half uv 4탭 (00,10,01,11)
    float2 uv00 = (vBase + 0.5f.xx) * vInvHalf;
    float2 uv10 = uv00 + float2(vInvHalf.x, 0.f);
    float2 uv01 = uv00 + float2(0.f, vInvHalf.y);
    float2 uv11 = uv00 + vInvHalf;

    // AO taps (half-res)
    float a00 = g_RenderTargetAOTexture.SampleLevel(PointClampSampler, uv00, 0).r;
    float a10 = g_RenderTargetAOTexture.SampleLevel(PointClampSampler, uv10, 0).r;
    float a01 = g_RenderTargetAOTexture.SampleLevel(PointClampSampler, uv01, 0).r;
    float a11 = g_RenderTargetAOTexture.SampleLevel(PointClampSampler, uv11, 0).r;

    // Depth taps (full-res depth에서 "같은 화면 좌표"로 바로 샘플)
    float z00 = GetViewZ(uv00);
    float z10 = GetViewZ(uv10);
    float z01 = GetViewZ(uv01);
    float z11 = GetViewZ(uv11);

    //============================
    // Bilateral weight (cheap)
    // exp() 대신 rcp 기반
    //============================
    float depthSigma = max(SSAOparam.fRadius * 0.5f, 0.0001f);
    float invSigma = rcp(depthSigma);

    float w00_d = rcp(1.f + abs(fCenterViewZ - z00) * invSigma);
    float w10_d = rcp(1.f + abs(fCenterViewZ - z10) * invSigma);
    float w01_d = rcp(1.f + abs(fCenterViewZ - z01) * invSigma);
    float w11_d = rcp(1.f + abs(fCenterViewZ - z11) * invSigma);

    // Background(0 depth) 무시
    w00_d *= (z00 > EPSILON) ? 1.f : 0.f;
    w10_d *= (z10 > EPSILON) ? 1.f : 0.f;
    w01_d *= (z01 > EPSILON) ? 1.f : 0.f;
    w11_d *= (z11 > EPSILON) ? 1.f : 0.f;

    // Spatial bilinear weight
    float w00_s = (1.f - vFrac.x) * (1.f - vFrac.y);
    float w10_s = (vFrac.x) * (1.f - vFrac.y);
    float w01_s = (1.f - vFrac.x) * (vFrac.y);
    float w11_s = (vFrac.x) * (vFrac.y);

    float w00 = w00_s * w00_d;
    float w10 = w10_s * w10_d;
    float w01 = w01_s * w01_d;
    float w11 = w11_s * w11_d;

    float wSum = (w00 + w10 + w01 + w11);

    // 전부 무시된 경우(가령 주변이 전부 clear) -> AO=1
    if (wSum <= EPSILON)
    {
        output.vAO = float4(1.f, 1.f, 1.f, 1.f);
        return output;
    }

    float fAO = (a00 * w00 + a10 * w10 + a01 * w01 + a11 * w11) / wSum;

    output.vAO = float4(fAO, fAO, fAO, 1.f);
    return output;
}

PS_OUT_HDR PS_MAIN_COMBINED(PS_IN_POS_TEX input)
{
    PS_OUT_HDR output;
    
    float4 vDepth = g_RenderTargetDepthTexture.Sample(LinearSampler, input.vUV);
    
    float4 vDiffuse = g_RenderTargetDiffuseTexture.Sample(LinearSampler, input.vUV);
    
    float4 vSpecular = g_RenderTargetSpecularTexture.Sample(LinearSampler, input.vUV);
    
    float4 vShade = g_RenderTargetShadeTexture.Sample(LinearSampler, input.vUV);
    
    float4 vEmissive = g_RenderTargetEmissiveTexture.Sample(LinearSampler, input.vUV);
    
    output.vColor = float4(vDiffuse.rgb * vShade.rgb + vSpecular.rgb + vEmissive.rgb, 1.f);
    return output;
}

PS_OUT_BLOOM PS_MAIN_BLOOM_EXTRACT(PS_IN_POS_TEX input)
{
    PS_OUT_BLOOM output;
    float3 vHDR = Downsample2x2_SceneHDR(input.vUV, BloomParam.vInvBloomSize * 0.5f);
    float3 vBright = BloomPrefilter(vHDR, BloomParam.fThreshold, BloomParam.fKnee);
    output.vColor = float4(vBright, 1.0f);
    return output;

}

PS_OUT_BLOOM PS_MAIN_BLOOM_PING(PS_IN_POS_TEX input)
{
    PS_OUT_BLOOM output;
    float3 vBlur = Blur9(g_RenderTargetBloomTexture, input.vUV, float2(1, 0), BloomParam.vInvBloomSize);
    output.vColor = float4(vBlur, 1.0f);
    return output;

}

PS_OUT_BLOOM PS_MAIN_BLOOM_PONG(PS_IN_POS_TEX input)
{
    PS_OUT_BLOOM output;
    float3 vBlur = Blur9(g_RenderTargetBloomTexture, input.vUV, float2(0, 1), BloomParam.vInvBloomSize);
    output.vColor = float4(vBlur, 1.0f);
    return output;
}

PS_OUT_BACKBUFFER PS_MAIN_TONEMAP(PS_IN_POS_TEX input)
{
    PS_OUT_BACKBUFFER output;
    float3 vBloom = g_RenderTargetBloomTexture.Sample(LinearClampSampler, input.vUV).rgb;
    float4 vScene = g_RenderTargetSceneHDRTexture.Sample(LinearClampSampler, input.vUV);
    // Bloom 합성
    float3 vHDR = vScene.rgb + vBloom * BloomParam.fIntensity;
    
    // Exposure
    vHDR *= HDRparam.fExposure;
    
    // Tonemap
    float3 vLDR = ToneMap_ACES(vHDR.rgb);
    
    // 대비
    vLDR = max(vLDR, 0.0.xxx);
    vLDR /= 0.18f;
    vLDR = pow(vLDR, HDRparam.fGamma);
    vLDR *= 0.18f;
    
    vLDR = ApplyLUT_16(vLDR);
    
    float3 invGamma = 1.f / max(0.001f, 2.2f);
    vLDR = pow(saturate(vLDR), invGamma);
    output.vColor = float4(vLDR, 1.f);
    return output;
}

technique11 T0
{
    PASS_RS_DS_BS_VP(Debug, RS_Default, DS_Default, BS_Default, VS_MAIN, PS_MAIN_DEBUG)
    PASS_RS_DS_BS_VP(DirectionalLight, RS_Default, DS_Disabled, BS_Default, VS_MAIN, PS_MAIN_DIRECTIONAL)
    PASS_RS_DS_BS_VP(PointLight, RS_Default, DS_Disabled, BS_PointLightBlend, VS_MAIN, PS_MAIN_POINT)
    PASS_RS_DS_BS_VP(Outline, RS_Default, DS_Disabled, BS_AlphaBlend, VS_MAIN, PS_MAIN_OUTLINE)
    PASS_RS_DS_BS_VP(SSAOGen, RS_Default, DS_Disabled, BS_Default, VS_MAIN, PS_MAIN_SSAOGEN)
    PASS_RS_DS_BS_VP(SSAOBLURH, RS_Default, DS_Disabled, BS_Default, VS_MAIN, PS_MAIN_SSAOBLURH)
    PASS_RS_DS_BS_VP(SSAOBLURV, RS_Default, DS_Disabled, BS_Default, VS_MAIN, PS_MAIN_SSAOBLURV)
    PASS_RS_DS_BS_VP(SSAOUpsample, RS_Default, DS_Disabled, BS_Default, VS_MAIN, PS_MAIN_SSAO_UPSAMPLE)
    PASS_RS_DS_BS_VP(CombinedHDR, RS_Default, DS_Disabled, BS_Default, VS_MAIN, PS_MAIN_COMBINED)
    PASS_RS_DS_BS_VP(BloomExtract, RS_Default, DS_Disabled, BS_Default, VS_MAIN, PS_MAIN_BLOOM_EXTRACT)
    PASS_RS_DS_BS_VP(BloomPing, RS_Default, DS_Disabled, BS_Default, VS_MAIN, PS_MAIN_BLOOM_PING)
    PASS_RS_DS_BS_VP(BloomPong, RS_Default, DS_Disabled, BS_Default, VS_MAIN, PS_MAIN_BLOOM_PONG)
    PASS_RS_DS_BS_VP(Tonemap, RS_Default, DS_Disabled, BS_Default, VS_MAIN, PS_MAIN_TONEMAP)
};