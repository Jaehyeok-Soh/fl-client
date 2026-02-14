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
#define SCENETEXTURE 6

texture2D g_EffectTexture;

// Render Flag
#define BILLBOARD 1 << 0
#define SCROLL 1 << 1
    
    // Scroll Direction
#define RIGHT 1 << 2 // LEFT -> RIGHT 이펙트 방향
#define DOWN 1 << 3     // DOWN -> UP

    // Use Sprite
#define SPRITE 1<< 5    // 스프라이트를 사용하는가?

    // Use Scroll (텍스처별)
#define SCROLL_DIFFUSE 1 << 6
#define SCROLL_NOISE 1 << 7
#define SCROLL_MASKING 1 << 8
#define SCROLL_GRADATION 1 << 9
        
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
    
    // 각 텍스처별 Scroll Weight (0 ~ 1)
    float2 DiffuseTexture_ScrollWeight;
    float2 NoiseTexture_ScrollWeight;
    float2 MaskingTexture_ScrollWeight;
    float2 GradationTexture_ScrollWeight;
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

float4 SceneTextureSample(float2 UV)
{
    return g_RenderTargetSceneTexture.Sample(LinearSampler, UV);
}

// =========== VS In  ==============

VS_OUT_POS_GS_PARTICLE VS_Texture(VS_IN_POS_GS_PARTICLE In)
{
    VS_OUT_POS_GS_PARTICLE Out;
    
    vector vPosition = mul(vector(In.vPosition, 1.f), INSTANCE_OUTPUT[In.vInstID].matTransform);
    

    Out.vPosition = mul(vPosition, W);
    Out.vPSize = float2(length(INSTANCE_OUTPUT[In.vInstID].matTransform._11_12_13), length(INSTANCE_OUTPUT[In.vInstID].matTransform._21_22_23));
    Out.vLifeTime = INSTANCE_OUTPUT[In.vInstID].vLifeTime;
    
    return Out;
}

[maxvertexcount(6)]
void GS_Texture(point VS_OUT_POS_GS_PARTICLE In[1], inout TriangleStream<GS_OUT_EFFECT_PARTICLE> OutStream)
{
    GS_OUT_EFFECT_PARTICLE Out[4];
    
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
    
    float3 vFinalPos[4];
    float2 vFinalUV[4];
    float2 vFinalSpriteUV[4];
    
     // ========         정점 4개 생성        =============
    vFinalPos[0] = In[0].vPosition.xyz - vRight + vUp; // 좌상 
    vFinalPos[1] = In[0].vPosition.xyz + vRight + vUp; // 우상 
    vFinalPos[2] = In[0].vPosition.xyz - vRight - vUp; // 좌하 
    vFinalPos[3] = In[0].vPosition.xyz + vRight - vUp; // 우하 

    vFinalUV[0] = float2(0, 0);
    vFinalUV[1] = float2(1, 0);
    vFinalUV[2] = float2(0, 1);
    vFinalUV[3] = float2(1, 1);
    
    
    if (HasSprite())
    {
        float2 vUVSize = float2(1.0f / g_Effect.g_SpriteCol, 1.0f / g_Effect.g_SpriteRow);
        uint curCol = g_Effect.g_CurSpriteIndex % g_Effect.g_SpriteCol;
        uint curRow = g_Effect.g_CurSpriteIndex / g_Effect.g_SpriteCol;
    
        float2 vStartUV = float2(curCol * vUVSize.x, curRow * vUVSize.y);
        
        vFinalSpriteUV[0] = vStartUV; // 좌상
        vFinalSpriteUV[1] = vStartUV + float2(vUVSize.x, 0); // 우상
        vFinalSpriteUV[2] = vStartUV + float2(0, vUVSize.y); // 좌하
        vFinalSpriteUV[3] = vStartUV + vUVSize; // 우하

    }
    else
    {
        vFinalSpriteUV[0] = vFinalUV[0];
        vFinalSpriteUV[1] = vFinalUV[1];
        vFinalSpriteUV[2] = vFinalUV[2];
        vFinalSpriteUV[3] = vFinalUV[3];
    }
    
    for (int i = 0; i < 4; ++i)
    {
        Out[i].vPosition = mul(float4(vFinalPos[i], 1.f), matVP);
        Out[i].vUV = vFinalUV[i];
        Out[i].vSpriteUV = vFinalSpriteUV[i];
        Out[i].vLifeTime = In[0].vLifeTime;
    }
    

    // 삼각형 스트립 출력 (0-1-2, 0-2-3)
    OutStream.Append(Out[0]);
    OutStream.Append(Out[1]);
    OutStream.Append(Out[2]);
    OutStream.Append(Out[3]);
    OutStream.RestartStrip();
}

float4 PS_Texture(GS_OUT_EFFECT_PARTICLE In) : SV_TARGET0
{
    float2 noiseUV = In.vUV + g_Effect.g_ScrollOffset;
    float4 noiseSample = { 1.f, 1.f, 1.f, 1.f };
    
    if (Has(g_Effect.g_TextureFlags, NOISETEXTURE))
    {
        noiseSample = NoiseTextureSample(noiseUV);
    }
    
    float noiseValue = noiseSample.r;
    float2 finalUV = In.vUV;
    
    if (HasSprite())
    {
        finalUV = In.vSpriteUV;
    }
    
    finalUV.x += (noiseValue - 0.5f) * g_Effect.g_DistortionScale.x;
    finalUV.y += (noiseValue - 0.5f) * g_Effect.g_DistortionScale.y;

    // =========== Diffuse Texture 여부 =============
    float4 DiffuseColor = float4(1.f, 1.f, 1.f, 1.f);
    
    if (Has(g_Effect.g_TextureFlags, NOISETEXTURE))
    {
        DiffuseColor = DefaultTextureSample(Get90DegreeRotatedUV(finalUV, g_Effect.g_RotationFlags, DEFAULTTEXTURE));
    }
    
    float finalAlpha = DiffuseColor.a * noiseValue;
    
    // ===========  라이프타임에 따른 투명도 적용  =============
    float LifeRatio = saturate(1.0f - (In.vLifeTime.x / In.vLifeTime.y));
    finalAlpha *= LifeRatio;
    
    if (finalAlpha < g_Effect.g_DiscardValue)
        discard;
    
    // =========== 사진 검은색 부분 자르기 ==========
    float3 finalColor = float3(DiffuseColor.rgb * g_Effect.g_EffectColor.rgb * 1.0f);
    // 휘도 계산 공식
    float brightness = dot(finalColor.rgb, float3(0.2126f, 0.7152f, 0.0722f));
    if (brightness < 0.05f)
        discard;
    
        return float4(finalColor.xyz, finalAlpha);
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
}