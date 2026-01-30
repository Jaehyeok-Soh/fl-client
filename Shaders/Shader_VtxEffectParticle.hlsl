#include "Struct_Defines.hlsl"
#include "Animation_Defines.hlsl"
#include "Light_Defines.hlsl"

// Texture Flag
#define DEFAULTTEXTURE 0 // 기본 텍스처
#define NOISETEXTURE 1
#define MASKINGTEXTURE 2
#define GRADATIONTEXTURE 3
#define TRAILTEXTURE 4
#define NORMALTEXTURE 5


// Render Flag
#define BILLBOARD 1 << 0
#define SCROLL 1 << 1
    
    // Scroll Direction
#define RIGHT 1 << 2 // LEFT -> RIGHT 이펙트 방향
#define DOWN 1 << 3     // DOWN -> UP

    // Use Sprite
#define SPRITE 1<< 5    // 스프라이트를 사용하는가?
        

// SamplerState Flag
#define LINEARSAMPLER 1 << 0
#define CLAMP 1 << 1
#define BORDER 1 << 2
#define MIRROR 1 << 3
#define POINT 1 << 4

// Roataion Flag
#define ROTATE_DEFAULT      (1 << 0)
#define ROTATE_NOISE        (1 << 1)
#define ROTATE_MASK         (1 << 2)
#define ROTATE_GRADATION    (1 << 3)
#define ROTATE_TRAIL        (1 << 4)
#define ROTATE_NORMAL       (1 << 5)

// 산술 연산자 
#define ADD 1 << 0      // Add          더하기
#define SUB 1 << 1      // Substract    빼기
#define MUL 1 << 2      // Multiply     곱하기
#define DIV 1 << 3      // Devide       나누기

struct EffectDesc
{
    // Texture 바인딩 Flag들
    uint g_TextureFlags;
    // 기타 Render 설정값들
    uint g_RenderFlags;
     // 특정 텍스처마다 먹일 SamplerState
    uint g_StateFlags;
    // discard 먹일 값 바인딩
    float g_DiscardValue;
    
    // 텍스처 산술 연산자 flag 
    uint g_OperatorFlags;
    uint g_RotationFlags;
    float2 g_Padding1;
    
    // sprite일 때
    uint g_SpriteCol; // 가로 프레임 수
    uint g_SpriteRow; // 세로 프레임 수
    uint g_CurSpriteIndex; // 현재 재생 중인 인덱스
    float g_SpritePadding;
    
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
    return (g_Effect.g_RenderFlags & BILLBOARD) != 0;
}

bool HasScroll()
{
    return (g_Effect.g_RenderFlags & SCROLL) != 0;
}

bool HasSprite()
{
    return (g_Effect.g_RenderFlags & SPRITE) != 0;
}

// ======== 연산용 함수들 ============

float2 ScrollUV_Calculator(uint Flag, float2 InUV)
{
    float2 finalUV = InUV;
    float2 offset = g_Effect.g_ScrollOffset;

    // X축 처리
    if (Flag & RIGHT)
    {
        finalUV.x = 1.0f - InUV.x;
        finalUV.x -= offset.x; // 반전축이므로 -= 가 정방향 흐름
    }
    else
    {
        finalUV.x += offset.x; // LEFT나 일반 상황
    }

    // Y축 처리
    if (Flag & DOWN)
    {
        finalUV.y = 1.0f - InUV.y;
        finalUV.y -= offset.y; // 반전축이므로 -= 가 정방향 흐름
    }
    else
    {
        finalUV.y += offset.y; // UP이나 일반 상황
    }

    return finalUV;
}

float Float_Operation(float Src1, float Src2, uint Operator)        // 부동 소수점 연산
{
    float dest;
    
    switch (Operator)
    {
        case ADD:
            dest = Src1 + Src2;
            break;
        case SUB:
            dest = Src1 - Src2;
            break;
        case MUL:
            dest = Src1 * Src2;
            break;
        case DIV:
            dest = Src1 / Src2;
            break;
        
        default:
            dest = Src1 - Src2;
            break;
    }
    
    return dest;
}

float2 RotateUV(float2 uv, float angle, float2 center = float2(0.5, 0.5f))
{
    if (angle == 0.0f)
        return uv;
    
    // 회전의 중심축을 (0,0)으로 이동
    uv -= center;
    
    // 회전 행렬 적용
    float s = sin(angle);
    float c = cos(angle);
    
    float2 rotatedUV;
    rotatedUV.x = uv.x * c - uv.y * s;
    rotatedUV.y = uv.x * s + uv.y * c;
    
    // 원래 위치로 복귀
    rotatedUV += center;
    
    return rotatedUV;
}

float2 Get90DegreeRotatedUV(float2 InUV, uint PackedFlags, uint TextureIndex)
{
    // 해당 텍스처의 4비트씩 추출한다.
    uint rotValue = (PackedFlags >> (TextureIndex * 4)) & 0xF;
    
    // 0도면 바로 리턴
    if (rotValue == 0 || rotValue > 3)
        return InUV;
    
    float2 uv = InUV - 0.5f;
    float2 rotated = uv;
    
    // 1: 90  // 2: 180  // 3: 270
    if (rotValue == 1)
    {
        rotated.x = -uv.y;
        rotated.y = uv.x;
    }
    else if (rotValue == 2)
    {
        rotated.x = -uv.x;
        rotated.y = -uv.y;
    }
    else if (rotValue == 3)
    {
        rotated.x = uv.y;
        rotated.y = -uv.x;
    }
    
    return rotated + 0.5f;
}

// ======== 사진 Rotation 함수들 =======

// ========  Texture Flags  ==========

bool HasDefaultTexture()
{
    return (g_Effect.g_TextureFlags & (1 << 0)) != 0;
}

bool HasNoiseTexture()
{
    return (g_Effect.g_TextureFlags & (1 << 1)) != 0;
}

bool HasMaskTexture()
{
    return (g_Effect.g_TextureFlags & (1 << 2)) != 0;
}

bool HasGradationTexture()
{
    return (g_Effect.g_TextureFlags & (1 << 3)) != 0;
}

bool HasTrailTexture()
{
    return (g_Effect.g_TextureFlags & (1 << 4)) != 0;

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

float4 TrailTextureSample(float2 UV)
{
    return SampleTextureWithFlags(g_DefaultTextures[TRAILTEXTURE], g_Effect.g_StateFlags, 12, UV);
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
    
    if (color.a < g_Effect.g_DiscardValue)
        discard;
    
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