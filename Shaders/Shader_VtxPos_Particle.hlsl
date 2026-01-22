#include "Struct_Defines.hlsl"
#include "Light_Defines.hlsl"


VS_OUT_POS_GS_PARTICLE VS_MAIN(VS_IN_POS_GS_PARTICLE input)
{
    VS_OUT_POS_GS_PARTICLE output;
    
    vector vPosition = mul(vector(input.vPosition, 1.f), input.matTransform);
    output.vPosition = mul(vPosition, W);
    output.vPSize = float2(length(input.matTransform._11_22_33), length(input.matTransform._21_22_23));
    output.vLieftTime = input.vLifeTime;
    
    return output;
}

[maxvertexcount(6)]
void GS_MAIN(point GS_IN_POS_PARTICLE input[1], inout TriangleStream<GS_OUT_POS_PARTICLE> outStream)
{
    GS_OUT_POS_PARTICLE output[4];

    float3 vLook = normalize((CameraPosition() - input[0].vPosition.xyz));
    float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * input[0].vPSize.x;
    float3 vUp = normalize(cross(vLook, vRight)) * input[0].vPSize.y;
    
    output[0].vPosition = mul(vector(input[0].vPosition.xyz + vRight + vUp, 1.f), VP);
    output[0].vUV = float2(0.f, 0.f);
    output[0].vLieftTime = input[0].vLieftTime;
    
    output[1].vPosition = mul(vector(input[0].vPosition.xyz - vRight + vUp, 1.f), VP);
    output[1].vUV = float2(1.f, 0.f);
    output[1].vLieftTime = input[0].vLieftTime;
    
    output[2].vPosition = mul(vector(input[0].vPosition.xyz - vRight - vUp, 1.f), VP);
    output[2].vUV = float2(1.f, 1.f);
    output[2].vLieftTime = input[0].vLieftTime;
    
    output[3].vPosition = mul(vector(input[0].vPosition.xyz + vRight - vUp, 1.f), VP);
    output[3].vUV = float2(0.f, 1.f);
    output[3].vLieftTime = input[0].vLieftTime;
    
    outStream.Append(output[0]);
    outStream.Append(output[1]);
    outStream.Append(output[2]);
    outStream.RestartStrip();

    outStream.Append(output[0]);
    outStream.Append(output[2]);
    outStream.Append(output[3]);
    outStream.RestartStrip();
}

PS_OUT PS_MAIN(PS_IN_POS_PARTICLE input)
{
    PS_OUT output;
    vector vColor = g_DefaultTextures[DEFAULT].Sample(LinearSampler, input.vUV);
    if (ALPHA_TEST(vColor.a, 0.3f))
        discard;
    
    vColor.a = saturate(input.vLifeTime.y - input.vLifeTime.x).r;
    vColor.rgb = vColor.rgb + (1.f - vColor.a);
    output.vColor = vColor;
    return output;
}

technique11 T0
{
    PASS_RS_DS_BS_GS_VP(P0, RS_Default_CullNone, DS_Default, BS_AlphaBlend, GS_MAIN, VS_MAIN, PS_MAIN)
};