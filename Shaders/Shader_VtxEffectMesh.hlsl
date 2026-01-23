#include "Struct_Defines.hlsl"
#include "Animation_Defines.hlsl"
#include "Light_Defines.hlsl"

#define DEFAULTTEXTURE 0 // 기본 텍스처
#define NOISETEXTURE 1

struct EffectDesc
{
    uint   g_Flags;
    float2 g_ScrollOffset;
    float2 g_DistortionScale;
    float4 g_EffectColor;
};

cbuffer ConstanntBuffer_Effect
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

struct VS_IN
{
    float3 vLocalPos : POSITION;
    row_major float4x4 TransformaionMatrix : INST_WORLD;
    float2 vLifeTime : INST_TEXCOORD0;
};

struct VS_OUT
{
    float4 vWorldPos : POSITION;
    float2 vPSize : PSIZE;
    float2 vLifeTime : TEXCOORD0;
};

VS_OUT VS_Texture(VS_IN In)
{
    VS_OUT Out;
    
    Out.vWorldPos = mul(float4(0.f, 0.f, 0.f, 1.f), W);
    
    // ============   크기 : World 행렬에서 스케일 값 정확히 추출   =============
    Out.vPSize.x = length(W._11_12_13);
    Out.vPSize.y = length(W._21_22_23);
    
    Out.vLifeTime = In.vLifeTime;
    
    return Out;
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

struct GS_OUT
{
    float4 vPosition : SV_Position;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
};

[maxvertexcount(6)]
void GS_Texture(point VS_OUT In[1], inout TriangleStream<GS_OUT> OutStream)
{
    GS_OUT Out[4];
    
    // =========        빌보드 계산          ==============
    float3 vLook = normalize(CameraPosition() - In[0].vWorldPos.xyz);
    float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * In[0].vPSize.x;
    float3 vUp = normalize(cross(vLook, vRight)) * In[0].vPSize.y;
    
    matrix matVP = mul(V, P);
    
    // ========         정점 4개 생성        =============
    float3 vPos[4];
    vPos[0] = In[0].vWorldPos.xyz + vRight + vUp;
    vPos[1] = In[0].vWorldPos.xyz - vRight + vUp;
    vPos[2] = In[0].vWorldPos.xyz - vRight - vUp;
    vPos[3] = In[0].vWorldPos.xyz + vRight - vUp;

    float2 vUV[4] = { float2(0, 0), float2(1, 0), float2(1, 1), float2(0, 1) };

    for (int i = 0; i < 4; ++i)
    {
        Out[i].vPosition = mul(float4(vPos[i], 1.f), matVP);
        Out[i].vTexcoord = vUV[i];
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

float4 PS_Texture(GS_OUT In) : SV_TARGET0
{
    float2 noiseUV = In.vTexcoord + g_Effect.g_ScrollOffset;
    float4 noiseSample = { 0.f, 0.f, 0.f, 1.f };
    
    if (HasNoiseTexture())
    {
        noiseSample = NoiseTextureSample(noiseUV);
    }
    float noiseValue = noiseSample.r;
    float2 finalUV = In.vTexcoord;
    
    finalUV.x += (noiseValue - 0.5f) * g_Effect.g_DistortionScale.x;
    finalUV.y += (noiseValue - 0.5f) * g_Effect.g_DistortionScale.y;

    float4 color = DefaultTextureSample(finalUV);
    
    float finalAlpha = color.a * noiseValue;
    
    // ===========  라이프타임에 따른 투명도 적용  =============
    float LifeRatio = saturate(1.0f - (In.vLifeTime.x / In.vLifeTime.y));
    finalAlpha *= LifeRatio;
    
    if (finalAlpha < 0.01f)
        discard;
    
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

    pass Texture_Effect
    {
        SetRasterizerState(RS_Default_CullNone);
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetVertexShader(CompileShader(vs_5_0, VS_Texture()));
        GeometryShader = compile gs_5_0 GS_Texture();
        SetPixelShader(CompileShader(ps_5_0, PS_Texture()));
    }
}