#include "Struct_Defines.hlsl"
#include "Animation_Defines.hlsl"
#include "Light_Defines.hlsl"
#include "DissolveEffect_Defines.hlsl"

#define MAX_RGBA_TEXTURE_COUNT  2    


#define RGBA 0
#define RGB  1

#define R 0
#define G 1
#define B 2
#define A 3

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

#define RECTANGLE   0
#define SPHERE      1

cbuffer CB_EnvData
{
    float4 vEnvColor = float4(1.f, 1.f, 1.f, 1.f); // 환경 Color값
    /*  16Byte  */
    
    float3 vWindDirection = float3(1.f, -1.f, 1.f); // 바람이 부는 방향
    float fWindPower = 1.f; // 바람이 부는 새기
    /*  16Byte  */
    
    
    // SkyBox Setting 
    // 16 byte
    float4 vSkyColor = float4(1.f, 1.f, 1.f, 1.f); //16
    float4 vCloudBaseColor = float4(1.f, 1.f, 1.f, 1.f); //16
    float4 vCloudHighlight = float4(1.f, 1.f, 1.f, 1.f); //16
    float4 vCloudShadowColor = float4(1.f, 1.f, 1.f, 1.f); //16
    
    float fCloudHighlightPower = 1.f;
    float fCloudShadowPower = 1.f;
    float2 EnvDataDummy2;
    
    int isChannelPacking = false; // 4 Byte 채널 패킹 사용한건지 아닌건지 
    int iSkyBoxTextureType = RECTANGLE; // 4 Byte 기본 사각형
    float fPolarRadiusScale = 1.f; // 4 Byte 
    float EnvDataDummy; // 4 Byte
    /* 16Byte  */
    
    float2 vSkyBoxTextureUVSpeed = float2(1.f, 1.f); // 8 Byte UV Speed 
    float fEvnAccDT = 0.f; //4Byte
    float EnvDataDummy3; //4bytes (16바이트 정렬 맞춤용)
    /* 16 Byte */
};

cbuffer CB_PlantData
{
    float g_fPlantDiffuseColorPower = 1.f;
    float3 g_fPlantDummy;
};

cbuffer CB_GrassData
{
    float g_fGrassDT = 0.f;
    float g_fGrassMaxHeight = 1.f; //이 모델의 잔디 MinMax중 Max의  Y값
    float g_fGrassSwaySpeed = 1.f; //이 잔디가 Sway = 흔들리는 Speed
    float g_fGrassWaveSize = 1.f; //이 잔디가 Power = 흔들리는 힘
};


struct WaterRippleEffect
{
    // ---------------- [ 파동 기본 형태 조절 ] ----------------
    float fMaxRadius; // 파동이 퍼져나가는 최대 반경 (예: 15.0f)
    float fRippleFreq; // 파동의 촘촘함. 높을수록 물결이 좁아짐 (예: 8.0f)
    float fRippleSpeed; // 파동이 퍼져나가는 속도 (예: 5.0f)
    float fRippleAmp; // 파동의 최대 강도/높이 (예: 0.5f)
    
    float   fRingThickness;
    int     g_iActiveRippleCount;
    int     g_vPadding[2];
    // ★ 이름과 크기(20)를 C++에 맞춰서 변경!
    float4 g_arrayRipplesPos[20];
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
    
    
    
    
    // Ripple Data
    WaterRippleEffect tWaterRippleEffect;   // 16Byte 정렬완료
};


Texture2D       g_Base_Texture;

//  RGBA 맵에 맵핑될 DH , NBR Texture 2DTextureArray로 되어있다
Texture2DArray  g_Mix_DH_Tile_Texture;
Texture2DArray  g_Mix_NBR_Tile_Texture;

//  RGBA 맵 
Texture2D g_Mix_RGBA_Texture[MAX_RGBA_TEXTURE_COUNT];


struct MIX_RGBA_DATA
{
	// Mix할떄 UV좌표에 곱해주어 정밀한 표현을 담당해준다
    float4 fRGBA_Mix_Forces;
    
    // Mix해서 Height 값과 섞어서 영향력을 표현할 Height mix Force
    float4 fRGBA_Mix_Height_Force;
    
	// Mix될 RGBA 맵에서 각 R , G , B , A 가 연결된 Splating Texture들의 Index
    int4 iRGBA_Connected_Tile_Index;
	// Mix될 RGBA 맵에서 각 R , G , B , A 가 Splating을 사용할건지 안할건지에 대한 Flag값 false => BaseTexture가 그대로 들어감 true => Splating
    int4 iUseFlags;
};

cbuffer CB_MIX_RGBA_INFO
{
    MIX_RGBA_DATA g_MIX_RGBA_DATA[MAX_RGBA_TEXTURE_COUNT];
    
    int g_iUse_Mix_RGBA_Count = { 0 };
    int3 g_Use_Mix_RGBA_Map_Count_Dummy;
};

cbuffer cbLandScap_TextureUV
{
    float2 g_LandScape_TextureUV_LT = { 0.f, 0.f };
    float2 g_LandScape_TextureUV_RB = { 1.f, 1.f };
};

cbuffer CB_MAPPING_RGB
{
    float4 Color_R = { 1.f, 1.f, 1.f, 1.f };
    float4 Color_G = { 1.f, 1.f, 1.f, 1.f };
    float4 Color_B = { 1.f, 1.f, 1.f, 1.f };
};

