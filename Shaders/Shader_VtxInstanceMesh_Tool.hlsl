#include "Struct_Defines.hlsl"
#include "Animation_Defines.hlsl"
#include "Light_Defines.hlsl"

#define CopicMask 0
#define SecondNormal 1

#define Default     0
#define Selected    1
#define Preview     2


#define Water_Normal_1    0
#define Water_Normal_2    1
#define Water_StarLight   2   
#define Water_Lighting    3
#define Water_Noise       4

#define Water_Deco1_D     5
#define Water_Deco1_N     6
#define Water_Deco1_ORH   7

#define Water_Deco2_D     8
#define Water_Deco2_N     9
#define Water_Deco2_ORH   10

#define Water_Deco3_D     11
#define Water_Deco3_N     12
#define Water_Deco3_ORH   13
#define MAX_WATER_TEXTURE_COUNT 14





bool HasCopickMask() { return (g_iGlobalMask & 1) != 0; }
bool HasSecNormal() { return (g_iGlobalMask & 2) != 0; }


cbuffer CB_EnvData
{
    float3 vWindDirection = float3(1.f, -1.f, 1.f); //바람이 부는 방향
    float fWindPower = 1.f; //바람이 부는 새기
};

cbuffer CB_GrassData
{
    float g_fGrassDT = 0.f;
    float g_fGrassMaxHeight = 1.f; //이 모델의 잔디 MinMax중 Max의  Y값
    float g_fGrassSwaySpeed = 1.f; //이 잔디가 Sway = 흔들리는 Speed
    float g_fGrassWaveSize = 1.f; //이 잔디가 Power = 흔들리는 힘
};



cbuffer CB_WaterData
{
// --- Register 0 ---
    uint g_WaterTexBindingFlags; // 4 Byte
    float g_fWaterDT; // 4 Byte
    float2 g_vWaterSpeed1; // 8 Byte
    
    // --- Register 1 ---
    float2 g_vWaterSpeed2; // 8 Byte
    float2 g_vWaterDistortionSpeed; // 8 Byte
    
    // --- Register 2 ---
    float2 g_vWaterUVPower; // 8 Byte (잔물결 촘촘함 조절) 전체적인 UV Tile Power
    float2 g_vWaterDistortionUVPower; // 8 Byte (노이즈 큼직함 조절) Noise UV Tile Power
    
    // --- Register 3 ---
    float g_fDistortionPower; // 4 Byte (왜곡 강도)
    
    float g_fSparklePower; // 4 Byte (윤슬 눈뽕 강도!)
    float2 g_vSparkleUVPower; // 8 Byte (윤슬 자글자글함 크기 조절!)  
};

Texture2D g_WaterTexture[MAX_WATER_TEXTURE_COUNT];


uint g_iMapObject_State = {0};
int g_iSelectInstanceID = { -1 };

float2 g_LandScape_TextureUV_LT = { 0.f , 0.f };
float2 g_LandScape_TextureUV_RB = { 1.f, 1.f };


cbuffer CB_DiscardColor
{
    float4 g_vDiscardColor;
};



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

VS_OUT_INST_MESH VS_GRASS(VS_IN_INST_MESH input)
{
    VS_OUT_INST_MESH output = (VS_OUT_INST_MESH)0;
    
    // LocalY 좌표를 현재 모델의 Max의 Y좌표를 나눠주어 현재 Y값의 비율을 알려준다
    float fSwayWeight = saturate(input.vPosition.y / g_fGrassMaxHeight);
    float2 vInstancePosXZ = float2(input.matTransform._41, input.matTransform._43);
    float fRandom = frac(sin(dot(vInstancePosXZ, float2(12.9898f, 78.233f))) * 43758.5453f);
    
    //월드 좌표
    output.vPosition = mul(float4(input.vPosition, 1.f), input.matTransform);
   
    
    // 괄호 안에는 '간격(Size)'을 곱하고!
    float fWindPhase = (g_fGrassDT * g_fGrassSwaySpeed) + (output.vPosition.x * g_fGrassWaveSize) + (output.vPosition.z * g_fGrassWaveSize) + (fRandom * 3.141592f);
    
    float fRandomPower = fWindPower * (0.5f + (fRandom * 0.5f));
    float fSway = sin(fWindPhase) * fRandomPower;
    
    output.vPosition.xyz += (vWindDirection * fSway * fSwayWeight);
    output.vWorldPos = output.vPosition;
    
    output.vPosition = mul(output.vPosition, VP);
    output.vUV = input.vUV;
    output.vNormal = normalize(mul(input.vNormal, (float3x3) input.matTransform));
    output.vTangent = normalize(mul(input.vTangent, (float3x3) input.matTransform));
    output.vBinormal = normalize(mul(input.vBinormal, (float3x3) input.matTransform));
    
    output.vProjPos = output.vPosition;
    
    return output;
}


