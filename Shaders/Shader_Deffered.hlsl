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
    float4 vNormalDesc = g_RenderTargetNormalTexture.Sample(LinearSampler, input.vUV);
    // (0 ~ 1) ==> (-1 ~ 1)
    float3 vNormal = float3(vNormalDesc.xyz * 2.f - 1.f);
    float4 vShade = max(dot(normalize(Light.vDirection) * -1.f, vNormal), 0.f);
    vShade = floor(vShade * 3.f + EPSILON) / 3.f;
    output.vShade = Light.vDiffuse * saturate(vShade + Light.vAmbient);
    return output;
}

PS_OUT_LIGHT PS_MAIN_POINT(PS_IN_POS_TEX input)
{
    PS_OUT_LIGHT output;
    float4 vNormalDesc = g_RenderTargetNormalTexture.Sample(LinearSampler, input.vUV);
    float3 vNormal = float3(vNormalDesc.xyz * 2.f - 1.f);
    float4 vDepthDesc = g_RenderTargetDepthTexture.Sample(LinearSampler, input.vUV);
    float fViewZ = vDepthDesc.y;
    
    float4 vProjPosition;
    vProjPosition.x = input.vUV.x * 2.f - 1.f;
    vProjPosition.y = input.vUV.y * -2.f + 1.f;
    vProjPosition.z = vDepthDesc.x;
    vProjPosition.w = 1.f;
    
    // 로컬위치 * 월드 * 뷰 * 투영 / V.z * V.z
    vProjPosition *= fViewZ;
    
    // 로컬위치 * 월드 * 뷰 * 투영 * 투영-1
    float4 vViewPosition = mul(vProjPosition, InvP);
    
    // 로컬위치 * 월드 * 뷰 * 뷰-1
    float4 vWorldPosition = mul(vViewPosition, InvV);
    
    float4 vLightDir = (vWorldPosition - Light.vPosition);
    float fDistance = length(vLightDir);
    vLightDir = normalize(vLightDir);
    
    float fAtt = saturate((Light.fRange - fDistance) / Light.fRange);
    float4 vShade = max(dot((vLightDir * -1.f).xyz, vNormal), 0.f) + (Light.vAmbient);
    
    output.vShade = Light.vDiffuse * saturate(vShade) * fAtt;
    return output;
}

PS_OUT_BACKBUFFER PS_MAIN_COMBINED(PS_IN_POS_TEX input)
{
    PS_OUT_BACKBUFFER output;
    
    float4 vDepth = g_RenderTargetDepthTexture.Sample(LinearSampler, input.vUV);
    
    float4 vDiffuse = g_RenderTargetDiffuseTexture.Sample(LinearSampler, input.vUV);
    
    if (0.f == vDiffuse.a)
        discard;
    
    float4 vShade = g_RenderTargetShadeTexture.Sample(LinearSampler, input.vUV);
    
    output.vColor = vDiffuse * vShade;
    
    //float fDistance = vDepth.y / 100.f;
    //float fog = saturate((fDistance - 0.1f) / max(0.0001, (0.5f - 0.1f)));
    //output.vColor = lerp(output.vColor, float4(0.6f, 0.6f, 0.6f, 1.f), fog);
    return output;
}

technique11 T0
{
    PASS_RS_DS_BS_VP(Debug, RS_Default, DS_Default, BS_Default, VS_MAIN, PS_MAIN_DEBUG)
    PASS_RS_DS_BS_VP(DirectionalLight, RS_Default, DS_Disabled, BS_Default, VS_MAIN, PS_MAIN_DIRECTIONAL)
    PASS_RS_DS_BS_VP(PointLight, RS_Default, DS_Disabled, BS_Blend, VS_MAIN, PS_MAIN_POINT)
    PASS_RS_DS_BS_VP(Combined, RS_Default, DS_Disabled, BS_Default, VS_MAIN, PS_MAIN_COMBINED)
};