void Apply_Dissolve_Discard_And_Alpha(
    in PS_IN_MESH input,
    inout float4 ioDiffuse)
{
    uint flags = g_DissolveEffect.g_iDissolveFlag;
    float amount = saturate(g_DissolveEffect.g_fDissolveAmount);

    if (!Is_Dissolve_Active())
        return;

    if (Should_Discard_By_Dissolve(input.vUV, flags, amount))
        discard;

    if (Has_DissolveFlag(flags, DF_USE_ALPHAFADE))
    {
        float alpha = Get_DissolveAlphaFactor(flags, amount);
        ioDiffuse.a *= alpha;

        if (ioDiffuse.a <= EPSILON)
            discard;
    }
}

float3 Get_DissolveEdgeEmissive(float2 uv)
{
    uint flags = g_DissolveEffect.g_iDissolveFlag;
    float amount = saturate(g_DissolveEffect.g_fDissolveAmount);

    if (!Is_Dissolve_Active())
        return float3(0.f, 0.f, 0.f);

    if (!Has_DissolveFlag(flags, DF_USEEDGE))
        return float3(0.f, 0.f, 0.f);

    float edge = Compute_DissolveEdge(
        uv,
        flags,
        amount,
        g_DissolveEffect.g_fDissolveEdgeWidth);

    return g_DissolveEffect.g_vDissolveEdgeColor * edge * 3.0f;
}

float3 MixMultiply(float3 vColor1, float3 vColor2, float factor)
{
    float3 multiplied = vColor1 * vColor2;
    return lerp(vColor1, multiplied, factor);
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

VS_OUT_MESH VS_SKYBOX(VS_IN_MESH input)
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
    output.vPosition = output.vPosition.xyww;
    return output;
}

VS_OUT_MESH VS_GRASS(VS_IN_MESH input)
{
    VS_OUT_MESH output = (VS_OUT_MESH) 0;
    output.vPosition = mul(float4(input.vPosition, 1.f) , W);
    
    // ==========================================================
    // [기본 세팅] 비밀 무기 1: 곡선 벤딩 (Quadratic Bending)
    // ==========================================================
    output.vPosition = mul(float4(input.vPosition, 1.f), W);
    
    // 기존: 뻣뻣한 선형(Linear) 벤딩
    float fLinearSway = saturate(input.vPosition.y / g_fGrassMaxHeight);
    
    // 변경: 선형 값을 한 번 더 곱해서(제곱) 부드러운 포물선 곡선으로 만듦!
    // 이렇게 하면 긴 풀도 밑동은 튼튼하게 버티고 끝부분만 찰랑거리게 됨.
    float fSwayWeight = fLinearSway * fLinearSway;
    
    float3 vGrassRootPos = float3(W._41, W._42, W._43);

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
    float2 vInstancePosXZ = float2(W._41, W._43);
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
    
    output.vWorldPos = output.vPosition;
    output.vPosition = mul(output.vPosition, VP);
    output.vUV = input.vUV;
    output.vNormal = normalize(mul(input.vNormal, (float3x3) W));
    output.vTangent = normalize(mul(input.vTangent, (float3x3) W));
    output.vBinormal = normalize(mul(input.vBinormal, (float3x3) W));
    
    output.vProjPos = output.vPosition;
    
    return output;
}

VS_OUT_SHADOW VS_SHADOW(VS_IN_MESH input)
{
    VS_OUT_SHADOW output;
    float4 vWorldPos = mul(float4(input.vPosition, 1.f), W);
    output.vPosition = mul(vWorldPos, bakedShadowParam.matLightVP);
    return output;
}

PS_OUT_DEFFERED PS_MAIN(PS_IN_MESH input)
{
    PS_OUT_DEFFERED output;
    
    float4 vDiffuse = float4(1.f, 1.f, 1.f, 1.f);
    
    Compute_Diffse(vDiffuse, input.vUV);
    
    if (vDiffuse.a < 0.3f)
        discard;
    
    vDiffuse.rgb *= MIDesc.vTintColor.rgb;
    output.vDiffuse = vDiffuse;
    
    
    float3 vNormal = input.vNormal;
    Compute_Normal(vNormal, input.vTangent, input.vBinormal, input.vUV);
    output.vNormal = float4(vNormal * 0.5f + 0.5f, 1.f);
    
    float3 vSpecMask = DEFAULT_SPECMASK_FLOAT3;
    if (Has(g_iMaterialMask, METALNESS))
        vSpecMask = g_MaterialTextures[METALNESS].Sample(LinearSampler, input.vUV).xyz;
    output.vSpecularMask = float4(vSpecMask, 1.f);
    output.vObjectInfo = PackObjectInfo(objectInfo.iObjectID, objectInfo.iFlags);
    output.vDepth = float4(input.vProjPos.z / input.vProjPos.w, input.vProjPos.w, 0.f, 0.f);
    float3 vEmissive = float3(0.f, 0.f, 0.f);
    if (Has(g_iMaterialMask, EMISSIVE))
    {
        vEmissive = g_MaterialTextures[EMISSIVE].Sample(LinearSampler, input.vUV).xyz;
        float fMask = max(vEmissive.r, max(vEmissive.g, vEmissive.b));
        vEmissive = output.vDiffuse.rgb * fMask * 4.5f;
    }
    output.vEmissive = float4(vEmissive, 1.f);
    
    return output;
}