PS_OUT_DEFFERED PS_MAIN(PS_IN_INST_MESH input)
{
    PS_OUT_DEFFERED output = (PS_OUT_DEFFERED) 0;
    
    float4 vDiffuse = float4(1.f, 1.f, 1.f, 1.f);
    
    Compute_Diffse(vDiffuse, input.vUV);
   
    if (vDiffuse.a < 0.3f)
        discard;
    
    vDiffuse.rgb *= MIDesc.vTintColor.rgb;
    output.vDiffuse = vDiffuse;
    
    float3 vNormal = input.vNormal;
    Compute_Normal(vNormal, input.vTangent, input.vBinormal, input.vUV);
    output.vNormal = float4(vNormal * 0.5f + 0.5f,1.f);
    
    float3 vSpecMask = float3(1.f, 1.f, 0.f);
    if (Has(g_iMaterialMask, SPECULAR))
        vSpecMask = g_MaterialTextures[SPECULAR].Sample(LinearSampler, input.vUV).xyz;
    output.vSpecularMask = float4(vSpecMask, 1.f);
    output.vObjectInfo = PackObjectInfo(objectInfo.iObjectID, objectInfo.iFlags);
    output.vDepth = float4(input.vProjPos.z / input.vProjPos.w, input.vProjPos.w, 0.f, 0.f);
   
    output.vDiffuse = Get_Modified_Diffuse(output.vDiffuse, input.iCurInstanceID);
    
    return output;
}

PS_OUT_DEFFERED PS_TREE(PS_IN_INST_MESH input)
{
    PS_OUT_DEFFERED output;
    
    float4 vDiffuse = float4(1.f, 1.f, 1.f, 1.f);
    
    Compute_Diffse(vDiffuse, input.vUV);
    
    if (length(vDiffuse.rgb) < 0.1f)
        discard;
    
    if (vDiffuse.a < 0.3f)
        discard;
    
    vDiffuse.rgb *= MIDesc.vTintColor.rgb;
    output.vDiffuse = vDiffuse;
    
    
    float3 vNormal = input.vNormal;
    Compute_Normal(vNormal, input.vTangent, input.vBinormal, input.vUV);
    output.vNormal = float4(vNormal * 0.5f + 0.5f, 1.f);
    
    float3 vSpecMask = float3(1.f, 1.f, 0.f);
    if (Has(g_iMaterialMask, METALNESS))
        vSpecMask = g_MaterialTextures[METALNESS].Sample(LinearSampler, input.vUV).xyz;
    output.vSpecularMask = float4(vSpecMask, 1.f);
    output.vObjectInfo = PackObjectInfo(objectInfo.iObjectID, objectInfo.iFlags);
    output.vDepth = float4(input.vProjPos.z / input.vProjPos.w, input.vProjPos.w, 0.f, 0.f);
    
    output.vDiffuse = Get_Modified_Diffuse(output.vDiffuse, input.iCurInstanceID);
    
    return output;
}

