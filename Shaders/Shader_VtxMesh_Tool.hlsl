#include "Struct_Defines.hlsl"
#include "Animation_Defines.hlsl"
#include "Light_Defines.hlsl"

#define CopicMask 0
#define SecondNormal 1

#define Default             0
#define Selected            1
#define Preview             2
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


//  전체적인 지형이들어갈 Base Texture
Texture2D       g_Base_Texture;
//  RGBA 맵에 맵핑될 DH , NBR Texture 2DTextureArray로 되어있다
Texture2DArray g_Mix_DH_Tile_Texture;
Texture2DArray g_Mix_NBR_Tile_Texture;

//  RGBA 맵 
Texture2D g_Mix_RGBA_Texture[MAX_RGBA_TEXTURE_COUNT];


Texture2D g_WaterTexture[MAX_WATER_TEXTURE_COUNT];


cbuffer CB_WaterData
{
// --- Register 0 ---
    uint    g_WaterTexBindingFlags;    // 4 Byte
    float   g_fWaterDT;               // 4 Byte
    float2  g_vWaterSpeed1;          // 8 Byte
    
    // --- Register 1 ---
    float2  g_vWaterSpeed2;          // 8 Byte
    float2  g_vWaterDistortionSpeed; // 8 Byte
    
    // --- Register 2 ---
    float2  g_vWaterUVPower;         // 8 Byte (잔물결 촘촘함 조절) 전체적인 UV Tile Power
    float2  g_vWaterDistortionUVPower; // 8 Byte (노이즈 큼직함 조절) Noise UV Tile Power
    
    // --- Register 3 ---
    float   g_fDistortionPower;             // 4 Byte (왜곡 강도)
    
    float   g_fSparklePower;            // 4 Byte (윤슬 눈뽕 강도!)
    float2  g_vSparkleUVPower;              // 8 Byte (윤슬 자글자글함 크기 조절!)
    
};

struct MIX_RGBA_DATA
{
	// Mix할떄 UV좌표에 곱해주어 정밀한 표현을 담당해준다
    float4 fRGBA_Mix_Forces;
    
    // Mix해서 Height 값과 섞어서 영향력을 표현할 Height mix Force
    float4 fRGBA_Mix_Height_Force;
    
	// Mix될 RGBA 맵에서 각 R , G , B , A 가 연결된 Splating Texture들의 Index
    int4   iRGBA_Connected_Tile_Index;
	// Mix될 RGBA 맵에서 각 R , G , B , A 가 Splating을 사용할건지 안할건지에 대한 Flag값 false => BaseTexture가 그대로 들어감 true => Splating
    int4   iUseFlags;
};

cbuffer CB_MIX_RGBA_INFO
{
    MIX_RGBA_DATA g_MIX_RGBA_DATA[MAX_RGBA_TEXTURE_COUNT];
    
    int     g_iUse_Mix_RGBA_Count = { 0 };
    int3    g_Use_Mix_RGBA_Map_Count_Dummy;
};


cbuffer CB_DiscardColor
{
    float4 g_vDiscardColor;
};


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

float4 Get_Modified_Diffuse(float4 vOriginDiffuse)
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