PS_OUT_DEFFERED PS_ROCK(PS_IN_MESH input)
{
    PS_OUT_DEFFERED output;
    
    float4 vDiffuse = float4(1.f, 1.f, 1.f, 1.f);
    
    Compute_Diffse(vDiffuse, input.vUV);
    
    if (vDiffuse.a < 0.3f)
        discard;
    
    vDiffuse.rgb *= MIDesc.vTintColor.rgb;
    output.vDiffuse = vDiffuse;
    
    
    float3 vNormal = input.vNormal;
    Compute_Normal(vNormal, input.vTangent, input.vBinormal, input.vUV);
    output.vNormal = float4(vNormal * 0.5f + 0.5f, 1.f);
    
    
    if (Has(g_iMaterialMask, METALNESS))
    {
        // 1. 마스크 텍스처 가져오기
        float3 vSpecMask = g_MaterialTextures[METALNESS].Sample(LinearSampler, input.vUV).xyz;
        
        // 2. 푹 파인 곳(그림자 질 곳) 마스크만 딱 뽑기
        float rockCavityMask = vSpecMask.g;
        
        // 3. 잡다한 색깔 다 빼고, 원래 질감(vDiffuse)에 곱해서 어둡게만 만들기 끝!
        // (그림자가 너무 새까맣게 타면 rockCavityMask 뒤에 * 0.7f 정도만 곱해주세요)
        vDiffuse.rgb *= (1.0f - rockCavityMask);
    }
    
    output.vSpecularMask = float4(DEFAULT_SPECMASK_FLOAT3, 1.f);
    output.vObjectInfo = PackObjectInfo(objectInfo.iObjectID, objectInfo.iFlags);
    output.vDepth = float4(input.vProjPos.z / input.vProjPos.w, input.vProjPos.w, 0.f, 0.f);
    float3 vEmissive = float3(0.f, 0.f, 0.f);
    if (Has(g_iMaterialMask, EMISSIVE))
    {
        vEmissive = g_MaterialTextures[EMISSIVE].Sample(LinearSampler, input.vUV).xyz;
        float fMask = max(vEmissive.r, max(vEmissive.g, vEmissive.b));
        vEmissive = output.vDiffuse.rgb * fMask * 4.5f;
    }
    output.vEmissive = float4(vEmissive, 1.f);
    
    return output;
}

