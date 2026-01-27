#include "Struct_Defines.hlsl"
#include "Animation_Defines.hlsl"
#include "Light_Defines.hlsl"

#define PATTERN 0 // 기본 패턴
#define DISSOLVE_NOISE 1 // 흔들림 / 마스크 / 디졸브
#define DISTORTION_NOISE 2 // UV 왜곡
#define GRADATION 3 // 그라데이션 텍스쳐 / Noise 재매핑
#define GRADATIONMAP 4 // 램프 색깔 입힘

struct SkillEffectDesc
{
    uint iFlags;
    float fIntensity;
    float fDissolveThreshold; // 강조 기준선으로 사용
    float fEdgeWidth; // 부드러움/폭 또는 강조 폭으로 사용
    float2 vUVOffset;
    float2 vGradationMap;
    float2 vPatternScale;
    float2 vPatternOffset;
    float fGradationHeight;
    float3 vPadding;
};

cbuffer SkillEffectBuffer
{
    SkillEffectDesc SkillEffect;
};


bool HasUVScroll()
{
    return (SkillEffect.iFlags & 1) != 0;
}
bool HasDissolve()
{
    return (SkillEffect.iFlags & 2) != 0;
}
bool HasDistortion()
{
    return (SkillEffect.iFlags & 4) != 0;
}
bool HasGradation()
{
    return (SkillEffect.iFlags & 8) != 0;
}

bool HasGradationMap()
{
    return (SkillEffect.iFlags & 16) != 0;
}

float4 SamplePattern(float2 vUV)
{
    vUV = vUV * SkillEffect.vPatternScale + SkillEffect.vPatternOffset;
    return g_DefaultTextures[PATTERN].Sample(LinearSampler, vUV);
}

float4 SampleClampPattern(float2 vUV)
{
    vUV = vUV * SkillEffect.vPatternScale + SkillEffect.vPatternOffset;
    return g_DefaultTextures[PATTERN].Sample(LinearClampSampler, vUV);
}

float SampleDissolveNoise(float2 vUV)
{
    return g_DefaultTextures[DISSOLVE_NOISE].Sample(LinearSampler, vUV).r;
}

float2 SampleDistortionVec(float2 vUV)
{
    // rg(0~1)를 -1 ~ 1로 변환해서 방향 벡터처럼 사용
    float2 rg = g_DefaultTextures[DISTORTION_NOISE].Sample(LinearSampler, vUV).rg; 
    return rg * 2.0f - 1.0f;
}

// 세로 방향만
float SampleGradation_Y(float v01)
{
    return g_DefaultTextures[GRADATION].Sample(LinearClampSampler, float2(0.5f, v01)).r;
}

// 가로 방향만
float SampleGradation_X(float v01)
{
    return g_DefaultTextures[GRADATION].Sample(LinearSampler, float2(v01, 0.5f)).r;
}

float SampleGradation(float2 vUV)
{
    return g_DefaultTextures[GRADATION].Sample(LinearClampSampler, vUV).r;
}

float3 SampleGradationMap(float t01)
{
    float fSlice = (1.f / SkillEffect.vGradationMap.y);
    float fY = SkillEffect.vGradationMap.x * fSlice;
    fY += SkillEffect.fGradationHeight * fSlice;
    return g_DefaultTextures[GRADATIONMAP].Sample(LinearClampSampler, float2(t01, fY)).rgb;
}

VS_OUT_MESH VS_MAIN(VS_IN_MESH input)
{
    VS_OUT_MESH output;
    
    output.vPosition = mul(float4(input.vPosition, 1.f), W);
    output.vPosition = mul(output.vPosition, VP);
    output.vUV = input.vUV;
    output.vNormal = normalize(mul(input.vNormal, (float3x3) W));
    output.vTangent = normalize(mul(input.vTangent, (float3x3) W));
    output.vBinormal = normalize(mul(input.vBinormal, (float3x3) W));
    
    output.vWorldPos = mul(float4(input.vPosition, 1.f), W);
    output.vProjPos = output.vPosition;
    return output;
}

PS_OUT PS_MAIN(PS_IN_MESH input)
{
    PS_OUT output;
    output.vColor = MIDesc.vTintColor;
    return output;
}

