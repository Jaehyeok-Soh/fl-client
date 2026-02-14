#include "Struct_Defines.hlsl"
#include "Light_Defines.hlsl"

#define UI_NOISE 1
#define UI_ALPHA_MASK 2

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
    float2 uv = input.vUV;
    // Flip X
    if (g_iFlip == 1)
        uv.x = 1.0f - uv.x;
    // Flip Y
    else if (g_iFlip == 2)   
        uv.y = 1.0f - uv.y;
    // Flip XY
    else if (g_iFlip == 3)   
    {
        uv.x = 1.0f - uv.x;
        uv.y = 1.0f - uv.y;
    }
    
    output.vColor = g_DefaultTextures[DEFAULT].Sample(PointSampler, uv);
    if(output.vColor.a < 0.3f)
        discard;
    return output;
}

PS_OUT PS_COLOR(PS_IN_POS_TEX input)
{
    PS_OUT output;
    float2 uv = input.vUV;
    // Flip X
    if (g_iFlip == 1)
        uv.x = 1.0f - uv.x;
    // Flip Y
    else if (g_iFlip == 2)   
        uv.y = 1.0f - uv.y;
    // Flip XY
    else if (g_iFlip == 3)
    {
        uv.x = 1.0f - uv.x;
        uv.y = 1.0f - uv.y;
    }
    
    vector vColor = g_DefaultTextures[DEFAULT].Sample(PointSampler, uv);
    
    if (vColor.a < 0.3f)
        discard;
    
    float t = 0.f;
    if (g_iFillDir == 0)                // Color Tint 가 왼쪽 / Gradiate Color Tint 가 오른쪽
        t = saturate(input.vUV.x);
    else if (g_iFillDir == 1)
        t = saturate(1.f - input.vUV.x);
    else if (g_iFillDir == 2)
        t = saturate(input.vUV.y);
    else if (g_iFillDir == 3)
        t = saturate(1.f - input.vUV.y);
    
    vColor.rgb = lerp(g_vColorTint, g_vGradiateColorTint, t).rgb;
    vColor.a *= g_fAlphaRatio;
    output.vColor = vColor;
    return output;
}

PS_OUT PS_FADE(PS_IN_POS_TEX input)
{
    PS_OUT output;
    float2 uv = input.vUV;
    // Flip X
    if (g_iFlip == 1)
        uv.x = 1.0f - uv.x;
    // Flip Y
    else if (g_iFlip == 2)   
        uv.y = 1.0f - uv.y;
    // Flip XY
    else if (g_iFlip == 3)
    {
        uv.x = 1.0f - uv.x;
        uv.y = 1.0f - uv.y;
    }
    
    vector vColor = g_DefaultTextures[0].Sample(PointSampler, uv);
    if(g_iColor == 1)
        vColor.rgb = g_vColorTint;
    
    vColor.a *= g_fAlphaRatio;
    output.vColor = vColor;
    return output;
}

PS_OUT PS_PROGRESS(PS_IN_POS_TEX input)
{
    PS_OUT output;
    float2 uv = input.vUV;
    float mask = 1.0f;
    
       // Flip X
    if (g_iFlip == 1)
        uv.x = 1.0f - uv.x;
    // Flip Y
    else if (g_iFlip == 2)   
        uv.y = 1.0f - uv.y;
    // Flip XY
    else if (g_iFlip == 3)
    {
        uv.x = 1.0f - uv.x;
        uv.y = 1.0f - uv.y;
    }
    
    vector vColor = g_DefaultTextures[0].Sample(PointSampler, uv);
    if(vColor.a < 0.3f)
        discard;
    
    if (g_iColor == 1)
        vColor = g_vColorTint;
    
    output.vColor = vColor;
    
    if (g_iFillDir == 0) 
        mask = step(uv.x, g_fProgressRatio);        //right -> left
    else if (g_iFillDir == 1)
        mask = step(1.0f - uv.x, g_fProgressRatio); //left -> right
    else if (g_iFillDir == 2)
        mask = step(1.0f - uv.y, g_fProgressRatio); //up -> down
    else if (g_iFillDir == 3)
        mask = step(uv.y, g_fProgressRatio); //down -> up
    else if (g_iFillDir == 4)
    {
        float dist = abs(uv.x - 0.5f);
        float haf = g_fProgressRatio * 0.5f;
        mask = step(dist, haf);
    }
    
    if (mask <= 0.0f)
        discard;
    
    return output;
}


PS_OUT PS_DISOLVE(PS_IN_POS_TEX input)
{
    PS_OUT output;

    float4 base = g_DefaultTextures[DEFAULT].Sample(LinearSampler, input.vUV);
    float noise = g_DefaultTextures[UI_NOISE].Sample(LinearSampler, input.vUV).r;
    float Alpha = g_DefaultTextures[UI_ALPHA_MASK].Sample(LinearSampler, input.vUV).a;

    if (g_iColor == 1)
        base.rgb = g_vColorTint.rgb;
    
    float a = smoothstep(g_fProgressRatio, g_fProgressRatio + 0.1f, noise);
    output.vColor = base;
    output.vColor.a *= a;
    output.vColor.a *= Alpha;
    return output;
}

technique11 T0
{
    PASS_RS_DS_BS_VP(Default,       RS_Default, DS_Disabled, BS_Default, VS_MAIN, PS_MAIN)
    PASS_RS_DS_BS_VP(DefaultAlpha,  RS_Default, DS_Disabled, BS_AlphaBlend, VS_MAIN, PS_MAIN)
    PASS_RS_DS_BS_VP(Color,         RS_Default, DS_Disabled, BS_AlphaBlend, VS_MAIN, PS_COLOR)
    PASS_RS_DS_BS_VP(Fade,          RS_Default, DS_Disabled, BS_AlphaBlend, VS_MAIN, PS_FADE)
    PASS_RS_DS_BS_VP(Progress,      RS_Default, DS_Disabled, BS_AlphaBlend, VS_MAIN, PS_PROGRESS)
    PASS_RS_DS_BS_VP(Disolve,       RS_Default, DS_Disabled, BS_AlphaBlend, VS_MAIN, PS_DISOLVE)
};