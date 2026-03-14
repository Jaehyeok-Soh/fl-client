#ifndef _STRUCT_DEFINES_HLSL_
#define _STRUCT_DEFINES_HLSL_
//////////////
// VertexIn //
//////////////

struct VS_IN_POS
{
    float3 vPosition : POSITION;
};

struct VS_IN_POS_COLOR
{
    float3 vPosition : POSITION;
    float4 vColor : COLOR;
};

struct VS_IN_POS_TEX
{
    float3 vPosition : POSITION;
    float2 vUV : TEXCOORD0;
};

struct VS_IN_CUBE
{
    float3 vPosition : POSITION;
    float3 vUV : TEXCOORD0;
};

struct VS_IN_POS_TEX_NOR
{
    float3 vPosition : POSITION;
    float2 vUV : TEXCOORD0;
    float3 vNormal : NORMAL;
};

struct VS_IN_MESH
{
    float3 vPosition : POSITION;
    float2 vUV : TEXCOORD0;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
};

struct VS_IN_SKELECTON
{
    float3 vPosition : POSITION;
    float2 vUV : TEXCOORD0;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    uint4 vBlendIndices : BLENDINDEX;
    float4 vBlendWeight : BLENDWEIGHT;
};

struct VS_IN_POS_TEX_PARTICLE
{
    float3 vPosition : POSITION;
    float2 vUV : TEXCOORD0;
    
    //float4 vRight : TEXCOORD1;
    //float4 vUp : TEXCOORD2;
    //float4 vLook : TEXCOORD3;
    //float4 vTranslation : TEXCOORD4;
    //float2 vLifeTime : TEXCOORD5;
};

struct VS_IN_POS_GS_PARTICLE
{
    float3 vPosition : POSITION;
    //row_major float4x4 matTransform : WORLD;
    //float2 vLifeTime : TEXCOORD0;
    
    // slot 1
    uint vInstID : TEXCOORD0;
};

struct VS_IN_INST_MESH_PARTICLE
{
    // Slot 0
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    float2 vUV : TEXCOORD0;
    
    //// Slot 1
    //row_major float4x4 matTransform : WORLD;
    uint   vInstID : TEXCOORD1;
};

struct VS_IN_INST_MESH
{
    // Slot 0
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    float2 vUV : TEXCOORD0;
    
    // Slot 1
    row_major float4x4 matTransform : WORLD;
    
    
    // GPU 가 알아서 추가해주는 Instance 번호
    uint iCurInstanceID : SV_InstanceID;
};



//////////////////
// VertexOutput //
//////////////////

struct VS_OUT_POS
{
    float4 vPosition : SV_POSITION;
};

struct VS_OUT_POS_COLOR
{
    float4 vPosition : SV_POSITION;
    float4 vColor : COLOR;
};

struct VS_OUT_CUBE
{
    float4 vPosition : SV_POSITION;
    float3 vUV : TEXCOORD0;
};

struct VS_OUT_POS_TEX
{
    float4 vPosition : SV_POSITION;
    float2 vUV : TEXCOORD0;
};

struct VS_OUT_POS_TEX_NOR
{
    float4 vPosition : SV_POSITION;
    float2 vUV : TEXCOORD0;
    float3 vNormal : NORMAL;
    
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

struct VS_OUT_MESH
{
    float4 vPosition : SV_POSITION;
    float2 vUV : TEXCOORD0;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

struct VS_OUT_INST_MESH
{
    float4 vPosition : SV_POSITION;
    float2 vUV : TEXCOORD0;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;

    uint iCurInstanceID : TEXCOORD3;
};

struct VS_OUT_SKELETON
{
    float4 vPosition : SV_POSITION;
    float2 vUV : TEXCOORD0;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    uint4 vBlendIndices : BLENDINDEX;
    float4 vBlendWeight : BLENDWEIGHT;
    
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

struct VS_OUT_POS_TEX_PARTICLE
{
    float4 vPosition : SV_POSITION;
    float2 vUV : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
};

struct VS_OUT_POS_GS_PARTICLE
{
    float4 vPosition : POSITION;
    float3 vPSize : PSIZE;
    float2 vLifeTime : TEXCOORD0;
    uint vInstID : TEXCOORD1;
    row_major float4x4 matTransform : WORLD;
};

struct VS_OUT_INST_MESH_PARTICLE
{
    float4 vPosition : SV_POSITION;
    float2 vUV : TEXCOORD0;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;

