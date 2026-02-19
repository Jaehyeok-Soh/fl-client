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

bool HasCopickMask() { return (g_iGlobalMask & 1) != 0; }
bool HasSecNormal() { return (g_iGlobalMask & 2) != 0; }

//  전체적인 지형이들어갈 Base Texture
Texture2D       g_Base_Texture;
//  RGBA 맵에 맵핑될 DH , NBR Texture 2DTextureArray로 되어있다
Texture2DArray g_Mix_DH_Tile_Texture;
Texture2DArray g_Mix_NBR_Tile_Texture;


//  RGBA 맵 
Texture2D g_Mix_RGBA_Texture[MAX_RGBA_TEXTURE_COUNT];


struct MIX_RGBA_DATA
{
	// Mix할떄 UV좌표에 곱해주어 정밀한 표현을 담당해준다
    float4 fRGBA_Mix_Forces;
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

cbuffer cbMix_ConnectedIndex
{
    
    uint    g_iMix_RGBA_Red_Connected_Index       = { 0 };
    uint    g_iMix_RGBA_Green_Connected_Index     = { 0 };
    uint    g_iMix_RGBA_Blue_Connected_Index      = { 0 };
    uint    g_iMix_RGBA_Alpha_Connected_Index     = { 0 };
    
    uint    g_iMix_RGB_Red_Connected_Index        = { 0 };
    uint    g_iMix_RGB_Green_Connected_Index      = { 0 };
    uint    g_iMix_RGB_Blue_Connected_Index       = { 0 };
    
    float4  g_fRGBA_Tile_Force  = { 1.f, 1.f, 1.f, 1.f };
    float3  g_fRGB_Tile_Force   = 1.f;
    
    float   Mix_ConnectedIndex_Dummy = { 0.f };
    
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
    
    output.vDiffuse = Get_Modified_Diffuse(output.vDiffuse);
    return output;
}

PS_OUT_DEFFERED PS_LANDSCAPE(PS_IN_MESH input)
{
    PS_OUT_DEFFERED output = (PS_OUT_DEFFERED) 0; 
    
    float2 FilpUV       = float2(input.vUV.x, 1.0 - input.vUV.y);
    float2 Size         = g_LandScape_TextureUV_RB - g_LandScape_TextureUV_LT;
    float2 vUV          = g_LandScape_TextureUV_LT + (Size * FilpUV);
    
    // TileForce 타일이 얼마나 촘촘하게 입혀질지에대한 기본 Force값
    float2 vTileUV      = vUV;
    
    
    
    float4 vDiffuse = float4(MIDesc.vTintColor.rgb,1.f);
    
    // 각각 Texture를 지형 Index에 맞는 UV에로 치환
    float4 vBaseColor   = g_Base_Texture.Sample(LinearSampler, vUV);
    
    //vBaseColor.rgb = lerp(vBaseColor.rgb, dot(vBaseColor.rgb, float3(0.3, 0.59, 0.11)), 0.5f); // 흑백과 50% 섞기 (채도 감소)
    
    float4 vMixRGBA , vRGBA_RedTile, vRGBA_GreenTile, vRGBA_BlueTile, vRGBA_AlphaTile;
    
    vDiffuse = vBaseColor;
    
    // 사용하는 RBBA Texture Count 만큼 Lerp 해주기 
    [unroll]
    for (int i = 0; i < g_iUse_Mix_RGBA_Count; ++i)
    {
        
        vMixRGBA = g_Mix_RGBA_Texture[i].Sample(LinearSampler, vUV);
        MIX_RGBA_DATA Data = g_MIX_RGBA_DATA[i];

        // [Red Channel] - 흙/바위 등
        if (Data.iUseFlags.r == 1 && vMixRGBA.r > 0.0f)
        {
            float3 vTileUV_R = float3(vTileUV * Data.fRGBA_Mix_Forces.r, Data.iRGBA_Connected_Tile_Index.r);
            vRGBA_RedTile = g_Mix_DH_Tile_Texture.Sample(LinearSampler, vTileUV_R);
            
            // ★ Height Blend 적용 ★
            float fLayerHeight = vRGBA_RedTile.a; 
            float fWeight = vMixRGBA.r; 

            // 1. 높이 기반 가중치 계산
            float fBlendFactor = fWeight * fLayerHeight;
            
            // 2. 경계 부드럽게 (Smoothstep)
            // 0.1 ~ 0.9: 경계를 매우 부드럽게 풉니다. (각진 느낌 해소)
            // 이 범위를 조절해서 부드러움 정도를 결정하세요.
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
        
        //vMixRGBA = g_Mix_RGBA_Texture[i].Sample(LinearSampler, vUV);
        
        //if (g_MIX_RGBA_DATA[i].iUseFlags.r == 1 && vMixRGBA.r > 0.0f)
        //{
        //    vRGBA_RedTile = g_Mix_DH_Tile_Texture.Sample(LinearSampler, float3(vTileUV * g_MIX_RGBA_DATA[i].fRGBA_Mix_Forces.r, g_MIX_RGBA_DATA[i].iRGBA_Connected_Tile_Index.r));
        //    vDiffuse = lerp(vDiffuse, vRGBA_RedTile, vMixRGBA.r);
        //}
        //if (g_MIX_RGBA_DATA[i].iUseFlags.g == 1 && vMixRGBA.g > 0.0f)
        //{
        //    vRGBA_GreenTile = g_Mix_DH_Tile_Texture.Sample(LinearSampler, float3(vTileUV * g_MIX_RGBA_DATA[i].fRGBA_Mix_Forces.g, g_MIX_RGBA_DATA[i].iRGBA_Connected_Tile_Index.g));
        //    vDiffuse = lerp(vDiffuse, vRGBA_GreenTile, vMixRGBA.g);
        //}
        //if (g_MIX_RGBA_DATA[i].iUseFlags.b == 1 && vMixRGBA.b > 0.0f)
        //{
        //    vRGBA_BlueTile = g_Mix_DH_Tile_Texture.Sample(LinearSampler, float3(vTileUV * g_MIX_RGBA_DATA[i].fRGBA_Mix_Forces.b, g_MIX_RGBA_DATA[i].iRGBA_Connected_Tile_Index.b));
        //    vDiffuse = lerp(vDiffuse, vRGBA_BlueTile, vMixRGBA.b);
        //}
        //if (g_MIX_RGBA_DATA[i].iUseFlags.a == 1 && vMixRGBA.a > 0.0f)
        //{
        //    vRGBA_AlphaTile = g_Mix_DH_Tile_Texture.Sample(LinearSampler, float3(vTileUV * g_MIX_RGBA_DATA[i].fRGBA_Mix_Forces.a, g_MIX_RGBA_DATA[i].iRGBA_Connected_Tile_Index.a));
        //    vDiffuse = lerp(vDiffuse, vRGBA_AlphaTile, vMixRGBA.a);
        //}
    }
    
    float3 vNormal  = input.vNormal;
    Compute_Normal(vNormal, input.vTangent, input.vBinormal, input.vUV);
    output.vNormal  = vNormal * 0.5f + 0.5f;
    output.vDepth   = float4(input.vProjPos.z / input.vProjPos.w, input.vProjPos.w, 0.f, 0.f);
    output.vDiffuse = Get_Modified_Diffuse(vDiffuse);
    
    return output;
}

technique11 T0
{
	PASS_RS_DS_BS_VP(P0, RS_Default_CullNone, DS_Default, BS_Default, VS_MAIN, PS_STATICOBJECT)
	PASS_RS_DS_BS_VP(P1, RS_Default_CullNone, DS_Default, BS_Default, VS_MAIN, PS_LANDSCAPE)
};