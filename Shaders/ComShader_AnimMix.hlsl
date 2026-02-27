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
    
    float Padding0;
};

// 가변 데이터 : cpu
struct MU_ELEMENT
{
    float   fCurTrackPosition;
    uint    iChannelCount;
    
    int     iRootMotionBoneIndex; // root motion일 경우 tralation을 0으로 만들기 위함
    
    float     iFirst;
    
    uint iMixType; // 0 : bone mix, 1 : addtive
    float3 Padding0;
};

// 불변 데이터 : cpu.. but 매번 바인딩
struct IMMU_MIX
{
    float   fMixRatio;

    float3  Padding0;
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

StructuredBuffer<IMMU_KEYFRAME> IMMU_KEYFRAMS : register(t0); // 한 애니메이션에 대한 모든 keyframe 정보를 일차원 배열로 들고 있는다
StructuredBuffer<IMMU_ELEMENT> IMMU_CHANNELDATAS : register(t1); // 한 채널에 대한 정보들            :  이 애니메이션 channel 수 만큼
StructuredBuffer<IMMU_MIX> IMMU_MIXDATA : register(t2); // bone index 기준으로 얼만큼씩 섞을건지 : 몇개의 애니메이션이 섞일지 모르기 때문에 매번 바인딩

StructuredBuffer<SRT> MU_PRETRANSFORMS : register(t3); // 이전에 업데이트한 animation

RWStructuredBuffer<SRT>         CHANNEL_OUTPUT;                 // bone 인덱스랑 1 : 1 매칭 -> bone update때 문제 없도록 하기 위함
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

        uint mid = (low + high) >> 1;

        float midPos = IMMU_KEYFRAMS[firstIdx + mid].fTrackPosition;

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
    
    uint iBoneIdx           = IMMU_CHANNELDATAS[iChannelIdx].iBoneIndex;
    bool bRootMotionBone = (g_InputData.iRootMotionBoneIndex == iBoneIdx);
    
    uint iFirstFrameIdx     = IMMU_CHANNELDATAS[iChannelIdx].iKeyStart;
    uint iLastFrameIdx      = iFirstFrameIdx + IMMU_CHANNELDATAS[iChannelIdx].iKeyCount - 1;
    
    float fCurrentTrackPosition = g_InputData.fCurTrackPosition;
    
    float fMixRatio = IMMU_MIXDATA[iBoneIdx].fMixRatio;
    
    bool bFirst = (g_InputData.iFirst == 1.f);

        // 만약 섞지 않을거라면
    if (fMixRatio <= 0.f)
    {
        if(bFirst)
        {
            CHANNEL_OUTPUT[iBoneIdx].vScale = MU_PRETRANSFORMS[iBoneIdx].vScale;
            CHANNEL_OUTPUT[iBoneIdx].vQuat = MU_PRETRANSFORMS[iBoneIdx].vQuat;
            CHANNEL_OUTPUT[iBoneIdx].vTranslation = MU_PRETRANSFORMS[iBoneIdx].vTranslation;
        }
        
        return;
    }
    
    // key 맵핑이 아니라 trackpositoin을 통해 left, right index 구한다
    uint iLeftIndex = iLastFrameIdx - 1;
    uint iRightIndex = iLastFrameIdx;
    
    // frame 정렬
    if (fCurrentTrackPosition <= 0.f)
    {
        iLeftIndex = iFirstFrameIdx;
        iRightIndex = iFirstFrameIdx + 1;
    }
    
