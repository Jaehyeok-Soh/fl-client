#include "Struct_Defines.hlsl"
#include "Animation_Defines.hlsl"
#include "Light_Defines.hlsl"

// Texture Flag
#define DEFAULTTEXTURE 0 // 기본 텍스처
#define NOISETEXTURE 1
#define MASKINGTEXTURE 2
#define GRADATIONTEXTURE 3
#define CURVETEXTURE 4
#define NORMALTEXTURE 5
#define GLOWTEXTURE 6
#define DISSOLVETEXTURE 7
#define SUBMASKINGTEXTURE 8

texture2D g_EffectTexture;

// Render Flag
#define BILLBOARD 1 << 0
#define SCROLL 1 << 1
    
    // Scroll Direction
#define RIGHT 1 << 2 // LEFT -> RIGHT 이펙트 방향
#define DOWN 1 << 3     // DOWN -> UP

// 진행 방향으로 y를 돌려버리는 플래그
#define DIR_BILLBOARD 1 << 4
    // Use Sprite
#define SPRITE 1<< 5    // 스프라이트를 사용하는가?  이제 이거 폐기.

    // Use Scroll (텍스처별)
#define SCROLL_DIFFUSE 1 << 6
#define SCROLL_NOISE 1 << 7
#define SCROLL_MASKING 1 << 8
#define SCROLL_GRADATION 1 << 9
#define SCROLL_DISSOLVE 1 << 10
#define SCROLL_GLOW 1 << 11
#define SCROLL_CURVE 1 << 12
#define SCROLL_SUBMASKING 1 << 14

#define USE_LIFEDISSOLVE 1 << 13
        
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
#define ROTATE_CURVE        (1 << 4)
#define ROTATE_NORMAL       (1 << 5)
#define ROTATE_GLOW         (1 << 6)

#define ROTATE_SUBMASK      (1 << 8)

// 산술 연산자 
#define ADD 1 << 0      // Add          더하기
#define SUB 1 << 1      // Substract    빼기
#define MUL 1 << 2      // Multiply     곱하기
#define DIV 1 << 3      // Devide       나누기


struct EffectDesc
{
    // Row 0
    uint g_TextureFlags;
    uint g_RenderFlags;
    uint g_StateFlags;
    float g_DiscardValue;

    // Row 1
    uint g_OperatorFlags;
    uint g_RotationFlags;
    float2 g_UVOffset;

    // Row 2
    float2 g_UVPower;
    float g_GlowPower;
    float g_AppearRatio;

    // Row 3
    float2 g_ScrollOffset;
    float2 g_DistortionScale;

    // Row 4
    float4 g_EffectColor; // 여기서부터는 위치가 절대 안 변함

    // Row 5
    float2 DiffuseTexture_ScrollWeight;
    float2 NoiseTexture_ScrollWeight;

    // Row 6
    float2 MaskingTexture_ScrollWeight;
    float2 GradationTexture_ScrollWeight;

    // Row 7
    float2 DissolveTexture_ScrollWeight;
    float2 GlowTexture_ScrollWeight;
    
    // Row 8
    float2 CurveTexture_ScrollWeight;
    float2 SubMaskTexture_ScrollWeight;
    
    // Row 9
    float4 DiffuseTexture_SpriteInfo;
    // Row 10
    float4 NoiseTexture_SpriteInfo;
    // Row 11
    float4 GradationTexture_SpriteInfo;
    // Row 12
    float4 DissolveTexture_SpriteInfo;
    // Row 13
    float4 GlowTexture_SpriteInfo;
    // Row 14
    float4 CurveTexture_SpriteInfo;
    // Row 15
    float4 MaskTexture_SpriteInfo;
    // Row 16
    float4 SubMaskTexture_SpriteInfo;
};

// ========== StruturedBuffer Binding value  ===========  (CS Shader에서 계산해서 넘어온 값.)
StructuredBuffer<VTXPARTICLE> INSTANCE_OUTPUT;

// ========== ConstantBuffer Binding value  ===========
cbuffer ConstantBuffer_Effect
{
    EffectDesc g_Effect;
};

// ========  Render Flags  =========

bool HasBillboard()
{
    return (g_Effect.g_RenderFlags & BILLBOARD) != 0;
}

