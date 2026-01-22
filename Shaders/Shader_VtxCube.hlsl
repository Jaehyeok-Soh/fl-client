#include "Struct_Defines.hlsl"
#include "Light_Defines.hlsl"

VS_OUT_CUBE VS_MAIN(VS_IN_CUBE input)
{
    VS_OUT_CUBE output;
    output.vPosition = mul(float4(input.vPosition, 1.f), W);
    output.vPosition = mul(output.vPosition, VP);
    output.vUV = input.vUV;
    return output;
}

PS_OUT PS_MAIN(PS_IN_CUBE input)
{
    PS_OUT output;
    float4 vColor = g_TextureCube.Sample(LinearSampler, input.vUV);
    output.vColor = vColor;
    return output;
}

PS_OUT PS_GRAY(PS_IN_CUBE input)
{
    PS_OUT output;
    float4 vColor = g_TextureCube.Sample(LinearSampler, input.vUV);
    float gray = dot(vColor.rgb, float3(0.2126f, 0.7152f, 0.0722f));
    vColor.rgb = lerp(vColor.rgb, gray.xxx, 0.9f) * float3(0.3f, 0.3f, 0.3f);
    output.vColor = vColor;
    return output;
}

technique11 T0
{
    PASS_RS_DS_BS_VP(P0, RS_Default_CullFront, DS_Disabled, BS_Default, VS_MAIN, PS_MAIN)
    PASS_RS_DS_BS_VP(P1, RS_Default_CullFront, DS_Disabled, BS_Default, VS_MAIN, PS_GRAY)
};