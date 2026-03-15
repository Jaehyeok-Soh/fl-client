#include "Struct_Defines.hlsl"
#include "Light_Defines.hlsl"


VS_OUT_POS_TEX_NOR VS_MAIN(VS_IN_POS_TEX_NOR input)
{
    VS_OUT_POS_TEX_NOR output;
    output.vPosition = mul(float4(input.vPosition, 1.f), W);
    output.vPosition = mul(output.vPosition, VP);
    output.vUV = input.vUV;
    
    output.vNormal = normalize(mul(input.vNormal, (float3x3) W));
    output.vWorldPos = mul(float4(input.vPosition, 1.f), W);
    output.vProjPos = mul(output.vWorldPos, VP);
    return output;
}

PS_OUT_DEFFERED PS_MAIN(PS_IN_POS_TEX_NOR input)
{
    PS_OUT_DEFFERED output = (PS_OUT_DEFFERED)0;
    output.vDiffuse = g_DefaultTextures[DEFAULT].Sample(LinearSampler, input.vUV);
    output.vNormal = float4(input.vNormal * 0.5f + 0.5f, 1.f);
    output.vDepth = float4(input.vProjPos.z / input.vProjPos.w, input.vProjPos.w, 0.f, 0.f);
    
    return output;
}

PS_OUT PS_RED(PS_IN_POS_TEX_NOR input)
{
    PS_OUT output;
    output.vColor = float4(1.f, 0.f, 0.f, 1.f);    
    return output;
}

technique11 T0
{
    PASS_RS_DS_BS_VP(P0, RS_Default, DS_Default, BS_Default, VS_MAIN, PS_MAIN)
    PASS_RS_DS_BS_VP(P1, RS_Wire, DS_Default, BS_Default, VS_MAIN, PS_RED)
};