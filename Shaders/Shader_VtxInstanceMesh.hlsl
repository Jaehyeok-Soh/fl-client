#include "Struct_Defines.hlsl"
#include "Animation_Defines.hlsl"
#include "Light_Defines.hlsl"

#define CopicMask 0
#define SecondNormal 1

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

cbuffer CB_PlantData
{
    float g_fPlantDiffuseColorPower = 1.f;
    float3 g_fPlantDummy;
};

cbuffer CB_GrassData
{
    float   g_fGrassDT = 0.f;
    float   g_fGrassMaxHeight = 1.f; //이 모델의 잔디 MinMax중 Max의  Y값
    float   g_fGrassSwaySpeed = 1.f; //이 잔디가 Sway = 흔들리는 Speed
    float   g_fGrassWaveSize = 1.f; //이 잔디가 Power = 흔들리는 힘
};



Texture2D g_WaterTexture[MAX_WATER_TEXTURE_COUNT];
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
    
    return Out;
}


VS_OUT_INST_MESH VS_GRASS(VS_IN_INST_MESH input)
{
    VS_OUT_INST_MESH output = (VS_OUT_INST_MESH) 0;
    output.vPosition = mul(float4(input.vPosition, 1.f), input.matTransform);
    
    
    // ==========================================================
    // [기본 세팅] 비밀 무기 1: 곡선 벤딩 (Quadratic Bending)
    // ==========================================================
    output.vPosition = mul(float4(input.vPosition, 1.f), input.matTransform);
    
    // 기존: 뻣뻣한 선형(Linear) 벤딩
    float fLinearSway = saturate(input.vPosition.y / g_fGrassMaxHeight);
    
    // 변경: 선형 값을 한 번 더 곱해서(제곱) 부드러운 포물선 곡선으로 만듦!
    // 이렇게 하면 긴 풀도 밑동은 튼튼하게 버티고 끝부분만 찰랑거리게 됨.
    float fSwayWeight = fLinearSway * fLinearSway;
    
    float3 vGrassRootPos = float3(input.matTransform._41, input.matTransform._42, input.matTransform._43);

    // ==========================================================
    // [STEP 1] 플레이어 거리 계산 (기존과 동일)
    // ==========================================================
    float3 vPlayerPos = float3(tPlayerInfo.matWorld._41, tPlayerInfo.matWorld._42, tPlayerInfo.matWorld._43);
    float3 vDirToGrass = vGrassRootPos - vPlayerPos;
    vDirToGrass.y = 0.f;
    float fDist = length(vDirToGrass);
    float fProximity = 1.0f - smoothstep(0.0f, tPlayerInfo.fCollisionRange * 1.5f, fDist);

    // ==========================================================
    // [STEP 2] 자연 바람 (기존과 동일)
    // ==========================================================
    float2 vInstancePosXZ = float2(input.matTransform._41, input.matTransform._43);
    float fRandom = frac(sin(dot(vInstancePosXZ, float2(12.9898f, 78.233f))) * 43758.5453f);
    float fWindPhase = (g_fGrassDT * g_fGrassSwaySpeed) + (output.vPosition.x * g_fGrassWaveSize) + (output.vPosition.z * g_fGrassWaveSize) + (fRandom * 3.141592f);
    float fRandomPower = fWindPower * (0.5f + (fRandom * 0.5f));
    float fBaseSway = sin(fWindPhase) * fRandomPower;

    // ==========================================================
    //[STEP 3] 파닥거림 안정화 (비밀 무기 2 & 3)
    // ==========================================================
    float fSpeedRatio = saturate(tPlayerInfo.fCurSpeed / max(0.1f, tPlayerInfo.fMaxSpeed));
    fSpeedRatio = min(0.5f, fSpeedRatio);
    
    // 비밀 무기 3: 엇박자 노이즈 섞기 & 진동 속도 조절
    // 25.0f는 긴 풀에 너무 빨라서 15.0f로 낮춤. 버텍스 x좌표를 더해서 풀마다 다르게 떨게 만듦!
    float fRustlePhase = (g_fGrassDT * 15.0f) + (output.vPosition.x * 2.0f);
    
    // 비밀 무기 2: 강도 대폭 축소! (1.0f -> 0.15f)
    // 풀이 길기 때문에 0.15f만 줘도 끝부분은 충분히 찰지게 흔들림.
    float fRustleStrength = 1.f;
    float fRustleSway = sin(fRustlePhase) * (fProximity * fSpeedRatio * fRustleStrength);
    
    // ==========================================================
    // [STEP 4] 최종 적용 (데드존 포함)
    // ==========================================================
    float fTotalSway = fBaseSway + fRustleSway;
    float fCenterSafe = smoothstep(0.0f, 0.2f, fDist);
    
    output.vPosition.xyz += (vWindDirection * fTotalSway * fSwayWeight);
    
    float3 vPushDir = fDist > 0.0f ? normalize(vDirToGrass) : float3(1.f, 0.f, 0.f);
    
    // 밀어내는 힘도 긴 풀에 맞춰서 살짝(0.2f)으로 줄임
    output.vPosition.xyz += vPushDir * (fProximity * 0.2f * fCenterSafe) * fSwayWeight;
    
    
    //float3 vGrassRootPos = float3(input.matTransform._41, input.matTransform._42, input.matTransform._43);
    
    
    //float4 vLocalRoot = float4(input.vPosition.x, 0.0f, input.vPosition.z, 1.0f);
    //// 2. 이 로컬 뿌리를 월드 행렬과 곱해서 '진짜 개별 풀잎의 월드 뿌리'를 구한다!
    //float3 vBladeRootWorld = mul(vLocalRoot, input.matTransform).xyz;
    
    //// Step1 플레이어와 충돌처리
    //float3 vPlayerPos = float3(tPlayerInfo.matWorld._41, tPlayerInfo.matWorld._42, tPlayerInfo.matWorld._43); // 플레이어 위치좌표
    //float fYDiff = vGrassRootPos.y - vPlayerPos.y; // 플레이어의 중점좌표와 , 현재 Grass Vertex의 Y좌표 까지의 길이 월드상에서의 길이 값
    //float fYOverHead = max(0.f, fYDiff - tPlayerInfo.fCollisionHeight); //  방금 구한 월드상에서의 fYDiff 플레이어의 콜리전 Y높이빼서 길이를 구한다 만약 -라면 0 
    //float fUnderFeet = max(0.f, -fYDiff); // 월드상에서 fYDiff = 길이값을 구했을떄 - 였다면 - 를 곱해주어 +가 되고 그대로 값이 들어가고 아니면 0 이되겠지
    //float fVerticalOutDist = fYOverHead + fUnderFeet; // 캐릭터 몸통 구간을 벗어난 총 수직거리 둘중하나일테니까 둘다 해당하지않는다면 0 0 으로 그냥 기본값으로 꺽이게되고 그 이상 ,이하라면 그에따라 값을 처리해준다
   

    
    //float fPushStrength = 0.45f; //tPlayerInfo.fPushPower;
    //float fFalloffMargin = 0.5f; //tPlayerInfo.fMargin;
    //float fHeightFalloff = saturate(1.0f - (fVerticalOutDist / fFalloffMargin)); // Margin = 거리의 최대값 각 벗어난 범이의 최대값을 지정해주고 그 지정된값을 넘어가면 0으로 처리해준다.
    
    
    //float3 vDirPlayerToGrass = vGrassRootPos - vPlayerPos;
    //vDirPlayerToGrass.y = 0.f;
    //float fDistPlayerToGrassXZ = length(vDirPlayerToGrass);
    //float fPushPower = 1.0f - smoothstep(0.0f, tPlayerInfo.fCollisionRange * 1.5f, fDistPlayerToGrassXZ);
    
    //float3 vPushDir = fDistPlayerToGrassXZ > 0.0f ? normalize(vDirPlayerToGrass) : float3(1.f, 0.f, 0.f); // 밀릴 방향값
    //float fFinalInteractionPower = fPushPower * fHeightFalloff; //플레이어 와 충돌처리 끝나고의 Insteractive Power
    
    
    //// Step2 바람관련
    //// LocalY 좌표를 현재 모델의 Max의 Y좌표를 나눠주어 현재 Y값의 비율을 알려준다
    //float fSwayWeight = saturate(input.vPosition.y / g_fGrassMaxHeight);
    //float2 vInstancePosXZ = float2(input.matTransform._41, input.matTransform._43);
    //float fRandom = frac(sin(dot(vInstancePosXZ, float2(12.9898f, 78.233f))) * 43758.5453f);
    
    //float fWindPhase = (g_fGrassDT * g_fGrassSwaySpeed) + (output.vPosition.x * g_fGrassWaveSize) + (output.vPosition.z * g_fGrassWaveSize) + (fRandom * 3.141592f);
 
    //float fRandomPower = fWindPower * (0.5f + (fRandom * 0.5f));
    //float fSway = sin(fWindPhase) * fRandomPower;
    //float fWindAttenuation = 1.0f - fFinalInteractionPower;
    
    // 풀의 바람 연산 후 포지션
    //output.vPosition.xyz += (vWindDirection * fSway * fSwayWeight * fWindAttenuation);
    
    // 풀 충돌처리 연산 후 포지션
    //output.vPosition.xyz += vPushDir * fPushPower * fHeightFalloff * fSwayWeight * fPushStrength;
    
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
    PS_OUT_DEFFERED output = (PS_OUT_DEFFERED)0;
    
    float4 vDiffuse = float4(1.f, 1.f, 1.f, 1.f);
    
    Compute_Diffse(vDiffuse, input.vUV);
    vDiffuse.rgb *= MIDesc.vTintColor.rgb;
    output.vDiffuse = vDiffuse;
    
    float3 vNormal = input.vNormal;
    Compute_Normal(vNormal, input.vTangent, input.vBinormal, input.vUV);
    output.vNormal = float4(vNormal * 0.5f + 0.5f, 1.f);
    
    float3 vSpecMask = float3(1.f, 1.f, 0.f);
    if (Has(g_iMaterialMask, SPECULAR))
        vSpecMask = g_MaterialTextures[SPECULAR].Sample(LinearSampler, input.vUV).xyz;
    output.vSpecularMask = float4(vSpecMask, 1.f);
    output.vObjectInfo = PackObjectInfo(objectInfo.iObjectID, objectInfo.iFlags);
    output.vDepth = float4(input.vProjPos.z / input.vProjPos.w, input.vProjPos.w, 0.f, 0.f);
    
    
    if(output.vDiffuse.a < 0.1f)
        discard;
    
    
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
    
    //output.vDiffuse = Get_Modified_Diffuse(output.vDiffuse, input.iCurInstanceID);

    
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
    
    //output.vDiffuse = Get_Modified_Diffuse(output.vDiffuse, input.iCurInstanceID);
    
    return output;
}

