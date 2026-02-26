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
#define GLOWTEXTURE 6
#define DISSOLVETEXTURE 7

texture2D g_EffectTexture;

// Render Flag
#define BILLBOARD 1 << 0
#define SCROLL 1 << 1
    
    // Scroll Direction
#define RIGHT 1 << 2 // LEFT -> RIGHT 이펙트 방향
#define DOWN 1 << 3     // DOWN -> UP

    // Use Sprite
#define DIRBILLBOARD 1 << 4 
#define SPRITE 1<< 5    // 스프라이트를 사용하는가?

    // Use Scroll (텍스처별)
#define SCROLL_DIFFUSE 1 << 6
#define SCROLL_NOISE 1 << 7
#define SCROLL_MASKING 1 << 8
#define SCROLL_GRADATION 1 << 9
#define SCROLL_DISSOLVE 1 << 10
#define SCROLL_GLOW 1 << 11
        
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
    uint g_SpriteCol;
    uint g_SpriteRow;
    uint g_CurSpriteIndex;
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

bool HasDirBillboard()
{
    return (g_Effect.g_RenderFlags & DIRBILLBOARD) != 0;
}

bool HasScroll()
{
    return (g_Effect.g_RenderFlags & SCROLL) != 0;
}

bool HasTextureScroll(uint Flag)
{
    return (g_Effect.g_RenderFlags & Flag) != 0;
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

float4 TrailTextureSample(float2 UV)
{
    return SampleTextureWithFlags(g_DefaultTextures[TRAILTEXTURE], g_Effect.g_StateFlags, 12, UV);
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

VS_OUT_POS_GS_PARTICLE VS_Texture(VS_IN_POS_GS_PARTICLE In)
{
    VS_OUT_POS_GS_PARTICLE Out;
    
    matrix matInst = INSTANCE_OUTPUT[In.vInstID].matTransform;
    
    vector vPosition = mul(vector(In.vPosition, 1.f), matInst);
    vPosition = mul(vPosition, W);
    
    float2 pSize = float2(length(matInst._11_12_13), length(matInst._21_22_23));

    // 사이즈 계산
    Out.vPosition = vPosition;
    Out.vPSize = float3(length(W._11_12_13) * pSize.x, length(W._21_22_23) * pSize.y, length(W._31_32_33));
    Out.vLifeTime = INSTANCE_OUTPUT[In.vInstID].vLifeTime;
    Out.vInstID = In.vInstID;
    
    return Out;
}


[maxvertexcount(6)]
void GS_Texture(point VS_OUT_POS_GS_PARTICLE In[1], inout TriangleStream<GS_OUT_EFFECT_PARTICLE> OutStream)
{
    GS_OUT_EFFECT_PARTICLE Out[4];
    
    float3 vRight = float3(1.f, 0.f, 0.f);
    float3 vUp = float3(0.f, 1.f, 0.f);

    // 1. 빌보드 플래그가 켜진 경우 (카메라 응시)
    if (HasBillboard())
    {
        // 카메라와의 방향 벡터(Look) 계산
        float3 vLook = normalize(CameraPosition() - In[0].vPosition.xyz);
        
        // 카메라의 Look과 월드 Up(0,1,0)을 외적하여 Right 축 생성
        vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook));
        
        // 생성된 Right와 Look을 외적하여 수직 Up 축 생성
        vUp = normalize(cross(vLook, vRight));
    }
    // 2. 빌보드 플래그가 꺼진 경우 (월드/인스턴스 행렬 회전 반영)
    else
    {
        matrix matInst = INSTANCE_OUTPUT[In[0].vInstID].matTransform;
        matrix matWorld = mul(matInst, W);
        
        // 행렬에서 월드 기준 X축(0번행)과 Y축(1번행) 추출
        vRight = normalize(matWorld[0].xyz);
        vUp = normalize(matWorld[1].xyz);
    }

    // 3. 최종 크기 적용
    vRight *= In[0].vPSize.x;
    vUp *= In[0].vPSize.y;
    
    matrix matVP = mul(V, P);
    
    // 4. 정점 위치 계산
    float3 vFinalPos[4];
    vFinalPos[0] = In[0].vPosition.xyz - vRight + vUp; // 좌상
    vFinalPos[1] = In[0].vPosition.xyz + vRight + vUp; // 우상
    vFinalPos[2] = In[0].vPosition.xyz - vRight - vUp; // 좌하
    vFinalPos[3] = In[0].vPosition.xyz + vRight - vUp; // 우하

    // 5. UV 설정 (Sprite 연산 포함)
    float2 vFinalUV[4] = { float2(0, 0), float2(1, 0), float2(0, 1), float2(1, 1) };
    float2 vFinalSpriteUV[4];

    if (HasSprite())
    {
        float2 vUVSize = float2(1.0f / g_Effect.g_SpriteCol, 1.0f / g_Effect.g_SpriteRow);
        uint curCol = g_Effect.g_CurSpriteIndex % g_Effect.g_SpriteCol;
        uint curRow = g_Effect.g_CurSpriteIndex / g_Effect.g_SpriteCol;
        float2 vStartUV = float2(curCol * vUVSize.x, curRow * vUVSize.y);
        
        vFinalSpriteUV[0] = vStartUV;
        vFinalSpriteUV[1] = vStartUV + float2(vUVSize.x, 0);
        vFinalSpriteUV[2] = vStartUV + float2(0, vUVSize.y);
        vFinalSpriteUV[3] = vStartUV + vUVSize;
    }
    else
    {
        for (int k = 0; k < 4; ++k)
            vFinalSpriteUV[k] = vFinalUV[k];
    }

    // 6. 스트림 출력
    for (int i = 0; i < 4; ++i)
    {
        Out[i].vPosition = mul(float4(vFinalPos[i], 1.f), matVP);
        Out[i].vUV = vFinalUV[i];
        Out[i].vSpriteUV = vFinalSpriteUV[i];
        Out[i].vLifeTime = In[0].vLifeTime;
        OutStream.Append(Out[i]);
    }
    OutStream.RestartStrip();
}

