#include "Struct_Defines.hlsl"
#include "Light_Defines.hlsl"


VS_OUT_POS_COLOR VS_MAIN(VS_IN_POS_COLOR input)
{
    VS_OUT_POS_COLOR output;
    output.vPosition = mul(float4(input.vPosition, 1.f), W);
    output.vPosition = mul(output.vPosition, VP);
    output.vColor = input.vColor;

    return output;
}

PS_OUT PS_MAIN(PS_IN_POS_COLOR input)
{
    PS_OUT output;
    output.vColor = input.vColor;
    return output;
}

technique11 T0
{
    PASS_RS_DS_BS_VP(P0, RS_Default, DS_Default, BS_Default, VS_MAIN, PS_MAIN)
};