    // 함수 지역 변수 셋팅    
    IMMU_KEYFRAME lastKeyFrame = IMMU_KEYFRAMS[iLastFrameIdx];
    float3 vScale, vTranslation;
    float4 vQuat;
    
    
    // 1. 내 애니메이션으로 SRT 생성
    if (fCurrentTrackPosition >= lastKeyFrame.fTrackPosition)
    {
        vScale = lastKeyFrame.vScale;
        vQuat = lastKeyFrame.vQuat;
        vTranslation = lastKeyFrame.vTranslation;
    }
    else
    {
        float3 vLeftScale, vRightScale;
        float4 vLeftQuat, vRightQuat;
        float3 vLeftTrans, vRightTrans;
        
        // left index와 right index를 구함
        uint localLeft = BinarySearchKeyframe(fCurrentTrackPosition, iFirstFrameIdx, IMMU_CHANNELDATAS[iChannelIdx].iKeyCount);

        uint iLeftIndex = iFirstFrameIdx + localLeft;
        uint iRightIndex = iLeftIndex + 1;
        
        // 이전 이후 프레임의 SRT 꺼내옴
        vLeftScale = IMMU_KEYFRAMS[iLeftIndex].vScale;
        vRightScale = IMMU_KEYFRAMS[iRightIndex].vScale;

        vLeftQuat = IMMU_KEYFRAMS[iLeftIndex].vQuat;
        vRightQuat = IMMU_KEYFRAMS[iRightIndex].vQuat;
    
        if (dot(vLeftQuat, vRightQuat) < 0.0f)
            vRightQuat = -vRightQuat;

        vLeftTrans = IMMU_KEYFRAMS[iLeftIndex].vTranslation;
        vRightTrans = IMMU_KEYFRAMS[iRightIndex].vTranslation;
        
        // 보간 값
        float fRatio = (fCurrentTrackPosition - IMMU_KEYFRAMS[iLeftIndex].fTrackPosition) /
			(IMMU_KEYFRAMS[iRightIndex].fTrackPosition - IMMU_KEYFRAMS[iLeftIndex].fTrackPosition);
        
       // SRT 보간
        vScale = lerp(vLeftScale, vRightScale, fRatio);
        vQuat = normalize(lerp(vLeftQuat, vRightQuat, fRatio)); // todo : 원래는 dot을 해서 음수일때 처리 해야하는데 일단 슛
        if (bRootMotionBone)
            vTranslation = float3(0.f, 0.f, 0.f);
        else
            vTranslation = lerp(vLeftTrans, vRightTrans, fRatio);
    }
    
    // 2. 이전 업데이트한 srt랑 lerp를 해
    float3 vFinalScale, vFinalTranslation;
    float4 vFinalQuat;
    
    // bone mix
    if(g_InputData.iMixType == 0 )
    {
        vFinalScale = lerp(MU_PRETRANSFORMS[iBoneIdx].vScale, vScale, fMixRatio);
   
        float4 q0 = MU_PRETRANSFORMS[iBoneIdx].vQuat;
        float4 q1 = vQuat;
    
        if (dot(q0, q1) < 0.0f)
            q1 = -q1;
        
        vFinalQuat = normalize(lerp(q0, q1, fMixRatio));
        
        if (g_InputData.iRootMotionBoneIndex == iBoneIdx)
            vFinalTranslation = float3(0.f, 0.f, 0.f);
        else
            vFinalTranslation = lerp(MU_PRETRANSFORMS[iBoneIdx].vTranslation, vTranslation, fMixRatio);
    }
    
    // addtive
    else if (g_InputData.iMixType == 1)
    {
        // 1. scale
        float3 ScaledDelta = 1 + (vScale - 1.f) * fMixRatio;
        vFinalScale = MU_PRETRANSFORMS[iBoneIdx].vScale * ScaledDelta;
        
        // 2. quat
        float4 Quatbase = MU_PRETRANSFORMS[iBoneIdx].vQuat;
        float4 delta = vQuat;
        
        // ratio만큼 줄이기
        float4 identity = float4(0, 0, 0, 1);

        if (dot(identity, delta) < 0.0f)
            delta = -delta;
        
        float4 QuatDelta = normalize(lerp(identity, delta, fMixRatio));

        // base에 곱하기
        float4 vFinalQuat = normalize(QuaternionMultiply(Quatbase, QuatDelta));
        
        // 3. translation
        // todo_eunbi : mix때도 zero로 만들어야 하나
        if (g_InputData.iRootMotionBoneIndex == iBoneIdx)
            vFinalTranslation = float3(0.f, 0.f, 0.f);
        else
            vFinalTranslation = MU_PRETRANSFORMS[iBoneIdx].vTranslation + vTranslation * fMixRatio;
    }
    
    // 결과 값 바인드
    CHANNEL_OUTPUT[iBoneIdx].vScale = vFinalScale;
    CHANNEL_OUTPUT[iBoneIdx].vQuat = vFinalQuat;
    CHANNEL_OUTPUT[iBoneIdx].vTranslation = vFinalTranslation;
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