float4 PS_Texture(GS_OUT_EFFECT_PARTICLE In) : SV_TARGET0
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
        float2 cellsize = float2(1.0f / g_Effect.g_SpriteCol, 1.0f / g_Effect.g_SpriteRow);
        float2 SpriteUV = float2(1.f, 1.f);
        
        if (HasSprite())
        {
            if (Has(g_Effect.g_TextureFlags, NOISETEXTURE))
            {
                SpriteUV = (In.vUV + distortionUV) * cellsize;
            }
            else
            {
                SpriteUV = In.vUV * cellsize;
            }
           
            // 4. 인덱스 계산
            uint xIndex = g_Effect.g_CurSpriteIndex % g_Effect.g_SpriteCol;
            uint yIndex = g_Effect.g_CurSpriteIndex / g_Effect.g_SpriteCol;
        
            SpriteUV.x += (float) xIndex * cellsize.x;
            SpriteUV.y += (float) yIndex * cellsize.y;

            DiffuseSample = DefaultTextureSample(Get90DegreeRotatedUV(SpriteUV, g_Effect.g_RotationFlags, DEFAULTTEXTURE));
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

    // =================  계산식 사용  ================
    
        // 5. 최종 결합 (아틀라스 색상 * 캐릭터 고유 색상)
    float3 finalRGB = DiffuseSample.rgb * GradationSample.rgb * g_Effect.g_EffectColor.rgb;
    
    float lifeAlpha = 1.0f - LifeRatio;
    float finalAlpha = DiffuseSample.a * GlowSample.r * MaskSample.r * dissolveMask * g_Effect.g_EffectColor.a /** lifeAlpha*/;


    // 7. 휘도 컷팅 (깔끔한 마무리)
    //float luminance = dot(finalRGB, float3(0.2126f, 0.7152f, 0.0722f));
    //if (luminance < 0.1f)
    //    discard;
    
    if (finalAlpha <= g_Effect.g_DiscardValue)
        discard;
    
    return float4(finalRGB, finalAlpha);
}

