#ifndef _GLOBAL_DEFINES_HLSL_
#define _GLOBAL_DEFINES_HLSL_

#include "Common_Defines.hlsl"

///////////////////
// Static Scalar //
///////////////////
static const float PI = 3.14159265359f;
static const float EPSILON = 1e-5f;

////////////
// vector //
////////////
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

float g_fBrightness;

/////////
// SRV //
/////////
textureCUBE g_TextureCube;
Texture2D g_DefaultTextures[10];
Texture2D g_MaterialTextures[MATERIAL_END];
Texture2D g_SSAONoiseTexture;
Texture2D g_RenderTargetTexture;
Texture2D g_RenderTargetDiffuseTexture;
Texture2D g_RenderTargetNormalTexture;
Texture2D g_RenderTargetShadeTexture;
Texture2D g_RenderTargetDepthTexture;
Texture2D<uint> g_RenderTargetObjInfoTexture;
Texture2D g_RenderTargetSpecularTexture;
Texture2D g_RenderTargetSpecularMaskTexture;
Texture2D g_RenderTargetAOTexture;
Texture2D g_RenderTargetSceneHDRTexture;
Texture2D g_RenderTargetSceneHDRCopyTexture;
Texture2D g_RenderTargetBloomTexture;
Texture2D g_LUT_Stand;
Texture3D g_PerlinNoise;

void DecodeDepth(float2 vUV, out float fNDCZ, out float fViewZ)
{
    float4 vDepthDesc = g_RenderTargetDepthTexture.Sample(PointClampSampler, vUV);
    fNDCZ = vDepthDesc.x;
    fViewZ = vDepthDesc.y;
}

float3 DecodeWorldNormal(float2 vUV)
{
    float4 vNormalDesc = g_RenderTargetNormalTexture.Sample(PointClampSampler, vUV);
    return normalize(vNormalDesc.xyz * 2.f - 1.f);
}

void DecodeSpecularMask(float2 vUV, out float fAO, out float fRough, out float fMetal)
{
    float4 vSpecularMaskDesc = g_RenderTargetSpecularMaskTexture.Sample(LinearSampler, vUV);
    fAO = vSpecularMaskDesc.x <= EPSILON ? 1.f : vSpecularMaskDesc.x;
    
    fRough = vSpecularMaskDesc.y;
    fMetal = vSpecularMaskDesc.z;
}

#endif