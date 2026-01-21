#ifndef _ANIMATION_DEFINES_HLSL_
#define _ANIMATION_DEFINES_HLSL_

#include "Struct_Defines.hlsl"
#include "Common_Defines.hlsl"

struct KeyframeDesc
{
    int iAnimIndex;
    uint iCurrFrame;
    uint iNextFrame;
    float fRatio;
    float fSumTime;
    float fSpeed;
    float2 fPadding;
};

struct TweenFrameDesc
{
    float fTweenDuration;
    float fTweenRatio;
    float fTweensumTime;
    float fPadding;
    KeyframeDesc curr;
    KeyframeDesc next;
};

cbuffer KeyframeBuffer
{
    KeyframeDesc Keyframes;
};

cbuffer TweenBuffer
{
    TweenFrameDesc TweenFrames;
};

cbuffer BoneBuffer
{
    row_major float4x4 BoneTransforms[MAX_BONE_TRANSFORMS];
};

uint g_iBoneIndex;
Texture2DArray g_TransformMap;

/*float4x4 Get_AnimationMatrix(VS_IN_SKELECTON input)
{
    float indices[4] = { input.vBlendIndices.x, input.vBlendIndices.y, input.vBlendIndices.z, input.vBlendIndices.w };
    float weights[4] = { input.vBlendWeight.x, input.vBlendWeight.y, input.vBlendWeight.z, input.vBlendWeight.w };

    int iAnimIndex[2];
    int iCurrFrame[2];
    int iNextFrame[2];
    float fRatio[2];
    
    iAnimIndex[0] = TweenFrames.curr.iAnimIndex;
    iCurrFrame[0] = TweenFrames.curr.iCurrFrame;
    iNextFrame[0] = TweenFrames.curr.iNextFrame;
    fRatio[0] = TweenFrames.curr.fRatio;
    
    iAnimIndex[1] = TweenFrames.next.iAnimIndex;
    iCurrFrame[1] = TweenFrames.next.iCurrFrame;
    iNextFrame[1] = TweenFrames.next.iNextFrame;
    fRatio[1] = TweenFrames.next.fRatio;
    
    float4 c0, c1, c2, c3;
    float4 n0, n1, n2, n3;
    
    float4x4 matCurr = 0;
    float4x4 matNext = 0;
    float4x4 matTransform = 0;
    

    for (int i = 0; i < 4; ++i)
    {
        if (0.f == weights[i])
            continue;
        
        c0 = g_TransformMap.Load(int4(indices[i] * 4 + 0, iCurrFrame[0], iAnimIndex[0], 0));
        c1 = g_TransformMap.Load(int4(indices[i] * 4 + 1, iCurrFrame[0], iAnimIndex[0], 0));
        c2 = g_TransformMap.Load(int4(indices[i] * 4 + 2, iCurrFrame[0], iAnimIndex[0], 0));
        c3 = g_TransformMap.Load(int4(indices[i] * 4 + 3, iCurrFrame[0], iAnimIndex[0], 0));
        matCurr = float4x4(c0, c1, c2, c3);
        
        n0 = g_TransformMap.Load(int4(indices[i] * 4 + 0, iNextFrame[0], iAnimIndex[0], 0));
        n1 = g_TransformMap.Load(int4(indices[i] * 4 + 1, iNextFrame[0], iAnimIndex[0], 0));
        n2 = g_TransformMap.Load(int4(indices[i] * 4 + 2, iNextFrame[0], iAnimIndex[0], 0));
        n3 = g_TransformMap.Load(int4(indices[i] * 4 + 3, iNextFrame[0], iAnimIndex[0], 0));
        matNext = float4x4(n0, n1, n2, n3);
        
        float4x4 matResult = lerp(matCurr, matNext, fRatio[0]);
        
        // 다음 애니메이션이 있는지
        if (iAnimIndex[1] >= 0)
        {
            c0 = g_TransformMap.Load(int4(indices[i] * 4 + 0, iCurrFrame[1], iAnimIndex[1], 0));
            c1 = g_TransformMap.Load(int4(indices[i] * 4 + 1, iCurrFrame[1], iAnimIndex[1], 0));
            c2 = g_TransformMap.Load(int4(indices[i] * 4 + 2, iCurrFrame[1], iAnimIndex[1], 0));
            c3 = g_TransformMap.Load(int4(indices[i] * 4 + 3, iCurrFrame[1], iAnimIndex[1], 0));
            matCurr = float4x4(c0, c1, c2, c3);
        
            n0 = g_TransformMap.Load(int4(indices[i] * 4 + 0, iNextFrame[1], iAnimIndex[1], 0));
            n1 = g_TransformMap.Load(int4(indices[i] * 4 + 1, iNextFrame[1], iAnimIndex[1], 0));
            n2 = g_TransformMap.Load(int4(indices[i] * 4 + 2, iNextFrame[1], iAnimIndex[1], 0));
            n3 = g_TransformMap.Load(int4(indices[i] * 4 + 3, iNextFrame[1], iAnimIndex[1], 0));
            matNext = float4x4(n0, n1, n2, n3);
            
            float4x4 matNextResult = lerp(matCurr, matNext, fRatio[1]);
            matResult = lerp(matResult, matNextResult, TweenFrames.fTweenRatio);
        }

        matTransform += mul(weights[i], matResult);
    }
    
    return matTransform;
}*/

/*
float4x4 Get_AnimationMatrix(VS_IN_SKELECTON input)
{
    float indices[4] = { input.vBlendIndices.x, input.vBlendIndices.y, input.vBlendIndices.z, input.vBlendIndices.w };
    float weights[4] = { input.vBlendWeight.x, input.vBlendWeight.y, input.vBlendWeight.z, input.vBlendWeight.w };

    int iAnimIndex = Keyframes.iAnimIndex;
    int iCurrFrame = Keyframes.iCurrFrame;
    
    float4 c0, c1, c2, c3;
    float4x4 curr = 0;
    float4x4 transform = 0;
    
    for (int i = 0; i < 4; ++i)
    {
        // *4를 해주는 이유는 현재 indices[i]의 경우 float이고 float4씩 4개 저장을 했으니
        // float 4개씩 건너뛰어가며 c0, c1, c2, c3에 넣고, 4개씩 저장했으므로 for문 4번을 순회한다.
        c0 = g_TransformMap.Load(int4(indices[i] * 4 + 0, iCurrFrame, iAnimIndex, 0));
        c1 = g_TransformMap.Load(int4(indices[i] * 4 + 1, iCurrFrame, iAnimIndex, 0));
        c2 = g_TransformMap.Load(int4(indices[i] * 4 + 2, iCurrFrame, iAnimIndex, 0));
        c3 = g_TransformMap.Load(int4(indices[i] * 4 + 3, iCurrFrame, iAnimIndex, 0));
     
        // 이 정보를 행렬로만들어서 weights, 각 bone에 붙은 정점의 가중치를 곱해서 합산하는식으로 transform을 형성한다.
        curr = float4x4(c0, c1, c2, c3);
        transform += mul(weights[i], curr);
    }
    
    return transform;
}
*/
float4x4 Get_BoneMatrix(VS_IN_SKELECTON input)
{
    float4x4 matBone =
    BoneTransforms[input.vBlendIndices.x] * input.vBlendWeight.x +
    BoneTransforms[input.vBlendIndices.y] * input.vBlendWeight.y +
    BoneTransforms[input.vBlendIndices.z] * input.vBlendWeight.z +
    BoneTransforms[input.vBlendIndices.w] * input.vBlendWeight.w;
    
    return matBone;
}

#endif