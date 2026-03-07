#include "Struct_Defines.hlsl"
#include "Animation_Defines.hlsl"
#include "Light_Defines.hlsl"

cbuffer CB_MAPPING_RGB
{
    float4 Color_R = { 1.f, 1.f, 1.f, 1.f };
    float4 Color_G = { 1.f, 1.f, 1.f, 1.f };
    float4 Color_B = { 1.f, 1.f, 1.f, 1.f };
};


VS_OUT_SKELETON VS_MAIN(VS_IN_SKELECTON input)
{
    VS_OUT_SKELETON output;
    float4x4 matBone = Get_BoneMatrix(input);
    
    float4 vWorldPosition = mul(float4(input.vPosition, 1.f), matBone);
    vWorldPosition = mul(vWorldPosition, W);
    float4 vProjPosition = mul(vWorldPosition, VP);
    
    input.vNormal = normalize(mul(input.vNormal, (float3x3) matBone));
    input.vTangent = normalize(mul(input.vTangent, (float3x3) matBone));
    input.vBinormal = normalize(mul(input.vBinormal, (float3x3) matBone));
    
    output.vPosition = vProjPosition;
    output.vUV = input.vUV;
    output.vNormal = normalize(mul(input.vNormal, (float3x3) W));
    output.vTangent = normalize(mul(input.vTangent, (float3x3) W));
    output.vBinormal = normalize(mul(input.vBinormal, (float3x3) W));
    
    output.vWorldPos = vWorldPosition;
    output.vProjPos = vProjPosition;
    return output;
}

PS_OUT_DEFFERED PS_MAIN(PS_IN_SKELETON input)
{
    PS_OUT_DEFFERED output;
    
    output.vDiffuse = 1.f;
    Compute_Diffse(output.vDiffuse, input.vUV);
    
    float3 vNormal = input.vNormal;
    Compute_Normal(vNormal, input.vTangent, input.vBinormal, input.vUV);
    output.vNormal = float4(vNormal * 0.5f + 0.5f, 1.f);
    
    float3 vSpecMask = float3(1.f, 1.f, 0.f);
    if (Has(g_iMaterialMask, METALNESS))
        vSpecMask = g_MaterialTextures[METALNESS].Sample(LinearSampler, input.vUV).xyz;
    output.vSpecularMask = float4(vSpecMask, 1.f); 
    output.vObjectInfo.r = PackObjectInfo(objectInfo.iObjectID, objectInfo.iFlags);
    output.vDepth = float4(input.vProjPos.z / input.vProjPos.w, input.vProjPos.w, 0.f, 0.f);
    
    return output;
}

PS_OUT PS_RED(PS_IN_SKELETON input)
{
    PS_OUT output;
    output.vColor = float4(1.f, 0.f, 0.f, 1.f);
    return output;
}

PS_OUT_DEFFERED PS_RGBMAPPING(PS_IN_SKELETON input)
{
    PS_OUT_DEFFERED output;
    
    float4 vDiffuse = 1.f;
    Compute_Diffse(vDiffuse, input.vUV);

    float3 final =
        (vDiffuse.r * Color_R.rgb) +
        (vDiffuse.g * Color_G.rgb) +
        (vDiffuse.b * Color_B.rgb);
     //saturate(vDiffuse.r * Color_R) +
    // saturate(vDiffuse.g * Color_G) +
     //saturate(vDiffuse.b * Color_B);
    
    float luminance = dot(final, float3(0.3, 0.59, 0.11));

    float3 finalRGB = final * luminance;
    
    float4 finalDiffuse = float4(saturate(finalRGB), vDiffuse.a);
    
    output.vDiffuse = finalDiffuse;
    
    float3 vNormal = input.vNormal;
    Compute_Normal(vNormal, input.vTangent, input.vBinormal, input.vUV);
    output.vNormal = float4(vNormal * 0.5f + 0.5f, 1.f);
    
    float3 vSpecMask = float3(1.f, 1.f, 0.f);
    if (Has(g_iMaterialMask, METALNESS))
        vSpecMask = g_MaterialTextures[METALNESS].Sample(LinearSampler, input.vUV).xyz;
    output.vSpecularMask = float4(vSpecMask, 1.f);
    output.vObjectInfo.r = PackObjectInfo(objectInfo.iObjectID, objectInfo.iFlags);
    output.vDepth = float4(input.vProjPos.z / input.vProjPos.w, input.vProjPos.w, 0.f, 0.f);
    
    return output;
}

technique11 T0
{
    PASS_RS_DS_BS_VP(P0, RS_Default, DS_Default, BS_Default, VS_MAIN, PS_MAIN)
    PASS_RS_DS_BS_VP(P1, RS_Wire, DS_Default, BS_Default, VS_MAIN, PS_RED)

    // RGB mapping : weapon 쪽에서 쓰임
	PASS_RS_DS_BS_VP(RGBMapping, RS_Default, DS_Default, BS_Default, VS_MAIN, PS_RGBMAPPING)
};