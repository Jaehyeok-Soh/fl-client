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

// 진행 방향으로 y를 돌려버리는 플래그
#define DIRBILLBOARD 1 << 4
    // Use Sprite
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

VS_OUT_POS_GS_PARTICLE VS_Particle(VS_IN_POS_GS_PARTICLE In)
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

//[maxvertexcount(6)]
//void GS_Particle(point VS_OUT_POS_GS_PARTICLE In[1], inout TriangleStream<GS_OUT_POS_PARTICLE> OutStream)
//{
//    GS_OUT_POS_PARTICLE Out[4];
    
//    // =========        빌보드 계산          ==============
//    float3 vRight = float3(1.f, 0.f, 0.f);
//    float3 vUp = float3(0.f, 1.f, 0.f);
//    matrix matVP = mul(V, P);
    
//    if (HasDirBillboard())
//    {
//        matrix matInst = INSTANCE_OUTPUT[In[0].vInstID].matTransform;
       
//        vUp = normalize(matInst[2].xyz) * In[0].vPSize.z; // Z스케일이 길이
//        vRight = normalize(matInst[0].xyz) * In[0].vPSize.x; // X스케일이 폭
//    }

//    else if (HasBillboard())
//    {
//        float3 vLook = normalize(CameraPosition() - In[0].vPosition.xyz);
//        vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * In[0].vPSize.x;
//        vUp = normalize(cross(vLook, vRight)) * In[0].vPSize.y;
//    }
//    else
//    {
//        vRight = float3(1.f, 0.f, 0.f) * In[0].vPSize.x;
//        vUp = float3(0.f, 1.f, 0.f) * In[0].vPSize.y;
//    }

//    // ========         정점 4개 생성        =============
//    float3 vPos[4];
//    vPos[0] = In[0].vPosition.xyz + vRight + vUp;
//    vPos[1] = In[0].vPosition.xyz - vRight + vUp;
//    vPos[2] = In[0].vPosition.xyz - vRight - vUp;
//    vPos[3] = In[0].vPosition.xyz + vRight - vUp;

//    float2 vUV[4] = { float2(0, 0), float2(1, 0), float2(1, 1), float2(0, 1) };

//    for (int i = 0; i < 4; ++i)
//    {
//        Out[i].vPosition = mul(float4(vPos[i], 1.f), matVP);
//        Out[i].vUV = vUV[i];
//        Out[i].vLifeTime = In[0].vLifeTime;
//    }

//    // 삼각형 스트립 출력 (0-1-2, 0-2-3)
//    OutStream.Append(Out[0]);
//    OutStream.Append(Out[1]);
//    OutStream.Append(Out[2]);       
//    OutStream.RestartStrip();
//    OutStream.Append(Out[0]);
//    OutStream.Append(Out[2]);
//    OutStream.Append(Out[3]);
//    OutStream.RestartStrip();
//}

[maxvertexcount(4)]
void GS_Particle(point VS_OUT_POS_GS_PARTICLE In[1], inout TriangleStream<GS_OUT_POS_PARTICLE> OutStream)
{
    GS_OUT_POS_PARTICLE Out[4];
    matrix matVP = mul(V, P);

    float3 vRight, vUp;

    // 1. 회전축(방향) 결정
    if (HasBillboard())
    {
        // 일반 빌보드: 카메라를 바라보는 축 계산
        float3 vLook = normalize(CameraPosition() - In[0].vPosition.xyz);
        vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook));
        vUp = normalize(cross(vLook, vRight));
    }
    else
    {
        // 회전 반영: 인스턴스 행렬에서 직접 X, Y축 추출 (W는 전역 월드행렬이라 가정)
        matrix matInst = INSTANCE_OUTPUT[In[0].vInstID].matTransform;
        matrix matWorld = mul(matInst, W);
        
        vRight = normalize(matWorld[0].xyz);
        vUp = normalize(matWorld[2].xyz);
    }

    // 2. 크기 적용 (기존 코드처럼 반경으로 계산하려면 0.5f 사용, 아니면 그대로 사용)
    // 기존 코드에서 사각형이 잘 나왔던 크기 스케일을 그대로 유지하세요.
    float3 vScaledRight = vRight * In[0].vPSize.x * 0.5f;
    float3 vScaledUp = vUp * In[0].vPSize.y * 0.5f;

    // 3. 정점 위치 계산 (기존에 잘 나오던 순서: 좌상-우상-좌하-우하)
    float3 vPos[4];
    vPos[0] = In[0].vPosition.xyz - vScaledRight + vScaledUp; // 좌상
    vPos[1] = In[0].vPosition.xyz + vScaledRight + vScaledUp; // 우상
    vPos[2] = In[0].vPosition.xyz - vScaledRight - vScaledUp; // 좌하
    vPos[3] = In[0].vPosition.xyz + vScaledRight - vScaledUp; // 우하

    // 4. UV 설정 (기존에 잘 나오던 순서 그대로)
    float2 vUV[4] = { float2(0, 0), float2(1, 0), float2(0, 1), float2(1, 1) };

    [unroll]
    for (int i = 0; i < 4; ++i)
    {
        Out[i].vPosition = mul(float4(vPos[i], 1.f), matVP);
        Out[i].vUV = vUV[i];
        Out[i].vLifeTime = In[0].vLifeTime;
        OutStream.Append(Out[i]);
    }
    OutStream.RestartStrip();
}

float4 PS_Particle(GS_OUT_POS_PARTICLE In) : SV_TARGET0
{
    // 클라에서 넘겨준 색상 그대로
    if (In.vLifeTime.x < 0.0f)
        discard;
    
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