PS_OUT_DEFFERED PS_MAIN(PS_IN_MESH input)
{
    PS_OUT_DEFFERED output = (PS_OUT_DEFFERED) 0;
    
    float4 vDiffuse = 1.f;
    Compute_Diffse(vDiffuse, input.vUV);
    vDiffuse.rgb *= MIDesc.vTintColor.rgb;
    output.vDiffuse = vDiffuse;
    
    float3 vNormal = input.vNormal;
    Compute_Normal(vNormal, input.vTangent, input.vBinormal, input.vUV);
    output.vNormal = float4(vNormal * 0.5f + 0.5f,1.F);
    
    float3 vSpecMask = float3(1.f, 1.f, 0.f);
    if (Has(g_iMaterialMask, SPECULAR))
        vSpecMask = g_MaterialTextures[SPECULAR].Sample(LinearSampler, input.vUV).xyz;
    output.vSpecularMask = float4(vSpecMask, 1.f);
    output.vObjectInfo = PackObjectInfo(objectInfo.iObjectID, objectInfo.iFlags);
    output.vDepth = float4(input.vProjPos.z / input.vProjPos.w, input.vProjPos.w, 0.f, 0.f);
    
    
    if (output.vDiffuse.a < 0.3f)
        discard;
    
    
    output.vDiffuse = Get_Modified_Diffuse(output.vDiffuse);
    
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
    float  fNBR_Tile_Roughness = 0.5f;
    
    
    // 각각 Texture를 지형 Index에 맞는 UV에로 치환
    float4 vBaseColor   = g_Base_Texture.Sample(LinearSampler, vUV);

    float4 vMixRGBA , vRGBA_DH_RedTile, vRGBA_DH_GreenTile, vRGBA_DH_BlueTile, vRGBA_DH_AlphaTile;
    float4 vRGBA_NBR_RedTile, vRGBA_NBR_GreenTile, vRGBA_NBR_BlueTile, vRGBA_NBR_AlphaTile;
    
    vDiffuse = vBaseColor;
    
    [unroll]
    for (int i = 0; i < g_iUse_Mix_RGBA_Count; ++i)
    {
        
        vMixRGBA = g_Mix_RGBA_Texture[i].Sample(LinearSampler, vUV);
        MIX_RGBA_DATA Data = g_MIX_RGBA_DATA[i];

        if (Data.iUseFlags.r == 1 && vMixRGBA.r > 0.0f)
        {
            float3 vTileUV_R    = float3(vTileUV * Data.fRGBA_Mix_Forces.r, Data.iRGBA_Connected_Tile_Index.r);
            vRGBA_DH_RedTile    = g_Mix_DH_Tile_Texture.Sample(LinearSampler, vTileUV_R);
            vRGBA_NBR_RedTile   = g_Mix_NBR_Tile_Texture.Sample(LinearSampler, vTileUV_R);
          
            float fLayerHeight = vRGBA_NBR_RedTile.b; 
            float fWeight = vMixRGBA.r; 
            float fMixHeight = lerp(1.0f, fLayerHeight, Data.fRGBA_Mix_Height_Force.r);
            float fBlendFactor = smoothstep(0.1f, 0.9f, (fWeight * fMixHeight) + fWeight);
            
            vDiffuse    = lerp(vDiffuse, vRGBA_DH_RedTile, fBlendFactor);
            
            float3 vNormal_R = { 0.f , 0.f, 0.f };            
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
    
    output.vNormal = float4(finalWorldNormal * 0.5f + 0.5f,1.f);
    output.vDepth           = float4(input.vProjPos.z / input.vProjPos.w, input.vProjPos.w, 0.f, 0.f);
    output.vDiffuse         = Get_Modified_Diffuse(vDiffuse);
    output.vSpecularMask    = float4( 0.f, fNBR_Tile_Roughness , 0.f,0.f);
    
    
    output.vDiffuse = Get_Modified_Diffuse(output.vDiffuse);
 
    
    return output;
}


PS_OUT_DEFFERED PS_TREE(PS_IN_MESH input)
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
    
    //output.vDiffuse = Get_Modified_Diffuse(output.vDiffuse);
    
    return output;
}

PS_OUT_DEFFERED PS_MOSS(PS_IN_MESH input)
{
    PS_OUT_DEFFERED output;
    
    float4 vDiffuse = 1.f;    
    Compute_Diffse(vDiffuse, input.vUV);
    
    if (vDiffuse.r < 0.3f ) 
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
    
   
    //output.vDiffuse = Get_Modified_Diffuse(output.vDiffuse);

    return output;
}

PS_OUT_DEFFERED PS_VINE(PS_IN_MESH input)
{
    PS_OUT_DEFFERED output;
    
    float4 vDiffuse = 1.f;
    
    float4 vMask = float4(1.f, 1.f, 1.f, 1.f);
    
    if (Has(g_iMaterialMask, METALNESS))
        vMask = g_MaterialTextures[METALNESS].Sample(LinearSampler, input.vUV);

    if (vMask.r < 0.1f) 
        discard;
    
    float4 vMI_Color = MIDesc.vTintColor;
    output.vDiffuse = vMI_Color * vMask.b;
    
    float3 vNormal = input.vNormal;
    Compute_Normal(vNormal, input.vTangent, input.vBinormal, input.vUV);
    output.vNormal = float4(vNormal * 0.5f + 0.5f, 1.f);
    output.vObjectInfo = PackObjectInfo(objectInfo.iObjectID, objectInfo.iFlags);
    output.vDepth = float4(input.vProjPos.z / input.vProjPos.w, input.vProjPos.w, 0.f, 0.f);
    output.vSpecularMask = float4(0.f,0.f,0.f,0.f);
    
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
    
    float3 vSpecMask = float3(1.f, 1.f, 0.f);
    if (Has(g_iMaterialMask, METALNESS))
        vSpecMask = g_MaterialTextures[METALNESS].Sample(LinearSampler, input.vUV).xyz;
    output.vSpecularMask = float4(vSpecMask, 1.f);
    output.vObjectInfo = PackObjectInfo(objectInfo.iObjectID, objectInfo.iFlags);
    output.vDepth = float4(input.vProjPos.z / input.vProjPos.w, input.vProjPos.w, 0.f, 0.f);
    
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
    
    //output.vDiffuse = Get_Modified_Diffuse(output.vDiffuse);
    
    return output;
}


PS_OUT_DEFFERED PS_WATER(PS_IN_MESH input)
{
    PS_OUT_DEFFERED output = (PS_OUT_DEFFERED) 0;
    
    // 기본 Tile UV
    float2 vTiledUV = input.vUV * g_vWaterUVPower;
    
    float4 vDiffuse = 1.f;
    vDiffuse = MIDesc.vTintColor.rgba;          //기본색깔 입히기

    
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
    
    
    float3 vSpecMask = float3(1.f,1.f,0.f);
    if (Has(g_WaterTexBindingFlags, Water_Lighting))
    {
        float2 vSparkleUV = input.vUV * g_vSparkleUVPower;
        vSparkleUV += float2(g_fWaterDT * g_vWaterSpeed1.x, g_fWaterDT * g_vWaterSpeed1.y);
        if (bHasAnyNormal)
            vSparkleUV += (vLocalNormal.xy * 0.3f); 
        //vSparkleUV += (vDistortion * fDistortionPower);
        float  fSparkle = g_WaterTexture[Water_Lighting].Sample(LinearSampler, vSparkleUV).r;
        
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
    PASS_RS_DS_BS_VP(LandScape, RS_Default_CullNone, DS_Default, BS_Default, VS_MAIN, PS_LANDSCAPE)

    // 식생
	PASS_RS_DS_BS_VP(Bush, RS_Default_CullNone, DS_Default, BS_Default, VS_MAIN,  PS_BUSH)
	PASS_RS_DS_BS_VP(Grass, RS_Default_CullNone, DS_Default, BS_Default, VS_MAIN, PS_GRASS)
	PASS_RS_DS_BS_VP(Moss, RS_Default_CullNone, DS_Default, BS_Default, VS_MAIN,  PS_MOSS)
	PASS_RS_DS_BS_VP(Tree, RS_Default_CullNone, DS_Default, BS_Default, VS_MAIN,  PS_TREE)
	PASS_RS_DS_BS_VP(Vine, RS_Default_CullNone, DS_Default, BS_Default, VS_MAIN,  PS_VINE)

    // 환경요소
	PASS_RS_DS_BS_VP(Rock, RS_Default_CullNone, DS_Default, BS_Default, VS_MAIN, PS_MAIN)
	PASS_RS_DS_BS_VP(Water, RS_Default_CullNone , DS_Default , BS_AlphaBlend , VS_MAIN, PS_WATER)

    //EXT
    PASS_RS_DS_BS_VP(SHADOW_BAKE, RS_Default, DS_Default, BS_Default, VS_MAIN, PS_MAIN)
	PASS_RS_DS_BS_VP(Debug, RS_Wire, DS_Default, BS_Default, VS_MAIN, PS_MAIN)
};