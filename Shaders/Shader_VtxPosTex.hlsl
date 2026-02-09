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
    output.vColor = g_DefaultTextures[DEFAULT].Sample(PointSampler, input.vUV);
    if(output.vColor.a < 0.3f)
        discard;
    return output;
}

PS_OUT PS_COLOR(PS_IN_POS_TEX input)
{
    PS_OUT output;
    output.vColor = g_DefaultTextures[DEFAULT].Sample(PointSampler, input.vUV);
    if (output.vColor.a < 0.3f)
        discard;
    
    output.vColor = g_vColorTint;
    return output;
}

PS_OUT PS_FADE(PS_IN_POS_TEX input)
{
    PS_OUT output;
    vector vColor = g_DefaultTextures[0].Sample(PointSampler, input.vUV);
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
        output.vColor = g_DefaultTextures[0].Sample(PointSampler, input.vUV);
    
    if (g_iFillDir == 0) 
        mask = step(uv.x, g_fProgressRatio);        //right -> left
    else if (g_iFillDir == 1)
        mask = step(1.0f - uv.x, g_fProgressRatio); //left -> right
    else if (g_iFillDir == 2)
        mask = step(1.0f - uv.y, g_fProgressRatio); //up -> down
    else if (g_iFillDir == 3)
        mask = step(uv.y, g_fProgressRatio); //down -> up
    
    if (mask <= 0.0f)
        discard;
    
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