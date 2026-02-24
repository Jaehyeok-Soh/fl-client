#include "Struct_Defines.hlsl"
#include "Animation_Defines.hlsl"
#include "Light_Defines.hlsl"


#define MAX_RGBA_TEXTURE_COUNT  2    


#define RGBA 0
#define RGB  1

#define R 0
#define G 1
#define B 2
#define A 3



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
    
    float3 vSpecMask = float3(1.f, 1.f, 0.f);
    if (Has(g_iMaterialMask, METALNESS))
        vSpecMask = g_MaterialTextures[METALNESS].Sample(LinearSampler, input.vUV).xyz;
    output.vSpecularMask = float4(vSpecMask, 1.f);
    output.vObjectInfo = PackObjectInfo(objectInfo.iObjectID, objectInfo.iFlags);
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

PS_OUT_DEFFERED PS_LANDSCAPE(PS_IN_MESH input)
{
    PS_OUT_DEFFERED output = (PS_OUT_DEFFERED) 0;
    
    float2 FilpUV = float2(input.vUV.x, 1.0 - input.vUV.y);
    float2 Size = g_LandScape_TextureUV_RB - g_LandScape_TextureUV_LT;
    float2 vUV = g_LandScape_TextureUV_LT + (Size * FilpUV);
    
    float2 vTileUV = vUV;
    
    
    
    float4 vDiffuse = float4(MIDesc.vTintColor.rgb, 1.f);
    
    float4 vBaseColor = g_Base_Texture.Sample(LinearSampler, vUV);
    
    
    float4 vMixRGBA, vRGBA_RedTile, vRGBA_GreenTile, vRGBA_BlueTile, vRGBA_AlphaTile;
    
    vDiffuse = vBaseColor;
    
    [unroll]
    for (int i = 0; i < g_iUse_Mix_RGBA_Count; ++i)
    {
        
        vMixRGBA = g_Mix_RGBA_Texture[i].Sample(LinearSampler, vUV);
        MIX_RGBA_DATA Data = g_MIX_RGBA_DATA[i];

        if (Data.iUseFlags.r == 1 && vMixRGBA.r > 0.0f)
        {
            float3 vTileUV_R = float3(vTileUV * Data.fRGBA_Mix_Forces.r, Data.iRGBA_Connected_Tile_Index.r);
            vRGBA_RedTile = g_Mix_DH_Tile_Texture.Sample(LinearSampler, vTileUV_R);
            
            float fLayerHeight = vRGBA_RedTile.a;
            float fWeight = vMixRGBA.r;

            float fBlendFactor = fWeight * fLayerHeight;
            
            fBlendFactor = smoothstep(0.1f, 0.9f, fBlendFactor + fWeight);
            
            // 3. 최종 섞기
            vDiffuse = lerp(vDiffuse, vRGBA_RedTile, fBlendFactor);
        }

        // [Green Channel] - 풀 등
        if (Data.iUseFlags.g == 1 && vMixRGBA.g > 0.0f)
        {
            float3 vTileUV_G = float3(vTileUV * Data.fRGBA_Mix_Forces.g, Data.iRGBA_Connected_Tile_Index.g);
            vRGBA_GreenTile = g_Mix_DH_Tile_Texture.Sample(LinearSampler, vTileUV_G);
            
            float fLayerHeight = vRGBA_GreenTile.a;
            float fWeight = vMixRGBA.g;
            float fBlendFactor = smoothstep(0.1f, 0.9f, (fWeight * fLayerHeight) + fWeight);

            vDiffuse = lerp(vDiffuse, vRGBA_GreenTile, fBlendFactor);
        }

        // [Blue Channel]
        if (Data.iUseFlags.b == 1 && vMixRGBA.b > 0.0f)
        {
            float3 vTileUV_B = float3(vTileUV * Data.fRGBA_Mix_Forces.b, Data.iRGBA_Connected_Tile_Index.b);
            vRGBA_BlueTile = g_Mix_DH_Tile_Texture.Sample(LinearSampler, vTileUV_B);
            
            float fLayerHeight = vRGBA_BlueTile.a;
            float fWeight = vMixRGBA.b;
            float fBlendFactor = smoothstep(0.1f, 0.9f, (fWeight * fLayerHeight) + fWeight);

            vDiffuse = lerp(vDiffuse, vRGBA_BlueTile, fBlendFactor);
        }

        // [Alpha Channel]
        if (Data.iUseFlags.a == 1 && vMixRGBA.a > 0.0f)
        {
            float3 vTileUV_A = float3(vTileUV * Data.fRGBA_Mix_Forces.a, Data.iRGBA_Connected_Tile_Index.a);
            vRGBA_AlphaTile = g_Mix_DH_Tile_Texture.Sample(LinearSampler, vTileUV_A);
            
            float fLayerHeight = vRGBA_AlphaTile.a;
            float fWeight = vMixRGBA.a;
            float fBlendFactor = smoothstep(0.1f, 0.9f, (fWeight * fLayerHeight) + fWeight);

            vDiffuse = lerp(vDiffuse, vRGBA_AlphaTile, fBlendFactor);
        }
    }
    
    float3 vNormal = input.vNormal;
    Compute_Normal(vNormal, input.vTangent, input.vBinormal, input.vUV);
    output.vNormal = vNormal * 0.5f + 0.5f;
    output.vDepth = float4(input.vProjPos.z / input.vProjPos.w, input.vProjPos.w, 0.f, 0.f);
    
    output.vDiffuse = vDiffuse;
    
    return output;
}

PS_OUT_DEFFERED PS_RGBMAPPING(PS_IN_MESH input)
{
    PS_OUT_DEFFERED output;
    
    float4 vDiffuse = 1.f;
    Compute_Diffse(vDiffuse, input.vUV);

    float4 final =
     saturate(vDiffuse.r * Color_R) +
     saturate(vDiffuse.g * Color_G) +
     saturate(vDiffuse.b * Color_B);
    
    saturate(final);
    
    output.vDiffuse = final;
    
    float3 vNormal = input.vNormal;
    Compute_Normal(vNormal, input.vTangent, input.vBinormal, input.vUV);
    output.vNormal = vNormal * 0.5f + 0.5f;
    
    float3 vSpecMask = float3(1.f, 1.f, 0.f);
    if (Has(g_iMaterialMask, METALNESS))
        vSpecMask = g_MaterialTextures[METALNESS].Sample(LinearSampler, input.vUV).xyz;
    output.vSpecularMask = float4(vSpecMask, 1.f);
    output.vObjectInfo = PackObjectInfo(objectInfo.iObjectID, objectInfo.iFlags);
    output.vDepth = float4(input.vProjPos.z / input.vProjPos.w, input.vProjPos.w, 0.f, 0.f);
    return output;
}

technique11 T0
{
	PASS_RS_DS_BS_VP(StaticObject, RS_Default_CullNone, DS_Default, BS_Default, VS_MAIN, PS_MAIN)

	PASS_RS_DS_BS_VP(LandScape, RS_Default_CullNone, DS_Default, BS_Default, VS_MAIN, PS_LANDSCAPE)
    PASS_RS_DS_BS_VP(SHADOW_BAKE, RS_Default, DS_Default, BS_Default, VS_MAIN, PS_BAKESHADOW)
	PASS_RS_DS_BS_VP(Debug, RS_Wire, DS_Default, BS_Default, VS_MAIN, PS_BLACK)
	PASS_RS_DS_BS_VP(RGBMapping, RS_Default_CullNone, DS_Default, BS_Default, VS_MAIN, PS_RGBMAPPING)
};