PS_OUT_DEFFERED PS_LANDSCAPE(PS_IN_MESH input)
{
    PS_OUT_DEFFERED output = (PS_OUT_DEFFERED) 0;
    
    float2 FilpUV = float2(input.vUV.x, 1.0 - input.vUV.y);
    float2 Size = g_LandScape_TextureUV_RB - g_LandScape_TextureUV_LT;
    float2 vUV = g_LandScape_TextureUV_LT + (Size * FilpUV);
    float2 vTileUV = vUV;
    
    float4 vDiffuse = float4(MIDesc.vTintColor.rgb, 1.f);
    
    float3 vNBR_Tile_TangentNormal = float3(0.0f, 0.0f, 1.0f);
    float fNBR_Tile_Roughness = 0.5f;
    
    
    // 각각 Texture를 지형 Index에 맞는 UV에로 치환
    float4 vBaseColor = g_Base_Texture.Sample(LinearSampler, vUV);

    float4 vMixRGBA, vRGBA_DH_RedTile, vRGBA_DH_GreenTile, vRGBA_DH_BlueTile, vRGBA_DH_AlphaTile;
    float4 vRGBA_NBR_RedTile, vRGBA_NBR_GreenTile, vRGBA_NBR_BlueTile, vRGBA_NBR_AlphaTile;
    
    vDiffuse = vBaseColor;
    
    [unroll]
    for (int i = 0; i < g_iUse_Mix_RGBA_Count; ++i)
    {
        
        vMixRGBA = g_Mix_RGBA_Texture[i].Sample(LinearSampler, vUV);
        MIX_RGBA_DATA Data = g_MIX_RGBA_DATA[i];

        if (Data.iUseFlags.r == 1 && vMixRGBA.r > 0.0f)
        {
            float3 vTileUV_R = float3(vTileUV * Data.fRGBA_Mix_Forces.r, Data.iRGBA_Connected_Tile_Index.r);
            vRGBA_DH_RedTile = g_Mix_DH_Tile_Texture.Sample(LinearSampler, vTileUV_R);
            vRGBA_NBR_RedTile = g_Mix_NBR_Tile_Texture.Sample(LinearSampler, vTileUV_R);
          
            float fLayerHeight = vRGBA_NBR_RedTile.b;
            float fWeight = vMixRGBA.r;
            float fMixHeight = lerp(1.0f, fLayerHeight, Data.fRGBA_Mix_Height_Force.r);
            float fBlendFactor = smoothstep(0.1f, 0.9f, (fWeight * fMixHeight) + fWeight);
            
            vDiffuse = lerp(vDiffuse, vRGBA_DH_RedTile, fBlendFactor);
            
            float3 vNormal_R = { 0.f, 0.f, 0.f };
            vNormal_R.x = vRGBA_NBR_RedTile.r * 2.f - 1.f;
            vNormal_R.y = vRGBA_NBR_RedTile.g * 2.f - 1.f;
            vNormal_R.z = sqrt(max(1.f - (vNormal_R.x * vNormal_R.x) - (vNormal_R.y * vNormal_R.y), 0.f));
            vNBR_Tile_TangentNormal = lerp(vNBR_Tile_TangentNormal, vNormal_R, fBlendFactor);
            fNBR_Tile_Roughness = lerp(fNBR_Tile_Roughness, vRGBA_NBR_RedTile.a, fBlendFactor);
        }

        if (Data.iUseFlags.g == 1 && vMixRGBA.g > 0.0f)
        {
            float3 vTileUV_G = float3(vTileUV * Data.fRGBA_Mix_Forces.g, Data.iRGBA_Connected_Tile_Index.g);
            vRGBA_DH_GreenTile = g_Mix_DH_Tile_Texture.Sample(LinearSampler, vTileUV_G);
            vRGBA_NBR_GreenTile = g_Mix_NBR_Tile_Texture.Sample(LinearSampler, vTileUV_G);
            
            float fLayerHeight = vRGBA_NBR_GreenTile.b;
            float fWeight = vMixRGBA.g;
            float fMixHeight = lerp(0.f, fLayerHeight, Data.fRGBA_Mix_Height_Force.g);
            float fBlendFactor = smoothstep(0.1f, 0.9f, (fWeight * fMixHeight) + fWeight);
            
            vDiffuse = lerp(vDiffuse, vRGBA_DH_GreenTile, fBlendFactor);

            
            float3 vNormal = { 0.f, 0.f, 0.f };
            vNormal.x = vRGBA_NBR_GreenTile.r * 2.f - 1.f;
            vNormal.y = vRGBA_NBR_GreenTile.g * 2.f - 1.f;
            vNormal.z = sqrt(max(1.f - (vNormal.x * vNormal.x) - (vNormal.y * vNormal.y), 0.f));
            vNBR_Tile_TangentNormal = lerp(vNBR_Tile_TangentNormal, vNormal, fBlendFactor);
            fNBR_Tile_Roughness = lerp(fNBR_Tile_Roughness, vRGBA_NBR_GreenTile.a, fBlendFactor);
        }

        if (Data.iUseFlags.b == 1 && vMixRGBA.b > 0.0f)
        {
            float3 vTileUV_B = float3(vTileUV * Data.fRGBA_Mix_Forces.b, Data.iRGBA_Connected_Tile_Index.b);
            vRGBA_DH_BlueTile = g_Mix_DH_Tile_Texture.Sample(LinearSampler, vTileUV_B);
            vRGBA_NBR_BlueTile = g_Mix_NBR_Tile_Texture.Sample(LinearSampler, vTileUV_B);

            float fLayerHeight = vRGBA_NBR_BlueTile.b;
            float fWeight = vMixRGBA.b;
            float fMixHeight = lerp(0.0f, fLayerHeight, Data.fRGBA_Mix_Height_Force.b);
            float fBlendFactor = smoothstep(0.1f, 0.9f, (fWeight * fMixHeight) + fWeight);
            
            
            vDiffuse = lerp(vDiffuse, vRGBA_DH_BlueTile, fBlendFactor);
            
            float3 vNormal = { 0.f, 0.f, 0.f };
            vNormal.x = vRGBA_NBR_BlueTile.r * 2.f - 1.f;
            vNormal.y = vRGBA_NBR_BlueTile.g * 2.f - 1.f;
            vNormal.z = sqrt(max(1.f - (vNormal.x * vNormal.x) - (vNormal.y * vNormal.y), 0.f));
            vNBR_Tile_TangentNormal = lerp(vNBR_Tile_TangentNormal, vNormal, fBlendFactor);
            fNBR_Tile_Roughness = lerp(fNBR_Tile_Roughness, vRGBA_NBR_BlueTile.a, fBlendFactor);
        }

        if (Data.iUseFlags.a == 1 && vMixRGBA.a > 0.0f)
        {
            float3 vTileUV_A = float3(vTileUV * Data.fRGBA_Mix_Forces.a, Data.iRGBA_Connected_Tile_Index.a);
            vRGBA_DH_AlphaTile = g_Mix_DH_Tile_Texture.Sample(LinearSampler, vTileUV_A);
            vRGBA_NBR_AlphaTile = g_Mix_NBR_Tile_Texture.Sample(LinearSampler, vTileUV_A);
            
            float fLayerHeight = vRGBA_NBR_AlphaTile.b;
            float fWeight = vMixRGBA.a;
            float fMixHeight = lerp(0.0f, fLayerHeight, Data.fRGBA_Mix_Height_Force.a);
            float fBlendFactor = smoothstep(0.1f, 0.9f, (fWeight * fMixHeight) + fWeight);
            
            
            vDiffuse = lerp(vDiffuse, vRGBA_DH_AlphaTile, fBlendFactor);
            
            float3 vNormal = { 0.f, 0.f, 0.f };
            vNormal.x = vRGBA_NBR_AlphaTile.r * 2.f - 1.f;
            vNormal.y = vRGBA_NBR_AlphaTile.g * 2.f - 1.f;
            vNormal.z = sqrt(max(1.f - (vNormal.x * vNormal.x) - (vNormal.y * vNormal.y), 0.f));
            vNBR_Tile_TangentNormal = lerp(vNBR_Tile_TangentNormal, vNormal, fBlendFactor);
            fNBR_Tile_Roughness = lerp(fNBR_Tile_Roughness, vRGBA_NBR_AlphaTile.a, fBlendFactor);
        }
    }
    
    vNBR_Tile_TangentNormal = normalize(vNBR_Tile_TangentNormal);
    
    float3 vNormal = normalize(input.vNormal);
    float3 vTangent = normalize(input.vTangent);
    float3 vBinormal = normalize(input.vBinormal);
    float3x3 TBN = float3x3(vTangent, vBinormal, vNormal);
    float3 finalWorldNormal = normalize(mul(vNBR_Tile_TangentNormal, TBN));
    
    output.vDiffuse = vDiffuse;
    output.vDepth = float4(input.vProjPos.z / input.vProjPos.w, input.vProjPos.w, 0.f, 0.f);
    //output.vNormal = float4(1.f,0.f,0.f,0.f);
    output.vNormal = float4(finalWorldNormal * 0.5f + 0.5f, 1.f);
    output.vSpecularMask = DEFAULT_SPECMASK_FLOAT4;
    output.vSpecularMask.y = fNBR_Tile_Roughness;
    //output.vSpecularMask = float4(1.f, 0.f, 0.f, 0.f);
    
    return output;
}


