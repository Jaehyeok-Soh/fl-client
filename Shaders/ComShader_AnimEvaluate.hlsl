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
    int     iRootMotionBoneIndex;   // root motion일 경우 tralation을 0으로 만들기 위함
    
    uint    iKeyStart;              // 키프레임 시작 위치
    uint    iKeyCount;              // 키프레임 개수
};

// 가변 데이터 : cpu
struct MU_ELEMENT
{
    float   fCurTrackPosition;
    uint    iAnimIndex;
    
    float2 Padding0;
};

// out put
struct CHANNEL_OUTPUT
{
    float3              vScale;
    uint                iCurKeyFrameIndex;
    
    float4              vQuat;
    
    float3              vTranslation;
    uint                iAnimIndex;
};

cbuffer MU_Track
{
    MU_ELEMENT g_InputData;
};

StructuredBuffer<IMMU_KEYFRAME> IMMU_KEYFRAMS; // 한 애니메이션에 대한 모든 keyframe 정보를 일차원 배열로 들고 있는다
StructuredBuffer<IMMU_ELEMENT>  IMMU_CHANNELDATAS;      // 한 채널에 대한 정보들            :  이 애니메이션 channel 수 만큼

RWStructuredBuffer<CHANNEL_OUTPUT> UPDATE_DATA;         // bone 인덱스랑 1 : 1 매칭 -> bone update때 문제 없도록 하기 위함


// Warp/Wavefront는 32명씩 묶여서 연산을 한다.
[numthreads(32, 1, 1)]
void CS_Main(uint3 id : SV_DispatchThreadID)
{
    // 받은 정보 정리
    uint iChannelIdx        = id.x;
    uint iBoneIdx           = IMMU_CHANNELDATAS[iChannelIdx].iBoneIndex;
    bool bRootMotionBone    = (IMMU_CHANNELDATAS[iChannelIdx].iRootMotionBoneIndex == iBoneIdx);
    
    uint iFirstFrameIdx     = IMMU_CHANNELDATAS[iChannelIdx].iKeyStart;
    uint iLastFrameIdx      = iFirstFrameIdx + IMMU_CHANNELDATAS[iChannelIdx].iKeyCount - 1;
    
    float fCurrentTrackPosition = g_InputData.fCurTrackPosition;
    uint iCurKeyFrameIndex = UPDATE_DATA[iBoneIdx].iCurKeyFrameIndex;
    
    // frame 정렬
    if (fCurrentTrackPosition <= 0.f)
        iCurKeyFrameIndex = 0;
    
    // animation 바뀌었는지 체크
    if (UPDATE_DATA[iBoneIdx].iAnimIndex != g_InputData.iAnimIndex)
    {
        iCurKeyFrameIndex = 0;
        UPDATE_DATA[iBoneIdx].iAnimIndex = g_InputData.iAnimIndex;
    }
    
    // 함수 지역 변수 셋팅    
    IMMU_KEYFRAME lastKeyFrame = IMMU_KEYFRAMS[iLastFrameIdx];
    float3 vScale, vTranslation;
    float4 vQuat;
    
    
    // SRT 생성
    if (fCurrentTrackPosition >= lastKeyFrame.fTrackPosition)
    {
        vScale          = lastKeyFrame.vScale;
        vQuat           = lastKeyFrame.vQuat;
        vTranslation    = lastKeyFrame.vTranslation;
    }
    
    else
    {
        float3 vLeftScale, vRightScale;
        float4 vLeftQuat, vRightQuat;
        float3 vLeftTrans, vRightTrans;
        
        // 범위 체크
        if (iCurKeyFrameIndex + 1 < IMMU_CHANNELDATAS[iChannelIdx].iKeyCount &&
            fCurrentTrackPosition >= IMMU_KEYFRAMS[iFirstFrameIdx + iCurKeyFrameIndex + 1].fTrackPosition)
        {
            iCurKeyFrameIndex++;
        }
        
        // 이전 이후 프레임의 SRT 꺼내옴
        vLeftScale = IMMU_KEYFRAMS[iFirstFrameIdx + iCurKeyFrameIndex].vScale;
        vRightScale = IMMU_KEYFRAMS[iFirstFrameIdx + iCurKeyFrameIndex + 1].vScale;

        vLeftQuat = IMMU_KEYFRAMS[iFirstFrameIdx + iCurKeyFrameIndex].vQuat;
        vRightQuat = IMMU_KEYFRAMS[iFirstFrameIdx + iCurKeyFrameIndex + 1].vQuat;

        vLeftTrans = IMMU_KEYFRAMS[iFirstFrameIdx + iCurKeyFrameIndex].vTranslation;
        vRightTrans = IMMU_KEYFRAMS[iFirstFrameIdx + iCurKeyFrameIndex + 1].vTranslation;
        
        // 보간 값
        float fRatio = (fCurrentTrackPosition - IMMU_KEYFRAMS[iFirstFrameIdx + iCurKeyFrameIndex].fTrackPosition) /
			(IMMU_KEYFRAMS[iFirstFrameIdx + iCurKeyFrameIndex + 1].fTrackPosition - IMMU_KEYFRAMS[iFirstFrameIdx + iCurKeyFrameIndex].fTrackPosition);
        
       // SRT 보간
        vScale  = lerp(vLeftScale, vRightScale, fRatio);
        vQuat   = normalize(lerp(vLeftQuat, vRightQuat, fRatio)); // todo : 원래는 dot을 해서 음수일때 처리 해야하는데 일단 슛
        if (bRootMotionBone)
            vTranslation = float3(0.f, 0.f, 0.f);
        else
            vTranslation = lerp(vLeftTrans, vRightTrans, fRatio);
    }
    
    // 결과 값 바인드
    UPDATE_DATA[iBoneIdx].iCurKeyFrameIndex     = iCurKeyFrameIndex;
    UPDATE_DATA[iBoneIdx].vScale                = vScale;
    UPDATE_DATA[iBoneIdx].vQuat                 = vQuat;
    UPDATE_DATA[iBoneIdx].vTranslation          = vTranslation;
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