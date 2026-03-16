#include "Struct_Defines.hlsl"
#include "Animation_Defines.hlsl"

struct IMMU_BONEDATA
{
    uint    iParentBoneIndex;
    
    uint    iBoneNums;
    float2  Padding0;
};

StructuredBuffer<IMMU_BONEDATA>     BONEINDEXES_DATA;

StructuredBuffer<BONE_MAT>          PARENT_BONECOMBINED_TRANSFORMS;

RWStructuredBuffer<BONE_MAT>        BONECOMBINED_TRANSFORMS;
StructuredBuffer<BONE_MAT>          BONECOMBINED_TRANSFORMS_SRV;


// Warp/Wavefront는 32명씩 묶여서 연산을 한다.
[numthreads(32, 1, 1)]
void CS_Main(uint3 id : SV_DispatchThreadID)
{
    uint index = id.x;
    uint iParentIdx = BONEINDEXES_DATA[index].iParentBoneIndex;
    
    if (index >= BONEINDEXES_DATA[index].iBoneNums)
        return;

    BONECOMBINED_TRANSFORMS[index].matBoneTransform = PARENT_BONECOMBINED_TRANSFORMS[iParentIdx].matBoneTransform;
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