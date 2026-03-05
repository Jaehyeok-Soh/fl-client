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
    float3 Padding0;
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
    float2 Padding1;
    
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

void DecodeDepth(float2 vUV, out float fNDCZ, out float fViewZ)
{
    float4 vDepthDesc = g_RenderTargetDepthTexture.Sample(PointClampSampler, vUV);
    fNDCZ = vDepthDesc.x;
    fViewZ = vDepthDesc.y;
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

float4 SceneTextureSample(float2 UV, uint Flag)
{
    if (Flag == 0)
        return g_RenderTargetSceneHDRCopyTexture.Sample(LinearSampler, UV);
    else if (Flag == 1)
        return g_RenderTargetSceneHDRCopyTexture.Sample(LinearClampSampler, UV);
}

// =========== VS In  ==============

VS_OUT_INST_MESH_PARTICLE
    VS_DEFAULT(VS_IN_INST_MESH_PARTICLE In)
{
    VS_OUT_INST_MESH_PARTICLE Out;

    float4 vWorldPos = mul(float4(In.vPosition, 1.f), INSTANCE_OUTPUT[In.vInstID].matTransform);
    vWorldPos = mul(float4(vWorldPos), W);
    float4 vViewPos = mul(vWorldPos, V);
    Out.vPosition = mul(vViewPos, P);
    
    Out.vWorldPos = vWorldPos;
    Out.vProjPos = Out.vPosition;
    
    Out.vNormal = normalize(mul(In.vNormal, (float3x3) INSTANCE_OUTPUT[In.vInstID].matTransform));
    Out.vTangent = normalize(mul(In.vTangent, (float3x3) INSTANCE_OUTPUT[In.vInstID].matTransform));
    Out.vBinormal = normalize(mul(In.vBinormal, (float3x3) INSTANCE_OUTPUT[In.vInstID].matTransform));
    
    Out.vUV = In.vUV;
    
    Out.vPSize = float2(length(INSTANCE_OUTPUT[In.vInstID].matTransform[0].xyz), length(INSTANCE_OUTPUT[In.vInstID].matTransform[1].xyz));
    Out.vLifeTime = INSTANCE_OUTPUT[In.vInstID].vLifeTime;
    
    return Out;
}

float4 PS_DefaultMesh(VS_OUT_INST_MESH_PARTICLE In) : SV_Target0
{
    if (In.vLifeTime.x < 0.0f)
        discard;
   // =======              노이즈 텍스처 샘플링             ===========
    float2 finalUV = In.vUV;
    
    float4 DiffuseSample = { 1.f, 1.f, 1.f, 1.f };
    float4 noiseSample = { 1.f, 1.f, 1.f, 1.f };
    float4 DetailNoiseSample = { 1.f, 1.f, 1.f, 1.f };
    float4 MaskSample = { 1.f, 1.f, 1.f, 1.f };
    float4 GradationSample = { 1.f, 1.f, 1.f, 1.f };
    float4 GlowSample = { 1.f, 1.f, 1.f, 1.f };
    float4 DissolveSample = { 1.f, 1.f, 1.f, 1.f };
    float4 CurveSample = { 1.f, 1.f, 1.f, 1.f };
    float noiseValue = { 1.f };
    
    // 1. 진행 비율 계산 (AppearRatio: 등장, DissolveProgress: 소멸)
    float LifeRatio = saturate(In.vLifeTime.x / In.vLifeTime.y);
    float AppearRatio = In.vLifeTime.x / (In.vLifeTime.y * g_Effect.g_AppearRatio);
    float DissolveProgress = saturate((LifeRatio - g_Effect.g_AppearRatio) / max(0.001f, 1.0f - g_Effect.g_AppearRatio));
   
     // ================     노이즈 텍스처     ===============

        // 3. 왜곡량(Offset) 계산
    float2 distortionOffset = float2(0.f, 0.f);
    
    if (Has(g_Effect.g_TextureFlags, NOISETEXTURE))
    {
        if (HasTextureScroll(SCROLL_NOISE))
        {
            float2 scrolledUV = In.vUV + g_Effect.g_UVOffset;
            scrolledUV += g_Effect.g_ScrollOffset * g_Effect.NoiseTexture_ScrollWeight;
            
            noiseSample = NoiseTextureSample(Get90DegreeRotatedUV(scrolledUV, g_Effect.g_RotationFlags, NOISETEXTURE));
            
            distortionOffset.x = (noiseSample.r - 0.5f) * g_Effect.g_DistortionScale.x;
            distortionOffset.y = (noiseSample.g - 0.5f) * g_Effect.g_DistortionScale.y;
        }
        else
        {
            noiseSample = NoiseTextureSample(Get90DegreeRotatedUV(In.vUV, g_Effect.g_RotationFlags, NOISETEXTURE));
            
            distortionOffset.x = (noiseSample.r - 0.5f) * g_Effect.g_DistortionScale.x;
            distortionOffset.y = (noiseSample.g - 0.5f) * g_Effect.g_DistortionScale.y;
        }
    }
    else
    {
        noiseSample = float4(1.f, 1.f, 1.f, 1.f);
        distortionOffset = float2(0.f, 0.f);
    }
    
    // 최종 왜곡 UV (왜곡량)
    float2 distortionUV = distortionOffset;
    
    // ================     메인 텍스처      ===============
    if (Has(g_Effect.g_TextureFlags, DEFAULTTEXTURE))
    {
        if (HasTextureSprite(g_Effect.DiffuseTexture_SpriteInfo))
        {
            float2 SpriteUV = GetStaticSpriteUV(In.vUV, g_Effect.DiffuseTexture_SpriteInfo);
            
            if (Has(g_Effect.g_TextureFlags, NOISETEXTURE))
            {
                SpriteUV += distortionUV;
                DiffuseSample = DefaultTextureSample(Get90DegreeRotatedUV(SpriteUV, g_Effect.g_RotationFlags, DEFAULTTEXTURE));
            }
            else
            {
                DiffuseSample = DefaultTextureSample(Get90DegreeRotatedUV(SpriteUV, g_Effect.g_RotationFlags, DEFAULTTEXTURE));
            }
        }
        else if (HasTextureScroll(SCROLL_DIFFUSE))
        {
            if (Has(g_Effect.g_TextureFlags, NOISETEXTURE))
            {
                float2 scrolledUV = In.vUV + g_Effect.g_UVOffset;
                scrolledUV += g_Effect.g_ScrollOffset * g_Effect.DiffuseTexture_ScrollWeight;
                scrolledUV += distortionUV;
                DiffuseSample = DefaultTextureSample(Get90DegreeRotatedUV(scrolledUV, g_Effect.g_RotationFlags, DEFAULTTEXTURE));
            }
            else
            {
                float2 scrolledUV = In.vUV + g_Effect.g_UVOffset;
                scrolledUV += g_Effect.g_ScrollOffset * g_Effect.DiffuseTexture_ScrollWeight;
                DiffuseSample = DefaultTextureSample(Get90DegreeRotatedUV(scrolledUV, g_Effect.g_RotationFlags, DEFAULTTEXTURE));
            }
        }
        else
        {
            DiffuseSample = DefaultTextureSample(Get90DegreeRotatedUV(In.vUV, g_Effect.g_RotationFlags, DEFAULTTEXTURE));
        }
    }
    else
        DiffuseSample = float4(1.f, 1.f, 1.f, 1.f);
    
    // ================     그라데이션 텍스처     ===============
    
    if (Has(g_Effect.g_TextureFlags, GRADATIONTEXTURE))
    {
        if (HasTextureScroll(SCROLL_GRADATION))
        {
            float2 scrolledUV = In.vUV + g_Effect.g_UVOffset;
            scrolledUV += g_Effect.g_ScrollOffset * g_Effect.GradationTexture_ScrollWeight;
            GradationSample = GradationTextureSample(Get90DegreeRotatedUV(scrolledUV, g_Effect.g_RotationFlags, GRADATIONTEXTURE));
        }
        else if (HasTextureSprite(g_Effect.GradationTexture_SpriteInfo))
        {
            float2 SpriteUV = GetStaticSpriteUV(In.vUV, g_Effect.GradationTexture_SpriteInfo);
            GradationSample = GradationTextureSample(Get90DegreeRotatedUV(SpriteUV, g_Effect.g_RotationFlags, GRADATIONTEXTURE));
        }
        else
        {
            GradationSample = GradationTextureSample(Get90DegreeRotatedUV(In.vUV, g_Effect.g_RotationFlags, GRADATIONTEXTURE));
        }

    }
    else
        GradationSample = float4(1.f, 1.f, 1.f, 1.f);
    
     // ================    GLOW 텍스처     ===============
    
    if (Has(g_Effect.g_TextureFlags, GLOWTEXTURE))
    {
        if (HasTextureScroll(SCROLL_GLOW))
        {
            float2 scrolledUV = In.vUV + g_Effect.g_UVOffset;
            scrolledUV += g_Effect.g_ScrollOffset * g_Effect.GlowTexture_ScrollWeight;
            GlowSample = GradationTextureSample(Get90DegreeRotatedUV(scrolledUV, g_Effect.g_RotationFlags, GLOWTEXTURE));
        }
        else if (HasTextureSprite(g_Effect.GlowTexture_SpriteInfo))
        {
            float2 SpriteUV = GetStaticSpriteUV(In.vUV, g_Effect.GlowTexture_SpriteInfo);
            GlowSample = GradationTextureSample(Get90DegreeRotatedUV(SpriteUV, g_Effect.g_RotationFlags, GRADATIONTEXTURE));
        }
        else
        {
            GlowSample = GradationTextureSample(Get90DegreeRotatedUV(In.vUV, g_Effect.g_RotationFlags, GLOWTEXTURE));
        }

    }
    else
        GlowSample = float4(1.f, 1.f, 1.f, 1.f);
    
    
     // ================    Mask 텍스처     ===============
    
    if (Has(g_Effect.g_TextureFlags, MASKINGTEXTURE))
    {
        if (HasTextureScroll(SCROLL_MASKING))
        {
            float2 scrolledUV = In.vUV + g_Effect.g_UVOffset;
            scrolledUV += g_Effect.g_ScrollOffset * g_Effect.MaskingTexture_ScrollWeight;
            MaskSample = MaskTextureSample(Get90DegreeRotatedUV(scrolledUV, g_Effect.g_RotationFlags, MASKINGTEXTURE));
        }
        else if (HasTextureSprite(g_Effect.MaskTexture_SpriteInfo))
        {
            float2 SpriteUV = GetStaticSpriteUV(In.vUV, g_Effect.MaskTexture_SpriteInfo);
            MaskSample = GradationTextureSample(Get90DegreeRotatedUV(SpriteUV, g_Effect.g_RotationFlags, MASKINGTEXTURE));
        }
        else
        {
            MaskSample = MaskTextureSample(Get90DegreeRotatedUV(In.vUV, g_Effect.g_RotationFlags, MASKINGTEXTURE));
        }

    }
    else
        MaskSample = float4(1.f, 1.f, 1.f, 1.f);
    
     // ================    DISSOLVE 텍스처     ===============
    
    float dissolveMask = 1.0f;
    float dissolveNoise = 0.f;
    
    if (Has(g_Effect.g_TextureFlags, DISSOLVETEXTURE))
    {
        if (HasTextureScroll(SCROLL_DISSOLVE))
        {
            float2 scrolledUV = In.vUV + g_Effect.g_UVOffset;
            scrolledUV += g_Effect.g_ScrollOffset * g_Effect.DissolveTexture_ScrollWeight;
            DissolveSample = DissolveTextureSample(Get90DegreeRotatedUV(scrolledUV, g_Effect.g_RotationFlags, DISSOLVETEXTURE));
            dissolveNoise = DissolveSample.r;
            dissolveMask = step(DissolveProgress, dissolveNoise);
        }
        else if (HasTextureSprite(g_Effect.DissolveTexture_SpriteInfo))
        {
            float2 SpriteUV = GetStaticSpriteUV(In.vUV, g_Effect.DissolveTexture_SpriteInfo);
            DissolveSample = GradationTextureSample(Get90DegreeRotatedUV(SpriteUV, g_Effect.g_RotationFlags, DISSOLVETEXTURE));
        }
        else
        {
            DissolveSample = DissolveTextureSample(Get90DegreeRotatedUV(In.vUV, g_Effect.g_RotationFlags, DISSOLVETEXTURE));
            dissolveNoise = DissolveSample.r;
            dissolveMask = step(DissolveProgress, dissolveNoise);
        }

    }
    else
    {
        DissolveSample = float4(1.f, 1.f, 1.f, 1.f);
    }
    
         // ================    Curve 텍스처     ===============
    
    float CurvePowerStrength = 1.0f;
    
    if (Has(g_Effect.g_TextureFlags, CURVETEXTURE))
    {
        if (HasTextureSprite(g_Effect.CurveTexture_SpriteInfo))
        {
            float2 SpriteUV = GetStaticSpriteUV(In.vUV, g_Effect.CurveTexture_SpriteInfo);

            SpriteUV.x += g_Effect.g_ScrollOffset * g_Effect.CurveTexture_ScrollWeight;
           
            CurveSample = CurveTextureSample(Get90DegreeRotatedUV(SpriteUV, g_Effect.g_RotationFlags, CURVETEXTURE));
            CurvePowerStrength = CurveSample.r;
            CurvePowerStrength *= 1.6f;
        }
        else
        {
            CurveSample = CurveTextureSample(Get90DegreeRotatedUV(In.vUV, g_Effect.g_RotationFlags, CURVETEXTURE));
            CurvePowerStrength = CurveSample.r;
            CurvePowerStrength *= 1.6f;
        }

    }
    else
    {
        CurvePowerStrength = 1.f;
    }

    // =================  계산식 사용  ================
    
        // 5. 최종 결합 (아틀라스 색상 * 캐릭터 고유 색상)
    float3 finalRGB = DiffuseSample.rgb * GradationSample.rgb * g_Effect.g_EffectColor.rgb * CurvePowerStrength;
    
    float lifeAlpha = 1.0f - LifeRatio;
    float finalAlpha = DiffuseSample.a * GlowSample.r * MaskSample.r * dissolveMask * g_Effect.g_EffectColor.a /** lifeAlpha*/;

    if (HasLifeDissolve())
        finalAlpha *= lifeAlpha;

    // 7. 휘도 컷팅 (깔끔한 마무리)
    //float luminance = dot(finalRGB, float3(0.2126f, 0.7152f, 0.0722f));
    //if (luminance < 0.1f)
    //    discard;
    
    if (finalAlpha <= g_Effect.g_DiscardValue)
        discard;
    
    return float4(finalRGB, finalAlpha);
}


float4 PS_SPRITEMESH(VS_OUT_INST_MESH_PARTICLE In) : SV_Target0
{
    if (In.vLifeTime.x < 0.0f)
        discard;
   // =======              노이즈 텍스처 샘플링             ===========
    float2 finalUV = In.vUV;
    float2 noiseUV = In.vUV;
    float2 detailNoiseUV = In.vUV * 5.0f;
    
    float4 DiffuseSample = { 1.f, 1.f, 1.f, 1.f };
    float4 noiseSample = { 0.5f, 1.f, 1.f, 1.f };
    float4 DetailNoiseSample = { 1.f, 1.f, 1.f, 1.f };
    float4 MaskSample = { 1.f, 1.f, 1.f, 1.f };
    float4 GradationSample = { 1.f, 1.f, 1.f, 1.f };
    float4 GlowSample = { 1.f, 1.f, 1.f, 1.f };
    float4 DissolveSample = { 1.f, 1.f, 1.f, 1.f };
    float noiseValue = { 1.f };
    
    // 생명 주기
    float LifeRatio = saturate(In.vLifeTime.x / In.vLifeTime.y);
    
    
    if (Has(g_Effect.g_TextureFlags, DEFAULTTEXTURE))
    {
        if (HasTextureSprite(g_Effect.DiffuseTexture_SpriteInfo))
        {
            float2 SpriteUV = GetStaticSpriteUV(In.vUV, g_Effect.DiffuseTexture_SpriteInfo);

            DiffuseSample = DefaultTextureSample(SpriteUV);
        }
        else
            DiffuseSample = g_DefaultTextures[DEFAULTTEXTURE].Sample(LinearClampSampler, In.vUV);
    }
    else
        DiffuseSample = float4(1.f, 1.f, 1.f, 1.f);

    float dissolveNoise = float(1.f);
    
    if (Has(g_Effect.g_TextureFlags, NOISETEXTURE))
    {
        dissolveNoise = NoiseTextureSample(Get90DegreeRotatedUV(In.vUV, g_Effect.g_TextureFlags, NOISETEXTURE)).r;
    }
    else
        dissolveNoise = float(1.f);
    
    float dissolveMask = step(LifeRatio, dissolveNoise);
    
    // 최종 알파 결합
    float finalAlpha = DiffuseSample.a * dissolveMask * g_Effect.g_EffectColor.a;

    // 알파 클리핑
    if (finalAlpha < g_Effect.g_DiscardValue)
        discard;
    
    // ======= 최종 결과값 출력 =======
    float4 FinalColor = DiffuseSample * g_Effect.g_EffectColor;
    FinalColor.a = finalAlpha;

    return FinalColor;
}

float4 PS_BloomHard(VS_OUT_INST_MESH_PARTICLE In) : SV_Target0
{
    if (In.vLifeTime.x < 0.0f)
        discard;
   // =======              노이즈 텍스처 샘플링             ===========
    float2 finalUV = In.vUV;
    
    float4 DiffuseSample = { 1.f, 1.f, 1.f, 1.f };
    float4 noiseSample = { 1.f, 1.f, 1.f, 1.f };
    float4 DetailNoiseSample = { 1.f, 1.f, 1.f, 1.f };
    float4 MaskSample = { 1.f, 1.f, 1.f, 1.f };
    float4 GradationSample = { 1.f, 1.f, 1.f, 1.f };
    float4 GlowSample = { 1.f, 1.f, 1.f, 1.f };
    float4 DissolveSample = { 1.f, 1.f, 1.f, 1.f };
    float4 CurveSample = { 1.f, 1.f, 1.f, 1.f };
    float noiseValue = { 1.f };
    
    // 1. 진행 비율 계산 (AppearRatio: 등장, DissolveProgress: 소멸)
    float LifeRatio = saturate(In.vLifeTime.x / In.vLifeTime.y);
    float AppearRatio = In.vLifeTime.x / (In.vLifeTime.y * g_Effect.g_AppearRatio);
    float DissolveProgress = saturate((LifeRatio - g_Effect.g_AppearRatio) / max(0.001f, 1.0f - g_Effect.g_AppearRatio));
   
     // ================     노이즈 텍스처     ===============

        // 3. 왜곡량(Offset) 계산
    float2 distortionOffset = float2(0.f, 0.f);
    
    if (Has(g_Effect.g_TextureFlags, NOISETEXTURE))
    {
        if (HasTextureScroll(SCROLL_NOISE))
        {
            float2 scrolledUV = In.vUV + g_Effect.g_UVOffset;
            scrolledUV += g_Effect.g_ScrollOffset * g_Effect.NoiseTexture_ScrollWeight;
            
            noiseSample = NoiseTextureSample(Get90DegreeRotatedUV(scrolledUV, g_Effect.g_RotationFlags, NOISETEXTURE));
            
            distortionOffset.x = (noiseSample.r - 0.5f) * g_Effect.g_DistortionScale.x;
            distortionOffset.y = (noiseSample.g - 0.5f) * g_Effect.g_DistortionScale.y;
        }
        else
        {
            noiseSample = NoiseTextureSample(Get90DegreeRotatedUV(In.vUV, g_Effect.g_RotationFlags, NOISETEXTURE));
            
            distortionOffset.x = (noiseSample.r - 0.5f) * g_Effect.g_DistortionScale.x;
            distortionOffset.y = (noiseSample.g - 0.5f) * g_Effect.g_DistortionScale.y;
        }
    }
    else
    {
        noiseSample = float4(1.f, 1.f, 1.f, 1.f);
        distortionOffset = float2(0.f, 0.f);
    }
    
    // 최종 왜곡 UV (왜곡량)
    float2 distortionUV = distortionOffset;
    
    // ================     메인 텍스처      ===============
    if (Has(g_Effect.g_TextureFlags, DEFAULTTEXTURE))
    {
        if (HasTextureSprite(g_Effect.DiffuseTexture_SpriteInfo))
        {
            float2 SpriteUV = GetStaticSpriteUV(In.vUV, g_Effect.DiffuseTexture_SpriteInfo);
            
            if (Has(g_Effect.g_TextureFlags, NOISETEXTURE))
            {
                SpriteUV += distortionUV;
                DiffuseSample = DefaultTextureSample(Get90DegreeRotatedUV(SpriteUV, g_Effect.g_RotationFlags, DEFAULTTEXTURE));
            }
            else
            {
                DiffuseSample = DefaultTextureSample(Get90DegreeRotatedUV(SpriteUV, g_Effect.g_RotationFlags, DEFAULTTEXTURE));
            }
        }
        else if (HasTextureScroll(SCROLL_DIFFUSE))
        {
            if (Has(g_Effect.g_TextureFlags, NOISETEXTURE))
            {
                float2 scrolledUV = In.vUV + g_Effect.g_UVOffset;
                scrolledUV += g_Effect.g_ScrollOffset * g_Effect.DiffuseTexture_ScrollWeight;
                scrolledUV += distortionUV;
                DiffuseSample = DefaultTextureSample(Get90DegreeRotatedUV(scrolledUV, g_Effect.g_RotationFlags, DEFAULTTEXTURE));
            }
            else
            {
                float2 scrolledUV = In.vUV + g_Effect.g_UVOffset;
                scrolledUV += g_Effect.g_ScrollOffset * g_Effect.DiffuseTexture_ScrollWeight;
                DiffuseSample = DefaultTextureSample(Get90DegreeRotatedUV(scrolledUV, g_Effect.g_RotationFlags, DEFAULTTEXTURE));
            }
        }
        else
        {
            DiffuseSample = DefaultTextureSample(Get90DegreeRotatedUV(In.vUV, g_Effect.g_RotationFlags, DEFAULTTEXTURE));
        }
    }
    else
        DiffuseSample = float4(1.f, 1.f, 1.f, 1.f);
    
    // ================     그라데이션 텍스처     ===============
    
    if (Has(g_Effect.g_TextureFlags, GRADATIONTEXTURE))
    {
        if (HasTextureScroll(SCROLL_GRADATION))
        {
            float2 scrolledUV = In.vUV + g_Effect.g_UVOffset;
            scrolledUV += g_Effect.g_ScrollOffset * g_Effect.GradationTexture_ScrollWeight;
            GradationSample = GradationTextureSample(Get90DegreeRotatedUV(scrolledUV, g_Effect.g_RotationFlags, GRADATIONTEXTURE));
        }
        else if (HasTextureSprite(g_Effect.GradationTexture_SpriteInfo))
        {
            float2 SpriteUV = GetStaticSpriteUV(In.vUV, g_Effect.GradationTexture_SpriteInfo);
            GradationSample = GradationTextureSample(Get90DegreeRotatedUV(SpriteUV, g_Effect.g_RotationFlags, GRADATIONTEXTURE));
        }
        else
        {
            GradationSample = GradationTextureSample(Get90DegreeRotatedUV(In.vUV, g_Effect.g_RotationFlags, GRADATIONTEXTURE));
        }

    }
    else
        GradationSample = float4(1.f, 1.f, 1.f, 1.f);
    
     // ================    GLOW 텍스처     ===============
    
    if (Has(g_Effect.g_TextureFlags, GLOWTEXTURE))
    {
        if (HasTextureScroll(SCROLL_GLOW))
        {
            float2 scrolledUV = In.vUV + g_Effect.g_UVOffset;
            scrolledUV += g_Effect.g_ScrollOffset * g_Effect.GlowTexture_ScrollWeight;
            GlowSample = GradationTextureSample(Get90DegreeRotatedUV(scrolledUV, g_Effect.g_RotationFlags, GLOWTEXTURE));
        }
        else if (HasTextureSprite(g_Effect.GlowTexture_SpriteInfo))
        {
            float2 SpriteUV = GetStaticSpriteUV(In.vUV, g_Effect.GlowTexture_SpriteInfo);
            GlowSample = GradationTextureSample(Get90DegreeRotatedUV(SpriteUV, g_Effect.g_RotationFlags, GRADATIONTEXTURE));
        }
        else
        {
            GlowSample = GradationTextureSample(Get90DegreeRotatedUV(In.vUV, g_Effect.g_RotationFlags, GLOWTEXTURE));
        }

    }
    else
        GlowSample = float4(1.f, 1.f, 1.f, 1.f);
    
    
     // ================    Mask 텍스처     ===============
    
    if (Has(g_Effect.g_TextureFlags, MASKINGTEXTURE))
    {
        if (HasTextureScroll(SCROLL_MASKING))
        {
            float2 scrolledUV = In.vUV + g_Effect.g_UVOffset;
            scrolledUV += g_Effect.g_ScrollOffset * g_Effect.MaskingTexture_ScrollWeight;
            MaskSample = MaskTextureSample(Get90DegreeRotatedUV(scrolledUV, g_Effect.g_RotationFlags, MASKINGTEXTURE));
        }
        else if (HasTextureSprite(g_Effect.MaskTexture_SpriteInfo))
        {
            float2 SpriteUV = GetStaticSpriteUV(In.vUV, g_Effect.MaskTexture_SpriteInfo);
            MaskSample = GradationTextureSample(Get90DegreeRotatedUV(SpriteUV, g_Effect.g_RotationFlags, MASKINGTEXTURE));
        }
        else
        {
            MaskSample = MaskTextureSample(Get90DegreeRotatedUV(In.vUV, g_Effect.g_RotationFlags, MASKINGTEXTURE));
        }

    }
    else
        MaskSample = float4(1.f, 1.f, 1.f, 1.f);
    
     // ================    DISSOLVE 텍스처     ===============
    
    float dissolveMask = 1.0f;
    float dissolveNoise = 0.f;
    
    if (Has(g_Effect.g_TextureFlags, DISSOLVETEXTURE))
    {
        if (HasTextureScroll(SCROLL_DISSOLVE))
        {
            float2 scrolledUV = In.vUV + g_Effect.g_UVOffset;
            scrolledUV += g_Effect.g_ScrollOffset * g_Effect.DissolveTexture_ScrollWeight;
            DissolveSample = DissolveTextureSample(Get90DegreeRotatedUV(scrolledUV, g_Effect.g_RotationFlags, DISSOLVETEXTURE));
            dissolveNoise = DissolveSample.r;
            dissolveMask = step(DissolveProgress, dissolveNoise);
        }
        else if (HasTextureSprite(g_Effect.DissolveTexture_SpriteInfo))
        {
            float2 SpriteUV = GetStaticSpriteUV(In.vUV, g_Effect.DissolveTexture_SpriteInfo);
            DissolveSample = GradationTextureSample(Get90DegreeRotatedUV(SpriteUV, g_Effect.g_RotationFlags, DISSOLVETEXTURE));
        }
        else
        {
            DissolveSample = DissolveTextureSample(Get90DegreeRotatedUV(In.vUV, g_Effect.g_RotationFlags, DISSOLVETEXTURE));
            dissolveNoise = DissolveSample.r;
            dissolveMask = step(DissolveProgress, dissolveNoise);
        }

    }
    else
    {
        DissolveSample = float4(1.f, 1.f, 1.f, 1.f);
    }
    
         // ================    Curve 텍스처     ===============
    
    float CurvePowerStrength = 1.0f;
    
    if (Has(g_Effect.g_TextureFlags, CURVETEXTURE))
    {
        if (HasTextureSprite(g_Effect.CurveTexture_SpriteInfo))
        {
            float2 SpriteUV = GetStaticSpriteUV(In.vUV, g_Effect.CurveTexture_SpriteInfo);

            SpriteUV.x += g_Effect.g_ScrollOffset * g_Effect.CurveTexture_ScrollWeight;
           
            CurveSample = CurveTextureSample(Get90DegreeRotatedUV(SpriteUV, g_Effect.g_RotationFlags, CURVETEXTURE));
            CurvePowerStrength = CurveSample.r;
            CurvePowerStrength *= 2.5f;
        }
        else
        {
            CurveSample = CurveTextureSample(Get90DegreeRotatedUV(In.vUV, g_Effect.g_RotationFlags, CURVETEXTURE));
            CurvePowerStrength = CurveSample.r;
            CurvePowerStrength *= 2.5f;
        }

    }
    else
    {
        CurvePowerStrength = 1.f;
    }

    // =================  계산식 사용  ================
    
        // 5. 최종 결합 (아틀라스 색상 * 캐릭터 고유 색상)
    float3 finalRGB = DiffuseSample.rgb * GradationSample.rgb * g_Effect.g_EffectColor.rgb * CurvePowerStrength;
    
    float lifeAlpha = 1.0f - LifeRatio;
    float finalAlpha = DiffuseSample.a * GlowSample.r * MaskSample.r * dissolveMask * g_Effect.g_EffectColor.a /** lifeAlpha*/;

    if (HasLifeDissolve())
        finalAlpha *= lifeAlpha;

    // 7. 휘도 컷팅 (깔끔한 마무리)
    //float luminance = dot(finalRGB, float3(0.2126f, 0.7152f, 0.0722f));
    //if (luminance < 0.1f)
    //    discard;
    
    if (finalAlpha <= g_Effect.g_DiscardValue)
        discard;
    
    return float4(finalRGB, finalAlpha);
}


float4 PS_UnityConvert(VS_OUT_INST_MESH_PARTICLE In) : SV_Target0
{
    if (In.vLifeTime.x < 0.0f)
        discard;
    
    float4 DissolveSample = { 0.f, 0.f, 0.f, 0.f };
    float4 GradationSample = { 0.f, 0.f, 0.f, 0.f };
    float4 DefaultSample = { 0.f, 0.f, 0.f, 0.f };
    
    //  기본 진행도 계산
    float LifeRatio = saturate(In.vLifeTime.x / In.vLifeTime.y); // 전체 수명 비율 (0~1)

    // AppearRatio가 0.3이라면, 0~0.3까지는 DissolveProgress가 0이고, 그 이후 1까지 증가함
    float AppearRatio = g_Effect.g_AppearRatio;
    float DissolveProgress = saturate((LifeRatio - AppearRatio) / max(0.001f, (1.0f - AppearRatio)));
    
    
    float2 scrolledUV = In.vUV + g_Effect.g_UVOffset;
    scrolledUV += g_Effect.g_ScrollOffset;
    
    DefaultSample = g_DefaultTextures[DEFAULTTEXTURE].Sample(LinearClampSampler, scrolledUV);
    
    if (Has(g_Effect.g_TextureFlags, GRADATIONTEXTURE))
    {
        if (HasTextureSprite(g_Effect.GradationTexture_SpriteInfo))
        {
            float2 SpriteUV = GetStaticSpriteUV(In.vUV, g_Effect.GradationTexture_SpriteInfo);
            
            SpriteUV.x += g_Effect.g_ScrollOffset * g_Effect.GradationTexture_ScrollWeight;
            
        // 어차피 1번줄을 참조할거지만.
            GradationSample = GradationTextureSample(Get90DegreeRotatedUV(SpriteUV, g_Effect.g_TextureFlags, GRADATIONTEXTURE));
        }
    }
    
    // 5. 디졸브 로직 (이미지의 Step 방식 적용)
    float dissolveMask = 1.0f;
    if (Has(g_Effect.g_TextureFlags, DISSOLVETEXTURE))
    {
        float dissolveNoise = DissolveTextureSample(In.vUV).r;
        
        // DissolveProgress(Edge)보다 노이즈가 클 때만 1을 뱉음
        // 수명 후반부로 갈수록 DissolveProgress가 커지므로 점점 사라짐
        dissolveMask = step(DissolveProgress, dissolveNoise);
    }
    
    // 유니티 셰이더의 최종 출력 공식: col * _Emissive * col.a
    float4 finalColor = DefaultSample * g_Effect.g_EffectColor * (GradationSample * 2.5f);
    //float EnergyStrength = GradationSample.rgb;
    //finalColor *= (EnergyStrength * 3);
   
    float lifeAlpha = 1.0f - LifeRatio;
    float finalAlpha = DefaultSample.a * dissolveMask * lifeAlpha;
    
    if (finalAlpha <= g_Effect.g_DiscardValue)
        discard;
    
    //finalColor.a *= finalAlpha;
    
    return finalColor;
}

float4 PS_DISTOTION(VS_OUT_INST_MESH_PARTICLE In) : SV_Target0
{
    if (In.vLifeTime.x < 0.0f)
        discard;
    
    // 1. 화면 좌표계(ScreenUV) 계산 - 현재 픽셀의 정확한 위치
    float2 ScreenUV = In.vProjPos.xy / In.vProjPos.w;
    ScreenUV.x = ScreenUV.x * 0.5f + 0.5f;
    ScreenUV.y = ScreenUV.y * -0.5f + 0.5f;

    // 2. 초기 깊이 판정 (원본 위치 기준)
    float fBackNDCZ, fBackViewZ;
    DecodeDepth(ScreenUV, fBackNDCZ, fBackViewZ); //

    float4 vMyViewPos = mul(In.vWorldPos, V);
    float fMyViewZ = vMyViewPos.z;

    // 캐릭터 등 이펙트보다 앞에 있는 물체는 왜곡하지 않음 (Z값이 작으면 앞)
    if (fBackViewZ > 0.0f && fBackViewZ < fMyViewZ - 0.1f)
    {
        discard;
    }

    // 3. 왜곡량(Offset) 계산
    float2 distortionOffset = float2(0.f, 0.f);
    if (Has(g_Effect.g_TextureFlags, NOISETEXTURE))
    {
        float2 noiseUV = Get90DegreeRotatedUV(In.vUV, g_Effect.g_RotationFlags, NOISETEXTURE);
        noiseUV += g_Effect.g_ScrollOffset; // 커브가 적용된 실시간 오프셋
        
        float4 noiseSample = NoiseTextureSample(noiseUV);
        
        // [공식 수정] 노이즈의 r, g 채널을 각각 X, Y 왜곡에 사용하여 단조로움 탈피
        // (값 - 0.5f)를 통해 양방향(-0.5 ~ 0.5)으로 흔들리게 설정
        distortionOffset.x = (noiseSample.r - 0.5f) * g_Effect.g_DistortionScale.x;
        distortionOffset.y = (noiseSample.g - 0.5f) * g_Effect.g_DistortionScale.y;

        // 마스킹 텍스처가 있다면 외곽선 왜곡을 부드럽게 감쇄
        if (Has(g_Effect.g_TextureFlags, MASKINGTEXTURE))
        {
            float4 MaskSample = MaskTextureSample(Get90DegreeRotatedUV(In.vUV, g_Effect.g_RotationFlags, MASKINGTEXTURE));
            distortionOffset *= MaskSample.r;
        }
    }

    // 최종 왜곡 UV (화면 좌표 + 왜곡량)
    float2 distortionUV = ScreenUV + distortionOffset;

    // 4. 경계 예외 처리 (화면 밖 픽셀 참조 방지)
    // 왜곡된 좌표가 0~1을 벗어나면 원본 위치를 참조하여 잘림 현상 방지
    if (any(distortionUV < 0.0f) || any(distortionUV > 1.0f))
    {
        distortionUV = ScreenUV;
    }

    // 5. 왜곡된 위치의 깊이 재검사 (앞 사물이 왜곡 안으로 들어오는 고스트 현상 방지)
    float fDistZ, fDistViewZ;
    DecodeDepth(distortionUV, fDistZ, fDistViewZ);
    if (fDistViewZ > 0.0f && fDistViewZ < fMyViewZ)
    {
        distortionUV = ScreenUV;
    }
    
    float4 refractionColor = g_RenderTargetSceneHDRCopyTexture.Sample(LinearClampSampler, distortionUV);

    float2 scrolledUV = In.vUV + g_Effect.g_UVOffset;
    scrolledUV += g_Effect.g_ScrollOffset;
    float4 mainTex = g_DefaultTextures[DEFAULTTEXTURE].Sample(LinearClampSampler, scrolledUV);
    // 수명에 따른 투명도 계산
    float lifeAlpha = 1.0f - (In.vLifeTime.x / In.vLifeTime.y);
    float finalAlpha = mainTex.a * lifeAlpha;

    if (finalAlpha < g_Effect.g_DiscardValue)
        discard;

    // 굴절된 배경색에 이펙트 고유 색상을 살짝 얹어줌
    return float4(refractionColor.rgb + (g_Effect.g_EffectColor.rgb * 0.2f), finalAlpha);
}

float4 PS_SWORDEFFECT(VS_OUT_INST_MESH_PARTICLE In) : SV_Target0
{
    if (In.vLifeTime.x < 0.0f)
        discard;
    
    float2 finalUV = In.vUV;
    float4 DiffuseSample = { 1.f, 1.f, 1.f, 1.f };
    
    // 1. 진행 비율 계산 (AppearRatio: 등장, DissolveProgress: 소멸)
    float LifeRatio = saturate(In.vLifeTime.x / In.vLifeTime.y);
    float AppearRatio = In.vLifeTime.x / (In.vLifeTime.y * g_Effect.g_AppearRatio);
    float DissolveProgress = saturate((LifeRatio - g_Effect.g_AppearRatio) / (1.0f - g_Effect.g_AppearRatio));

    // 2. [DNA 스타일] 이중 노이즈 레이어링
    float noiseValue = 1.0f;
    if (Has(g_Effect.g_TextureFlags, NOISETEXTURE))
    {
        // JSON 분석 수치: 큰 흐름(2x3)과 미세 흐름(5x5)을 섞음
        float2 uv1 = In.vUV * float2(2.0f, 3.0f) + g_Effect.g_ScrollOffset * 0.25f;
        float2 uv2 = In.vUV * float2(5.0f, 5.0f) + g_Effect.g_ScrollOffset * 0.25f;
        
        float n1 = NoiseTextureSample(uv1).g; // Turbulence G채널
        float n2 = NoiseTextureSample(uv2).r; // Turbulence R채널
        noiseValue = n1 * n2;
        
        // 일렁이는 왜곡 효과 (Distortion)
        finalUV += (noiseValue - 0.5f) * g_Effect.g_DistortionScale;
    }

    // 3. [모양 잡기] 메인 텍스처(칼날 마스크) 및 등장 로직
    if (Has(g_Effect.g_TextureFlags, DEFAULTTEXTURE))
    {
        // 오른쪽에서 왼쪽으로 슥- 나타나는 컷팅
        if ((1.0f - finalUV.x) > AppearRatio)
            discard;
            
        DiffuseSample = DefaultTextureSample(finalUV);
    }

    // 4. [색상 뽑기] 컬러 아틀라스(GradationTexture) 적용
    // V 좌표는 캐릭터별 인덱스(예: bx=20번 칸)를 사용함
    float targetRow = 20.0f; // 툴에서 캐릭터별로 받아올 변수
    float vCoord = (targetRow + 0.5f) / 100.0f;
    
    // 노이즈 밝기(X)와 캐릭터 인덱스(V)로 아틀라스에서 색상 추출
    float4 AtlasColor = GradationTextureSample(float2(noiseValue, vCoord));

    // 5. 최종 결합 (아틀라스 색상 * 캐릭터 고유 색상)
    float3 finalRGB = AtlasColor.rgb * g_Effect.g_EffectColor.rgb;
    float finalAlpha = DiffuseSample.a * AtlasColor.a * g_Effect.g_EffectColor.a;

    // 6. [소멸 로직] 아래에서 위로 밀려나며 사라지기
    if (AppearRatio > 1.0f)
    {
        finalUV.y += g_Effect.g_ScrollOffset.y; // 위로 스크롤
        if ((1.0f - In.vUV.y) < DissolveProgress)
            discard; // 아래부터 깎기
    }

    // 7. 휘도 컷팅 (깔끔한 마무리)
    float luminance = dot(finalRGB, float3(0.2126f, 0.7152f, 0.0722f));
    if (luminance < 0.1f)
        discard;

    return float4(finalRGB, finalAlpha);
}


technique11 T0
{
    pass Mesh_Effect
    {
        SetRasterizerState(RS_Default_CullNone);
        SetDepthStencilState(DS_ReadOnly, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetVertexShader(CompileShader(vs_5_0, VS_DEFAULT()));
        GeometryShader = NULL;
        SetPixelShader(CompileShader(ps_5_0, PS_DefaultMesh()));
    }

    pass SPRITE_EFFECT
    {
        SetRasterizerState(RS_Default_CullNone);
        SetDepthStencilState(DS_ReadOnly, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetVertexShader(CompileShader(vs_5_0, VS_DEFAULT()));
        GeometryShader = NULL;
        SetPixelShader(CompileShader(ps_5_0, PS_SPRITEMESH()));
    }
    
    pass Mesh_Effect_BloomHard
    {
        SetRasterizerState(RS_Default_CullNone);
        SetDepthStencilState(DS_ReadOnly, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetVertexShader(CompileShader(vs_5_0, VS_DEFAULT()));
        GeometryShader = NULL;
        SetPixelShader(CompileShader(ps_5_0, PS_BloomHard()));
    }

    pass Distotion_EFFECT
    {
        SetRasterizerState(RS_Default_CullNone);
        SetDepthStencilState(DS_ReadOnly, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetVertexShader(CompileShader(vs_5_0, VS_DEFAULT()));
        GeometryShader = NULL;
        SetPixelShader(CompileShader(ps_5_0, PS_DISTOTION()));
    }

    pass BLOOM_SWORDEFFECT
    {
        SetRasterizerState(RS_Default_CullNone);
        SetDepthStencilState(DS_ReadOnly, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetVertexShader(CompileShader(vs_5_0, VS_DEFAULT()));
        GeometryShader = NULL;
        SetPixelShader(CompileShader(ps_5_0, PS_UnityConvert()));
    }

}
