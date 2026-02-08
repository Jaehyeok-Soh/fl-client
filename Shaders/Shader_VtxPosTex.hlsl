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

PS_OUT PS_COLOR(PS_IN_POS_TEX input)
{
    PS_OUT output;
    output.vColor = g_vColorTint;
    return output;
}

PS_OUT PS_FADE(PS_IN_POS_TEX input)
{
    PS_OUT output;
    vector vColor = g_DefaultTextures[0].Sample(LinearSampler, input.vUV);
    vColor.a *= g_fAlphaRatio;
    output.vColor = vColor;
    return output;
}

PS_OUT PS_PROGRESS(PS_IN_POS_TEX input)
{
    PS_OUT output;
    float2 uv = input.vUV;
    float mask = 1.0f;

    if (g_isColor)
        output.vColor = g_vColorTint;
    else
        output.vColor = g_DefaultTextures[0].Sample(LinearSampler, input.vUV);
    
    if (g_iFillDir == 0)
        mask = step(uv.x, g_fProgressRatio);        //right -> left
    else if (g_iFillDir == 1)
        mask = step(1.0f - uv.x, g_fProgressRatio); //left -> right
    else if (g_iFillDir == 2)
        mask = step(uv.y, g_fProgressRatio);        //up -> down
    else
        mask = step(1.0f - uv.y, g_fProgressRatio); //down -> up

    output.vColor.a *= mask;
    return output;
}








PS_OUT PS_SKILLICON(PS_IN_POS_TEX input)
{
    PS_OUT output;
    float4 vIconMask = g_DefaultTextures[0].Sample(LinearSampler, input.vUV);
    if (ALPHA_TEST(vIconMask.a, 0.2f))
        discard;
    vIconMask *= (input.vUV.y >= 1.f - input.fHP) ? float4(0.3f, 0.3f, 0.3f, 1.f) : 1.f;
    
    float4 vIcon = g_DefaultTextures[1].Sample(LinearSampler, input.vUV);
    output.vColor = vIconMask * vIcon;
    return output;
}

PS_OUT PS_LOCKON(PS_IN_POS_TEX input)
{
    PS_OUT output;
    float4 vColor = g_DefaultTextures[DEFAULT].Sample(LinearSampler, input.vUV);
    vColor.a = vColor.r;
    output.vColor = vColor;
    return output;
}

technique11 T0
{
    PASS_RS_DS_BS_VP(Default, RS_Default, DS_Default, BS_Default, VS_MAIN, PS_MAIN)
    PASS_RS_DS_BS_VP(DefaultAlpha, RS_Default, DS_Default, BS_AlphaBlend, VS_MAIN, PS_MAIN)
    PASS_RS_DS_BS_VP(Color, RS_Default, DS_Default, BS_AlphaBlend, VS_MAIN, PS_COLOR)
    PASS_RS_DS_BS_VP(Fade, RS_Default, DS_Default, BS_AlphaBlend, VS_MAIN, PS_FADE)
    PASS_RS_DS_BS_VP(Progress, RS_Default, DS_Default, BS_AlphaBlend, VS_MAIN, PS_PROGRESS)
};