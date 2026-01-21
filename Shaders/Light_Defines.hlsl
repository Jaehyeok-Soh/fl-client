#ifndef _LIGHT_DEFINES_HLSL_
#define _LIGHT_DEFINES_HLSL_

#include "Common_Defines.hlsl"
#include "Global_Defines.hlsl"

////////////
// Struct //
////////////
struct LightDesc
{
    float4 vAmbient;
    float4 vDiffuse;
    float4 vSpecular;
    float4 vPosition;
    float3 vDirection;
    float fRange;
};

struct MaterialDesc
{
    float4 vAmbient;
    float4 vDiffuse;
    float4 vSpecular;
    float4 vEmissive;
};

struct MaterialInstanceDesc
{
    float4 vTintColor;
    float3 vPadding;
    float fEmissivePower;
};

/////////////////
// ConstBuffer //
/////////////////
cbuffer LightBuffer
{
    LightDesc Light;
};

cbuffer MaterialBuffer
{
    MaterialDesc Material;
};

cbuffer MaterialInstanceBuffer
{
    MaterialInstanceDesc MIDesc;
};

cbuffer MaterialInstance
{
    uint TextureMapMask;
    float3 vPadding;
};

//////////
// Func //
//////////
// Gouraud - Vertex 단위 연산 (VS 에서 시행)
// Phong - Pixel 단위 연산 (PS 에서 시행)
// ======
// Albedo - 반사율
void Compute_Diffse(inout float4 _vDiffuse, float2 _vUV)
{
    if (HasDiffuse())
        _vDiffuse = g_MaterialTextures[DIFFUSE].Sample(LinearSampler, _vUV);
}

void Compute_Normal(inout float3 _vNormal, float3 _vTangent, float3 _vBinormal, float2 _vUV)
{
    if (HasNormal())
    {
        // T, B, N
        float3x3 matWorld = float3x3(_vTangent, _vBinormal, _vNormal);
        float3 vNormalDesc = g_MaterialTextures[NORMAL].Sample(LinearSampler, _vUV).xyz;
        _vNormal = vNormalDesc * 2.f - 1.f;
        _vNormal = mul(_vNormal, matWorld);
    }
}

float4 Compute_Diffuse_Ambient(float3 _vWorldSpace_Normal, float2 _vUV)
{
    float4 vDiffuse;
    if (HasDiffuse())
        vDiffuse = g_MaterialTextures[DIFFUSE].Sample(LinearSampler, _vUV);
    else
        vDiffuse = 1.f;
    
    float fShade = max(dot(normalize(Light.vDirection) * -1.f, normalize(_vWorldSpace_Normal)), 0.f);
    
    fShade = saturate(fShade + (Light.vAmbient * Material.vAmbient)).a;
    
    return fShade * Light.vDiffuse * vDiffuse;
}

float4 Compute_Specular(float2 _vUV, float3 _vWorldSpace_Normal, float3 _vWorldPosition)
{
    // float3 vReflect = -GlobalLight.vDirection + (2 * _vWorldSpace_Normal * dot(-GlobalLight.vDirection, _vWorldSpace_Normal));
    float3 vReflect = normalize(reflect(normalize(Light.vDirection) * -1.f, normalize(_vWorldSpace_Normal)));
    float3 vLook = normalize(_vWorldPosition - CameraPosition());
    float fResultDot = saturate(dot(vReflect, vLook)); // clamp(0~1)
    float fPower = 1.f;
    float fShine = 20.f;
    if (HasSpecular())
        fPower = g_MaterialTextures[SPECULAR].Sample(LinearSampler, _vUV).a;
    
    float fSpecular = pow(fResultDot, fShine) * fPower;

    return Light.vSpecular/* * Material.specular*/ * fSpecular;
}

float4 Compute_Emissive(float2 _vUV)
{
    float4 vColor = Material.vEmissive;
    return g_MaterialTextures[EMISSIVE].Sample(LinearSampler, _vUV) * vColor;
}

float4 Compute_StandardLight(float3 _vWorldSpace_Normal, float2 _vUV, float3 _vWorldPosition)
{
    return Compute_Diffuse_Ambient(_vWorldSpace_Normal, _vUV) + Compute_Specular(_vUV, _vWorldSpace_Normal, _vWorldPosition) /* + Compute_Emissive(_vUV)*/;
}
#endif