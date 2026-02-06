#include "Struct_Defines.hlsl"
#include "Animation_Defines.hlsl"

struct KeyFrame
{
    float3  vScale;
    float4  vQuat;
    float3  vTranslation;
    float   fTrackPosiitoin;
};

// 불변 데이터
struct IMMU_ELEMENT
{
    int     iAnimIdex;
    int     iBoneIndex;
    int     iRootMotionBoneIndex;
    
    vector<KeyFrame> KeyFrams;
};

// 가변 데이터
struct MU_ELEMENT
{
    float   fCurTrackPosition;
    uint    iCurKeyFrameIndex;
    
    uint    iKeyStart; // 키프레임 시작 위치
    uint    iKeyCount; // 키프레임 개수
};

// out put
struct CHANNEL_OUTPUT
{
    row_major float4x4 matLerpedTransform;
};

StructuredBuffer<IMMU_ELEMENT>  IMMU_CHANNELDATAS;
StructuredBuffer<MU_ELEMENT>    TRACK_DATA;

RWStructuredBuffer<CHANNEL_OUTPUT> LERPED_TRANSFORMS;


// Warp/Wavefront는 32명씩 묶여서 연산을 한다.
[numthreads(32, 1, 1)]
void CS(uint3 id : SV_DispatchThreadID)
{
    uint i = id.x;

    int iParent = IMMU_BONEDATAS[i].iParentIndex;

    float4x4 matParent =
        (iParent < 0) ? float4x4_identity : BONECOMBINED_TRANSFORMS[iParent].matCombinedTransform;

    BONECOMBINED_TRANSFORMS[i] = mul(LOCAL_TRANSFORMS[i].matLocalTransform, matParent);
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