PS_OUT_DEFFERED PS_MOSS(PS_IN_INST_MESH input)
{
    PS_OUT_DEFFERED output;
    
    float4 vDiffuse = 1.f;
    Compute_Diffse(vDiffuse, input.vUV);
    
    if (vDiffuse.r < 0.3f) 
        discard;
    
    float4 vMI_Color = MIDesc.vTintColor;
    
    output.vDiffuse = vMI_Color * vDiffuse.b;
    
    float3 vNormal = input.vNormal;
    Compute_Normal(vNormal, input.vTangent, input.vBinormal, input.vUV);
    output.vNormal = float4(vNormal * 0.5f + 0.5f, 1.f);
    
    float3 vSpecMask = float3(1.f, 1.f, 0.f);
    if (Has(g_iMaterialMask, METALNESS))
        vSpecMask = g_MaterialTextures[METALNESS].Sample(LinearSampler, input.vUV).xyz;
    output.vSpecularMask = float4(vSpecMask, 1.f);
    output.vObjectInfo = PackObjectInfo(objectInfo.iObjectID, objectInfo.iFlags);
    output.vDepth = float4(input.vProjPos.z / input.vProjPos.w, input.vProjPos.w, 0.f, 0.f);
    
    output.vDiffuse = Get_Modified_Diffuse(output.vDiffuse, input.iCurInstanceID);
    
    return output;
}
PS_OUT_DEFFERED PS_VINE(PS_IN_INST_MESH input)
{
    PS_OUT_DEFFERED output;
    
    float4 vDiffuse = 1.f;
    
    float4 vMask = float4(1.f, 1.f, 1.f, 1.f);
    
    if (Has(g_iMaterialMask, METALNESS))
        vMask = g_MaterialTextures[METALNESS].Sample(LinearSampler, input.vUV);

    if (vMask.r < 0.2f) 
        discard;
    
    float4 vMI_Color = MIDesc.vTintColor;
    output.vDiffuse = vMI_Color * vMask.b;
    
    float3 vNormal = input.vNormal;
    Compute_Normal(vNormal, input.vTangent, input.vBinormal, input.vUV);
    output.vNormal = float4(vNormal * 0.5f + 0.5f, 1.f);
    
    output.vObjectInfo = PackObjectInfo(objectInfo.iObjectID, objectInfo.iFlags);
    output.vDepth = float4(input.vProjPos.z / input.vProjPos.w, input.vProjPos.w, 0.f, 0.f);
    output.vSpecularMask = float4(0.f, 0.f, 0.f, 0.f);
    
    output.vDiffuse = Get_Modified_Diffuse(output.vDiffuse, input.iCurInstanceID);
    
    return output;
}
PS_OUT_DEFFERED PS_GRASS(PS_IN_INST_MESH input)
{
    PS_OUT_DEFFERED output;
    
    float4 vDiffuse = 1.f;
    
    Compute_Diffse(vDiffuse, input.vUV);
    
    if (length(vDiffuse.rgb) < 0.1f)
        discard;
    
    if (vDiffuse.a < 0.3f)
        discard;
    
    vDiffuse.rgb *= MIDesc.vTintColor.rgb;
    output.vDiffuse = vDiffuse;
    
    
    float3 vNormal = input.vNormal;
    Compute_Normal(vNormal, input.vTangent, input.vBinormal, input.vUV);
    output.vNormal = float4(vNormal * 0.5f + 0.5f, 1.f);
    
    float3 vSpecMask = float3(1.f, 1.f, 0.f);
    if (Has(g_iMaterialMask, METALNESS))
        vSpecMask = g_MaterialTextures[METALNESS].Sample(LinearSampler, input.vUV).xyz;
    output.vSpecularMask = float4(vSpecMask, 1.f);
    output.vObjectInfo = PackObjectInfo(objectInfo.iObjectID, objectInfo.iFlags);
    output.vDepth = float4(input.vProjPos.z / input.vProjPos.w, input.vProjPos.w, 0.f, 0.f);
    
    output.vDiffuse = Get_Modified_Diffuse(output.vDiffuse, input.iCurInstanceID);
    
    return output;
}
PS_OUT_DEFFERED PS_BUSH(PS_IN_INST_MESH input)
{
    PS_OUT_DEFFERED output;
    
    float4 vDiffuse = 1.f;
    
    Compute_Diffse(vDiffuse, input.vUV);
    
    if (length(vDiffuse.rgb) < 0.1f)
        discard;
    
    if (vDiffuse.a < 0.3f)
        discard;
    
    vDiffuse.rgb *= MIDesc.vTintColor.rgb;
    output.vDiffuse = vDiffuse;
    
    float3 vNormal = input.vNormal;
    Compute_Normal(vNormal, input.vTangent, input.vBinormal, input.vUV);
    output.vNormal = float4(vNormal * 0.5f + 0.5f, 1.f);
    
    float3 vSpecMask = float3(1.f, 1.f, 0.f);
    
    if (Has(g_iMaterialMask, METALNESS))
    {
        vSpecMask = g_MaterialTextures[METALNESS].Sample(LinearSampler, input.vUV).xyz;
        
        if (vSpecMask.r < 0.3f)
            discard;
        
        if (length(vDiffuse.rgb) < 0.1f)
            discard;
    }
    
    output.vSpecularMask = float4(vSpecMask, 1.f);
    output.vObjectInfo = PackObjectInfo(objectInfo.iObjectID, objectInfo.iFlags);
    output.vDepth = float4(input.vProjPos.z / input.vProjPos.w, input.vProjPos.w, 0.f, 0.f);
    
    output.vDiffuse = Get_Modified_Diffuse(output.vDiffuse, input.iCurInstanceID);
    
    return output;
}