PS_OUT PS_FIRE(PS_IN_MESH input)
{
    PS_OUT output;
    float2 vBaseUV = input.vUV;
    float2 vScrolledUV = frac(input.vUV + float2(SkillEffect.vUVOffset.x, SkillEffect.vUVOffset.y));
    float2 vScrolledUV_Y = float2(vBaseUV.x, vScrolledUV.y);
    float2 vScrolledUV_X = float2(vScrolledUV.x, vBaseUV.y);
    
    float fNoiseR = 1.f;
    if (HasDissolve())
        fNoiseR = SampleDissolveNoise(vScrolledUV);
    
    float fPatternR = SamplePattern(vScrolledUV_X).r;
    fNoiseR *= fPatternR;
    
    float fAlpha = 1.f;
    float fGradationR = 0.f;
    if (HasGradation())
    {
        fGradationR = SampleGradation_Y(vBaseUV.y);
        fAlpha = smoothstep(fGradationR, fGradationR + SkillEffect.fEdgeWidth, fNoiseR);
    }
    
    if (ALPHA_TEST(fAlpha, 0.3f))
        discard;
    
    float4 vBaseColor = MIDesc.vTintColor;
    if (HasGradationMap())
    {
        float3 vRamp = SampleGradationMap(fNoiseR);
        vBaseColor.rgb = vRamp;
    }
    vBaseColor.rgb *= fAlpha;
    output.vColor = vBaseColor;
    return output;
}

PS_OUT PS_SPHERE_GRADATION(PS_IN_MESH input)
{
    PS_OUT output;
    float2 vBaseUV = input.vUV;
    float2 vScrolledUV = frac(input.vUV + float2(SkillEffect.vUVOffset.x, SkillEffect.vUVOffset.y));
    float2 vScrolledUV_Y = float2(vBaseUV.x, vScrolledUV.y);
    float2 vScrolledUV_X = float2(vScrolledUV.x, vBaseUV.y);
    
    float fGradationR = 0.01f;
    if (HasGradation())
    {
        fGradationR = SampleGradation_Y(vBaseUV.y);
    }
    
    float4 vBaseColor = MIDesc.vTintColor;
    if (HasGradationMap())
    {
        float3 vRamp = SampleGradationMap(fGradationR);
        vBaseColor.rgb = vRamp;
    }
    output.vColor = vBaseColor;
    return output;
}

PS_OUT PS_RIBON_GRADATION(PS_IN_MESH input)
{
    PS_OUT output;
    float2 vBaseUV = input.vUV;
    float2 vScrolledUV = frac(input.vUV + float2(SkillEffect.vUVOffset.x, SkillEffect.vUVOffset.y));
    float2 vScrolledUV_Y = float2(vBaseUV.x, vScrolledUV.y);
    float2 vScrolledUV_X = float2(vScrolledUV.x, vBaseUV.y);
        
    float fPatternR = SamplePattern(vScrolledUV_X).r;
    
    float fAlpha = 1.f;
    float fGradationR = 0.f;
    if (HasGradation())
    {
        fGradationR = SampleGradation_Y(vBaseUV.y);
        fAlpha = smoothstep(fGradationR, fGradationR + SkillEffect.fEdgeWidth, fPatternR);
    }
    
    if (ALPHA_TEST(fAlpha, 0.3f))
        discard;
    
    float4 vBaseColor = MIDesc.vTintColor;
    if (HasGradationMap())
    {
        float3 vRamp = SampleGradationMap(fPatternR);
        vBaseColor.rgb = vRamp;
    }
    vBaseColor.rgb *= fAlpha;
    output.vColor = vBaseColor;
    return output;
}

PS_OUT PS_WATERPORISON(PS_IN_MESH input)
{
    PS_OUT output;
    float2 vBaseUV = input.vUV;
    float2 vScrolledUV = frac(input.vUV + float2(SkillEffect.vUVOffset.x, SkillEffect.vUVOffset.y));
    float2 vScrolledUV_Y = float2(vBaseUV.x, vScrolledUV.y);
    float2 vScrolledUV_X = float2(vScrolledUV.x, vBaseUV.y);
    
    float fMask = SamplePattern(vScrolledUV).r;
    
    float fGradationR = 0.f;
    if (HasGradation())
    {
        fGradationR = SampleGradation_Y(vBaseUV.y);
    }
    
    float3 vGradColor = HasGradationMap() ? SampleGradationMap(fGradationR) : float3(1.f, 1.f, 1.f);
    
    float3 rgb = lerp(vGradColor, float3(1.f, 1.f, 1.f), fMask);
    float fAlpha = lerp(0.5f, 0.8f, fMask);
    output.vColor = float4(rgb, fAlpha);
    return output; 
}

