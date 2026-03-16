#include "Struct_Defines.hlsl"
#include "Light_Defines.hlsl"

//============================
// 3D Perlin Noise
//============================
float SamplePerlinNoise(float3 vWorldPosition)
{
    if (fogParam.fFogNoiseScale <= EPSILON)
        return 1.f;

    float3 vCoord = vWorldPosition * 0.06f;
    vCoord.x += fAccTime * fogParam.fFogNoiseSpeed;
    vCoord.y += fAccTime * fogParam.fFogNoiseSpeed * 0.4f;
    vCoord.z += fAccTime * fogParam.fFogNoiseSpeed * 0.7f;
    
    float fNoise = g_PerlinNoise.SampleLevel(LinearSampler, vCoord, 0.f).x;
    
    return lerp(1.f - fogParam.fFogNoiseScale, 1.f + fogParam.fFogNoiseScale * 0.3f, fNoise);
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
    
    // 카메라와 월드포인트의 높이
    float fCamHeight = vCameraPos.y - fogParam.fFogBaseHeight;
    float fPointHeight = vWorldPos.y - fogParam.fFogBaseHeight;
    
    // 레이 방향의 높이 변화량
    float fDeltaY = vRayDir.y;
    
    float fFogAmount = 0.f;
    
    if (abs(fDeltaY) > EPSILON)
    {
        // 적분: density * (e^(-falloff*h0) - e^(-falloff*h1)) / (falloff * deltaY)
        float fExp0 = exp(-fogParam.fFogHeightFalloff * fCamHeight);
        float fExp1 = exp(-fogParam.fFogHeightFalloff * fPointHeight);
        fFogAmount = fogParam.fFogHeightDensity * (fExp0 - fExp1) / (fogParam.fFogHeightFalloff * fDeltaY);
    }
    else
    {
        // 수평 레이 - 균일 밀도
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
    
    float fNoise = SamplePerlinNoise(vWorldPos.xyz);
    
    // float fFogFactor = saturate(fDistanceFog + fHeightFog - fDistanceFog * fHeightFog);
    float fFogFactor = fDistanceFog * lerp(1.f, fHeightFog, 0.5f);
    fFogFactor *= fNoise;
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