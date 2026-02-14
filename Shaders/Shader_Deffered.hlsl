#include "Struct_Defines.hlsl"
#include "Light_Defines.hlsl"

float3 DecodeWorldNormal(float2 vUV)
{
    float4 vNormalDesc = g_RenderTargetNormalTexture.Sample(PointClampSampler, vUV);
    return normalize(vNormalDesc.xyz * 2.f - 1.f);
}

void DecodeDepth(float2 vUV, out float fNDCZ, out float fViewZ)
{
    float4 vDepthDesc = g_RenderTargetDepthTexture.Sample(PointClampSampler, vUV);
    fNDCZ = vDepthDesc.x;
    fViewZ = vDepthDesc.y;
}

void DecodeSpecularMask(float2 vUV, out float fAO, out float fRough, out float fMetal)
{
    float4 vSpecularMaskDesc = g_RenderTargetSpecularMaskTexture.Sample(LinearSampler, vUV);
    fAO = vSpecularMaskDesc.x;
    fRough = vSpecularMaskDesc.y;
    fMetal = vSpecularMaskDesc.z;
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

float GetViewZ(float2 vUV)
{
    return g_RenderTargetDepthTexture.Sample(PointClampSampler, vUV).y;
}

float2 UV_ToNDC(float2 vUV)
{
    // [0, 0] ==> [-1, 1]
    // [1, 1] ==> [1, -1]
    return float2(vUV.x * 2.f - 1.f, vUV.y * -2.f + 1.f);
}

float2 NDC_ToUV(float2 vNDC)
{
    // [-1, 1] ==> [0, 0]
    // [1, -1] ==> [1, 1]
    return float2(vNDC.x * 0.5f + 0.5f, -vNDC.y * 0.5f + 0.5f);
}

float3 WorldToViewNormal(float3 vNormalWorld)
{
    return normalize(mul(vNormalWorld, (float3x3)CamV));
}

float4 ReconstructProjPosition(float2 vUV, float fNDCZ, float fViewZ)
{
    float4 vProjPos;
    /* 투영공간상의 좌표를 구한다. */
    /* 로컬위치 * 월드 * 뷰 * 투영 / V.z ( 현재 Clip 좌표계 )*/
    vProjPos.xy = UV_ToNDC(vUV);
    vProjPos.z = fNDCZ;
    vProjPos.w = 1.f;
    /* 투영행렬까지 곱한 상태를 만들어준다. */ 
    /* 로컬위치 * 월드 * 뷰 * 투영 / V.z  * V.z */ 
    vProjPos *= fViewZ;
    return vProjPos;
}

VS_OUT_POS_TEX VS_MAIN(VS_IN_POS_TEX input)
{
    VS_OUT_POS_TEX output;
    output.vPosition = mul(float4(input.vPosition, 1.f), W);
    output.vPosition = mul(output.vPosition, VP);
    output.vUV = input.vUV;

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
    output.vShade = float4(0.f, 0.f, 0.f, 1.f);
    output.vSpecular = float4(0.f, 0.f, 0.f, 1.f);
    //============================
    // Depth Decode (NDC Z, View Z)
    //============================
    float fNDCZ = 0.f;
    float fViewZ = 0.f;
    DecodeDepth(input.vUV, fNDCZ, fViewZ);
    
    //=================================
    // Packed Mask (AO / Rough / Metal)
    //=================================
    float fAO = 0.f;
    float fRough = 0.f;
    float fMetal = 0.f;
    DecodeSpecularMask(input.vUV, fAO, fRough, fMetal);
    
    //=================================
    // SSAO
    //=================================
    float fSSAO = g_RenderTargetAOTexture.Sample(LinearSampler, input.vUV).r;
    
    //===============
    // Normal Decode
    //===============
    float3 vNormal = DecodeWorldNormal(input.vUV);
    float3 vLightDir = normalize(Light.vDirection * -1.f);
    float3 vShade = max(dot(vLightDir, vNormal), 0.f);
    float fOcc = saturate(fAO * fSSAO);
    float3 vAmbient = Light.vAmbient.rgb * fOcc;
    const float fDirectOccStrength = 0.35f;
    float fDirectOcc = lerp(1.f, fOcc, fDirectOccStrength);
    float3 vDiffuse = Light.vDiffuse.rgb * vShade * fDirectOcc;
    output.vShade = float4((vAmbient + vDiffuse), 1.f);
    //===========================
    // Reconstruct World Position
    //===========================
    float4 vProjPos = ReconstructProjPosition(input.vUV, fNDCZ, fViewZ);    
    
    /* 로컬위치 * 월드 * 뷰 * 투영 * 투영-1 */
    float4 vViewPos = mul(vProjPos, InvP);
    vViewPos.xyz /= max(EPSILON, vViewPos.w);
    vViewPos.w = 1.f;
    /* 로컬위치 * 월드 * 뷰 * 뷰-1 */
    float4 vWorldPos = mul(vViewPos, InvV);
    
    float3 vViewDir = normalize(CameraPosition() - vWorldPos.xyz);
    float3 vHalf = normalize(vLightDir + vViewDir);
    
    float fShininess = lerp(256.f, 8.f, saturate(fRough));
    float fSpec = pow(saturate(dot(vNormal, vHalf)), fShininess);
    
    float3 vF0 = lerp(float3(0.04f, 0.04f, 0.04f), float3(1.f, 1.f, 1.f), saturate(fMetal));
    float3 vSpecRGB = Light.vSpecular.rgb * fSpec * vF0;
    
    output.vSpecular = float4(vSpecRGB, 1.f);
    return output;
}

PS_OUT_LIGHT PS_MAIN_POINT(PS_IN_POS_TEX input)
{
    PS_OUT_LIGHT output;
    output.vShade = float4(0.f, 0.f, 0.f, 1.f);
    output.vSpecular = float4(0.f, 0.f, 0.f, 1.f);

    //============================
    // Depth Decode (NDC Z, View Z)
    //============================
    float fNDCZ = 0.f;
    float fViewZ = 0.f;
    DecodeDepth(input.vUV, fNDCZ, fViewZ);
    
    
    //=================================
    // Packed Mask (AO / Rough / Metal)
    //=================================
    float fAO = 0.f;
    float fRough = 0.f;
    float fMetal = 0.f;
    DecodeSpecularMask(input.vUV, fAO, fRough, fMetal);

    //===============
    // Normal Decode
    //===============
    float3 vNormal = DecodeWorldNormal(input.vUV);

    //===========================
    // Reconstruct World Position
    //===========================
    float4 vProjPos = ReconstructProjPosition(input.vUV, fNDCZ, fViewZ);
    float4 vViewPos = mul(vProjPos, InvP);
    vViewPos.xyz /= max(EPSILON, vViewPos.w);
    vViewPos.w = 1.f;
    float4 vWorldPos = mul(vViewPos, InvV);

    //========================
    // Point Light Vector / Att
    //========================
    float3 vToLight = (Light.vPosition.xyz - vWorldPos.xyz); // Surface -> Light
    float fDistance = length(vToLight);

    // 0 나눗셈 방지
    float3 vLightDir = vToLight / max(fDistance, 1e-6f);

    // 가장 단순한 선형 감쇠(지금 단계에 OK)
    float fAtt = saturate((Light.fRange - fDistance) / max(Light.fRange, 1e-6f));

    //=====================
    // Shade (Diffuse only)
    //=====================
    float fNdotL = saturate(dot(vLightDir, vNormal));

    // Point 라이트에서 Ambient를 누적시키면 쉽게 과해지므로,
    // 일단은 "Diffuse만" 추천 (Ambient는 전역/Directional에서 처리)
    float3 vDiffuse = Light.vDiffuse.rgb * fNdotL;

    output.vShade = float4(vDiffuse * fAtt, 1.f);

    //=====================
    // Specular (Half vector)
    //=====================
    float3 vViewDir = normalize(CameraPosition() - vWorldPos.xyz);
    float3 vHalf = normalize(vLightDir + vViewDir);

    float fShininess = lerp(256.f, 8.f, saturate(fRough));
    float fSpec = pow(saturate(dot(vNormal, vHalf)), fShininess);

    float3 vF0 = lerp(float3(0.04f, 0.04f, 0.04f), float3(1.f, 1.f, 1.f), saturate(fMetal));
    float3 vSpecRGB = Light.vSpecular.rgb * fSpec * vF0;

    output.vSpecular = float4(vSpecRGB * fAtt, 1.f);

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
        output.vAO = float4(1.f, 1.f, 1.f, 1.f);
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
    float3 vRand = g_SSAONoiseTexture.Sample(PointSampler, vNoiseUV).xyz * 2.f - 1.f;
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
        output.vAO = float4(1.f, 1.f, 1.f, 1.f);
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

PS_OUT_BACKBUFFER PS_MAIN_COMBINED(PS_IN_POS_TEX input)
{
    PS_OUT_BACKBUFFER output;
    
    float4 vDepth = g_RenderTargetDepthTexture.Sample(LinearSampler, input.vUV);
    
    float4 vDiffuse = g_RenderTargetDiffuseTexture.Sample(LinearSampler, input.vUV);
    
    if (0.f == vDiffuse.a)
        discard;
    
    float4 vSpecular = g_RenderTargetSpecularTexture.Sample(LinearSampler, input.vUV);
    
    float4 vShade = g_RenderTargetShadeTexture.Sample(LinearSampler, input.vUV);
    
    output.vColor = vDiffuse * vShade + vSpecular;
    return output;
}

technique11 T0
{
    PASS_RS_DS_BS_VP(Debug, RS_Default, DS_Default, BS_Default, VS_MAIN, PS_MAIN_DEBUG)
    PASS_RS_DS_BS_VP(DirectionalLight, RS_Default, DS_Disabled, BS_Default, VS_MAIN, PS_MAIN_DIRECTIONAL)
    PASS_RS_DS_BS_VP(PointLight, RS_Default, DS_Disabled, BS_Blend, VS_MAIN, PS_MAIN_POINT)
    PASS_RS_DS_BS_VP(SSAOGen, RS_Default, DS_Disabled, BS_Default, VS_MAIN, PS_MAIN_SSAOGEN)
    PASS_RS_DS_BS_VP(SSAOBLURH, RS_Default, DS_Disabled, BS_Default, VS_MAIN, PS_MAIN_SSAOBLURH)
    PASS_RS_DS_BS_VP(SSAOBLURV, RS_Default, DS_Disabled, BS_Default, VS_MAIN, PS_MAIN_SSAOBLURV)
    PASS_RS_DS_BS_VP(SSAOUpsample, RS_Default, DS_Disabled, BS_Default, VS_MAIN, PS_MAIN_SSAO_UPSAMPLE)
    PASS_RS_DS_BS_VP(Combined, RS_Default, DS_Disabled, BS_Default, VS_MAIN, PS_MAIN_COMBINED)
};