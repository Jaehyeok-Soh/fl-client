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

PS_OUT PS_MAIN(PS_IN_POS_TEX input)
{
    PS_OUT output;
    output.vColor = g_DefaultTextures[DEFAULT].Sample(LinearSampler, input.vUV);
    return output;
}

technique11 T0
{
    PASS_RS_DS_BS_VP(Default, RS_Default, DS_Default, BS_Default, VS_MAIN, PS_MAIN)
    PASS_RS_DS_BS_VP(P1, RS_Default, DS_Default, BS_AlphaBlend, VS_MAIN, PS_MAIN)
};