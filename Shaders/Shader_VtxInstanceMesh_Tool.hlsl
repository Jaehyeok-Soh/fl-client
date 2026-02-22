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





uint g_iMapObject_State = {0};
int g_iSelectInstanceID = { -1 };

float2 g_LandScape_TextureUV_LT = { 0.f , 0.f };
float2 g_LandScape_TextureUV_RB = { 1.f, 1.f };




float4 Get_Modified_Diffuse(float4 vOriginDiffuse, int iCurInstanceID)
{
    float4 vOutDiffuse = vOriginDiffuse;

    if (g_iMapObject_State == Selected)
    {
        if (g_iSelectInstanceID == iCurInstanceID)
        {
            vOutDiffuse = float4(vOriginDiffuse.rgb * float3(1.0f, 0.0f, 0.1f), 1.0f);
        }
        else
        {
            vOutDiffuse = float4(vOriginDiffuse.rgb * float3(0.1f, 1.0f, 0.1f), 1.0f);
        }
    }
    
    return vOutDiffuse;
}




VS_OUT_INST_MESH VS_MAIN(VS_IN_INST_MESH input)
{
    VS_OUT_INST_MESH Out = (VS_OUT_INST_MESH)0;
    
    //월드 좌표 재구성
    
    Out.vWorldPos = Out.vPosition = mul(float4(input.vPosition, 1.f), input.matTransform);
    Out.vProjPos  = Out.vPosition = mul(Out.vPosition, VP);
    Out.vUV = input.vUV;
    Out.vNormal = normalize(mul(input.vNormal, (float3x3) input.matTransform));
    Out.vTangent = normalize(mul(input.vTangent, (float3x3) input.matTransform));
    Out.vBinormal = normalize(mul(input.vBinormal, (float3x3) input.matTransform));
   
    Out.iCurInstanceID = input.iCurInstanceID;
  
    return Out;
}

                                                                                                                          
PS_OUT_DEFFERED PS_STATICOBJECT(PS_IN_INST_MESH input)
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

    
    output.vDiffuse = Get_Modified_Diffuse(output.vDiffuse, input.iCurInstanceID);
    
    
    if(output.vDiffuse.a < 0.1f)
        discard;
    
    
    return output;
}


PS_OUT_DEFFERED PS_LANDSCAPE(PS_IN_INST_MESH input)
{
    PS_OUT_DEFFERED output = (PS_OUT_DEFFERED) 0;
    
    float4 vDiffuse = 1.f;
    Compute_Diffse(vDiffuse, input.vUV);
    vDiffuse.rgb *= MIDesc.vTintColor.rgb;
    output.vDiffuse = vDiffuse;
    
    float3 vNormal = input.vNormal;
    Compute_Normal(vNormal, input.vTangent, input.vBinormal, input.vUV);
    output.vNormal = vNormal * 0.5f + 0.5f;
    output.vDepth = float4(input.vProjPos.z / input.vProjPos.w, input.vProjPos.w, 0.f, 0.f);

    output.vDiffuse = Get_Modified_Diffuse(output.vDiffuse, input.iCurInstanceID);
    
    
    return output;
}


technique11 T0
{
	PASS_RS_DS_BS_VP(P0, RS_Default_CullNone, DS_Default, BS_Default, VS_MAIN, PS_STATICOBJECT)
	PASS_RS_DS_BS_VP(P1, RS_Default_CullNone, DS_Default, BS_Default, VS_MAIN, PS_LANDSCAPE)
};