PS_OUT PS_LIGHTNING(PS_IN_MESH input)
{
    PS_OUT output;
    float2 vBaseUV = input.vUV;
    float2 vScrolledUV = input.vUV + float2(SkillEffect.vUVOffset.x, SkillEffect.vUVOffset.y);
    float2 vScrolledUV_Y = float2(vBaseUV.x, vScrolledUV.y);
    float2 vScrolledUV_X = float2(vScrolledUV.x, vBaseUV.y);
    
    float fShape = SamplePattern(vBaseUV).r;
    
    fShape *= SampleGradation_X(1.f - frac(vScrolledUV).x).r;
    if (ALPHA_TEST(fShape, 0.6f))
        discard;
    
    float3 vRGB = MIDesc.vTintColor.rgb;
    if (HasGradationMap())
    {
        vRGB = SampleGradationMap(fShape);
    }
    
    vRGB *= fShape;
    
    output.vColor = float4(vRGB, 1.f);
    return output;
}

PS_OUT PS_WINDRING(PS_IN_MESH input)
{
    PS_OUT output;
    float2 vBaseUV = input.vUV;
    float2 vScrolledUV = frac(input.vUV + float2(SkillEffect.vUVOffset.x, SkillEffect.vUVOffset.y));
    float2 vScrolledUV_Y = float2(vBaseUV.x, vScrolledUV.y);
    float2 vScrolledUV_X = float2(vScrolledUV.x, vBaseUV.y);
    
    float fNoiseR = 1.f;
    if (HasDissolve())
        fNoiseR = SampleDissolveNoise(vScrolledUV);
    
    float fPatternR = SamplePattern(vScrolledUV_X).r;
    fNoiseR *= fPatternR;
    
    float fAlpha = 1.f;
    float fGradationR = 0.f;
    if (HasGradation())
    {
        fGradationR = SampleGradation_Y(vBaseUV.y);
        fAlpha = smoothstep(fGradationR, fGradationR + SkillEffect.fEdgeWidth, fNoiseR);
    }
    
    if (ALPHA_TEST(fAlpha, 0.3f))
        discard;
    
    float4 vBaseColor = MIDesc.vTintColor;
    if (HasGradationMap())
    {
        float3 vRamp = SampleGradationMap(fNoiseR);
        vBaseColor.rgb *= vRamp;
    }
    vBaseColor.rgb *= fAlpha;
    output.vColor = vBaseColor;
    return output;
}

/*
PS_OUT PS_MAIN(PS_IN_MESH input)
{
    PS_OUT output;
    
    float2 vUV = input.vUV;
    
    // UV
    if (HasDistortion())
    {
        float2 vDV = SampleDistortionVec(vUV + SkillEffect.vUVOffset * 0.5f);
        float fDistortionStrnegth = 0.01f * SkillEffect.fIntensity;
        vUV += vDV * fDistortionStrnegth;
    }
    
    if (HasUVScroll())
        vUV += SkillEffect.vUVOffset;
    
    float4 vBaseColor = SamplePattern(input.vUV);
    vBaseColor.rgb *= SkillEffect.fIntensity;
    
    vBaseColor.rgb *= MIDesc.vTintColor.rgb;
    vBaseColor.a *= MIDesc.vTintColor.a;
    vBaseColor.rgb *= MIDesc.fEmissivePower;
    
    ApplyFlickerAndSoftHighlight(vBaseColor, vUV);
    output.vColor = vBaseColor;
    return output;
}
*/

PS_OUT PS_BLACK(PS_IN_MESH input)
{
    PS_OUT output;
    output.vColor = float4(1.f, 1.f, 1.f, 1.f);
    
    return output;
};

technique11 T0
{
	PASS_RS_DS_BS_VP(P0, RS_Default, DS_Default, BS_Default, VS_MAIN, PS_MAIN)
    PASS_RS_DS_BS_VP(P1, RS_Default_CullNone, DS_Default, BS_Default, VS_MAIN, PS_FIRE)
    PASS_RS_DS_BS_VP(P2, RS_Default, DS_Default, BS_Default, VS_MAIN, PS_SPHERE_GRADATION)
    PASS_RS_DS_BS_VP(P3, RS_Default_CullNone, DS_Default, BS_Default, VS_MAIN, PS_RIBON_GRADATION)
    PASS_RS_DS_BS_VP(P4, RS_Default, DS_ReadOnly, BS_AlphaBlend, VS_MAIN, PS_WATERPORISON)
    PASS_RS_DS_BS_VP(P5, RS_Default_CullNone, DS_Default, BS_Default, VS_MAIN, PS_LIGHTNING)
    PASS_RS_DS_BS_VP(P6, RS_Default_CullNone, DS_Default, BS_Default, VS_MAIN, PS_WINDRING)
	PASS_RS_DS_BS_VP(Debug, RS_Wire, DS_Default, BS_Default, VS_MAIN, PS_BLACK)
};