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

struct SSAODesc
{
    float fRadius;
    float fBias;
    float fPower;
    float fIntensity;
    float fFadeStart;
    float fFadeEnd;
    float2 vInvAOSize;
};

struct SSAOKernalDesc
{
    float4 vKernel[SSAO_KERNEL_COUNT];
    float2 vNoiseScale;
    float2 vPadding;
};

struct HDRDesc
{
    float fExposure;
    float fGamma;
    float2 vPadding;
};

struct BLOOMDesc
{
    float2 vInvBloomSize;
    float fThreshold;
    float fKnee;
    float fIntensity;
    float3 vPadding;
};

struct OUTLINEDesc
{
    float4 vColor;
    float2 vInvSize;
    float fThicknessPx;
    float fOpacity;
    float fNormalThreshold;
    float fDepthThreshold;
    float fNormalStrength;
    float fDepthStrength;
    float fFadeStart;
    float fFadeEnd;
    float2 vPadding;
};

struct ObjectInfoDesc
{
    uint iObjectID;
    uint iFlags;
    float2 vPadding;
};

#define FX_EMISSIVE 1
#define FX_SHAKE 2

struct RenderFxDesc
{
    uint iFalgs;
    float fEmissiveIntensity;
    float fShakeAmpX;
    float fShakeAmpY;
    
    float3 vEmissiveColor;
    float fReserved0;
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
cbuffer SSAOKernelBuffer
{
    SSAOKernalDesc SSAOkernel;
};
cbuffer SSAOParamBuffer
{
    SSAODesc SSAOparam;
};
cbuffer HDRParamBuffer
{
    HDRDesc HDRparam;
};
cbuffer BLOOMParamBuffer
{
    BLOOMDesc BloomParam;
};
cbuffer OUTLINEParamBuffer
{
    OUTLINEDesc OutlineParam;
};
cbuffer ObjectInfoBuffer
{
    ObjectInfoDesc objectInfo;
};
cbuffer RenderFxParamBuffer
{
    RenderFxDesc renderFx;
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
    if (Has(g_iMaterialMask, DIFFUSE))
        _vDiffuse = g_MaterialTextures[DIFFUSE].Sample(LinearSampler, _vUV);
}

void Compute_Normal(inout float3 _vNormal, float3 _vTangent, float3 _vBinormal, float2 _vUV)
{
    if (Has(g_iMaterialMask, NORMAL))
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
    if (Has(g_iMaterialMask, NORMAL))
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
    if (Has(g_iMaterialMask, SPECULAR))
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

uint UnpackFlags8(uint iPacked)
{
    return (iPacked >> 24) & 0xFF;
}
uint UnpackID24(uint iPacked)
{
    return iPacked & 0x00FFFFFF;
}

bool HasOutline(uint iFlags8)
{
    return (iFlags8 & 1u) != 0;
}

uint PackObjectInfo(uint iID, uint iFlags)
{
    uint iID_24 = iID & 0x00FFFFFF;
    uint iFlags_8 = iFlags & 0x000000FF;
    return (iFlags_8 << 24) | iID_24;
}
uint LoadObjectInfo(float2 vUV, float2 vInvSize)
{
    int2 iPix = int2(vUV / vInvSize);
    return g_RenderTargetObjInfoTexture.Load(int3(iPix, 0));
}
#endif