PS_OUT_DEFFERED PS_TREE(PS_IN_MESH input)
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
    
    float3 vSpecMask = DEFAULT_SPECMASK_FLOAT3;
    if (Has(g_iMaterialMask, METALNESS))
        vSpecMask = g_MaterialTextures[METALNESS].Sample(LinearSampler, input.vUV).xyz;
    output.vSpecularMask = float4(vSpecMask, 1.f);
    output.vObjectInfo = PackObjectInfo(objectInfo.iObjectID, objectInfo.iFlags);
    output.vDepth = float4(input.vProjPos.z / input.vProjPos.w, input.vProjPos.w, 0.f, 0.f);
    float3 vEmissive = float3(0.f, 0.f, 0.f);
    if (Has(g_iMaterialMask, EMISSIVE))
    {
        vEmissive = g_MaterialTextures[EMISSIVE].Sample(LinearSampler, input.vUV).xyz;
        float fMask = max(vEmissive.r, max(vEmissive.g, vEmissive.b));
        vEmissive = output.vDiffuse.rgb * fMask * 4.5f;
    }
    output.vEmissive = float4(vEmissive, 1.f);
    
    return output;
}

PS_OUT_DEFFERED PS_MOSS(PS_IN_MESH input)
{
    PS_OUT_DEFFERED output;
    
    float4 vDiffuse = 1.f;
    Compute_Diffse(vDiffuse, input.vUV);
    
    if (vDiffuse.r < 0.1f) 
        discard;
    
    float4 vMI_Color = MIDesc.vTintColor;
    
    output.vDiffuse = vMI_Color * vDiffuse.b;
    
    float3 vNormal = input.vNormal;
    Compute_Normal(vNormal, input.vTangent, input.vBinormal, input.vUV);
    output.vNormal = float4(vNormal * 0.5f + 0.5f, 1.f);
    
    float3 vSpecMask = DEFAULT_SPECMASK_FLOAT3;
    if (Has(g_iMaterialMask, METALNESS))
        vSpecMask = g_MaterialTextures[METALNESS].Sample(LinearSampler, input.vUV).xyz;
    output.vSpecularMask = float4(vSpecMask, 1.f);
    output.vObjectInfo = PackObjectInfo(objectInfo.iObjectID, objectInfo.iFlags);
    output.vDepth = float4(input.vProjPos.z / input.vProjPos.w, input.vProjPos.w, 0.f, 0.f);
    float3 vEmissive = float3(0.f, 0.f, 0.f);
    if (Has(g_iMaterialMask, EMISSIVE))
    {
        vEmissive = g_MaterialTextures[EMISSIVE].Sample(LinearSampler, input.vUV).xyz;
        float fMask = max(vEmissive.r, max(vEmissive.g, vEmissive.b));
        vEmissive = output.vDiffuse.rgb * fMask * 4.5f;
    }
    output.vEmissive = float4(vEmissive, 1.f);
   
    //output.vDiffuse = Get_Modified_Diffuse(output.vDiffuse);

    return output;
}

PS_OUT_DEFFERED PS_VINE(PS_IN_MESH input)
{
    PS_OUT_DEFFERED output = (PS_OUT_DEFFERED)0;
    
    float4 vDiffuse = 1.f;
    
    float4 vMask = DEFAULT_SPECMASK_FLOAT4;
    
    if (Has(g_iMaterialMask, METALNESS))
        vMask = g_MaterialTextures[METALNESS].Sample(LinearSampler, input.vUV);

    if (vMask.r < 0.3f) 
        discard;
    
    float4 vMI_Color = MIDesc.vTintColor;
    output.vDiffuse = vMI_Color * vDiffuse.b;
    
    float3 vNormal = input.vNormal;
    Compute_Normal(vNormal, input.vTangent, input.vBinormal, input.vUV);
    output.vNormal = float4(vNormal * 0.5f + 0.5f, 1.f);
    
    output.vObjectInfo = PackObjectInfo(objectInfo.iObjectID, objectInfo.iFlags);
    output.vDepth = float4(input.vProjPos.z / input.vProjPos.w, input.vProjPos.w, 0.f, 0.f);
    float3 vEmissive = float3(0.f, 0.f, 0.f);
    if (Has(g_iMaterialMask, EMISSIVE))
    {
        vEmissive = g_MaterialTextures[EMISSIVE].Sample(LinearSampler, input.vUV).xyz;
        float fMask = max(vEmissive.r, max(vEmissive.g, vEmissive.b));
        vEmissive = output.vDiffuse.rgb * fMask * 4.5f;
    }
    output.vEmissive = float4(vEmissive, 1.f);
    
    //output.vDiffuse = Get_Modified_Diffuse(output.vDiffuse, input.iCurInstanceID);
    
    return output;
}

PS_OUT_DEFFERED PS_GRASS(PS_IN_MESH input)
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
    
    float3 vSpecMask = DEFAULT_SPECMASK_FLOAT3;
    if (Has(g_iMaterialMask, METALNESS))
        vSpecMask = g_MaterialTextures[METALNESS].Sample(LinearSampler, input.vUV).xyz;
    output.vSpecularMask = float4(vSpecMask, 1.f);
    output.vObjectInfo = PackObjectInfo(objectInfo.iObjectID, objectInfo.iFlags);
    output.vDepth = float4(input.vProjPos.z / input.vProjPos.w, input.vProjPos.w, 0.f, 0.f);
    float3 vEmissive = float3(0.f, 0.f, 0.f);
    if (Has(g_iMaterialMask, EMISSIVE))
    {
        vEmissive = g_MaterialTextures[EMISSIVE].Sample(LinearSampler, input.vUV).xyz;
        float fMask = max(vEmissive.r, max(vEmissive.g, vEmissive.b));
        vEmissive = output.vDiffuse.rgb * fMask * 4.5f;
    }
    output.vEmissive = float4(vEmissive, 1.f);
    
    //output.vDiffuse = Get_Modified_Diffuse(output.vDiffuse);
    
    return output;
}

