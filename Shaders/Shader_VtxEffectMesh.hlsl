#include "Struct_Defines.hlsl"
#include "Animation_Defines.hlsl"
#include "Light_Defines.hlsl"

#define DEFAULTTEXTURE 0 // 기본 텍스처
#define NOISETEXTURE 1

struct EffectDesc
{
    uint   g_Flags;
    float3 vPadding;
    float2 g_ScrollOffset;
    float2 g_DistortionScale;
    float4 g_EffectColor;
};

cbuffer ConstantBuffer_Effect
{
    EffectDesc  g_Effect;
};

bool HasNoiseTexture()
{
    return (g_Effect.g_Flags & 1) != 0;
}

float4 NoiseTextureSample(float2 UV)
{
    return g_DefaultTextures[NOISETEXTURE].Sample(LinearSampler, UV);
}

float4 DefaultTextureSample(float2 UV)
{
    return g_DefaultTextures[DEFAULTTEXTURE].Sample(LinearSampler, UV);

}

VS_OUT_MESH VS_EffectMesh(VS_IN_MESH In)
{
    VS_OUT_MESH Out;
    
    // ========                 위치 변환                  =============
    float4 vWorldPos = mul(float4(In.vPosition, 1.f), W);
    
    Out.vWorldPos = vWorldPos;
    Out.vPosition = mul(vWorldPos, V);
    Out.vPosition = mul(vWorldPos, P);
    
    // =======   ProjPos가 필요하다면 사용 (픽셀 셰이더에서 사용) =========
    Out.vProjPos = Out.vPosition;
    
    // =======               Normal 변환                    ===========
    Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), W));
    
    // =======              Texture 좌표 전달               ============
    Out.vUV = In.vUV;
    
    return Out;
}

float4 PS_EffectMesh(VS_OUT_MESH In) : SV_Target0
{
    // =======              노이즈 계산                     ===========
    float2 noiseUV = In.vUV + g_Effect.g_ScrollOffset;
    
    // =======              노이즈 텍스처 샘플링             ===========
    float4 noiseSample = {0.f, 0.f, 0.f, 1.f};
    
    if (HasNoiseTexture())
    {
        noiseSample = NoiseTextureSample(noiseUV);
    }
    float noiseValue = noiseSample.r;
    
    //=======               디스토션 (왜곡) 로직            ===========
    float2 finalUV = In.vUV;
    
    // (NoiseValue - 0.5f) -> -0.5 ~ 0.5 범위로 흔들기
    finalUV.x += (noiseValue - 0.5f) * g_Effect.g_DistortionScale.x;
    finalUV.y += (noiseValue - 0.5f) * g_Effect.g_DistortionScale.y;
    
    // =======      디스토션 처리된 UV로 Diffuse Texture 샘플링 =======
    float4 color = DefaultTextureSample(finalUV);
    
    // ==========               알파 마스킹                   =========
    float finalAlpha = color.a * noiseValue;
    
    // ==========               알파 클리핑                   =========
    if (finalAlpha < 0.01f)
        discard;
    
    // ==========   미리 지정한 이펙트 색깔을 곱해서 출력하기   =========
    return float4(color.rgb * g_Effect.g_EffectColor.rgb * 3.0f, finalAlpha);
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