#include "Struct_Defines.hlsl"
#include "Light_Defines.hlsl"

//============================
// Fog Noise Helpers
//============================
float SamplePerlinRaw(float3 vCoord)
{
    // 현재 3D noise texture가 mip 없이 1레벨이라고 가정하고
    // 기존처럼 SampleLevel(..., 0)을 유지
    return g_PerlinNoise.SampleLevel(LinearSampler, vCoord, 0.f).x;
}

float ComputeFogNoiseFactor(float3 vWorldPosition, float fViewZ)
{
    if (fogParam.fFogNoiseScale <= EPSILON)
        return 1.f;

    // -----------------------------------------
    // 주파수는 여기서 직접 조절
    // base : 큰 덩어리
    // detail : 가까운 곳 디테일
    // -----------------------------------------
    const float fBaseFreq = 0.010f;
    const float fDetailFreq = 0.050f;

    // -----------------------------------------
    // Far에서 detail noise를 줄이는 거리
    // -----------------------------------------
    const float fDetailFadeStart = 15.f;
    const float fDetailFadeEnd = 40.f;

    // -----------------------------------------
    // Far에서 전체 noise 영향 자체를 줄이는 거리
    // -----------------------------------------
    const float fBaseKeep = 0.45f;

    // -----------------------------------------
    // 시간 이동
    // detail 쪽은 약간 다른 오프셋을 줘서 패턴 일관성 완화
    // -----------------------------------------
    float3 vWind = float3(
        fAccTime * fogParam.fFogNoiseSpeed,
        fAccTime * fogParam.fFogNoiseSpeed * 0.4f,
        fAccTime * fogParam.fFogNoiseSpeed * 0.7f
    );

    float3 vBaseCoord = vWorldPosition * fBaseFreq + vWind;
    float3 vDetailCoord = vWorldPosition * fDetailFreq + vWind * 1.7f + float3(17.3f, 9.1f, 23.7f);

    // [0,1] -> [-1,1]
    float fBase = SamplePerlinRaw(vBaseCoord) * 2.f - 1.f;
    float fDetail = SamplePerlinRaw(vDetailCoord) * 2.f - 1.f;

    // Far로 갈수록 detail만 먼저 제거
    float fDetailFade = 1.f - saturate((fViewZ - fDetailFadeStart) / max(fDetailFadeEnd - fDetailFadeStart, EPSILON));

    // Near에서는 detail이 조금 섞이고, Far에서는 base 위주
    float fCombined = fBase * 0.85f + fDetail * (0.20f * fDetailFade);

    // Far에서도 base는 일정 비율 유지
    float fFarFade = saturate((fViewZ - fDetailFadeStart) / max(fDetailFadeEnd - fDetailFadeStart, EPSILON));
    float fStrength = fogParam.fFogNoiseScale * lerp(1.0f, fBaseKeep, fFarFade);

    // noise factor를 1 중심으로 조절
    float fMinFactor = 1.f - fStrength;
    float fMaxFactor = 1.f + fStrength * 0.3f;

    float fNoiseFactor = 1.f + fCombined * fStrength;
    fNoiseFactor = clamp(fNoiseFactor, fMinFactor, fMaxFactor);

    return fNoiseFactor;
}

//============================
// Distance Fog
//============================
float ComputeDistanceFog(float fViewZ)
{
    if (fogParam.fFogDensity > EPSILON)
        return 1.f - exp(-fogParam.fFogDensity * max(fViewZ - fogParam.fFogStart, 0.f));
    else
        return saturate((fViewZ - fogParam.fFogStart) / max(fogParam.fFogEnd - fogParam.fFogStart, EPSILON));
}

//============================
// Height Fog
//============================
float ComputeHeightFog(float3 vWorldPos, float3 vCameraPos)
{
    if (fogParam.fFogHeightDensity <= EPSILON)
        return 0.f;

    float3 vRay = vWorldPos - vCameraPos;
    float fRayLength = length(vRay);

    if (fRayLength < EPSILON)
        return 0.f;

    float3 vRayDir = vRay / fRayLength;

    float fCamHeight = vCameraPos.y - fogParam.fFogBaseHeight;
    float fPointHeight = vWorldPos.y - fogParam.fFogBaseHeight;

    float fDeltaY = vRayDir.y;
    float fFogAmount = 0.f;

    if (abs(fDeltaY) > EPSILON)
    {
        float fExp0 = exp(-fogParam.fFogHeightFalloff * fCamHeight);
        float fExp1 = exp(-fogParam.fFogHeightFalloff * fPointHeight);
        fFogAmount = fogParam.fFogHeightDensity * (fExp0 - fExp1) / (fogParam.fFogHeightFalloff * fDeltaY);
    }
    else
    {
        fFogAmount = fogParam.fFogHeightDensity * exp(-fogParam.fFogHeightFalloff * fCamHeight) * fRayLength;
    }

    return saturate(1.f - exp(-max(fFogAmount, 0.f)));
}

VS_OUT_POS_TEX VS_MAIN(VS_IN_POS_TEX input)
{
    VS_OUT_POS_TEX output;
    output.vPosition = mul(float4(input.vPosition, 1.f), W);
    output.vPosition = mul(output.vPosition, VP);
    output.vUV = saturate(input.vUV);
    return output;
}

PS_OUT_HDR PS_FOG(PS_IN_POS_TEX input)
{
    PS_OUT_HDR output;
    output.vColor = float4(0.f, 0.f, 0.f, 0.f);

    float fNDCZ;
    float fViewZ;
    DecodeDepth(input.vUV, fNDCZ, fViewZ);

    if (fViewZ <= EPSILON || fNDCZ >= (1.f - EPSILON))
    {
        output.vColor = float4(0.f, 0.f, 0.f, 0.f);
        return output;
    }

    float3 vCamPos = CameraPosition();
    float4 vWorldPos = ReconstructWorldPos(input.vUV, fNDCZ, fViewZ);

    float fDistanceFog = ComputeDistanceFog(fViewZ);
    float fHeightFog = ComputeHeightFog(vWorldPos.xyz, vCamPos);

    // 변경된 noise factor
    float fNoiseFactor = ComputeFogNoiseFactor(vWorldPos.xyz, fViewZ);

    // 기존 구조 유지
    float fFogFactor = fDistanceFog * lerp(1.f, fHeightFog, 0.5f);
    fFogFactor *= fNoiseFactor;
    fFogFactor = min(fFogFactor, fogParam.fFogMaxOpacity);

    float fHeightBlend = saturate((vWorldPos.y - fogParam.fFogBaseHeight) * 0.05f);
    float3 vFinalFogColor = lerp(fogParam.vColor.rgb, fogParam.vHighColor.rgb, fHeightBlend);

    output.vColor = float4(vFinalFogColor, fFogFactor);
    return output;
}

technique11 T0
{
    PASS_RS_DS_BS_VP(Fog, RS_Default, DS_Disabled, BS_AlphaBlend, VS_MAIN, PS_FOG)
};