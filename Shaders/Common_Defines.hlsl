#ifndef _COMMON_DEFINES_HLSL_
#define _COMMON_DEFINES_HLSL_

///////////
// Macro //
///////////
#define DEFAULT 0

#define SSAO_KERNEL_COUNT 16

#define SHADOW_CASCADE_COUNT 2

#define ACTIVE_BAKED_SECTION_COUNT 9

//////////////////
// MaterialSlot //
//////////////////
#define DIFFUSE 0
#define SPECULAR 1
#define AMBIENT 2
#define EMISSIVE 3
#define HEIGHT 4
#define NORMAL 5
#define SHININESS 6
#define OPACITY 7
#define DISPLACEMENT 8
#define LIGHTMAP 9
#define REFLECTION 10
#define BASE_COLOR 11
#define NORMAL_CAMERA 12
#define EMISSION_COLOR 13
#define METALNESS 14
#define DIFFUSE_ROUGHNESS 15
#define AMBIENT_OCCLUSION 16
#define UNKNOWN 17
#define MATERIAL_END 18


#define DISSOLVE_MAX 4
////////////
// Scalar //
////////////
uint g_iMaterialMask;
uint Bit(uint iSlot) { return 1u << iSlot; }
bool Has(uint iMask, uint iSlot)
{
    return (iMask & Bit(iSlot)) != 0;
}

#define MAX_BONE_TRANSFORMS 512
#define MAX_MODEL_KEYFRAMES 512

#define ALPHA_TEST(Alpha, Dest) Alpha <= Dest

#define PASS_RS_DS_BS_VP(PassName, RS, DS, BS, VS, PS)              \
pass PassName                                                       \
{                                                                   \
    SetRasterizerState(RS);                                         \
    SetDepthStencilState(DS, 0);                                    \
    SetBlendState(BS, float4(0.f,0.f,0.f,0.f), 0xFFFFFFFF);         \
    SetVertexShader(CompileShader(vs_5_0, VS()));                   \
    GeometryShader = NULL;                                          \
    SetPixelShader(CompileShader(ps_5_0, PS()));                    \
}

#define PASS_RS_DS_BS_GS_VP(PassName, RS, DS, BS, GS, VS, PS)       \
pass PassName                                                       \
{                                                                   \
    SetRasterizerState(RS);                                         \
    SetDepthStencilState(DS, 0);                                    \
    SetBlendState(BS, float4(0.f,0.f,0.f,0.f), 0xFFFFFFFF);         \
    SetVertexShader(CompileShader(vs_5_0, VS()));                   \
    SetGeometryShader(CompileShader(gs_5_0, GS()));                 \
    SetPixelShader(CompileShader(ps_5_0, PS()));                    \
}

//////////////////
// SamplerState //
//////////////////
sampler LinearSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = wrap;
    AddressV = wrap;
    AddressW = wrap;
};

sampler LinearClampSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = clamp;
    AddressV = clamp;
    AddressW = clamp;
};

sampler LinearBorderSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = BORDER;
    AddressV = BORDER;
    AddressW = BORDER;
};

sampler LinearMirrorSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = mirror;
    AddressV = mirror;
    AddressW = mirror;
};

sampler PointSampler = sampler_state
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Wrap;
    AddressV = Wrap;
    AddressW = Wrap;
};

sampler PointClampSampler = sampler_state
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = clamp;
    AddressV = clamp;
    AddressW = clamp;
};

/////////////////////
// RasterizerState //
/////////////////////
RasterizerState RS_Default
{
    FillMode = Solid;
    CullMode = BACK;
};

RasterizerState RS_Default_CullFront
{
    FillMode = Solid;
    CullMode = FRONT;
};

RasterizerState RS_Default_CullNone
{
    FillMode = Solid;
    CullMode = NONE;
};

RasterizerState RS_Wire
{
    FillMode = WireFrame;
    CullMode = BACK;
};

////////////////
// BlendState //
////////////////
BlendState BS_Default
{
    BlendEnable[0] = FALSE;
    AlphaToCoverageEnable = FALSE;
};

BlendState BS_AlphaBlend
{
    BlendEnable[0] = TRUE;

    AlphaToCoverageEnable = FALSE;
    SrcBlend = SRC_ALPHA;
    DestBlend = INV_SRC_ALPHA;
    BlendOp = ADD;

    SrcBlendAlpha = ONE;
    DestBlendAlpha = INV_SRC_ALPHA;
    BlendOpAlpha = ADD;
};

BlendState BS_Blend
{
    BlendEnable[0] = TRUE;

    AlphaToCoverageEnable = FALSE;
    SrcBlend = ONE;
    DestBlend = ONE;
    BlendOp = ADD;

    SrcBlendAlpha = ONE;
    DestBlendAlpha = ONE;
    BlendOpAlpha = ADD;
};

BlendState BS_PointLightBlend
{
    BlendEnable[0] = TRUE;
    BlendEnable[1] = TRUE;

    AlphaToCoverageEnable = FALSE;
    SrcBlend = ONE;
    DestBlend = ONE;
    BlendOp = ADD;

    SrcBlendAlpha = ONE;
    DestBlendAlpha = ONE;
    BlendOpAlpha = ADD;
};

BlendState BS_AlphaAdditive
{
    BlendEnable[0] = TRUE;

    AlphaToCoverageEnable = FALSE;

    SrcBlend = SRC_ALPHA;
    DestBlend = ONE;
    BlendOp = ADD;

    SrcBlendAlpha = ONE;
    DestBlendAlpha = ONE;
    BlendOpAlpha = ADD;
};

BlendState BS_WBOIT_Accumulate
{
    AlphaToCoverageEnable = FALSE;
    //IndependentBlendEnable = TRUE;

    // RenderTarget[0] 설정
    BlendEnable[0] = TRUE;
    SrcBlend[0] = ONE;
    DestBlend[0] = ONE;
    BlendOp[0] = ADD;
    SrcBlendAlpha[0] = ONE;
    DestBlendAlpha[0] = ONE;
    BlendOpAlpha[0] = ADD;
    RenderTargetWriteMask[0] = 0x0F;

    // RenderTarget[1] 설정
    BlendEnable[1] = TRUE;
    SrcBlend[1] = ZERO;
    DestBlend[1] = INV_SRC_COLOR; // 또는 INV_SRC1_COLOR (사용 환경에 따라 확인 필요)
    BlendOp[1] = ADD;
    SrcBlendAlpha[1] = ZERO;
    DestBlendAlpha[1] = INV_SRC_ALPHA;
    BlendOpAlpha[1] = ADD;
    RenderTargetWriteMask[1] = 0x01;
};
/////////////////////
// Depth / Stencil //
/////////////////////
DepthStencilState DS_Default
{
    DepthEnable = true;
    DepthWriteMask = all;
    DepthFunc = LESS_EQUAL;
    StencilEnable = false;
};

DepthStencilState DS_Disabled
{
    DepthEnable = false; // 테스트 끔
    DepthWriteMask = zero; // 쓰기 끔
};

DepthStencilState DS_ReadOnly
{
    DepthEnable = TRUE; // 테스트 켜기
    DepthWriteMask = ZERO; // 쓰기 끔 (투명물 전용)
    DepthFunc = LESS_EQUAL;
    StencilEnable = FALSE;
};

DepthStencilState DS_Write
{
    DepthEnable = TRUE; // 테스트 켜기
    DepthWriteMask = ALL; // 쓰기 켜기 (불투명물)
    DepthFunc = LESS_EQUAL;
    StencilEnable = FALSE;
};

#endif