PS_OUT_DEFFERED PS_WATER(PS_IN_INST_MESH input)
{
    PS_OUT_DEFFERED output = (PS_OUT_DEFFERED) 0;
    
    // 기본 Tile UV
    float2 vTiledUV = input.vUV * g_vWaterUVPower;
    
    float4 vDiffuse = 1.f;
    vDiffuse = MIDesc.vTintColor.rgba; //기본색깔 입히기

    
    bool bHasAnyNormal = false;
    float3 vNormal = input.vNormal;
    float3 vLocalNormal = float3(0.0f, 0.0f, 1.0f);

    // Noise Texture 
    float2 vDistortion = float2(0.f, 0.f);
    float fDistortionPower = 0.f;
    if (Has(g_WaterTexBindingFlags, Water_Noise))
    {
        float2 vDistortionTiledUV = input.vUV * g_vWaterDistortionUVPower;
        float2 vDistortionUV = vDistortionTiledUV + float2(g_fWaterDT * g_vWaterDistortionSpeed.x, g_fWaterDT * g_vWaterDistortionSpeed.y);
        vDistortion = g_WaterTexture[Water_Noise].Sample(LinearSampler, vDistortionUV).rg * 2.f - 1.f;
        fDistortionPower = g_fDistortionPower;
    }
    
    // 1. 물결 1 계산
    if (Has(g_WaterTexBindingFlags, Water_Normal_1))
    {
        float2 uv1 = vTiledUV + float2(g_fWaterDT * g_vWaterSpeed1.x, g_fWaterDT * g_vWaterSpeed1.y);
        uv1 += (vDistortion * fDistortionPower);

        vLocalNormal = g_WaterTexture[Water_Normal_1].Sample(LinearSampler, uv1).xyz * 2.f - 1.f;
        
        bHasAnyNormal = true;
    }
    
    // 2. 물결 2 계산 (Speed2, Normal_2, uv2 사용!)
    if (Has(g_WaterTexBindingFlags, Water_Normal_2))
    {
        float2 uv2 = vTiledUV + float2(g_fWaterDT * g_vWaterSpeed2.x, g_fWaterDT * g_vWaterSpeed2.y);
        uv2 += (vDistortion * fDistortionPower);
        
        
        
        float3 n2 = g_WaterTexture[Water_Normal_2].Sample(LinearSampler, uv2).xyz * 2.f - 1.f;
        if (bHasAnyNormal)
            vLocalNormal += n2; // 1번이 이미 있으면 벡터를 더해서 섞어준다!
        else
            vLocalNormal = n2; // 1번이 없으면 그냥 2번을 쓴다.
                    
        bHasAnyNormal = true;
    }

    // 3. 월드 노멀로 변환
    if (bHasAnyNormal)
    {
        // 두 벡터를 더했으므로 길이가 1이 아닐 수 있음. 다시 정규화 필수!
        vLocalNormal = normalize(vLocalNormal);
        
        // TBN 행렬 생성
        float3x3 TBN = float3x3(normalize(input.vTangent), normalize(input.vBinormal), normalize(input.vNormal));
        
        // 로컬 노멀을 월드(또는 뷰) 노멀로 변환
        vNormal = normalize(mul(vLocalNormal, TBN));
    }
    else
    {
        // 바인딩된 텍스처가 아무것도 없으면 기본 함수 사용
        Compute_Normal(vNormal, input.vTangent, input.vBinormal, input.vUV);
    }
   
    output.vDiffuse = vDiffuse;
    output.vNormal = float4(vNormal * 0.5f + 0.5f, 1.f);
    
    
    float3 vSpecMask = float3(1.f, 1.f, 0.f);
    if (Has(g_WaterTexBindingFlags, Water_Lighting))
    {
        float2 vSparkleUV = input.vUV * g_vSparkleUVPower;
        vSparkleUV += float2(g_fWaterDT * g_vWaterSpeed1.x, g_fWaterDT * g_vWaterSpeed1.y);
        if (bHasAnyNormal)
            vSparkleUV += (vLocalNormal.xy * 0.3f);
        //vSparkleUV += (vDistortion * fDistortionPower);
        float fSparkle = g_WaterTexture[Water_Lighting].Sample(LinearSampler, vSparkleUV).r;
        
        vSpecMask.g -= (fSparkle * g_fSparklePower);
        vSpecMask.g = max(vSpecMask.g, 0.0f);
        
    }
    output.vSpecularMask = float4(vSpecMask, 1.f);
    output.vObjectInfo = PackObjectInfo(objectInfo.iObjectID, objectInfo.iFlags);
    output.vDepth = float4(input.vProjPos.z / input.vProjPos.w, input.vProjPos.w, 0.f, 0.f);
    
    //output.vDiffuse = Get_Modified_Diffuse(output.vDiffuse);
    
    return output;
}



