#include "Global_Defines.hlsl"
#include "Common_Defines.hlsl"

#define DF_NONE 0

#define DF_SPAWN_START 1 << 0
#define DF_DISSOLVE_START 1 << 1

#define DF_USEEDGE 1 << 2
#define DF_USE_ALPHAFADE 1 << 3
#define DF_USE_DISSOLVEMAP 1 << 4

#define DF_TYPE_MONSTER 1 << 10
#define DF_TYPE_BOSS    1 << 11
#define DF_TYPE_SWORD   1 << 12
#define DF_TYPE_NPC     1 << 13

struct DISSOLVE_Effect
{
    // Slot 1
    float g_fDissolveAmount;
    float g_fDissolveEdgeWidth;
    float2 Padding0;
    
    // Slot 2
    float3 g_vDissolveEdgeColor;
    uint g_iDissolveFlag;
};

cbuffer CB_DISSOLVE
{
    DISSOLVE_Effect g_DissolveEffect;
};

bool Has_DissolveFlag(uint flags, uint mask)
{
    return (flags & mask) != 0;
}

bool Is_Dissolve_Active()
{
    return Has_DissolveFlag(g_DissolveEffect.g_iDissolveFlag, DF_SPAWN_START) ||
           Has_DissolveFlag(g_DissolveEffect.g_iDissolveFlag, DF_DISSOLVE_START);
}

int Get_DissolveTexIndex(uint flags)
{
    if (Has_DissolveFlag(flags, DF_TYPE_BOSS))
        return 1;
    if (Has_DissolveFlag(flags, DF_TYPE_SWORD))
        return 2;
    if (Has_DissolveFlag(flags, DF_TYPE_NPC))
        return 3;

    // 기본은 monster
    return 0;
}

float Sample_DissolveMask(float2 uv, uint flags)
{
    if (Has_DissolveFlag(flags, DF_TYPE_BOSS))
        return g_DissolveTexture[1].Sample(LinearSampler, uv).r;

    if (Has_DissolveFlag(flags, DF_TYPE_SWORD))
        return g_DissolveTexture[2].Sample(LinearSampler, uv).r;

    if (Has_DissolveFlag(flags, DF_TYPE_NPC))
        return g_DissolveTexture[3].Sample(LinearSampler, uv).r;

    return g_DissolveTexture[0].Sample(LinearSampler, uv).r;
}

bool Should_Discard_By_Dissolve(float2 uv, uint flags, float amount)
{
    if (!Is_Dissolve_Active())
        return false;
    
    if (Has_DissolveFlag(flags, DF_USE_DISSOLVEMAP) == false)
        return false;

    float noise = Sample_DissolveMask(uv, flags);

    // spawn / dissolve 모두 CPU에서 amount 방향을 이미 정해주므로
    // 여기서는 동일한 기준 사용
    
    // 잔여물이 남아서 방지용으로 코드 작성합니데이
    //if (amount >= 0.95f)
    //    return true;
    
    return noise <= amount;
}

float Compute_DissolveEdge(float2 uv, uint flags, float amount, float edgeWidth)
{
    if (Has_DissolveFlag(flags, DF_USEEDGE) == false)
        return 0.f;

    if (Has_DissolveFlag(flags, DF_USE_DISSOLVEMAP) == false)
        return 0.f;

    float noise = Sample_DissolveMask(uv, flags);
    float diff = noise - amount;

    // amount 경계 주변만 edge
    float edge = 1.f - saturate(diff / max(edgeWidth, 0.0001f));
    return edge;
}

float Get_DissolveAlphaFactor(uint flags, float amount)
{
    if (Has_DissolveFlag(flags, DF_USE_ALPHAFADE))
    {
        // spawn은 amount 1->0, dissolve는 0->1
        if (Has_DissolveFlag(flags, DF_SPAWN_START))
            return saturate(1.f - amount); // spawn 진행될수록 alpha 올라감

        if (Has_DissolveFlag(flags, DF_DISSOLVE_START))
            return saturate(1.f - amount); // dissolve 진행될수록 alpha 내려감
    }

    return 1.f;
}