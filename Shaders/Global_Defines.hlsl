#ifndef _GLOBAL_DEFINES_HLSL_
#define _GLOBAL_DEFINES_HLSL_

#include "Common_Defines.hlsl"

/////////////////
// ConstBuffer //
/////////////////

cbuffer GlobalBuffer
{
    row_major float4x4 V;
    row_major float4x4 P; 
    row_major float4x4 VP;
};

cbuffer InvBuffer
{
    row_major float4x4 InvV;
    row_major float4x4 InvP;
};

cbuffer TransformBuffer
{
    row_major float4x4 W;
};

///////////////////
// Static Scalar //
///////////////////
static const float PI = 3.14159265359f;
static const float EPSILON = 1e-5f;

////////////
// vector //
////////////
float3 CameraPosition()
{
    return InvV._41_42_43;
}

float4 g_vColor_R;
float4 g_vColor_G;
float4 g_vColor_B;

uint g_iGlobalMask = 0;

float g_fAlphaRatio;

float   g_fProgressRatio;
int     g_iFillDir;

int     g_iColor;
float4  g_vColorTint;
float4  g_vGradiateColorTint;
float   g_fDelay;
int     g_iFlip;

/////////
// SRV //
/////////
textureCUBE g_TextureCube;
Texture2D g_DefaultTextures[8];
Texture2D g_MaterialTextures[MATERIAL_END];
Texture2D g_RenderTargetTexture;
Texture2D g_RenderTargetDiffuseTexture;
Texture2D g_RenderTargetNormalTexture;
Texture2D g_RenderTargetShadeTexture;
Texture2D g_RenderTargetDepthTexture;
Texture2D g_RenderTargetSceneTexture;

#endif