PS_OUT_DEFFERED PS_VINE(PS_IN_INST_MESH input)
{
    PS_OUT_DEFFERED output = (PS_OUT_DEFFERED)0;
    
    float4 vDiffuse = 1.f;
    
    float4 vMask = float4(1.f, 1.f, 1.f, 1.f);
    
    if (Has(g_iMaterialMask, METALNESS))
        vMask = g_MaterialTextures[METALNESS].Sample(LinearSampler, input.vUV);

    if (vMask.r < 0.2f) 
        discard;
    
    float4 vMI_Color = MIDesc.vTintColor;
    output.vDiffuse = vMI_Color * vDiffuse.b;
    
    float3 vNormal = input.vNormal;
    Compute_Normal(vNormal, input.vTangent, input.vBinormal, input.vUV);
    output.vNormal = float4(vNormal * 0.5f + 0.5f, 1.f);
    
    output.vObjectInfo = PackObjectInfo(objectInfo.iObjectID, objectInfo.iFlags);
    output.vDepth = float4(input.vProjPos.z / input.vProjPos.w, input.vProjPos.w, 0.f, 0.f);
    
    //output.vDiffuse = Get_Modified_Diffuse(output.vDiffuse, input.iCurInstanceID);
    
    return output;
}

PS_OUT_DEFFERED PS_GRASS(PS_IN_INST_MESH input)
{
    PS_OUT_DEFFERED output;
    
    float4 vDiffuse = 1.f;
    
    Compute_Diffse(vDiffuse, input.vUV);
    
    if (length(vDiffuse.rgb) < 0.01f)
        discard;
    
    if (vDiffuse.a < 0.25f)
        discard;
    
    float3 vBaseColor = vDiffuse.rgb * MIDesc.vTintColor.rgb;
      
    float fLuminance = dot(vBaseColor, float3(0.299f, 0.587f, 0.114f));
    float fSaturationBoost = 1.0f + max(0.0f, (g_fPlantDiffuseColorPower - 1.0f) * 0.5f);
    float3 vVibrantColor = lerp(float3(fLuminance, fLuminance, fLuminance), vBaseColor, fSaturationBoost);
    
    vDiffuse.rgb = vVibrantColor * g_fPlantDiffuseColorPower;
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
    
    float3 vBaseColor = vDiffuse.rgb * MIDesc.vTintColor.rgb;
    float fLuminance = dot(vBaseColor, float3(0.299f, 0.587f, 0.114f));
    float fSaturationBoost = 1.0f + max(0.0f, (g_fPlantDiffuseColorPower - 1.0f) * 0.5f);
    float3 vVibrantColor = lerp(float3(fLuminance, fLuminance, fLuminance), vBaseColor, fSaturationBoost);
    
    vDiffuse.rgb = vVibrantColor * g_fPlantDiffuseColorPower;
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
	PASS_RS_DS_BS_VP(StaticObject, RS_Default_CullNone, DS_Default, BS_Default, VS_MAIN, PS_MAIN)
	
    PASS_RS_DS_BS_VP(LandScape, RS_Default_CullNone, DS_Default, BS_Default, VS_MAIN, PS_MAIN)

    // 식생
	PASS_RS_DS_BS_VP(Bush, RS_Default_CullNone, DS_Default, BS_Default, VS_MAIN, PS_BUSH)
	PASS_RS_DS_BS_VP(Grass, RS_Default_CullNone, DS_Default, BS_Default, VS_GRASS, PS_GRASS)
	PASS_RS_DS_BS_VP(Moss, RS_Default_CullNone, DS_Default, BS_Default, VS_MAIN, PS_MOSS)
	PASS_RS_DS_BS_VP(Tree, RS_Default_CullNone, DS_Default, BS_Default, VS_MAIN, PS_TREE)
	PASS_RS_DS_BS_VP(Vine, RS_Default_CullNone, DS_Default, BS_Default, VS_MAIN, PS_VINE)

    // 환경요소
	PASS_RS_DS_BS_VP(Rock, RS_Default_CullNone, DS_Default, BS_Default, VS_MAIN, PS_MAIN)
	PASS_RS_DS_BS_VP(Water, RS_Default_CullNone, DS_Default, BS_AlphaBlend, VS_MAIN, PS_WATER)

    // RGB mapping : weapon 쪽에서 쓰임
	PASS_RS_DS_BS_VP(RGBMapping, RS_Default_CullNone, DS_Default, BS_Default, VS_MAIN, PS_MAIN)

    //EXT
    PASS_RS_DS_BS_VP(SHADOW_BAKE, RS_Default, DS_Default, BS_Default, VS_MAIN, PS_MAIN)
	PASS_RS_DS_BS_VP(Debug, RS_Wire, DS_Default, BS_Default, VS_MAIN, PS_MAIN)
	PASS_RS_DS_BS_VP(SkyBox, RS_Wire, DS_ReadOnly , BS_Default, VS_MAIN, PS_MAIN)
};