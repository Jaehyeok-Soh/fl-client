#include "Struct_Defines.hlsl"
#include "Animation_Defines.hlsl"

// 하나의 채널의 SRT를 보간을 통해 뼈 localMatrix를 계산한다

struct KEYFRAME
{
    float3  vScale;
    float4  vQuat;
    float3  vTranslation;
    float   fTrackPosition;
    
    float   fPadding;
};

// 불변 데이터
struct IMMU_ELEMENT
{
    int     iBoneIndex;             // 내 bone transform을 잘 업데이트 하기 위함
    int     iRootMotionBoneIndex;   // root motion일 경우 tralation을 0으로 만들기 위함
    
    uint    iKeyStart;              // 키프레임 시작 위치
    uint    iKeyCount;              // 키프레임 개수
};

// 가변 데이터
struct MU_ELEMENT
{
    float   fCurTrackPosition;
    
    float3  vPadding3;
};

cbuffer MU_Track
{
    MU_ELEMENT g_InputTrackData;
};

// out put
struct CHANNEL_OUTPUT
{
    row_major float4x4  matLerpedTransform;
    uint                iCurKeyFrameIndex;
    float3              vPadding3;
};

StructuredBuffer<KEYFRAME>      IMMU_KEYFRAMS;          // 한 애니메이션에 대한 모든 keyframe 정보를 일차원 배열로 들고 있는다
StructuredBuffer<IMMU_ELEMENT>  IMMU_CHANNELDATAS;      // 한 채널에 대한 정보들            :  이 애니메이션 channel 수 만큼

RWStructuredBuffer<CHANNEL_OUTPUT> UPDATE_DATA;         // bone 인덱스랑 1 : 1 매칭 -> bone update때 문제 없도록 하기 위함


// Warp/Wavefront는 32명씩 묶여서 연산을 한다.
[numthreads(32, 1, 1)]
void CS_Main(uint3 id : SV_DispatchThreadID)
{
    // 받은 정보 정리
    uint iChannelIdx        = id.x;
    uint iBoneIdx           = IMMU_CHANNELDATAS[iChannelIdx].iBoneIndex;
    bool bRootMotionBone    = IMMU_CHANNELDATAS[iChannelIdx].iRootMotionBoneIndex;
    
    uint iFirstFrameIdx     = IMMU_CHANNELDATAS[iChannelIdx].iKeyStart;
    uint iLastFrameIdx      = iFirstFrameIdx + IMMU_CHANNELDATAS[iChannelIdx].iKeyCount - 1;
    
    float fCurrentTrackPosition = g_InputTrackData.fCurTrackPosition;
    float fCurKeyFrameIndex     = UPDATE_DATA[iBoneIdx].iCurKeyFrameIndex;
    
    // frame 정렬
    if (fCurrentTrackPosition <= 0.f)
        fCurKeyFrameIndex = 0.f;
    
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
        
        if (fCurrentTrackPosition >= IMMU_KEYFRAMS[iFirstFrameIdx + fCurKeyFrameIndex + 1].fTrackPosition)
            fCurKeyFrameIndex++;
        
        // 이전 이후 프레임의 SRT 꺼내옴
        vLeftScale = IMMU_KEYFRAMS[iFirstFrameIdx + fCurKeyFrameIndex].vScale;
        vRightScale = IMMU_KEYFRAMS[iFirstFrameIdx + fCurKeyFrameIndex + 1].vScale;

        vLeftQuat = IMMU_KEYFRAMS[iFirstFrameIdx + fCurKeyFrameIndex].vQuat;
        vRightQuat = IMMU_KEYFRAMS[iFirstFrameIdx + fCurKeyFrameIndex + 1].vQuat;

        vLeftTrans = IMMU_KEYFRAMS[iFirstFrameIdx + fCurKeyFrameIndex].vTranslation;
        vRightTrans = IMMU_KEYFRAMS[iFirstFrameIdx + fCurKeyFrameIndex + 1].vTranslation;
        
        // 보간 값
        float fRatio = (fCurrentTrackPosition - IMMU_KEYFRAMS[iFirstFrameIdx + fCurKeyFrameIndex].fTrackPosition) /
			(IMMU_KEYFRAMS[iFirstFrameIdx + fCurKeyFrameIndex + 1].fTrackPosition - IMMU_KEYFRAMS[iFirstFrameIdx + fCurKeyFrameIndex].fTrackPosition);
        
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
    UPDATE_DATA[iBoneIdx].iCurKeyFrameIndex     = fCurKeyFrameIndex;
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