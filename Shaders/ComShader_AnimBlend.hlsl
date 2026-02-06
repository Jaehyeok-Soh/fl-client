#include "Struct_Defines.hlsl"
#include "Animation_Defines.hlsl"

// 각 채널에서 transform을 만든 다음
// 그 두개의 transform을 선형 보간 한다
// 뼈 기준으로 들어옴

// 불변 데이터
struct IMMU_ELEMENT
{
    int     iRootMotionBoneIndex; // root motion일 경우 tralation을 0으로 만들기 위함
    
    float3  vPadding3;
};

// 가변 데이터
struct MU_ELEMENT
{
    float4x4 matPrevLocalPos;
    float4x4 matCurrLocalPos;
};

cbuffer MU_Track
{
    MU_ELEMENT g_InputData;
};

// out put
struct BLENDANIM_OUTPUT
{
    row_major float4x4  matLerpedTransform;     // 찐찐찐 최종 local
};

StructuredBuffer<MU_ELEMENT>    MU_TRANSFORMS;  // 한 애니메이션에 대한 모든 keyframe 정보를 일차원 배열로 들고 있는다
StructuredBuffer<IMMU_ELEMENT>  IMMU_CURCHANNELDATAS;       // 한 채널에 대한 정보들            :  이 애니메이션 channel 수 만큼

RWStructuredBuffer<BLENDANIM_OUTPUT> UPDATE_DATA; // bone 인덱스랑 1 : 1 매칭 -> bone update때 문제 없도록 하기 위함


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
    row_major float4x4 matResult;
    KEYFRAME lastKeyFrame = IMMU_KEYFRAMS[iLastFrameIdx];
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
    
    // 행렬 완성
    matResult = mul(mul(CreateScale(vScale), CreateRotaionMat_FromQuaternion(vQuat)), CreatTranslation(vTranslation));
    
    // 결과 값 바인드
    UPDATE_DATA[iBoneIdx].iCurKeyFrameIndex = iCurKeyFrameIndex;
    UPDATE_DATA[iBoneIdx].matLerpedTransform    = matResult;
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