    float2 vPSize : PSIZE;
    float2 vLifeTime : TEXCOORD3;

};
///////////////////
// GeometryInput //
///////////////////
struct GS_IN_POS_PARTICLE
{
    float4 vPosition : POSITION;
    float2 vPSize : PSIZE;
    float2 vLifeTime : TEXCOORD0;
};

////////////////////
// GeometryOutput //
////////////////////
struct GS_OUT_POS_PARTICLE
{
    float4 vPosition : SV_POSITION;
    float2 vUV : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
};

struct GS_OUT_EFFECT_PARTICLE
{
    float4 vPosition : SV_POSITION;
    float2 vUV : TEXCOORD0;
    float2 vSpriteUV : TEXCOORD1;
    float2 vLifeTime : TEXCOORD2;
};

////////////////
// PiexlInput //
////////////////

struct PS_IN_POS
{
    float4 vPosition : SV_POSITION;
};

struct PS_IN_POS_COLOR
{
    float4 vPosition : SV_POSITION;
    float4 vColor : COLOR;
};

struct PS_IN_CUBE
{
    float4 vPosition : SV_POSITION;
    float3 vUV : TEXCOORD0;
};

struct PS_IN_POS_TEX
{
    float4 vPosition : SV_POSITION;
    float2 vUV : TEXCOORD0;
};

struct PS_IN_POS_TEX_NOR
{
    float4 vPosition : SV_POSITION;
    float2 vUV : TEXCOORD0;
    float3 vNormal : NORMAL;
    
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

struct PS_IN_MESH
{
    float4 vPosition : SV_POSITION;
    float2 vUV : TEXCOORD0;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

struct PS_IN_INST_MESH
{
    float4 vPosition : SV_POSITION;
    float2 vUV : TEXCOORD0;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    uint iCurInstanceID : TEXCOORD3;
   
};

struct PS_IN_SKELETON
{
    float4 vPosition : SV_POSITION;
    float2 vUV : TEXCOORD0;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    uint4 vBlendIndices : BLENDINDEX;
    float4 vBlendWeight : BLENDWEIGHT;
    
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

struct PS_IN_POS_TEX_PARTICLE
{
    float4 vPosition : SV_POSITION;
    float2 vUV : TEXCOORD0;
    
    float2 vLifeTime : TEXCOORD1;
};

struct PS_IN_POS_PARTICLE
{
    float4 vPosition : SV_POSITION;
    float2 vUV : TEXCOORD0;
    
    float2 vLifeTime : TEXCOORD1;
};

struct PS_IN_SPRITEFONT
{
    float4 vPos : SV_POSITION;
    float4 vColor : COLOR;
    float2 vTexCoord : TEXCOORD0;
    float2 vTexCoord1 : TEXCOORD1;
};

/////////////////
// PiexlOutput //
/////////////////

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

struct PS_OUT_LIGHT
{
    float4 vShade : SV_TARGET0;
    float4 vSpecular : SV_TARGET1;
};

struct PS_OUT_AO
{
    float4 vAO : SV_TARGET0;
};

struct PS_OUT_DEFFERED
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vSpecularMask : SV_TARGET2;
    float4 vDepth : SV_TARGET3;
    uint4 vObjectInfo : SV_Target4;
    float4 vEmissive : SV_Target5;
};

struct PS_OUT_BAKESHADOW
{
    float4 vDepth : SV_TARGET0;
};

struct PS_OUT_HDR
{
    float4 vColor : SV_TARGET0;
};

struct PS_OUT_BLOOM
{
    float4 vColor : SV_TARGET0;
};

struct PS_OUT_BACKBUFFER
{
    float4 vColor : SV_TARGET0;
};

struct PS_OUT_SPRITEFONT
{
    float4 vColor : SV_TARGET0;
};
    
////////////////////
// Compute Shader//
//////////////////

struct VTXPARTICLE
{
    row_major float4x4 matTransform;
    float2 vLifeTime;
};
#endif