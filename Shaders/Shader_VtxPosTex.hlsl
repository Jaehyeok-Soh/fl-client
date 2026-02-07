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

PS_OUT PS_FADE(PS_IN_POS_TEX input)
{
    PS_OUT output;
    output.vColor = g_DefaultTextures[DEFAULT].Sample(LinearSampler, input.vUV);
    output.vColor.a *= g_AlphaRatio;
    return output;
}

PS_OUT PS_PROGRESS(PS_IN_POS_TEX input)
{
    PS_OUT output;
    output.vColor = g_DefaultTextures[DEFAULT].Sample(LinearSampler, input.vUV * g_ProgressRatio);
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

PS_OUT PS_UIHPBAR(PS_IN_POS_TEX input)
{
    PS_OUT output;
    float4 vTexture = g_DefaultTextures[0].Sample(LinearSampler, input.vUV);
    if (ALPHA_TEST(vTexture.a, 0.2f))
        discard;
    
    float fNewUVX = (input.vUV.x - g_fU0) / (g_fU1 - g_fU0);
    float fFill = step(fNewUVX, input.fHP);
    float4 vFillColor = { 0.1f, 0.7f, 0.1f, 1.f };
    float4 vEmptyColor = { 0.6f, 0.3f, 0.1f, 1.f };
    output.vColor = lerp(vEmptyColor, vFillColor, fFill);
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
    PASS_RS_DS_BS_VP(P1, RS_Default, DS_Default, BS_AlphaBlend, VS_MAIN, PS_MAIN)
    PASS_RS_DS_BS_VP(Fade, RS_Default, DS_Default, BS_AlphaBlend, VS_MAIN, PS_FADE)
    PASS_RS_DS_BS_VP(Progress, RS_Default, DS_Default, BS_AlphaBlend, VS_MAIN, PS_PROGRESS)

    PASS_RS_DS_BS_VP(SkillIcon, RS_Default, DS_Default, BS_Default, VS_MAIN, PS_SKILLICON)
    PASS_RS_DS_BS_VP(UIHpBar, RS_Default, DS_Default, BS_Default, VS_MAIN, PS_UIHPBAR)
    PASS_RS_DS_BS_VP(UILockon, RS_Default, DS_Default, BS_AlphaBlend, VS_MAIN, PS_LOCKON)
};