float4 PS_MaskMain(GS_OUT_EFFECT_PARTICLE In) : SV_TARGET0
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
    if (Has(g_Effect.g_TextureFlags, MASKINGTEXTURE))
    {
        float2 cellsize = float2(1.0f / g_Effect.g_SpriteCol, 1.0f / g_Effect.g_SpriteRow);
        float2 SpriteUV = float2(1.f, 1.f);
        
        if (HasSprite())
        {
            if (Has(g_Effect.g_TextureFlags, MASKINGTEXTURE))
            {
                SpriteUV = (In.vUV + distortionUV) * cellsize;
            }
            else
            {
                SpriteUV = In.vUV * cellsize;
            }
           
            // 4. 인덱스 계산
            uint xIndex = g_Effect.g_CurSpriteIndex % g_Effect.g_SpriteCol;
            uint yIndex = g_Effect.g_CurSpriteIndex / g_Effect.g_SpriteCol;
        
            SpriteUV.x += (float) xIndex * cellsize.x;
            SpriteUV.y += (float) yIndex * cellsize.y;

            MaskSample = MaskTextureSample(Get90DegreeRotatedUV(SpriteUV, g_Effect.g_RotationFlags, MASKINGTEXTURE));
        }
        else if (HasTextureScroll(SCROLL_MASKING))
        {
            if (Has(g_Effect.g_TextureFlags, NOISETEXTURE))
            {
                float2 scrolledUV = In.vUV + g_Effect.g_UVOffset;
                scrolledUV += g_Effect.g_ScrollOffset * g_Effect.MaskingTexture_ScrollWeight;
                scrolledUV += distortionUV;
                MaskSample = MaskTextureSample(Get90DegreeRotatedUV(scrolledUV, g_Effect.g_RotationFlags, MASKINGTEXTURE));
            }
            else
            {
                float2 scrolledUV = In.vUV + g_Effect.g_UVOffset;
                scrolledUV += g_Effect.g_ScrollOffset * g_Effect.MaskingTexture_ScrollWeight;
                MaskSample = MaskTextureSample(Get90DegreeRotatedUV(scrolledUV, g_Effect.g_RotationFlags, MASKINGTEXTURE));
            }
        }
        else
        {
            MaskSample = MaskTextureSample(Get90DegreeRotatedUV(In.vUV, g_Effect.g_RotationFlags, MASKINGTEXTURE));
        }

    }
    else
        MaskSample = float4(1.f, 1.f, 1.f, 1.f);
    
    // ================     그라데이션 텍스처     ===============
    
    if (Has(g_Effect.g_TextureFlags, GRADATIONTEXTURE))
    {
        if (HasTextureScroll(SCROLL_GRADATION))
        {
            float2 scrolledUV = In.vUV + g_Effect.g_UVOffset;
            scrolledUV += g_Effect.g_ScrollOffset * g_Effect.GradationTexture_ScrollWeight;
            GradationSample = GradationTextureSample(Get90DegreeRotatedUV(scrolledUV, g_Effect.g_RotationFlags, GRADATIONTEXTURE));
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
        else
        {
            GlowSample = GradationTextureSample(Get90DegreeRotatedUV(In.vUV, g_Effect.g_RotationFlags, GLOWTEXTURE));
        }

    }
    else
        GlowSample = float4(1.f, 1.f, 1.f, 1.f);
    
    
     // ================    Mask 텍스처     ===============
    
    if (Has(g_Effect.g_TextureFlags, DEFAULTTEXTURE))
    {
        if (HasSprite())
        {
            float2 cellsize = float2(1.0f / g_Effect.g_SpriteCol, 1.0f / g_Effect.g_SpriteRow);
            float2 SpriteUV = float2(1.f, 1.f);
            
            if (Has(g_Effect.g_TextureFlags, DEFAULTTEXTURE))
            {
                SpriteUV = (In.vUV + distortionUV) * cellsize;
            }
            else
            {
                SpriteUV = In.vUV * cellsize;
            }
           
            // 4. 인덱스 계산
            uint xIndex = g_Effect.g_CurSpriteIndex % g_Effect.g_SpriteCol;
            uint yIndex = g_Effect.g_CurSpriteIndex / g_Effect.g_SpriteCol;
        
            SpriteUV.x += (float) xIndex * cellsize.x;
            SpriteUV.y += (float) yIndex * cellsize.y;

            DiffuseSample = DefaultTextureSample(Get90DegreeRotatedUV(SpriteUV, g_Effect.g_RotationFlags, DEFAULTTEXTURE));
        }
        
        else if (HasTextureScroll(SCROLL_DIFFUSE))
        {
            float2 scrolledUV = In.vUV + g_Effect.g_UVOffset;
            scrolledUV += g_Effect.g_ScrollOffset * g_Effect.DiffuseTexture_ScrollWeight;
            DiffuseSample = MaskTextureSample(Get90DegreeRotatedUV(scrolledUV, g_Effect.g_RotationFlags, DEFAULTTEXTURE));
        }
        else
        {
            DiffuseSample = MaskTextureSample(Get90DegreeRotatedUV(In.vUV, g_Effect.g_RotationFlags, DEFAULTTEXTURE));
        }

    }
    else
        DiffuseSample = float4(1.f, 1.f, 1.f, 1.f);
    
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

    // =================  계산식 사용  ================
    
        // 5. 최종 결합 (아틀라스 색상 * 캐릭터 고유 색상)
    float3 finalRGB = DiffuseSample.rgb * GradationSample.rgb * g_Effect.g_EffectColor.rgb;
    
    float lifeAlpha = 1.0f - LifeRatio;
    float finalAlpha = GlowSample.r * MaskSample.r * dissolveMask * g_Effect.g_EffectColor.a /** lifeAlpha*/;


    // 7. 휘도 컷팅 (깔끔한 마무리)
    //float luminance = dot(finalRGB, float3(0.2126f, 0.7152f, 0.0722f));
    //if (luminance < 0.1f)
    //    discard;
    
    if (finalAlpha <= g_Effect.g_DiscardValue)
        discard;
    
    return float4(finalRGB, finalAlpha);
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

    pass Texture_BlendEffect
    {
        SetRasterizerState(RS_Default_CullNone);
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetVertexShader(CompileShader(vs_5_0, VS_Texture()));
        SetGeometryShader(CompileShader(gs_5_0, GS_Texture()));
        SetPixelShader(CompileShader(ps_5_0, PS_Texture()));
    }

    pass MaskMain_Effect
    {
        SetRasterizerState(RS_Default_CullNone);
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetVertexShader(CompileShader(vs_5_0, VS_Texture()));
        SetGeometryShader(CompileShader(gs_5_0, GS_Texture()));
        SetPixelShader(CompileShader(ps_5_0, PS_MaskMain()));
    }

    pass MaskMain_BlendEffect
    {
        SetRasterizerState(RS_Default_CullNone);
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetVertexShader(CompileShader(vs_5_0, VS_Texture()));
        SetGeometryShader(CompileShader(gs_5_0, GS_Texture()));
        SetPixelShader(CompileShader(ps_5_0, PS_MaskMain()));
    }

}