technique11 T0
{
    // 기본 오브젝트
	PASS_RS_DS_BS_VP(StaticObject, RS_Default_CullNone, DS_Default, BS_Default, VS_MAIN, PS_MAIN)
	
    // 지형
    PASS_RS_DS_BS_VP(LandScape, RS_Default_CullNone, DS_Default, BS_Default, VS_MAIN, PS_MAIN)

    // 식생   절대 순서를 건들지 말것
	PASS_RS_DS_BS_VP(Bush,RS_Default_CullNone, DS_Default, BS_Default,VS_MAIN,PS_MAIN)
	PASS_RS_DS_BS_VP(Grass,RS_Default_CullNone,DS_Default,BS_Default,VS_GRASS,PS_GRASS)
	PASS_RS_DS_BS_VP(Moss,RS_Default_CullNone, DS_Default, BS_Default, VS_MAIN,PS_MOSS)
	PASS_RS_DS_BS_VP(Tree,RS_Default_CullNone, DS_Default, BS_Default, VS_MAIN,PS_TREE)
	PASS_RS_DS_BS_VP(Vine,RS_Default_CullNone, DS_Default, BS_Default, VS_MAIN,PS_VINE)

    // 환경요소
	PASS_RS_DS_BS_VP(Rock, RS_Default_CullNone, DS_Default, BS_Default, VS_MAIN ,PS_MAIN)
	PASS_RS_DS_BS_VP(Water, RS_Default_CullNone, DS_Default, BS_Default, VS_MAIN, PS_WATER)

    //EXT
    PASS_RS_DS_BS_VP(SHADOW_BAKE, RS_Default, DS_Default, BS_Default, VS_MAIN, PS_MAIN)
	PASS_RS_DS_BS_VP(Debug, RS_Wire, DS_Default, BS_Default, VS_MAIN, PS_MAIN)
};