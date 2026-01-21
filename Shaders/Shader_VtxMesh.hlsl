#include "Struct_Defines.hlsl"
#include "Animation_Defines.hlsl"
#include "Light_Defines.hlsl"

#define CopicMask 0
#define SecondNormal 1
bool HasCopickMask() { return (g_iGlobalMask & 1) != 0; }
bool HasSecNormal() { return (g_iGlobalMask & 2) != 0; }


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

PS_OUT_DEFFERED PS_MAIN(PS_IN_MESH input)
{
    PS_OUT_DEFFERED output;
    
    float4 vDiffuse = 1.f;
    Compute_Diffse(vDiffuse, input.vUV);
    vDiffuse.rgb *= MIDesc.vTintColor.rgb;
    output.vDiffuse = vDiffuse;
    
    float3 vNormal = input.vNormal;
    Compute_Normal(vNormal, input.vTangent, input.vBinormal, input.vUV);
    output.vNormal = vNormal * 0.5f + 0.5f;
    
    output.vDepth = float4(input.vProjPos.z / input.vProjPos.w, input.vProjPos.w, 0.f, 0.f);
    
    return output;
}

PS_OUT_DEFFERED PS_MASK(PS_IN_MESH input)
{
    PS_OUT_DEFFERED output;
    
    output.vDiffuse = MIDesc.vTintColor;
    
    float3 T = input.vTangent;
    float3 B = input.vBinormal;
    float3 N = input.vNormal;
    float3x3 TBN = float3x3(T, B, N);
    
    bool bOnRange = (length(CameraPosition() - input.vWorldPos.xyz) <= 10.f);
    
    float3 vCopicNormal = N;
    //if (HasCopickMask())
    if (bOnRange && HasCopickMask())
    {
        float3 vNormalDesc = g_DefaultTextures[CopicMask].Sample(LinearSampler, input.vUV).xyz;
        vCopicNormal = vNormalDesc * 2.f - 1.f;
        vCopicNormal = normalize(mul(vCopicNormal, TBN));
    }
    
    float3 vSecNormal = N;
    //if (HasSecNormal())
    if (bOnRange && HasSecNormal())
    {
        float3 vNormalDesc = g_DefaultTextures[SecondNormal].Sample(LinearSampler, input.vUV).xyz;
        vSecNormal = vNormalDesc * 2.f - 1.f;
        vSecNormal = normalize(mul(vSecNormal, TBN));
    }
    
    float4 vMask = 1.f;
    Compute_Diffse(vMask, input.vUV);
    float fT = vMask.a;
    float3 vMixedNormal = normalize(lerp(vCopicNormal, vSecNormal, fT));
    
    
    float3 vBaseNormal = N;
    Compute_Normal(vBaseNormal, T, B, input.vUV);
    
    float3 vFinalNormal = normalize(lerp(vBaseNormal, vMixedNormal, fT));
    
    output.vNormal = vFinalNormal * 0.5f + 0.5f;
    output.vDepth = float4(input.vProjPos.z / input.vProjPos.w, input.vProjPos.w, 0.f, 0.f);
    return output;
}

PS_OUT_DEFFERED PS_MASKPIPE(PS_IN_MESH input)
{
    PS_OUT_DEFFERED output;
    
    //float4 vDiffuse = 1.f;
    //Compute_Diffse(vDiffuse, input.vUV);
    //float fRatio = saturate(1.f - vDiffuse.a);
    //vDiffuse.rgb = (vDiffuse.rgb * vDiffuse.a) + (MIDesc.vTintColor.rgb * fRatio);
    //vDiffuse.a = 1.f;
    output.vDiffuse = MIDesc.vTintColor;
    
    float3 vNormal = input.vNormal;
    Compute_Normal(vNormal, input.vTangent, input.vBinormal, input.vUV);
    output.vNormal = vNormal * 0.5f + 0.5f;
    
    output.vDepth = float4(input.vProjPos.z / input.vProjPos.w, input.vProjPos.w, 0.f, 0.f);
    
    return output;
}

PS_OUT_BAKESHADOW PS_BAKESHADOW(PS_IN_MESH input)
{
    PS_OUT_BAKESHADOW output;
    output.vDepth = float4(input.vProjPos.z / input.vProjPos.w, input.vProjPos.w, 0.f, 0.f);
    return output;
}

PS_OUT PS_BLACK(PS_IN_MESH input)
{
    PS_OUT output;
    output.vColor = float4(1.f, 1.f, 1.f, 1.f);
    
    return output;
}

technique11 T0
{
	PASS_RS_DS_BS_VP(P0, RS_Default, DS_Default, BS_Default, VS_MAIN, PS_MAIN)
    PASS_RS_DS_BS_VP(P1, RS_Default_CullNone, DS_Default, BS_Default, VS_MAIN, PS_MAIN)
    PASS_RS_DS_BS_VP(P2, RS_Default, DS_Default, BS_Default, VS_MAIN, PS_MASK)
    PASS_RS_DS_BS_VP(P3, RS_Default, DS_Default, BS_Default, VS_MAIN, PS_MASKPIPE)
    PASS_RS_DS_BS_VP(SHADOW_BAKE, RS_Default, DS_Default, BS_Default, VS_MAIN, PS_BAKESHADOW)
	PASS_RS_DS_BS_VP(Debug, RS_Wire, DS_Default, BS_Default, VS_MAIN, PS_BLACK)
};