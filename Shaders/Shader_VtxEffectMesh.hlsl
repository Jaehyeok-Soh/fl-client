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
    EffectDesc  g_Effect;
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
   
    if (Index == LINEARSAMPLER) return tex.Sample(LinearSampler, uv);
    else if (Index == CLAMP) return tex.Sample(LinearClampSampler, uv);
    else if (Index == BORDER) return tex.Sample(LinearBorderSampler, uv);
    else if (Index == MIRROR) return tex.Sample(LinearMirrorSampler, uv);
    else if (Index == POINT) return tex.Sample(PointSampler, uv);
    else return tex.Sample(LinearSampler, uv);
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


VS_OUT_INST_MESH_PARTICLE VS_EffectMesh(VS_IN_INST_MESH_PARTICLE In)
{
    VS_OUT_INST_MESH_PARTICLE Out;

    
    float4 vWorldPos = mul(float4(In.vPosition, 1.f), In.matTransform);
    vWorldPos = mul(float4(vWorldPos), W);
    float4 vViewPos = mul(vWorldPos, V);
    Out.vPosition = mul(vViewPos, P);
    
    Out.vWorldPos = vWorldPos;
    Out.vProjPos = Out.vPosition;
    
    Out.vNormal = normalize(mul(In.vNormal, (float3x3) In.matTransform));
    Out.vTangent = normalize(mul(In.vTangent, (float3x3) In.matTransform));
    Out.vBinormal = normalize(mul(In.vBinormal, (float3x3) In.matTransform));
    
    Out.vUV = In.vUV;
    
    Out.vPSize = float2(length(In.matTransform[0].xyz), length(In.matTransform[1].xyz));
    Out.vLifeTime = In.vLifeTime;
    
    return Out;
}

float4 PS_EffectMesh(VS_OUT_INST_MESH_PARTICLE In) : SV_Target0
{
    // =======              노이즈 계산                     ===========
    float2 noiseUV = In.vUV + g_Effect.g_ScrollOffset;
    
    // =======              노이즈 텍스처 샘플링             ===========
    float4 noiseSample = {0.5f, 1.f, 1.f, 1.f};
    float2 finalUV = In.vUV;
    
    if (HasNoiseTexture())
    {
        noiseSample = NoiseTextureSample(noiseUV);
    }
    float noiseValue = noiseSample.r;
    
    //=======               디스토션 (왜곡) 로직            ===========

    if (HasNoiseTexture())
    {
            // (NoiseValue - 0.5f) -> -0.5 ~ 0.5 범위로 흔들기
        finalUV.x += (noiseValue - 0.5f) * g_Effect.g_DistortionScale.x;
        finalUV.y += (noiseValue - 0.5f) * g_Effect.g_DistortionScale.y;
    }

    else
    {
        finalUV.x += g_Effect.g_ScrollOffset.x;
        finalUV.y += g_Effect.g_ScrollOffset.y;
    }
    
    // =======      디스토션 처리된 UV로 Diffuse Texture 샘플링 =======

    float4 DiffuseSample = { 1.f, 1.f, 1.f, 1.f };
    
    if (HasDefaultTexture())
    {
        DiffuseSample = DefaultTextureSample(finalUV);
    }
    // ==========               알파 마스킹                   =========
    float finalAlpha = DiffuseSample.a * noiseValue;
    
    float lifeAlpha = 1.0f - (In.vLifeTime.x / In.vLifeTime.y);
    finalAlpha *= lifeAlpha;
    
    // ==========               알파 클리핑                   =========
    
    if (finalAlpha < 0.05f)
        discard;
    
    // ==========   미리 지정한 이펙트 색깔을 곱해서 출력하기   =========
    
    return float4(DiffuseSample.rgb * g_Effect.g_EffectColor.rgb, finalAlpha);
}

technique11 T0
{
    pass Mesh_Effect
    {
        SetRasterizerState(RS_Default_CullNone);
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetVertexShader(CompileShader(vs_5_0, VS_EffectMesh()));
        GeometryShader = NULL;
        SetPixelShader(CompileShader(ps_5_0, PS_EffectMesh()));
    }
}