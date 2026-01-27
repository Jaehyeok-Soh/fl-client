#include "Struct_Defines.hlsl"
#include "Animation_Defines.hlsl"
#include "Light_Defines.hlsl"


// Texture Flag
#define DEFAULTTEXTURE 0 // 기본 텍스처
#define NOISETEXTURE 1
#define MASKINGTEXTURE 2
#define GRADATIONTEXTURE 3

// Render Flag
#define BILLBOARD 1
#define SCROLL 2

// SamplerState Flag
#define LINEARSAMPLER 1
#define CLAMP 2
#define BORDER 4
#define MIRROR 8
#define POINT 16

struct EffectDesc
{
    // Texture 바인딩 Flag들
    uint g_TextureFlags;
    float3 vPadding;
    
    // 기타 Render 설정값들
    uint g_RenderFlags;
    float3 vPadding1;
    
     // 특정 텍스처마다 먹일 SamplerState
    uint g_StateFlags;
    float3 vPadding2;
    
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

bool HasScroll()
{
    return (g_Effect.g_RenderFlags & 2) != 0;
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

// ========= SamplerState Flags ===========
    // Diffuse
float4 SampleTextureWithFlags(Texture2D tex, uint flags, uint Shift, float2 uv) // texture라는 이름을 사용하지 못함
{
    // Shift만큼 밀어버리고, 하위 3비트씩만 서로 읽어서 인덱스로 사용.
    uint Index = (flags >> Shift) & 0x7; // 0x7이 하위 3비트
   
    if (Index == LINEARSAMPLER)
        return tex.Sample(LinearSampler, uv);
    else if (Index == CLAMP)
        return tex.Sample(LinearClampSampler, uv);
    else if (Index == BORDER)
        return tex.Sample(LinearBorderSampler, uv);
    else if (Index == MIRROR)
        return tex.Sample(LinearMirrorSampler, uv);
    else if (Index == POINT)
        return tex.Sample(PointSampler, uv);
    else
        return tex.Sample(LinearSampler, uv);
}

// ========== Texture Sampling =============
float4 DefaultTextureSample(float2 UV)
{
    return SampleTextureWithFlags(g_DefaultTextures[DEFAULTTEXTURE], g_Effect.g_StateFlags, 0, UV);
}

float4 NoiseTextureSample(float2 UV)
{
    return SampleTextureWithFlags(g_DefaultTextures[NOISETEXTURE], g_Effect.g_StateFlags, 3, UV);
}

float4 MaskTextureSample(float2 UV)
{
    return SampleTextureWithFlags(g_DefaultTextures[MASKINGTEXTURE], g_Effect.g_StateFlags, 6, UV);
}

float4 GradationTextureSample(float2 UV)
{
    return SampleTextureWithFlags(g_DefaultTextures[GRADATIONTEXTURE], g_Effect.g_StateFlags, 9, UV);
}

// =========== VS In  ==============


VS_OUT_POS_GS_PARTICLE VS_Particle(VS_IN_POS_GS_PARTICLE In)
{
    VS_OUT_POS_GS_PARTICLE Out;
    
    vector vPosition = mul(vector(In.vPosition, 1.f), In.matTransform);
    

    Out.vPosition = mul(vPosition, W);
    Out.vPSize = float2(length(In.matTransform._11_12_13), length(In.matTransform._21_22_23));
    Out.vLifeTime = In.vLifeTime;
    
    return Out;
}

[maxvertexcount(6)]
void GS_Particle(point VS_OUT_POS_GS_PARTICLE In[1], inout TriangleStream<GS_OUT_POS_PARTICLE> OutStream)
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

float4 PS_Particle(GS_OUT_POS_PARTICLE In) : SV_TARGET0
{
    // 클라에서 넘겨준 색상 그대로
    
    vector color = g_Effect.g_EffectColor;

    // ===========  라이프타임에 따른 투명도 적용  =============
    color.a = saturate(In.vLifeTime.y - In.vLifeTime.x);
    return color;
}


technique11 T0
{
    pass ParticleEffect
    {
        SetRasterizerState(RS_Default_CullNone);
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetVertexShader(CompileShader(vs_5_0, VS_Particle()));
        SetGeometryShader(CompileShader(gs_5_0, GS_Particle()));
        SetPixelShader(CompileShader(ps_5_0, PS_Particle()));
    }
}