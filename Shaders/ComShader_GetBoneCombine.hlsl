#include "Struct_Defines.hlsl"
#include "Animation_Defines.hlsl"

// 불변 데이터
struct IMMU_ELEMENT
{
    int                 iNeedBoneIdx;
    float3              Padding0;
};

StructuredBuffer<IMMU_ELEMENT>  IMMU_BONEINDIECS;
StructuredBuffer<BONE_MAT>      MU_COMBINEDBONES;

RWStructuredBuffer<BONE_MAT>    SELECTED_COMBINETRANSFORMS;
StructuredBuffer<BONE_MAT>      SELECTED_COMBINETRANSFORMS_SRV;


// Warp/Wavefront는 32명씩 묶여서 연산을 한다.
[numthreads(32, 1, 1)]
void CS_Main(uint3 id : SV_DispatchThreadID)
{
    // bone index 추출
    uint index = IMMU_BONEINDIECS[id.x].iNeedBoneIdx;
    
    // n번째에 해당 bone index의 matrix 저장
    SELECTED_COMBINETRANSFORMS[id.x].matBoneTransform = MU_COMBINEDBONES[index].matBoneTransform;
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