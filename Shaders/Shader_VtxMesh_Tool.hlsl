#include "Struct_Defines.hlsl"
#include "Animation_Defines.hlsl"
#include "Light_Defines.hlsl"

#define CopicMask 0
#define SecondNormal 1

#define Default     0
#define Selected    1
#define Preview     2

bool HasCopickMask() { return (g_iGlobalMask & 1) != 0; }
bool HasSecNormal() { return (g_iGlobalMask & 2) != 0; }


cbuffer cbMapObjectState
{
    uint   g_iMapObject_State;
    float3 g_vPadding_MapObject_State; // 16바이트(4+12) 정렬 맞춤 (안전을 위해 필수)
};

cbuffer cbLandScap_TextureUV
{
    float2 g_LandScape_TextureUV_LT = { 0.f, 0.f};
    float2 g_LandScape_TextureUV_RB = { 1.f, 1.f };
};

float4 Get_Modified_Diffuse(float4 vOriginDiffuse , uint iState)
{
    float4 vOutDiffuse = vOriginDiffuse;

    if (g_iMapObject_State == 1)
    {
        vOutDiffuse = float4(vOriginDiffuse.rgb * float3(1.1f, 0.0f, 0.1f), 1.0f);
    }
    
    return vOutDiffuse;
}

VS_OUT_MESH VS_MAIN(VS_IN_MESH input)
{
    VS_OUT_MESH output;
    
    output.vPosition = mul(float4(input.vPosition, 1.f), W);
    output.vPosition = mul(output.vPosition, VP);
    output.vUV = input.vUV;
    output.vNormal = normalize(mul(input.vNormal, (float3x3) W));
    output.vTangent = normalize(mul(input.vTangent, (float3x3) W));
    output.vBinormal = normalize(mul(input.vBinormal, (float3x3) W));
    
    output.vWorldPos = mul(float4(input.vPosition, 1.f), W);
    output.vProjPos = output.vPosition;
    return output;
}

PS_OUT_DEFFERED PS_STATICOBJECT(PS_IN_MESH input)
{
    PS_OUT_DEFFERED output = (PS_OUT_DEFFERED)0;
    
    float4 vDiffuse = 1.f;
    
    Compute_Diffse(vDiffuse, input.vUV);
    vDiffuse.rgb *= MIDesc.vTintColor.rgb;
    output.vDiffuse = vDiffuse;
    
    float3 vNormal = input.vNormal;
    Compute_Normal(vNormal, input.vTangent, input.vBinormal, input.vUV);
    output.vNormal = vNormal * 0.5f + 0.5f;
    output.vDepth = float4(input.vProjPos.z / input.vProjPos.w, input.vProjPos.w, 0.f, 0.f);
    
    //uint iState = g_iMapObject_State;
    
    if (g_iMapObject_State == 1)
    {
        output.vDiffuse = float4(output.vDiffuse.rgb * float3(1.0f, 0.0f, 0.1f), 1.0f);
    }

    
    return output;
}

PS_OUT_DEFFERED PS_LANDSCAPE(PS_IN_MESH input)
{
    PS_OUT_DEFFERED output = (PS_OUT_DEFFERED) 0;
    float2 FilpUV = float2(input.vUV.x, 1.0 - input.vUV.y);
    
    float4 vDiffuse = 1.f;
    float2 Size = g_LandScape_TextureUV_RB - g_LandScape_TextureUV_LT;
    float2 vUV = g_LandScape_TextureUV_LT + (Size * FilpUV);
        
    
    
    Compute_Diffse(vDiffuse, vUV);
    
    vDiffuse.rgb *= MIDesc.vTintColor.rgb;
    output.vDiffuse = vDiffuse;
    
    float3 vNormal = input.vNormal;
    Compute_Normal(vNormal, input.vTangent, input.vBinormal, input.vUV);
    output.vNormal = vNormal * 0.5f + 0.5f;
    output.vDepth = float4(input.vProjPos.z / input.vProjPos.w, input.vProjPos.w, 0.f, 0.f);
    
    if (g_iMapObject_State == 1)
    {
        output.vDiffuse = float4(output.vDiffuse.rgb * float3(1.0f, 0.0f, 0.1f), 1.0f);
    }
    
    return output;
}

technique11 T0
{
	PASS_RS_DS_BS_VP(P0, RS_Default_CullNone, DS_Default, BS_Default, VS_MAIN, PS_STATICOBJECT)
	PASS_RS_DS_BS_VP(P1, RS_Default_CullNone, DS_Default, BS_Default, VS_MAIN, PS_LANDSCAPE)
};