PS_OUT_DEFFERED PS_BUSH(PS_IN_MESH input)
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
    
    float3 vSpecMask = DEFAULT_SPECMASK_FLOAT3;
    
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
    float3 vEmissive = float3(0.f, 0.f, 0.f);
    if (Has(g_iMaterialMask, EMISSIVE))
    {
        vEmissive = g_MaterialTextures[EMISSIVE].Sample(LinearSampler, input.vUV).xyz;
        float fMask = max(vEmissive.r, max(vEmissive.g, vEmissive.b));
        vEmissive = output.vDiffuse.rgb * fMask * 4.5f;
    }
    output.vEmissive = float4(vEmissive, 1.f);
    
    //output.vDiffuse = Get_Modified_Diffuse(output.vDiffuse);
    
    return output;
}

PS_OUT_DEFFERED PS_RGBMAPPING(PS_IN_MESH input)
{
    PS_OUT_DEFFERED output;
    
    float4 vDiffuse = float4(1.f, 1.f, 1.f, 1.f);
    
    Compute_Diffse(vDiffuse, input.vUV);

    float3 final =
        (vDiffuse.r * Color_R.rgb) +
        (vDiffuse.g * Color_G.rgb) +
        (vDiffuse.b * Color_B.rgb);
    
    float luminance = dot(final, float3(0.3, 0.59, 0.11));

    float3 finalRGB = final * luminance;
    
    float4 finalDiffuse = float4(saturate(finalRGB), vDiffuse.a);
    
    saturate(finalDiffuse);
    
    output.vDiffuse = finalDiffuse;
    
    // 디졸브 디스카드
    Apply_Dissolve_Discard_And_Alpha(input, output.vDiffuse);
    
    float3 vNormal = input.vNormal;
    Compute_Normal(vNormal, input.vTangent, input.vBinormal, input.vUV);
    output.vNormal = float4(vNormal * 0.5f + 0.5f, 1.f);
    
    float3 vSpecMask = DEFAULT_SPECMASK_FLOAT3;
    if (Has(g_iMaterialMask, METALNESS))
        vSpecMask = g_MaterialTextures[METALNESS].Sample(LinearSampler, input.vUV).xyz;
    output.vSpecularMask = float4(vSpecMask, 1.f);
    output.vObjectInfo = PackObjectInfo(objectInfo.iObjectID, objectInfo.iFlags);
    output.vDepth = float4(input.vProjPos.z / input.vProjPos.w, input.vProjPos.w, 0.f, 0.f);
    float3 vEmissive = float3(0.f, 0.f, 0.f);
    if (Has(g_iMaterialMask, EMISSIVE))
    {
        vEmissive = g_MaterialTextures[EMISSIVE].Sample(LinearSampler, input.vUV).xyz;
        float fMask = max(vEmissive.r, max(vEmissive.g, vEmissive.b));
        vEmissive = output.vDiffuse.rgb * fMask * 4.5f;
    }
    
    // dissolve edge emissive
    vEmissive += Get_DissolveEdgeEmissive(input.vUV);
    
    output.vEmissive = float4(vEmissive, 1.f);
    
    return output;
}



