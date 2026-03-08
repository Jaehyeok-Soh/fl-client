#include "Struct_Defines.hlsl"
#include "Animation_Defines.hlsl"

// 하나의 채널의 SRT를 보간을 통해 뼈 localMatrix를 계산한다

// 하나의 애니메이션에 대한 정보를 가지고 업데이트를 도는거임
// 즉 애니메이션이 바뀌면 값들을 처리해줘야함

// IMMU_KEYFRAMS, IMMU_CHANNELDATAS 값 다시 바인드

struct IMMU_KEYFRAME
{
    float3  vScale;
    float   fTrackPosition;
    
    float4  vQuat;
    
    float3  vTranslation;
    float   fPadding0;
};

// 불변 데이터
struct IMMU_ELEMENT
{
    int     iBoneIndex;             // 내 bone transform을 잘 업데이트 하기 위함

    
    uint    iKeyStart;              // 키프레임 시작 위치
    uint    iKeyCount;              // 키프레임 개수
    
    float   Padding0;
};

// 가변 데이터 : cpu
struct MU_ELEMENT
{
    float       fCurTrackPosition;
    uint        iChannelCount;
    
    int         iRootMotionBoneIndex; // root motion일 경우 tralation을 0으로 만들기 위함
    
    float       fMixRatioOffset;    // mixfatio를 계속해서 바꿔야하는데 그럼 가변으로 값을 offset 형식으로 주기 위함
    uint        iFirst; // 0 : bone mix, 1 : addtive
    
    float3      Padding0;
};

// 불변 데이터 : cpu.. but 매번 바인딩
struct IMMU_MIX
{
    float           fMixRatio; // 뼈기준으로 할래말래

    float3          Padding0;
};

// out put
struct SRT
{
    float3              vScale;
    float               Padding0;
    
    float4              vQuat;
    
    float3              vTranslation;
    float               Padding1;
};

cbuffer MU_MIX
{
    MU_ELEMENT g_InputData;
};

StructuredBuffer<IMMU_KEYFRAME> IMMU_KEYFRAMS           : register(t0);     // 한 애니메이션에 대한 모든 keyframe 정보를 일차원 배열로 들고 있는다
StructuredBuffer<IMMU_ELEMENT>  IMMU_CHANNELDATAS       : register(t1);     // 한 채널에 대한 정보들            :  이 애니메이션 channel 수 만큼

StructuredBuffer<IMMU_MIX>      IMMU_MIXDATA            : register(t2);     // bone index 기준으로 얼만큼씩 섞을건지 : 몇개의 애니메이션이 섞일지 모르기 때문에 매번 바인딩

StructuredBuffer<SRT>           MU_PRETRANSFORMS        : register(t3);     // 이전에 업데이트한 animation

StructuredBuffer<IMMU_KEYFRAME> REF_KEYFRAMS            : register(t4);     // ref animation data들
StructuredBuffer<IMMU_ELEMENT>  REF_CHANNELDATAS        : register(t5); 


RWStructuredBuffer<SRT>         CHANNEL_OUTPUT;                             // bone 인덱스랑 1 : 1 매칭 -> bone update때 문제 없도록 하기 위함
StructuredBuffer<SRT>           CHANNEL_OUTPUT_SRV;


uint BinarySearchKeyframe(float trackPos, uint firstIdx, uint keyCount)
{
    uint low = 0;
    uint high = keyCount - 1;

    // 최대 7회 (2^7 = 128)
    [unroll]
    for (uint iter = 0; iter < 7; ++iter)
    {
        if (low + 1 >= high)
            break;

        uint mid        = (low + high) >> 1;

        float midPos    = IMMU_KEYFRAMS[firstIdx + mid].fTrackPosition;

        if (trackPos < midPos)
            high = mid;
        else
            low = mid;
    }

    return low; // left index (local)
}

float4 QuaternionMultiply(float4 q1, float4 q2)
{
    float3 v1 = q1.xyz;
    float3 v2 = q2.xyz;
    float w1 = q1.w;
    float w2 = q2.w;
    
    float3 v =
    w1 * v2 +
    w2 * v1 +
    cross(v1, v2);

    float w =
    w1 * w2 -
    dot(v1, v2);

    return float4(v, w);
}


