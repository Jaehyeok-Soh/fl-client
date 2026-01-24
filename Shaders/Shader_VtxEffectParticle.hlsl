#include "Struct_Defines.hlsl"
#include "Light_Defines.hlsl"

#define DEFAULTTEXTURE 0 // 기본 텍스처
#define NOISETEXTURE 1

struct EffectDesc
{
    uint g_Flags;
    float2 g_ScrollOffset;
    float2 g_DistortionScale;
    float4 g_EffectColor;
};

cbuffer ConstantBuffer_Effect
{
    EffectDesc g_Effect;
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

VS_OUT_POS_GS_PARTICLE VS_Particle(VS_IN_POS_GS_PARTICLE In)
{
    VS_OUT_POS_GS_PARTICLE Out;
    
    Out.vPosition = mul(float4(0.f, 0.f, 0.f, 1.f), W);
    
    // ============   크기 : World 행렬에서 스케일 값 정확히 추출   =============
    Out.vPSize.x = length(W._11_12_13);
    Out.vPSize.y = length(W._21_22_23);
    
    Out.vLifeTime = In.vLifeTime;
    
    return Out;
}

[maxvertexcount(6)]
void GS_Particle(point VS_OUT_POS_GS_PARTICLE In[1], inout TriangleStream<GS_OUT_POS_PARTICLE> OutStream)
{
    GS_OUT_POS_PARTICLE Out[4];
    
    // =========        빌보드 계산          ==============
    float3 vLook = normalize(CameraPosition() - In[0].vPosition.xyz);
    float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * In[0].vPSize.x;
    float3 vUp = normalize(cross(vLook, vRight)) * In[0].vPSize.y;
    
    matrix matVP = mul(V, P);
    
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
    color.a *= saturate(In.vLifeTime.y - In.vLifeTime.x);
    
    return color;
}


technique11 T0
{
    pass ParticleEffect
    {
        SetRasterizerState(RS_Default_CullNone);
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetVertexShader(CompileShader(vs_5_0, VS_Particle()));
        GeometryShader = compile gs_5_0 GS_Particle();
        SetPixelShader(CompileShader(ps_5_0, PS_Particle()));
    }
}