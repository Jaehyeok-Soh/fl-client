#include "Struct_Defines.hlsl"
#include "Animation_Defines.hlsl"
#include "Light_Defines.hlsl"

#define DEFAULTTEXTURE 0 // 기본 텍스처
#define NOISETEXTURE 1
#define MASKINGTEXTURE 2
#define GRADATIONTEXTURE 3

struct EffectDesc
{
    uint g_TextureFlags;
    float3 vPadding;
    
    uint g_RenderFlags;       // 
    float3 vPadding1;
    
    float2 g_ScrollOffset;
    float2 g_DistortionScale;
    float4 g_EffectColor;
};

cbuffer ConstantBuffer_Effect
{
    EffectDesc g_Effect;
};

// ========  Render Flags  =========
bool HasBillboard()
{
    return (g_Effect.g_RenderFlags & 1) != 0;
}

// ========  Texture Flags  ==========

bool HasDefaultTexture()
{
    return (g_Effect.g_TextureFlags & 1) != 0;
}

bool HasNoiseTexture()
{
    return (g_Effect.g_TextureFlags & 2) != 0;
}

bool HasMaskTexture()
{
    return (g_Effect.g_TextureFlags & 4) != 0;
}

bool HasGradationTexture()
{
    return (g_Effect.g_TextureFlags & 8) != 0;

}

// ========== Texture Sampling =============
float4 DefaultTextureSample(float2 UV)
{
    return g_DefaultTextures[DEFAULTTEXTURE].Sample(LinearSampler, UV);
}

float4 NoiseTextureSample(float2 UV)
{
    return g_DefaultTextures[NOISETEXTURE].Sample(LinearSampler, UV);
}

float4 MaskTextureSample(float2 UV)
{
    return g_DefaultTextures[MASKINGTEXTURE].Sample(LinearSampler, UV);
}

float4 GradationTextureSample(float2 UV)
{
    return g_DefaultTextures[GRADATIONTEXTURE].Sample(LinearSampler, UV);
}

// =========== VS In  ==============


VS_OUT_POS_GS_PARTICLE VS_Texture(VS_IN_POS_GS_PARTICLE In)
{
    VS_OUT_POS_GS_PARTICLE Out;
    
    vector vPosition = mul(vector(In.vPosition, 1.f), In.matTransform);
    

    Out.vPosition = mul(vPosition, W);
    Out.vPSize = float2(length(In.matTransform._11_12_13), length(In.matTransform._21_22_23));
    Out.vLifeTime = In.vLifeTime;
    
    return Out;
}

[maxvertexcount(6)]
void GS_Texture(point VS_OUT_POS_GS_PARTICLE In[1], inout TriangleStream<GS_OUT_POS_PARTICLE> OutStream)
{
    GS_OUT_POS_PARTICLE Out[4];
    
    // =========        빌보드 계산          ==============
    float3 vRight = float3(1.f, 0.f, 0.f) * In[0].vPSize.x;
    float3 vUp = float3(0.f, 1.f, 0.f) * In[0].vPSize.y;
    matrix matVP = mul(V, P);
    
    if (HasBillboard())
    {
        float3 vLook = normalize(CameraPosition() - In[0].vPosition.xyz);
        vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * In[0].vPSize.x;
        vUp = normalize(cross(vLook, vRight)) * In[0].vPSize.y;
    }

    // ========         정점 4개 생성        =============
    float3 vPos[4];
    vPos[0] = In[0].vPosition.xyz + vRight + vUp;
    vPos[1] = In[0].vPosition.xyz - vRight + vUp;
    vPos[2] = In[0].vPosition.xyz - vRight - vUp;
    vPos[3] = In[0].vPosition.xyz + vRight - vUp;

    float2 vUV[4] = { float2(0, 0), float2(1, 0), float2(1, 1), float2(0, 1) };

    for (int i = 0; i < 4; ++i)
    {
        Out[i].vPosition = mul(float4(vPos[i], 1.f), matVP);
        Out[i].vUV = vUV[i];
        Out[i].vLifeTime = In[0].vLifeTime;
    }

    // 삼각형 스트립 출력 (0-1-2, 0-2-3)
    OutStream.Append(Out[0]);
    OutStream.Append(Out[1]);
    OutStream.Append(Out[2]);
    OutStream.RestartStrip();
    OutStream.Append(Out[0]);
    OutStream.Append(Out[2]);
    OutStream.Append(Out[3]);
    OutStream.RestartStrip();
}

float4 PS_Texture(GS_OUT_POS_PARTICLE In) : SV_TARGET0
{
    float2 noiseUV = In.vUV + g_Effect.g_ScrollOffset;
    float4 noiseSample = { 1.f, 1.f, 1.f, 1.f };
    
    if (HasNoiseTexture())
    {
        noiseSample = NoiseTextureSample(noiseUV);
    }
    
    float noiseValue = noiseSample.r;
    float2 finalUV = In.vUV;
    
    finalUV.x += (noiseValue - 0.5f) * g_Effect.g_DistortionScale.x;
    finalUV.y += (noiseValue - 0.5f) * g_Effect.g_DistortionScale.y;

    // =========== Diffuse Texture 여부 =============
    float4 DiffuseColor = float4(1.f, 1.f, 1.f, 1.f);
    
    if (HasDiffuse())
    {
        DiffuseColor = DefaultTextureSample(finalUV);
    }
    
    float finalAlpha = DiffuseColor.a * noiseValue;
    
    // ===========  라이프타임에 따른 투명도 적용  =============
    float LifeRatio = saturate(1.0f - (In.vLifeTime.x / In.vLifeTime.y));
    finalAlpha *= LifeRatio;
    if (finalAlpha < 0.05f)
        discard;
    
    return float4(DiffuseColor.rgb * g_Effect.g_EffectColor.rgb * 1.0f, finalAlpha);
}


technique11 T0
{
    pass Texture_Effect
    {
        SetRasterizerState(RS_Default_CullNone);
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetVertexShader(CompileShader(vs_5_0, VS_Texture()));
        SetGeometryShader(CompileShader(gs_5_0, GS_Texture()));
        SetPixelShader(CompileShader(ps_5_0, PS_Texture()));
    }
}