bool HasTextureScroll(uint Flag)
{
    return (g_Effect.g_RenderFlags & Flag) != 0;
}

bool HasTextureSprite(float4 spriteInfo)
{
    return spriteInfo.x > 0.5f;
}

bool HasLifeDissolve()
{
    return (g_Effect.g_RenderFlags & USE_LIFEDISSOLVE) != 0;
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

float2 GetStaticSpriteUV(float2 InUV, float4 spriteInfo)
{
    // x가 0이면 스프라이트 미사용이므로 원본 UV 반환
    if (spriteInfo.x < 0.5f)
        return InUV;

    float numCols = spriteInfo.y; // 가로 칸 수
    float numRows = spriteInfo.z; // 세로 칸 수
    uint curIdx = (uint) spriteInfo.w; // Cpp에서 계산해서 넘겨준 그려야 할 번호

    // 행/열 위치 계산
    uint xIdx = curIdx % (uint) numCols;
    uint yIdx = curIdx / (uint) numCols;

    // 한 칸의 크기
    float2 cellSize = float2(1.0f / numCols, 1.0f / numRows);
    
    // UV를 한 칸 크기로 줄이고 해당 위치로 오프셋 이동
    float2 spriteUV = InUV * cellSize;
    spriteUV.x += (float) xIdx * cellSize.x;
    spriteUV.y += (float) yIdx * cellSize.y;

    return spriteUV;
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

float4 CurveTextureSample(float2 UV)
{
    return SampleTextureWithFlags(g_DefaultTextures[CURVETEXTURE], g_Effect.g_StateFlags, 12, UV);
}

float4 DissolveTextureSample(float2 UV)
{
    return SampleTextureWithFlags(g_DefaultTextures[DISSOLVETEXTURE], g_Effect.g_StateFlags, 15, UV);
}

float GlowTextureSample(float2 UV)
{
    return SampleTextureWithFlags(g_DefaultTextures[GLOWTEXTURE], g_Effect.g_StateFlags, 18, UV);
}

float SubMaskTextureSample(float2 UV)
{
    return SampleTextureWithFlags(g_DefaultTextures[SUBMASKINGTEXTURE], g_Effect.g_StateFlags, 0, UV);
}

float4 SceneTextureSample(float2 UV, uint Flag)
{
    if (Flag == 0)
        return g_RenderTargetSceneHDRCopyTexture.Sample(LinearSampler, UV);
    else if (Flag == 1)
        return g_RenderTargetSceneHDRCopyTexture.Sample(LinearClampSampler, UV);
}

float McGuire_CalculateWeight(float viewZ, float alpha)
{
    //  Morgan McGuire, Louis Bavoil 공식 그대로 사용. 엔비디아 뭐시기
    float z = abs(viewZ);
    float weight = 0.f;
    
    const float TransitionDistance = 300.0f; // 오픈월드용 함수로 넘어갈 기준 거리.
    const float MaxDistance = 1000.f;       // 이펙트가 가중치 잃는 최대 거리

    // 비교적 가까운 거리의 가중치 계산
    if (z < TransitionDistance)
    {
        // 논문 9번째 함수
        // 가까운 곳에서 중거리까지 색감과 깊이감을 보존한다.
        weight = alpha * max(0.01f, min(3000.0f, 0.03f / (0.00001f + pow(z / 200.0f, 4.0f))));
    }
    
    // 오픈 월드용. 엄청 먼곳의 구름을 표현할 때 좋지 않을까.
    else
    {
        // 논문 10번째 함수
        // 먼 곳의 이펙트들이 화면을 지저분하게 만드는 것을 방지한다.
        float falloff = 1.0f - saturate((z - TransitionDistance) / (MaxDistance - TransitionDistance));

        weight = alpha * max(0.01f, 3000.f * pow(falloff, 3.f));
    }

    return weight;
}

// 2차 베지에 곡선 함수
float3 GetBezierPos(float3 p0, float3 p1, float3 p2, float t)
{
    float invT = 1.f - t;
    return invT * invT * p0 + 2.f * invT * t * p1 + t * t * p2;
}

// 곡선의 접선 계산 
float3 GetBezierTangent(float3 p0, float3 p1, float3 p2, float t)
{
    return normalize(2.f * (1.f - t) * (p1 - p0) + 2.f * t * (p2 - p1));

}