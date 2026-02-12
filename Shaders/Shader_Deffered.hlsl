#include "Struct_Defines.hlsl"
#include "Light_Defines.hlsl"

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
    float4 vDepthDesc = g_RenderTargetDepthTexture.Sample(LinearSampler, input.vUV);
    float fNDCZ = vDepthDesc.x;
    float fViewZ = vDepthDesc.y;
    
    //=================================
    // Packed Mask (AO / Rough / Metal)
    //=================================
    float4 vSpecularMaskDesc = g_RenderTargetSpecularMaskTexture.Sample(LinearSampler, input.vUV);
    float fAO = vSpecularMaskDesc.r;
    float fRough = vSpecularMaskDesc.g;
    float fMetal = vSpecularMaskDesc.b;
    
    //===============
    // Normal Decode
    //===============
    float4 vNormalDesc = g_RenderTargetNormalTexture.Sample(LinearSampler, input.vUV);
    float3 vNormal = normalize(float3(vNormalDesc.xyz * 2.f - 1.f));
    float3 vLightDir = normalize(Light.vDirection * -1.f);
    float3 vShade = max(dot(vLightDir, vNormal), 0.f);
    float3 vAmbient = Light.vAmbient.rgb * fAO;
    float3 vDiffuse = Light.vDiffuse.rgb * vShade;
    output.vShade = float4((vAmbient + vDiffuse), 1.f);
    //===========================
    // Reconstruct World Position
    //===========================
    float4 vProjPos;
    /* 투영공간상의 좌표를 구한다. */
    /* 로컬위치 * 월드 * 뷰 * 투영 / V.z */ 
    vProjPos.x = input.vUV.x * 2.f - 1.f;
    vProjPos.y = input.vUV.y * -2.f + 1.f;
    vProjPos.z = fNDCZ;
    vProjPos.w = 1.f;
    /* 투영행렬까지 곱한 상태를 만들어준다. */ 
    /* 로컬위치 * 월드 * 뷰 * 투영 / V.z  * V.z */ 
    vProjPos *= fViewZ;
    
    /* 로컬위치 * 월드 * 뷰 * 투영 * 투영-1 */
    float4 vViewPos = mul(vProjPos, InvP);
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
    float4 vDepthDesc = g_RenderTargetDepthTexture.Sample(LinearSampler, input.vUV);
    float fNDCZ = vDepthDesc.x;
    float fViewZ = vDepthDesc.y;

    //=================================
    // Packed Mask (AO / Rough / Metal)
    //=================================
    float4 vSpecularMaskDesc = g_RenderTargetSpecularMaskTexture.Sample(LinearSampler, input.vUV);
    float fAO = vSpecularMaskDesc.r;
    float fRough = vSpecularMaskDesc.g;
    float fMetal = vSpecularMaskDesc.b;

    //===============
    // Normal Decode
    //===============
    float4 vNormalDesc = g_RenderTargetNormalTexture.Sample(LinearSampler, input.vUV);
    float3 vNormal = normalize(vNormalDesc.xyz * 2.f - 1.f);

    //===========================
    // Reconstruct World Position
    //===========================
    float4 vProjPos;
    vProjPos.x = input.vUV.x * 2.f - 1.f;
    vProjPos.y = input.vUV.y * -2.f + 1.f;
    vProjPos.z = fNDCZ;
    vProjPos.w = 1.f;

    vProjPos *= fViewZ;

    float4 vViewPos = mul(vProjPos, InvP);
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
    PASS_RS_DS_BS_VP(Combined, RS_Default, DS_Disabled, BS_Default, VS_MAIN, PS_MAIN_COMBINED)
};