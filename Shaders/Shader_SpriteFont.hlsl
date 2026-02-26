#include "Struct_Defines.hlsl"
#include "Light_Defines.hlsl"

#define kDARK 0.8f
#define kBRIGHT 1.5f

struct CB_FONT_OUTLINE_NOISE
{
    float4 vOutlineColor;       // 아웃라인 색  
    float fOutlineSizePx;       // 외곽선 사이즈 
    float fOutlineStrength;     // 외곽선 Alpha
    float2 vNoiseUVScale;       // Noise 패턴의 크기
    float2 vNoiseUVScroll;      // Noise를 흐르게 하는 Offset
    float fNoiseStrength;       // Noise가 내부 색에 얼마나 영향을 줄지
    float fFillMix;             // Noise랑 원래 글자 색을 얼마나 섞을지
};

cbuffer Constant_Buffer_Fonts : register(b1)
{
    CB_FONT_OUTLINE_NOISE g_FontDesc;
};

Texture2D<float4>   g_FontTexture           : register(t0);
sampler             g_FontTextureSampler    : register(s0);

Texture2D       g_NoiseTex      : register(t1); // 우리가 추가로 바인딩할 노이즈
SamplerState    g_NoiseSamp     : register(s1);

float2 Get_TexelSize()
{
    uint w, h;
    g_FontTexture.GetDimensions(w, h);
    return (1.0 / float2(w, h));
}

float4 PS_OUTLINE_NOISE(float4 color : COLOR0, float2 texCoord : TEXCOORD0) : SV_Target0
{
    // --- Glyph coverage ---
    float4 vGlyph   = g_FontTexture.Sample(g_FontTextureSampler, texCoord);
    float fCoverage = vGlyph.a;

    // --- Noise ---
    float2 vNoiseUV = texCoord * g_FontDesc.vNoiseUVScale + g_FontDesc.vNoiseUVScroll;
    float fNoise    = g_NoiseTex.Sample(g_NoiseSamp, vNoiseUV).r;
    fNoise          = saturate((fNoise - 0.5f) * g_FontDesc.fNoiseStrength + 0.5f);
    fNoise = smoothstep(0.25f, 0.75f, fNoise);
    
    // --- Fill color mod (pre-multiply later) ---
    float3 vBaseRGB = color.rgb;
    float vBaseA    = color.a;

    float k = lerp(kDARK, kBRIGHT, fNoise);
    float3 vModRGB = vBaseRGB * k;
    float3 vFillRGB = lerp(vBaseRGB, vModRGB, g_FontDesc.fFillMix);
    float fFillA = vBaseA * fCoverage;
    float4 vFinalPMColor = float4(vFillRGB * fFillA, fFillA);

    // --- Outline mask (8 taps) ---
    float outlineMask = 0.0f;
    if (g_FontDesc.fOutlineSizePx > 0.0f && g_FontDesc.fOutlineStrength > 0.0f && g_FontDesc.vOutlineColor.a > 0.0f)
    {
        float2 texel = Get_TexelSize() * g_FontDesc.fOutlineSizePx;

        float a0 = g_FontTexture.Sample(g_FontTextureSampler, texCoord + float2(texel.x, 0)).a;
        float a1 = g_FontTexture.Sample(g_FontTextureSampler, texCoord + float2(-texel.x, 0)).a;
        float a2 = g_FontTexture.Sample(g_FontTextureSampler, texCoord + float2(0, texel.y)).a;
        float a3 = g_FontTexture.Sample(g_FontTextureSampler, texCoord + float2(0, -texel.y)).a;

        float a4 = g_FontTexture.Sample(g_FontTextureSampler, texCoord + float2(texel.x, texel.y)).a;
        float a5 = g_FontTexture.Sample(g_FontTextureSampler, texCoord + float2(-texel.x, texel.y)).a;
        float a6 = g_FontTexture.Sample(g_FontTextureSampler, texCoord + float2(texel.x, -texel.y)).a;
        float a7 = g_FontTexture.Sample(g_FontTextureSampler, texCoord + float2(-texel.x, -texel.y)).a;

        float aN = max(max(a0, a1), max(a2, a3));
        aN = max(aN, max(max(a4, a5), max(a6, a7)));

        outlineMask = saturate(aN - fCoverage);
        outlineMask = smoothstep(0.0f, 1.0f, outlineMask);
    }

    float outlineA = outlineMask * g_FontDesc.fOutlineStrength * g_FontDesc.vOutlineColor.a * vBaseA;
    float3 outlineRGB = g_FontDesc.vOutlineColor.rgb;
    float4 outlinePM = float4(outlineRGB * outlineA, outlineA);
    float4 vResult = vFinalPMColor + outlinePM * (1.0f - vFinalPMColor.a);
    return vResult;
}

float4 PS_OUTLINE_GRADATION(float4 color : COLOR0, float2 texCoord : TEXCOORD0) : SV_Target0
{
    float4 vGlyph = g_FontTexture.Sample(g_FontTextureSampler, texCoord);
    float fCoverage = vGlyph.a;
    
    g_FontDesc.vNoiseUVScale;

    float3 vBaseRGB = color.rgb;
    float vBaseA = color.a;

    // RGB만 그라데이션, A는 고정
    float3 gradRGB = lerp(float3(1.f, 1.f, 1.f), vBaseRGB, texCoord.y);

    // Fill (premultiplied)
    float fillA = fCoverage * vBaseA;
    float4 fillPM = float4(gradRGB * fillA, fillA);

    float outlineMask = 0.0f;
    if (g_FontDesc.fOutlineSizePx > 0.0f && g_FontDesc.fOutlineStrength > 0.0f && g_FontDesc.vOutlineColor.a > 0.0f)
    {
        float2 texel = Get_TexelSize() * g_FontDesc.fOutlineSizePx;

        float a0 = g_FontTexture.Sample(g_FontTextureSampler, texCoord + float2(texel.x, 0)).a;
        float a1 = g_FontTexture.Sample(g_FontTextureSampler, texCoord + float2(-texel.x, 0)).a;
        float a2 = g_FontTexture.Sample(g_FontTextureSampler, texCoord + float2(0, texel.y)).a;
        float a3 = g_FontTexture.Sample(g_FontTextureSampler, texCoord + float2(0, -texel.y)).a;

        float a4 = g_FontTexture.Sample(g_FontTextureSampler, texCoord + float2(texel.x, texel.y)).a;
        float a5 = g_FontTexture.Sample(g_FontTextureSampler, texCoord + float2(-texel.x, texel.y)).a;
        float a6 = g_FontTexture.Sample(g_FontTextureSampler, texCoord + float2(texel.x, -texel.y)).a;
        float a7 = g_FontTexture.Sample(g_FontTextureSampler, texCoord + float2(-texel.x, -texel.y)).a;

        float aN = max(max(a0, a1), max(a2, a3));
        aN = max(aN, max(max(a4, a5), max(a6, a7)));

        outlineMask = saturate(aN - fCoverage);
        outlineMask = smoothstep(0.0f, 1.0f, outlineMask);
    }

    float outlineA = outlineMask * g_FontDesc.fOutlineStrength * g_FontDesc.vOutlineColor.a * vBaseA;
    float3 outlineRGB = g_FontDesc.vOutlineColor.rgb;
    float4 outlinePM = float4(outlineRGB * outlineA, outlineA);

    // outlineMask가 이미 외곽만 잡으므로 그냥 합산
    return outlinePM + fillPM;
}