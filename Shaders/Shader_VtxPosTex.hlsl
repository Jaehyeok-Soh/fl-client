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

float2 ApplyFlip(float2 Uv)
{
    if      (1 == g_iFlip)      {        Uv.x = 1.0f - Uv.x;    } // Flip X
    else if (2 == g_iFlip)      {        Uv.y = 1.0f - Uv.y;    } // Flip Y
    else if (3 == g_iFlip)      {        Uv.x = 1.0f - Uv.x;        Uv.y = 1.0f - Uv.y;    } // Flip XY
    return Uv;
}

float3 SRGBToLinear(float3 c)
{
    return pow(saturate(c), 2.2f);
}

PS_OUT PS_MAIN(PS_IN_POS_TEX input)
{
    PS_OUT output;
    float2 Uv = input.vUV;

    Uv = ApplyFlip(Uv);
    
    vector vBaseColor = g_DefaultTextures[DEFAULT].Sample(PointSampler, Uv);
    
    vBaseColor.a *= g_fAlphaRatio;
    if (vBaseColor.a < 0.001f)
        discard;
    
    vBaseColor.rgb *= g_fBrightness;
    output.vColor = vBaseColor;
    return output;
}


PS_OUT PS_COLOR(PS_IN_POS_TEX input)
{
    PS_OUT output;
    float2 Uv = input.vUV;
 
    Uv = ApplyFlip(Uv);
    
    vector vBaseColor = g_DefaultTextures[DEFAULT].Sample(PointSampler, Uv);
    
    float t = 0.f;
    if      (0 == g_iFillDir) t = saturate(Uv.x);       // Color -> Gradiant Color
    else if (1 == g_iFillDir) t = saturate(1.f - Uv.x); // Gradiant Color -> Color
    else if (2 == g_iFillDir) t = saturate(Uv.y);       
    else if (3 == g_iFillDir) t = saturate(1.f - Uv.y);
    
    float3 c0 = SRGBToLinear(g_vColorTint.rgb);
    float3 c1 = SRGBToLinear(g_vGradiateColorTint.rgb);
    vBaseColor.rgb = lerp(c0, c1, t);
    vBaseColor.rgb *= g_fBrightness;
    
    vBaseColor.a *= g_fAlphaRatio;
    if (vBaseColor.a < 0.001f)
        discard;
    output.vColor = vBaseColor;
    return output;
}

PS_OUT PS_PROGRESS(PS_IN_POS_TEX input)
{
    PS_OUT output;
    float2 Uv = input.vUV;
    float Mask = 1.0f;
 
    Uv = ApplyFlip(Uv);
    
    vector vBaseColor = g_DefaultTextures[DEFAULT].Sample(PointSampler, Uv);
    
    if (1 == g_iColor)
    {
        float t = 0.f;
        if      (0 == g_iFillDir)            t = saturate(Uv.x);     
        else if (1 == g_iFillDir)            t = saturate(1.f - Uv.x);
        else if (2 == g_iFillDir)            t = saturate(Uv.y);
        else if (3 == g_iFillDir)            t = saturate(1.f - Uv.y);
    
        float3 c0 = SRGBToLinear(g_vColorTint.rgb);
        float3 c1 = SRGBToLinear(g_vGradiateColorTint.rgb);
        vBaseColor.rgb = lerp(c0, c1, t);
    }
        
    if      (0 == g_iFillDir)        Mask = step(Uv.x, g_fProgressRatio);           // Left  ->  Right
    else if (1 == g_iFillDir)        Mask = step(1.0f - Uv.x, g_fProgressRatio);    // Right ->  Left
    else if (2 == g_iFillDir)        Mask = step(1.0f - Uv.y, g_fProgressRatio);    // Up    ->  Down
    else if (3 == g_iFillDir)        Mask = step(Uv.y, g_fProgressRatio);           // Down  ->  Up
    if (Mask <= 0.0f)
        discard;
    
    vBaseColor.rgb *= g_fBrightness;
    
    vBaseColor.a *= g_fAlphaRatio;
    if (vBaseColor.a < 0.1f)
        discard;

    output.vColor = vBaseColor;
    return output;
}

PS_OUT PS_DISOLVE(PS_IN_POS_TEX input)
{
    PS_OUT output;
    float2 Uv = input.vUV;
    
    Uv = ApplyFlip(Uv);
    
    float4 vBaseColor = g_DefaultTextures[DEFAULT].Sample(LinearSampler, Uv);
    float Noise = g_DefaultTextures[UI_NOISE].Sample(LinearSampler, Uv).r;
    float Alpha = g_DefaultTextures[UI_ALPHA_MASK].Sample(LinearSampler, Uv).a;

    if (1 == g_iColor)
    {
        float t = 0.f;
        if      (0 == g_iFillDir)            t = saturate(Uv.x);
        else if (1 == g_iFillDir)            t = saturate(1.f - Uv.x);
        else if (2 == g_iFillDir)            t = saturate(Uv.y);
        else if (3 == g_iFillDir)            t = saturate(1.f - Uv.y);
        
        float3 c0 = SRGBToLinear(g_vColorTint.rgb);
        float3 c1 = SRGBToLinear(g_vGradiateColorTint.rgb);
        vBaseColor.rgb = lerp(c0, c1, t);
    }
    float Edge = smoothstep(g_fProgressRatio, g_fProgressRatio + 0.1f, Noise);
    vBaseColor.rgb *= g_fBrightness;
    
    output.vColor = vBaseColor;
    output.vColor.a *= Edge;
    output.vColor.a *= Alpha;
    return output;
}

technique11 T0
{
    PASS_RS_DS_BS_VP(Default,       RS_Default, DS_Disabled, BS_AlphaBlend, VS_MAIN, PS_MAIN)
    PASS_RS_DS_BS_VP(Color,         RS_Default, DS_Disabled, BS_AlphaBlend, VS_MAIN, PS_COLOR)
    PASS_RS_DS_BS_VP(Progress,      RS_Default, DS_Disabled, BS_AlphaBlend, VS_MAIN, PS_PROGRESS)
    PASS_RS_DS_BS_VP(Disolve,       RS_Default, DS_Disabled, BS_AlphaBlend, VS_MAIN, PS_DISOLVE)
};