PS_OUT_DEFFERED PS_WATER(PS_IN_MESH input)
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
    
    
    float3 vPlayerPos = tPlayerInfo.matWorld._41_42_43;
    
    float fDist = distance(input.vWorldPos.xz, vPlayerPos.xz);
    float fMaxRadius = 0.5f;
  
 
    
    float2 vTotalSlope = float2(0.f, 0.f);
    bool bHasRippleNormal = false;

    //if(tWaterRippleEffect.g_vPadding[0] == true)
    //{
        for (int i = 0; i < tWaterRippleEffect.g_iActiveRippleCount; ++i)
        {
            float3 vRipplePos = tWaterRippleEffect.g_arrayRipplesPos[i].xyz;
            float fRippleAge = tWaterRippleEffect.g_arrayRipplesPos[i].w;

            float fDist = distance(input.vWorldPos.xz, vRipplePos.xz);
        
            float fRandomSeed = frac(sin(dot(vRipplePos.xz, float2(12.9898f, 78.233f))) * 43758.5453f);
        
            float fRandomMaxRadius = tWaterRippleEffect.fMaxRadius * lerp(0.7f, 1.3f, fRandomSeed);
          
        // 1. 현재 파동이 퍼져나간 '반지름' (테두리의 위치)
            float fCurrentRadius = fRippleAge * tWaterRippleEffect.fRippleSpeed;
        
        // 2. 현재 픽셀이 그 테두리에서 얼마나 떨어져 있는가? (핵심!)
            float fDistFromRing = fDist - fCurrentRadius;
        
        // 3. 파동의 두께 (이 두께 안에서만 파동이 존재함)
            float fRingThickness = tWaterRippleEffect.fRingThickness;
        
        // 현재 픽셀이 링 두께 안에 있을 때만 연산
            if (fDist < fRandomMaxRadius && abs(fDistFromRing) < fRingThickness)
            {
            // 4. 뚝 끊기지 않게 테두리를 부드럽게 풀어줌 (smoothstep 사용)
                float fRingFade = smoothstep(fRingThickness, 0.0f, abs(fDistFromRing));

            // 5. 나이가 들수록 투명해짐 (예: 3초 뒤 소멸)
                float fAgeFade = saturate(1.0f - (fCurrentRadius / fRandomMaxRadius));
            // fRippleAge를 곱하던 걸 빼버리고, 
            // "테두리와의 거리차이(fDistFromRing)"에만 의존해서 sin 곡선을 그림!
            // 이렇게 하면 파동의 형태가 링의 확장을 완벽하게 똑같이 따라감.
                float fSlope = sin(fDistFromRing * tWaterRippleEffect.fRippleFreq) * tWaterRippleEffect.fRippleAmp;
            
            // 페이드 적용
                fSlope *= (fRingFade * fAgeFade);

            // 7. 방향 적용
                float2 vDirXZ = input.vWorldPos.xz - vRipplePos.xz;
                if (length(vDirXZ) > 0.001f)
                {
                    float2 vDirFromCenter = normalize(vDirXZ);
                
                    vTotalSlope += (vDirFromCenter * fSlope);
                    bHasRippleNormal = true;
                }
            }
        }
    //}
    
    if (bHasRippleNormal)
    {
        vLocalNormal.xy += vTotalSlope;
        bHasAnyNormal = true; // 파동이 생겼으니 노멀 맵 연산을 하도록 플래그 ON!
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
    
    float3 vSpecMask = DEFAULT_SPECMASK_FLOAT3;
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


PS_OUT_BACKBUFFER PS_SKYBOX(PS_IN_MESH input)
{
    PS_OUT_BACKBUFFER output;
    
    float4 vDiffuse = float4(1.f, 1.f, 1.f, 1.f);
        
    // 1. 시간에 따른 UV 애니메이션 적용
    float2 animUV = input.vUV + vSkyBoxTextureUVSpeed * fEvnAccDT;
    
    // 최종적으로 샘플링할 UV를 담을 변수
    float2 finalUV;
    
    if (iSkyBoxTextureType == SPHERE)
    {
        // [핵심] 둥근 텍스처는 U축(x) 애니메이션만 각도(회전)로 사용하고, 
        // V축(y)은 애니메이션이 적용되지 않은 원본 input.vUV.y를 써야 안전합니다!
        float angle = animUV.x * 2.0f * 3.14159265f;
        
        float radius = input.vUV.y * 0.5f; // animUV.y 대신 input.vUV.y 사용!
        radius *= fPolarRadiusScale;

        finalUV.x = 0.5f + radius * cos(angle);
        finalUV.y = 0.5f + radius * sin(angle);
    }
    else // RECTANGLE 등 기본 사각형
    {
        // 사각형 텍스처는 U, V 양방향으로 스크롤되어도 무방함
        finalUV = animUV;
    }
   
    vDiffuse = g_DefaultTextures[0].Sample(LinearSampler, finalUV);
    
    
    if (isChannelPacking)
    {
        // 1. [최종 확정] 디버깅 결과에 따라 채널 배치를 확정합니다!
        float baseCloudMask = vDiffuse.b; // B채널: '번지르르'하고 부드러운 몸통 (Base)
        float highlightCloudMask = vDiffuse.r; // R채널: '디테일이 있는' 선명한 하이라이트 (Highlight)
        float shadowMask = vDiffuse.g; // G채널: '어두운 부분이 많은' 상세 음영 (Shadow)
        

        // 3. 베이스 구름 깔기 (이건 lerp가 맞습니다)
        float4 finalPackedColor = lerp(vSkyColor, vCloudBaseColor, baseCloudMask);

        // 4. [섀도우 적용] 베이스 구름 위에 섀도우 마스크를 사용하여 섀도우 색상을 섞어줍니다.
        finalPackedColor = lerp(finalPackedColor, vCloudShadowColor, shadowMask);

        // 5. 하이라이트 빛 더하기 (덧셈 연산)
        finalPackedColor += (vCloudHighlight * highlightCloudMask * fCloudHighlightPower); // 강도 2.0배 고정

        // 6. 최종 환경광 곱하기
        vDiffuse = finalPackedColor * vEnvColor;
    }

    
    output.vColor = vDiffuse;
    
    return output;
}

PS_OUT_SHADOW PS_SHADOW(VS_OUT_SHADOW input)
{
    PS_OUT_SHADOW output;
    output.vDepth = float4(input.vPosition.z, 0.f, 0.f, 1.f);
    return output;
}

PS_OUT_DEFFERED PS_LIGHTOBJECT(PS_IN_MESH input)
{
    PS_OUT_DEFFERED output;
    
    float4 vDiffuse = float4(1.f, 1.f, 1.f, 1.f);
    
    Compute_Diffse(vDiffuse, input.vUV);
    
    if (vDiffuse.a < 0.3f)
        discard;
    
    vDiffuse.rgb *= MIDesc.vTintColor.rgb;
    output.vDiffuse = vDiffuse;
    
    
    float3 vNormal = input.vNormal;
    Compute_Normal(vNormal, input.vTangent, input.vBinormal, input.vUV);
    output.vNormal = float4(vNormal * 0.5f + 0.5f, 1.f);
    
    float3 vSpecMask = DEFAULT_SPECMASK_FLOAT3;
    if (Has(g_iMaterialMask, METALNESS))
        vSpecMask = g_MaterialTextures[METALNESS].Sample(LinearSampler, input.vUV).xyz;
    output.vSpecularMask = float4(vSpecMask, 1.f);
    output.vObjectInfo = PackObjectInfo(objectInfo.iObjectID, objectInfo.iFlags);
    output.vDepth = float4(input.vProjPos.z / input.vProjPos.w, input.vProjPos.w, 0.f, 0.f);
    float3 vEmissive = float3(0.f, 0.f, 0.f);
    if (Has(g_iMaterialMask, EMISSIVE))
    {
        vEmissive = g_MaterialTextures[EMISSIVE].Sample(LinearSampler, input.vUV).xyz;
        float fMask = max(vEmissive.r, max(vEmissive.g, vEmissive.b));
        vEmissive = MIDesc.vEmissive.rgb * fMask * MIDesc.fEmissivePower;
    }
    output.vEmissive = float4(vEmissive, 1.f);
    
    return output;
}


PS_OUT_DEFFERED PS_RGBMUTIPLY(PS_IN_MESH input)
{
    PS_OUT_DEFFERED output;
    
    float4 vDiffuse = float4(1.f, 1.f, 1.f, 1.f);
    
    Compute_Diffse(vDiffuse, input.vUV);
    
    float3 final =
    (vDiffuse.r * Color_R.rgb *2.f) +
    (vDiffuse.g * Color_G.rgb * 2.f) +
    (vDiffuse.b * Color_B.rgb * 2.f);
    
    float luminance = dot(final, float3(0.3, 0.59, 0.11));
    float3 finalRGB = final * luminance;
    float4 finalDiffuse = float4(saturate(finalRGB), vDiffuse.a);
    
    saturate(finalDiffuse);

    output.vDiffuse = finalDiffuse;
    
    // 디졸브 디스카드
    //Apply_Dissolve_Discard_And_Alpha(input, output.vDiffuse);
    
    float3 vNormal = input.vNormal;
    Compute_Normal(vNormal, input.vTangent, input.vBinormal, input.vUV);
    output.vNormal = float4(vNormal * 0.5f + 0.5f, 1.f);
    
    float3 vSpecMask = DEFAULT_SPECMASK_FLOAT3;
    if (Has(g_iMaterialMask, METALNESS))
        vSpecMask = g_MaterialTextures[METALNESS].Sample(LinearSampler, input.vUV).xyz;
    output.vSpecularMask = float4(vSpecMask, 1.f);
    output.vObjectInfo = PackObjectInfo(objectInfo.iObjectID, objectInfo.iFlags);
    output.vDepth = float4(input.vProjPos.z / input.vProjPos.w, input.vProjPos.w, 0.f, 0.f);
    float3 vEmissive = float3(0.f, 0.f, 0.f);
    if (Has(g_iMaterialMask, EMISSIVE))
    {
        vEmissive = g_MaterialTextures[EMISSIVE].Sample(LinearSampler, input.vUV).xyz;
        float fMask = max(vEmissive.r, max(vEmissive.g, vEmissive.b));
        vEmissive = output.vDiffuse.rgb * fMask * 4.5f;
    }
    
    ////림라이트
    //float fresnel = pow(1.0 - saturate(dot(N, V)), 0.7); // FresnelRange = 0.7
    //float3 fresnelColor = fresnel * (0.1, 0, 0) * 200; // (0.1,0,0) * 200

    ////이미시브
    //float3 emission = vEmissive.rgb * (1, 0, 0) * 25; // (1,0,0) * 25
    // emission += fresnelColor;
    
    
    output.vEmissive = float4(vEmissive, 1.f);
    //output.vEmissive = float4(emission, 1.f);
    
    return output;
}


technique11 T0
{

    // 기본 오브젝트
	PASS_RS_DS_BS_VP(StaticObject, RS_Default_CullNone, DS_Default, BS_Default, VS_MAIN, PS_MAIN) // 0
	// LandScape
    PASS_RS_DS_BS_VP(LandScape, RS_Default_CullNone, DS_Default, BS_Default, VS_MAIN, PS_LANDSCAPE) // 1

    // 식생
	PASS_RS_DS_BS_VP(Bush, RS_Default_CullNone, DS_Default, BS_Default, VS_MAIN, PS_BUSH) // 2
	PASS_RS_DS_BS_VP(Grass, RS_Default_CullNone, DS_Default, BS_Default, VS_GRASS, PS_GRASS) // 3
	PASS_RS_DS_BS_VP(Moss, RS_Default_CullNone, DS_Default, BS_Default, VS_MAIN, PS_MOSS) // 4
	PASS_RS_DS_BS_VP(Tree, RS_Default_CullNone, DS_Default, BS_Default, VS_MAIN, PS_TREE) // 5
	PASS_RS_DS_BS_VP(Vine, RS_Default_CullNone, DS_Default, BS_Default, VS_MAIN, PS_VINE) // 6

    // 환경요소
	PASS_RS_DS_BS_VP(Rock, RS_Default_CullNone, DS_Default, BS_Default, VS_MAIN, PS_ROCK) // 7
	PASS_RS_DS_BS_VP(Water, RS_Default_CullNone, DS_ReadOnly, BS_AlphaBlend , VS_MAIN, PS_WATER) // 8

    pass Env
    {

        SetRasterizerState(RS_Default_CullNone);
        SetDepthStencilState(DS_Default, 0);
        //SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xFFFFFFFF);
        SetVertexShader(CompileShader(vs_5_0, VS_MAIN()));
        GeometryShader = NULL;
        SetPixelShader(CompileShader(ps_5_0, PS_MAIN()));
    } //9

    // RGB mapping : weapon 쪽에서 쓰임
	PASS_RS_DS_BS_VP(RGBMapping, RS_Default_CullNone, DS_Default, BS_Default, VS_MAIN, PS_RGBMAPPING) // 10
    
    //EXT
    PASS_RS_DS_BS_VP(Debug, RS_Wire, DS_Default, BS_Default, VS_MAIN, PS_BLACK) // 11
	PASS_RS_DS_BS_VP(SkyBox, RS_Default_CullNone  , DS_ReadOnly , BS_Default, VS_SKYBOX, PS_SKYBOX) // 12

    // Shadow - Index 변경되면 Render_Shadow에서 Set_Pass Index 바꿔줘야함
    PASS_RS_DS_BS_VP(Shadow, RS_Default, DS_Default, BS_Default, VS_SHADOW, PS_SHADOW) // 13
    PASS_RS_DS_BS_VP(LightObject,RS_Default, DS_Default, BS_Default, VS_MAIN, PS_LIGHTOBJECT) // 14

    // RGB mapping2 : weapon 쪽에서 쓰임
	PASS_RS_DS_BS_VP(RGBMapping2, RS_Default_CullNone, DS_Default, BS_Default, VS_MAIN, PS_RGBMUTIPLY) // 15
};