// Warp/Wavefront는 32명씩 묶여서 연산을 한다.
[numthreads(32, 1, 1)]
void CS_Main(uint3 id : SV_DispatchThreadID)
{
    // 받은 정보 정리
    uint iChannelIdx        = id.x;
    
    if (iChannelIdx >= g_InputData.iChannelCount)
        return;
    
    uint iBoneIdx               = IMMU_CHANNELDATAS[iChannelIdx].iBoneIndex;
    bool bRootMotionBone        = (g_InputData.iRootMotionBoneIndex == iBoneIdx);
    
    uint iMyFrameIdx            = IMMU_CHANNELDATAS[iChannelIdx].iKeyStart;
    uint iRefFrameIdx           = REF_CHANNELDATAS[iChannelIdx].iKeyStart;
    
    float fCurrentTrackPosition = g_InputData.fCurTrackPosition;
    
    float fMixRatio             = IMMU_MIXDATA[iBoneIdx].fMixRatio * g_InputData.fMixRatioOffset;
    
    bool bFirst = (g_InputData.iFirst == 1);

    // 만약 섞지 않을거라면
    if (fMixRatio <= 0.f)
    {
        // 처음이면 pre channel 꺼 할당 : 아마 이거는 무조건 first이긴 할텐디
        if(bFirst)
        {
            CHANNEL_OUTPUT[iBoneIdx].vScale         = MU_PRETRANSFORMS[iBoneIdx].vScale;
            CHANNEL_OUTPUT[iBoneIdx].vQuat          = MU_PRETRANSFORMS[iBoneIdx].vQuat;
            CHANNEL_OUTPUT[iBoneIdx].vTranslation   = MU_PRETRANSFORMS[iBoneIdx].vTranslation;
        }
        
        // 아니라면 값 갱신 x
        return;
    }
    
    // 1. 필요한 SQT 가져오기
    float3 baseScale        = MU_PRETRANSFORMS[iBoneIdx].vScale;
    float4 baseQuat         = MU_PRETRANSFORMS[iBoneIdx].vQuat;
    float3 baseTranslation  = MU_PRETRANSFORMS[iBoneIdx].vTranslation;
    
    float3 refScale         = REF_KEYFRAMS[iRefFrameIdx].vScale;
    float4 refQuat          = REF_KEYFRAMS[iRefFrameIdx].vQuat;
    float3 refTranslation   = REF_KEYFRAMS[iRefFrameIdx].vTranslation;
    
    float3 Scale            = IMMU_KEYFRAMS[iMyFrameIdx].vScale;
    float4 Quat             = IMMU_KEYFRAMS[iMyFrameIdx].vQuat;
    float3 Translation      = IMMU_KEYFRAMS[iMyFrameIdx].vTranslation;
    
    {
       // uint keyStart = IMMU_CHANNELDATAS[iChannelIdx].iKeyStart;
       // uint keyCount = IMMU_CHANNELDATAS[iChannelIdx].iKeyCount;
       //
       // uint left = BinarySearchKeyframe(fCurrentTrackPosition, keyStart, keyCount);
       // uint right = left + 1;
       //
       // IMMU_KEYFRAME k0 = IMMU_KEYFRAMS[keyStart + left];
       // IMMU_KEYFRAME k1 = IMMU_KEYFRAMS[keyStart + right];
       //
       // float ratio = (fCurrentTrackPosition - k0.fTrackPosition) /
       //       (k1.fTrackPosition - k0.fTrackPosition);

       // float3 Scale = lerp(k0.vScale, k1.vScale, ratio);
       // float4 q0 = k0.vQuat;
       // float4 q1 = k1.vQuat;

        //if (dot(q0, q1) < 0)
          //  q1 = -q1;

        //float4 Quat = normalize(lerp(q0, q1, ratio));
        //float3 Translation = lerp(k0.vTranslation, k1.vTranslation, ratio);
    }

    
    // 2. delta SQT 구하기
    float3 deltaScale = Scale / refScale;
    
    float4 invRefQuat = float4(-refQuat.xyz, refQuat.w);
    float4 deltaQuat = QuaternionMultiply(Quat, invRefQuat);
    
    float3 deltaTranslation = Translation - refTranslation;
    
    // 3. delta SQT에 mixRatio 적용
    float3 scaledDelta = 1.0.xxx + (deltaScale - 1.0.xxx) * fMixRatio;
    
    float4 identity = float4(0, 0, 0, 1);
    if (dot(identity, deltaQuat) < 0)
        deltaQuat = -deltaQuat;
    float4 scaledDeltaQuat = normalize(lerp(identity, deltaQuat, fMixRatio));
    
    float3 scaledDeltaTranslation = deltaTranslation * fMixRatio;
    
    // 4. final SQT 구하기
    float3 vFinalScale = baseScale * scaledDelta;
    float4 vFinalQuat = normalize(QuaternionMultiply(scaledDeltaQuat, baseQuat));
    float3 vFinalTranslation = baseTranslation + scaledDeltaTranslation;
    if (bRootMotionBone)
    {
        vFinalTranslation = float3(0, 0, 0);
    }
    
    CHANNEL_OUTPUT[iBoneIdx].vScale         = vFinalScale;
    CHANNEL_OUTPUT[iBoneIdx].vQuat          = vFinalQuat;
    CHANNEL_OUTPUT[iBoneIdx].vTranslation   = vFinalTranslation;
}

technique11 T0
{
    pass Particle_Default
    {
        SetVertexShader(NULL);
        GeometryShader = NULL;
        SetPixelShader(NULL);
        SetComputeShader(CompileShader(cs_5_0, CS_Main()));
    }
}