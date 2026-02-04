//#include "Struct_Defines.hlsl"
//#include "Light_Defines.hlsl"


//VS_OUT_POS_TEX_PARTICLE VS_MAIN(VS_IN_POS_TEX_PARTICLE input)
//{
//    VS_OUT_POS_TEX_PARTICLE output;
    
//    float4x4 matTransform = float4x4(input.vRight, input.vUp, input.vLook, input.vTranslation);
//    output.vPosition = mul(float4(input.vPosition, 1.f), matTransform);
//    output.vPosition = mul(output.vPosition, W);
//    output.vPosition = mul(output.vPosition, VP);
//    output.vUV = input.vUV;
//    output.vLifeTime = input.vLifeTime;
//    return output;
//}

//PS_OUT PS_MAIN(PS_IN_POS_TEX_PARTICLE input)
//{
//    PS_OUT output;
//    vector vColor = g_DefaultTextures[DEFAULT].Sample(LinearSampler, input.vUV);
//    if (ALPHA_TEST(vColor.a, 0.3f))
//        discard;
    
//    vColor.a = saturate(input.vLifeTime.y - input.vLifeTime.x).r;
//    vColor.rgb = vColor.rgb + (1.f - vColor.a);
//    output.vColor = vColor;
//    return output;
//}

//technique11 T0
//{
//    PASS_RS_DS_BS_VP(P0, RS_Default_CullNone, DS_Default, BS_AlphaBlend, VS_MAIN, PS_MAIN)
//};