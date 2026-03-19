#include "Effect_Defines.hlsl"
// =========== VS In  ==============

VS_OUT_INST_MESH_PARTICLE
    VS_DEFAULT(VS_IN_INST_MESH_PARTICLE In)
{
    VS_OUT_INST_MESH_PARTICLE Out;

    float4 vWorldPos = mul(float4(In.vPosition, 1.f), INSTANCE_OUTPUT[In.vInstID].matTransform);
    vWorldPos = mul(float4(vWorldPos), W);
    float4 vViewPos = mul(vWorldPos, V);
    Out.vPosition = mul(vViewPos, P);
    
    Out.vWorldPos = vWorldPos;
    Out.vProjPos = Out.vPosition;
    
    Out.vNormal = normalize(mul(In.vNormal, (float3x3) INSTANCE_OUTPUT[In.vInstID].matTransform));
    Out.vTangent = normalize(mul(In.vTangent, (float3x3) INSTANCE_OUTPUT[In.vInstID].matTransform));
    Out.vBinormal = normalize(mul(In.vBinormal, (float3x3) INSTANCE_OUTPUT[In.vInstID].matTransform));
    
    Out.vUV = In.vUV;
    
    Out.vPSize = float2(length(INSTANCE_OUTPUT[In.vInstID].matTransform[0].xyz), length(INSTANCE_OUTPUT[In.vInstID].matTransform[1].xyz));
    Out.vLifeTime = INSTANCE_OUTPUT[In.vInstID].vLifeTime;
    Out.vViewZ = vViewPos.z;
    
    return Out;
}

PS_OUT_WBOIT PS_DefaultMesh(VS_OUT_INST_MESH_PARTICLE In)
{
    PS_OUT_WBOIT Out;
    
    if (In.vLifeTime.x < 0.0f)
        discard;
   // =======              노이즈 텍스처 샘플링             ===========
    float2 finalUV = In.vUV;
    
    float4 DiffuseSample = { 1.f, 1.f, 1.f, 1.f };
    float4 noiseSample = { 1.f, 1.f, 1.f, 1.f };
    float4 DetailNoiseSample = { 1.f, 1.f, 1.f, 1.f };
    float4 MaskSample = { 1.f, 1.f, 1.f, 1.f };
    float4 GradationSample = { 1.f, 1.f, 1.f, 1.f };
    float4 GlowSample = { 1.f, 1.f, 1.f, 1.f };
    float4 DissolveSample = { 1.f, 1.f, 1.f, 1.f };
    float4 CurveSample = { 1.f, 1.f, 1.f, 1.f };
    float4 SubMaskSample = { 1.f, 1.f, 1.f, 1.f };
    float noiseValue = { 1.f };
    
    // 1. 진행 비율 계산 (AppearRatio: 등장, DissolveProgress: 소멸)
    float LifeRatio = saturate(In.vLifeTime.x / In.vLifeTime.y);
    float AppearRatio = In.vLifeTime.x / (In.vLifeTime.y * g_Effect.g_AppearRatio);
    float DissolveProgress = saturate((LifeRatio - g_Effect.g_AppearRatio) / max(0.001f, 1.0f - g_Effect.g_AppearRatio));
   
     // ================     노이즈 텍스처     ===============

        // 3. 왜곡량(Offset) 계산
    float2 distortionOffset = float2(0.f, 0.f);
    
    if (Has(g_Effect.g_TextureFlags, NOISETEXTURE))
    {
        if (HasTextureScroll(SCROLL_NOISE))
        {
            float2 scrolledUV = In.vUV + g_Effect.g_UVOffset;
            scrolledUV += g_Effect.g_ScrollOffset * g_Effect.NoiseTexture_ScrollWeight;
            
            noiseSample = NoiseTextureSample(Get90DegreeRotatedUV(scrolledUV, g_Effect.g_RotationFlags, NOISETEXTURE));
            
            distortionOffset.x = (noiseSample.r - 0.5f) * g_Effect.g_DistortionScale.x;
            distortionOffset.y = (noiseSample.g - 0.5f) * g_Effect.g_DistortionScale.y;
        }
        else
        {
            noiseSample = NoiseTextureSample(Get90DegreeRotatedUV(In.vUV, g_Effect.g_RotationFlags, NOISETEXTURE));
            
            distortionOffset.x = (noiseSample.r - 0.5f) * g_Effect.g_DistortionScale.x;
            distortionOffset.y = (noiseSample.g - 0.5f) * g_Effect.g_DistortionScale.y;
        }
    }
    else
    {
        noiseSample = float4(1.f, 1.f, 1.f, 1.f);
        distortionOffset = float2(0.f, 0.f);
    }
    
    // 최종 왜곡 UV (왜곡량)
    float2 distortionUV = distortionOffset;
    
    // ================     메인 텍스처      ===============
    if (Has(g_Effect.g_TextureFlags, DEFAULTTEXTURE))
    {
        if (HasTextureSprite(g_Effect.DiffuseTexture_SpriteInfo))
        {
            float2 SpriteUV = GetStaticSpriteUV(In.vUV, g_Effect.DiffuseTexture_SpriteInfo);
            
            if (Has(g_Effect.g_TextureFlags, NOISETEXTURE))
            {
                SpriteUV += distortionUV;
                DiffuseSample = DefaultTextureSample(Get90DegreeRotatedUV(SpriteUV, g_Effect.g_RotationFlags, DEFAULTTEXTURE));
            }
            else
            {
                DiffuseSample = DefaultTextureSample(Get90DegreeRotatedUV(SpriteUV, g_Effect.g_RotationFlags, DEFAULTTEXTURE));
            }
        }
        else if (HasTextureScroll(SCROLL_DIFFUSE))
        {
            if (Has(g_Effect.g_TextureFlags, NOISETEXTURE))
            {
                float2 scrolledUV = In.vUV + g_Effect.g_UVOffset;
                scrolledUV += g_Effect.g_ScrollOffset * g_Effect.DiffuseTexture_ScrollWeight;
                scrolledUV += distortionUV;
                DiffuseSample = DefaultTextureSample(Get90DegreeRotatedUV(scrolledUV, g_Effect.g_RotationFlags, DEFAULTTEXTURE));
            }
            else
            {
                float2 scrolledUV = In.vUV + g_Effect.g_UVOffset;
                scrolledUV += g_Effect.g_ScrollOffset * g_Effect.DiffuseTexture_ScrollWeight;
                DiffuseSample = DefaultTextureSample(Get90DegreeRotatedUV(scrolledUV, g_Effect.g_RotationFlags, DEFAULTTEXTURE));
            }
        }
        else
        {
            DiffuseSample = DefaultTextureSample(Get90DegreeRotatedUV(In.vUV, g_Effect.g_RotationFlags, DEFAULTTEXTURE));
        }
    }
    else
        DiffuseSample = float4(1.f, 1.f, 1.f, 1.f);
    
    // ================     그라데이션 텍스처     ===============
    
    if (Has(g_Effect.g_TextureFlags, GRADATIONTEXTURE))
    {
        if (HasTextureScroll(SCROLL_GRADATION))
        {
            float2 scrolledUV = In.vUV + g_Effect.g_UVOffset;
            scrolledUV += g_Effect.g_ScrollOffset * g_Effect.GradationTexture_ScrollWeight;
            GradationSample = GradationTextureSample(Get90DegreeRotatedUV(scrolledUV, g_Effect.g_RotationFlags, GRADATIONTEXTURE));
        }
        else if (HasTextureSprite(g_Effect.GradationTexture_SpriteInfo))
        {
            float2 SpriteUV = GetStaticSpriteUV(In.vUV, g_Effect.GradationTexture_SpriteInfo);
            GradationSample = GradationTextureSample(Get90DegreeRotatedUV(SpriteUV, g_Effect.g_RotationFlags, GRADATIONTEXTURE));
        }
        else
        {
            GradationSample = GradationTextureSample(Get90DegreeRotatedUV(In.vUV, g_Effect.g_RotationFlags, GRADATIONTEXTURE));
        }

    }
    else
        GradationSample = float4(1.f, 1.f, 1.f, 1.f);
    
     // ================    GLOW 텍스처     ===============
    
    if (Has(g_Effect.g_TextureFlags, GLOWTEXTURE))
    {
        if (HasTextureScroll(SCROLL_GLOW))
        {
            float2 scrolledUV = In.vUV + g_Effect.g_UVOffset;
            scrolledUV += g_Effect.g_ScrollOffset * g_Effect.GlowTexture_ScrollWeight;
            GlowSample = GlowTextureSample(Get90DegreeRotatedUV(scrolledUV, g_Effect.g_RotationFlags, GLOWTEXTURE));
        }
        else if (HasTextureSprite(g_Effect.GlowTexture_SpriteInfo))
        {
            float2 SpriteUV = GetStaticSpriteUV(In.vUV, g_Effect.GlowTexture_SpriteInfo);
            GlowSample = GlowTextureSample(Get90DegreeRotatedUV(SpriteUV, g_Effect.g_RotationFlags, GLOWTEXTURE));
        }
        else
        {
            GlowSample = GlowTextureSample(Get90DegreeRotatedUV(In.vUV, g_Effect.g_RotationFlags, GLOWTEXTURE));
        }

    }
    else
        GlowSample = float4(0.f, 0.f, 0.f, 0.f);
    
    
     // ================    Mask 텍스처     ===============
    
    if (Has(g_Effect.g_TextureFlags, MASKINGTEXTURE))
    {
        if (HasTextureScroll(SCROLL_MASKING))
        {
            float2 scrolledUV = In.vUV + g_Effect.g_UVOffset;
            scrolledUV += g_Effect.g_ScrollOffset * g_Effect.MaskingTexture_ScrollWeight;
            MaskSample = MaskTextureSample(Get90DegreeRotatedUV(scrolledUV, g_Effect.g_RotationFlags, MASKINGTEXTURE));
        }
        else if (HasTextureSprite(g_Effect.MaskTexture_SpriteInfo))
        {
            float2 SpriteUV = GetStaticSpriteUV(In.vUV, g_Effect.MaskTexture_SpriteInfo);
            MaskSample = MaskTextureSample(Get90DegreeRotatedUV(SpriteUV, g_Effect.g_RotationFlags, MASKINGTEXTURE));
        }
        else
        {
            MaskSample = MaskTextureSample(Get90DegreeRotatedUV(In.vUV, g_Effect.g_RotationFlags, MASKINGTEXTURE));
        }

    }
    else
        MaskSample = float4(1.f, 1.f, 1.f, 1.f);
    
     // ================    DISSOLVE 텍스처     ===============
    
    float dissolveMask = 1.0f;
    float dissolveNoise = 0.f;
    
    if (Has(g_Effect.g_TextureFlags, DISSOLVETEXTURE))
    {
        if (HasTextureScroll(SCROLL_DISSOLVE))
        {
            float2 scrolledUV = In.vUV + g_Effect.g_UVOffset;
            scrolledUV += g_Effect.g_ScrollOffset * g_Effect.DissolveTexture_ScrollWeight;
            DissolveSample = DissolveTextureSample(Get90DegreeRotatedUV(scrolledUV, g_Effect.g_RotationFlags, DISSOLVETEXTURE));
            dissolveNoise = DissolveSample.r;
            dissolveMask = step(DissolveProgress, dissolveNoise);
        }
        else if (HasTextureSprite(g_Effect.DissolveTexture_SpriteInfo))
        {
            float2 SpriteUV = GetStaticSpriteUV(In.vUV, g_Effect.DissolveTexture_SpriteInfo);
            DissolveSample = DissolveTextureSample(Get90DegreeRotatedUV(SpriteUV, g_Effect.g_RotationFlags, DISSOLVETEXTURE));
        }
        else
        {
            DissolveSample = DissolveTextureSample(Get90DegreeRotatedUV(In.vUV, g_Effect.g_RotationFlags, DISSOLVETEXTURE));
            dissolveNoise = DissolveSample.r;
            dissolveMask = step(DissolveProgress, dissolveNoise);
        }

    }
    else
    {
        DissolveSample = float4(1.f, 1.f, 1.f, 1.f);
    }
    
         // ================    Curve 텍스처     ===============
    
    float CurvePowerStrength = 1.0f;
    
    if (Has(g_Effect.g_TextureFlags, CURVETEXTURE))
    {
        if (HasTextureSprite(g_Effect.CurveTexture_SpriteInfo))
        {
            float2 SpriteUV = GetStaticSpriteUV(In.vUV, g_Effect.CurveTexture_SpriteInfo);

            if (HasTextureScroll(SCROLL_CURVE))
            {
                SpriteUV += g_Effect.g_ScrollOffset * g_Effect.CurveTexture_ScrollWeight;
            }

            CurveSample = CurveTextureSample(Get90DegreeRotatedUV(SpriteUV, g_Effect.g_RotationFlags, CURVETEXTURE));
            CurvePowerStrength = CurveSample.r;
            CurvePowerStrength *= 2.5f;
        }
        else
        {
            CurveSample = CurveTextureSample(Get90DegreeRotatedUV(In.vUV, g_Effect.g_RotationFlags, CURVETEXTURE));
            CurvePowerStrength = CurveSample.r;
            CurvePowerStrength *= 2.5f;
        }

    }
    else
    {
        CurvePowerStrength = 1.f;
    }
    
    // ===================== SubMask 텍스처 ====================
    if (Has(g_Effect.g_TextureFlags, SUBMASKINGTEXTURE))
    {
        if (HasTextureScroll(SCROLL_SUBMASKING))
        {
            float2 scrolledUV = In.vUV + g_Effect.g_UVOffset;
            scrolledUV += g_Effect.g_ScrollOffset * g_Effect.SubMaskTexture_ScrollWeight;
            SubMaskSample = SubMaskTextureSample(Get90DegreeRotatedUV(scrolledUV, g_Effect.g_RotationFlags, SUBMASKINGTEXTURE));
        }
        else if (HasTextureSprite(g_Effect.SubMaskTexture_SpriteInfo))
        {
            float2 SpriteUV = GetStaticSpriteUV(In.vUV, g_Effect.SubMaskTexture_SpriteInfo);
            SubMaskSample = SubMaskTextureSample(Get90DegreeRotatedUV(SpriteUV, g_Effect.g_RotationFlags, SUBMASKINGTEXTURE));
        }
        else
        {
            SubMaskSample = SubMaskTextureSample(Get90DegreeRotatedUV(In.vUV, g_Effect.g_RotationFlags, SUBMASKINGTEXTURE));
        }

    }
    else
        SubMaskSample = float4(1.f, 1.f, 1.f, 1.f);
    
    

    // =================  계산식 사용  ================
    
        // 5. 최종 결합 (아틀라스 색상 * 캐릭터 고유 색상)
    float3 finalRGB = DiffuseSample.rgb * GradationSample.rgb * g_Effect.g_EffectColor.rgb * CurvePowerStrength;
    finalRGB += (GlowSample.rgb * g_Effect.g_GlowPower);
    
    float lifeAlpha = 1.0f - DissolveProgress;
    float finalAlpha = DiffuseSample.a * MaskSample.r * SubMaskSample.r * dissolveMask * g_Effect.g_EffectColor.a/* * lifeAlpha*/;

    if (HasLifeDissolve())
        finalAlpha *= lifeAlpha;
    
    if (finalAlpha <= g_Effect.g_DiscardValue)
        discard;
    
    float3 srcRGB = finalRGB;
    float srcAlpha = finalAlpha;
    float w = pow(saturate(1.0f - In.vViewZ / 1000.0f), 3.0f); // 3승으로 변화율 조절
    w = clamp(w, 0.01f, 3000.0f); // 상한선을 적당히 열어주되, 하한선으로 방어

    Out.vAccum = float4(srcRGB * srcAlpha, srcAlpha) * w;
    Out.vReveal = srcAlpha;

    return Out;
}


float4 PS_SPRITEMESH(VS_OUT_INST_MESH_PARTICLE In) : SV_Target0
{
    if (In.vLifeTime.x < 0.0f)
        discard;
   // =======              노이즈 텍스처 샘플링             ===========
    float2 finalUV = In.vUV;
    float2 noiseUV = In.vUV;
    float2 detailNoiseUV = In.vUV * 5.0f;
    
    float4 DiffuseSample = { 1.f, 1.f, 1.f, 1.f };
    float4 noiseSample = { 0.5f, 1.f, 1.f, 1.f };
    float4 DetailNoiseSample = { 1.f, 1.f, 1.f, 1.f };
    float4 MaskSample = { 1.f, 1.f, 1.f, 1.f };
    float4 GradationSample = { 1.f, 1.f, 1.f, 1.f };
    float4 GlowSample = { 1.f, 1.f, 1.f, 1.f };
    float4 DissolveSample = { 1.f, 1.f, 1.f, 1.f };
    float noiseValue = { 1.f };
    
    // 생명 주기
    float LifeRatio = saturate(In.vLifeTime.x / In.vLifeTime.y);
    
    
    if (Has(g_Effect.g_TextureFlags, DEFAULTTEXTURE))
    {
        if (HasTextureSprite(g_Effect.DiffuseTexture_SpriteInfo))
        {
            float2 SpriteUV = GetStaticSpriteUV(In.vUV, g_Effect.DiffuseTexture_SpriteInfo);

            DiffuseSample = DefaultTextureSample(SpriteUV);
        }
        else
            DiffuseSample = g_DefaultTextures[DEFAULTTEXTURE].Sample(LinearClampSampler, In.vUV);
    }
    else
        DiffuseSample = float4(1.f, 1.f, 1.f, 1.f);

    float dissolveNoise = float(1.f);
    
    if (Has(g_Effect.g_TextureFlags, NOISETEXTURE))
    {
        dissolveNoise = NoiseTextureSample(Get90DegreeRotatedUV(In.vUV, g_Effect.g_TextureFlags, NOISETEXTURE)).r;
    }
    else
        dissolveNoise = float(1.f);
    
    float dissolveMask = step(LifeRatio, dissolveNoise);
    
    // 최종 알파 결합
    float finalAlpha = DiffuseSample.a * dissolveMask * g_Effect.g_EffectColor.a;

    // 알파 클리핑
    if (finalAlpha < g_Effect.g_DiscardValue)
        discard;
    
    // ======= 최종 결과값 출력 =======
    float4 FinalColor = DiffuseSample * g_Effect.g_EffectColor;
    FinalColor.a = finalAlpha;

    return FinalColor;
}

PS_OUT_WBOIT PS_BloomHard(VS_OUT_INST_MESH_PARTICLE In) : SV_Target0
{
    PS_OUT_WBOIT Out;
    
    if (In.vLifeTime.x < 0.0f)
        discard;
   // =======              노이즈 텍스처 샘플링             ===========
    float2 finalUV = In.vUV;
    
    float4 DiffuseSample = { 1.f, 1.f, 1.f, 1.f };
    float4 noiseSample = { 1.f, 1.f, 1.f, 1.f };
    float4 DetailNoiseSample = { 1.f, 1.f, 1.f, 1.f };
    float4 MaskSample = { 1.f, 1.f, 1.f, 1.f };
    float4 GradationSample = { 1.f, 1.f, 1.f, 1.f };
    float4 GlowSample = { 1.f, 1.f, 1.f, 1.f };
    float4 DissolveSample = { 1.f, 1.f, 1.f, 1.f };
    float4 CurveSample = { 1.f, 1.f, 1.f, 1.f };
    float4 SubMaskSample = { 1.f, 1.f, 1.f, 1.f };
    float noiseValue = { 1.f };
    
    // 1. 진행 비율 계산 (AppearRatio: 등장, DissolveProgress: 소멸)
    float LifeRatio = saturate(In.vLifeTime.x / In.vLifeTime.y);
    float AppearRatio = In.vLifeTime.x / (In.vLifeTime.y * g_Effect.g_AppearRatio);
    float DissolveProgress = saturate((LifeRatio - g_Effect.g_AppearRatio) / max(0.001f, 1.0f - g_Effect.g_AppearRatio));
   
     // ================     노이즈 텍스처     ===============

        // 3. 왜곡량(Offset) 계산
    float2 distortionOffset = float2(0.f, 0.f);
    
    if (Has(g_Effect.g_TextureFlags, NOISETEXTURE))
    {
        if (HasTextureScroll(SCROLL_NOISE))
        {
            float2 scrolledUV = In.vUV + g_Effect.g_UVOffset;
            scrolledUV += g_Effect.g_ScrollOffset * g_Effect.NoiseTexture_ScrollWeight;
            
            noiseSample = NoiseTextureSample(Get90DegreeRotatedUV(scrolledUV, g_Effect.g_RotationFlags, NOISETEXTURE));
            
            distortionOffset.x = (noiseSample.r - 0.5f) * g_Effect.g_DistortionScale.x;
            distortionOffset.y = (noiseSample.g - 0.5f) * g_Effect.g_DistortionScale.y;
        }
        else
        {
            noiseSample = NoiseTextureSample(Get90DegreeRotatedUV(In.vUV, g_Effect.g_RotationFlags, NOISETEXTURE));
            
            distortionOffset.x = (noiseSample.r - 0.5f) * g_Effect.g_DistortionScale.x;
            distortionOffset.y = (noiseSample.g - 0.5f) * g_Effect.g_DistortionScale.y;
        }
    }
    else
    {
        noiseSample = float4(1.f, 1.f, 1.f, 1.f);
        distortionOffset = float2(0.f, 0.f);
    }
    
    // 최종 왜곡 UV (왜곡량)
    float2 distortionUV = distortionOffset;
    
    // ================     메인 텍스처      ===============
    if (Has(g_Effect.g_TextureFlags, DEFAULTTEXTURE))
    {
        if (HasTextureSprite(g_Effect.DiffuseTexture_SpriteInfo))
        {
            float2 SpriteUV = GetStaticSpriteUV(In.vUV, g_Effect.DiffuseTexture_SpriteInfo);
            
            if (Has(g_Effect.g_TextureFlags, NOISETEXTURE))
            {
                SpriteUV += distortionUV;
                DiffuseSample = DefaultTextureSample(Get90DegreeRotatedUV(SpriteUV, g_Effect.g_RotationFlags, DEFAULTTEXTURE));
            }
            else
            {
                DiffuseSample = DefaultTextureSample(Get90DegreeRotatedUV(SpriteUV, g_Effect.g_RotationFlags, DEFAULTTEXTURE));
            }
        }
        else if (HasTextureScroll(SCROLL_DIFFUSE))
        {
            if (Has(g_Effect.g_TextureFlags, NOISETEXTURE))
            {
                float2 scrolledUV = In.vUV + g_Effect.g_UVOffset;
                scrolledUV += g_Effect.g_ScrollOffset * g_Effect.DiffuseTexture_ScrollWeight;
                scrolledUV += distortionUV;
                DiffuseSample = DefaultTextureSample(Get90DegreeRotatedUV(scrolledUV, g_Effect.g_RotationFlags, DEFAULTTEXTURE));
            }
            else
            {
                float2 scrolledUV = In.vUV + g_Effect.g_UVOffset;
                scrolledUV += g_Effect.g_ScrollOffset * g_Effect.DiffuseTexture_ScrollWeight;
                DiffuseSample = DefaultTextureSample(Get90DegreeRotatedUV(scrolledUV, g_Effect.g_RotationFlags, DEFAULTTEXTURE));
            }
        }
        else
        {
            DiffuseSample = DefaultTextureSample(Get90DegreeRotatedUV(In.vUV, g_Effect.g_RotationFlags, DEFAULTTEXTURE));
        }
    }
    else
        DiffuseSample = float4(1.f, 1.f, 1.f, 1.f);
    
    // ================     그라데이션 텍스처     ===============
    
    if (Has(g_Effect.g_TextureFlags, GRADATIONTEXTURE))
    {
        if (HasTextureScroll(SCROLL_GRADATION))
        {
            float2 scrolledUV = In.vUV + g_Effect.g_UVOffset;
            scrolledUV += g_Effect.g_ScrollOffset * g_Effect.GradationTexture_ScrollWeight;
            GradationSample = GradationTextureSample(Get90DegreeRotatedUV(scrolledUV, g_Effect.g_RotationFlags, GRADATIONTEXTURE));
        }
        else if (HasTextureSprite(g_Effect.GradationTexture_SpriteInfo))
        {
            float2 SpriteUV = GetStaticSpriteUV(In.vUV, g_Effect.GradationTexture_SpriteInfo);
            GradationSample = GradationTextureSample(Get90DegreeRotatedUV(SpriteUV, g_Effect.g_RotationFlags, GRADATIONTEXTURE));
        }
        else
        {
            GradationSample = GradationTextureSample(Get90DegreeRotatedUV(In.vUV, g_Effect.g_RotationFlags, GRADATIONTEXTURE));
        }

    }
    else
        GradationSample = float4(1.f, 1.f, 1.f, 1.f);
    
     // ================    GLOW 텍스처     ===============
    
    if (Has(g_Effect.g_TextureFlags, GLOWTEXTURE))
    {
        if (HasTextureScroll(SCROLL_GLOW))
        {
            float2 scrolledUV = In.vUV + g_Effect.g_UVOffset;
            scrolledUV += g_Effect.g_ScrollOffset * g_Effect.GlowTexture_ScrollWeight;
            GlowSample = GlowTextureSample(Get90DegreeRotatedUV(scrolledUV, g_Effect.g_RotationFlags, GLOWTEXTURE));
        }
        else if (HasTextureSprite(g_Effect.GlowTexture_SpriteInfo))
        {
            float2 SpriteUV = GetStaticSpriteUV(In.vUV, g_Effect.GlowTexture_SpriteInfo);
            GlowSample = GlowTextureSample(Get90DegreeRotatedUV(SpriteUV, g_Effect.g_RotationFlags, GLOWTEXTURE));
        }
        else
        {
            GlowSample = GlowTextureSample(Get90DegreeRotatedUV(In.vUV, g_Effect.g_RotationFlags, GLOWTEXTURE));
        }

    }
    else
        GlowSample = float4(0.f, 0.f, 0.f, 0.f);
    
    
    
     // ================    Mask 텍스처     ===============
    
    if (Has(g_Effect.g_TextureFlags, MASKINGTEXTURE))
    {
        if (HasTextureScroll(SCROLL_MASKING))
        {
            float2 scrolledUV = In.vUV + g_Effect.g_UVOffset;
            scrolledUV += g_Effect.g_ScrollOffset * g_Effect.MaskingTexture_ScrollWeight;
            MaskSample = MaskTextureSample(Get90DegreeRotatedUV(scrolledUV, g_Effect.g_RotationFlags, MASKINGTEXTURE));
        }
        else if (HasTextureSprite(g_Effect.MaskTexture_SpriteInfo))
        {
            float2 SpriteUV = GetStaticSpriteUV(In.vUV, g_Effect.MaskTexture_SpriteInfo);
            MaskSample = MaskTextureSample(Get90DegreeRotatedUV(SpriteUV, g_Effect.g_RotationFlags, MASKINGTEXTURE));
        }
        else
        {
            MaskSample = MaskTextureSample(Get90DegreeRotatedUV(In.vUV, g_Effect.g_RotationFlags, MASKINGTEXTURE));
        }

    }
    else
        MaskSample = float4(1.f, 1.f, 1.f, 1.f);
    
     // ================    DISSOLVE 텍스처     ===============
    
    float dissolveMask = 1.0f;
    float dissolveNoise = 0.f;
    
    if (Has(g_Effect.g_TextureFlags, DISSOLVETEXTURE))
    {
        if (HasTextureScroll(SCROLL_DISSOLVE))
        {
            float2 scrolledUV = In.vUV + g_Effect.g_UVOffset;
            scrolledUV += g_Effect.g_ScrollOffset * g_Effect.DissolveTexture_ScrollWeight;
            DissolveSample = DissolveTextureSample(Get90DegreeRotatedUV(scrolledUV, g_Effect.g_RotationFlags, DISSOLVETEXTURE));
            dissolveNoise = DissolveSample.r;
            dissolveMask = step(DissolveProgress, dissolveNoise);
        }
        else if (HasTextureSprite(g_Effect.DissolveTexture_SpriteInfo))
        {
            float2 SpriteUV = GetStaticSpriteUV(In.vUV, g_Effect.DissolveTexture_SpriteInfo);
            DissolveSample = DissolveTextureSample(Get90DegreeRotatedUV(SpriteUV, g_Effect.g_RotationFlags, DISSOLVETEXTURE));
        }
        else
        {
            DissolveSample = DissolveTextureSample(Get90DegreeRotatedUV(In.vUV, g_Effect.g_RotationFlags, DISSOLVETEXTURE));
            dissolveNoise = DissolveSample.r;
            dissolveMask = step(DissolveProgress, dissolveNoise);
        }

    }
    else
    {
        DissolveSample = float4(1.f, 1.f, 1.f, 1.f);
    }
    
         // ================    Curve 텍스처     ===============
    
    float CurvePowerStrength = 1.0f;
    
    if (Has(g_Effect.g_TextureFlags, CURVETEXTURE))
    {
       if (HasTextureSprite(g_Effect.CurveTexture_SpriteInfo))
        {
            float2 SpriteUV = GetStaticSpriteUV(In.vUV, g_Effect.CurveTexture_SpriteInfo);

            if (HasTextureScroll(SCROLL_CURVE))
            {
                SpriteUV += g_Effect.g_ScrollOffset * g_Effect.CurveTexture_ScrollWeight;  
            }

            CurveSample = CurveTextureSample(Get90DegreeRotatedUV(SpriteUV, g_Effect.g_RotationFlags, CURVETEXTURE));
            CurvePowerStrength = CurveSample.r;
            CurvePowerStrength *= 2.5f;
        }
        else
        {
            CurveSample = CurveTextureSample(Get90DegreeRotatedUV(In.vUV, g_Effect.g_RotationFlags, CURVETEXTURE));
            CurvePowerStrength = CurveSample.r;
            CurvePowerStrength *= 2.5f;
        }

    }
    else
    {
        CurvePowerStrength = 1.f;
    }
    
    // ===================== SubMask 텍스처 ====================
    
        if (Has(g_Effect.g_TextureFlags, SUBMASKINGTEXTURE))
    {
        if (HasTextureScroll(SCROLL_SUBMASKING))
        {
            float2 scrolledUV = In.vUV + g_Effect.g_UVOffset;
            scrolledUV += g_Effect.g_ScrollOffset * g_Effect.SubMaskTexture_ScrollWeight;
            SubMaskSample = SubMaskTextureSample(Get90DegreeRotatedUV(scrolledUV, g_Effect.g_RotationFlags, SUBMASKINGTEXTURE));
        }
        else if (HasTextureSprite(g_Effect.SubMaskTexture_SpriteInfo))
        {
            float2 SpriteUV = GetStaticSpriteUV(In.vUV, g_Effect.SubMaskTexture_SpriteInfo);
            SubMaskSample = SubMaskTextureSample(Get90DegreeRotatedUV(SpriteUV, g_Effect.g_RotationFlags, SUBMASKINGTEXTURE));
        }
        else
        {
            SubMaskSample = SubMaskTextureSample(Get90DegreeRotatedUV(In.vUV, g_Effect.g_RotationFlags, SUBMASKINGTEXTURE));
        }

    }
    else
        SubMaskSample = float4(1.f, 1.f, 1.f, 1.f);
    
    
    // =================  계산식 사용  ================
    
        // 5. 최종 결합 (아틀라스 색상 * 캐릭터 고유 색상)
    float3 finalRGB = DiffuseSample.rgb * GradationSample.rgb * g_Effect.g_EffectColor.rgb * CurvePowerStrength;
    finalRGB += (GlowSample.rgb * g_Effect.g_GlowPower);
    
    float lifeAlpha = 1.0f - DissolveProgress;
    float finalAlpha = DiffuseSample.a * MaskSample.r * SubMaskSample.r * dissolveMask * g_Effect.g_EffectColor.a/* * lifeAlpha*/;

    if (HasLifeDissolve())
        finalAlpha *= lifeAlpha;

    // 7. 휘도 컷팅 (깔끔한 마무리)
    //float luminance = dot(finalRGB, float3(0.2126f, 0.7152f, 0.0722f));
    //if (luminance < 0.1f)
    //    discard;
    
    if (finalAlpha <= g_Effect.g_DiscardValue)
        discard;
    
    float3 srcRGB = finalRGB;
    float srcAlpha = finalAlpha;

    float w = pow(saturate(1.0f - In.vViewZ / 1000.0f), 3.0f); // 3승으로 변화율 조절
    w = clamp(w, 0.01f, 3000.0f); // 상한선을 적당히 열어주되, 하한선으로 방어

    Out.vAccum = float4(srcRGB * srcAlpha, srcAlpha) * w;
    Out.vReveal = saturate(srcAlpha);

    return Out;
}


PS_OUT_WBOIT PS_UnityConvert(VS_OUT_INST_MESH_PARTICLE In) : SV_Target0
{
    PS_OUT_WBOIT Out;
    
    if (In.vLifeTime.x < 0.0f)
        discard;
   // =======              노이즈 텍스처 샘플링             ===========
    float2 finalUV = In.vUV;
    
    float4 DiffuseSample = { 1.f, 1.f, 1.f, 1.f };
    float4 noiseSample = { 1.f, 1.f, 1.f, 1.f };
    float4 DetailNoiseSample = { 1.f, 1.f, 1.f, 1.f };
    float4 MaskSample = { 1.f, 1.f, 1.f, 1.f };
    float4 GradationSample = { 1.f, 1.f, 1.f, 1.f };
    float4 GlowSample = { 1.f, 1.f, 1.f, 1.f };
    float4 DissolveSample = { 1.f, 1.f, 1.f, 1.f };
    float4 CurveSample = { 1.f, 1.f, 1.f, 1.f };
    float4 SubMaskSample = { 1.f, 1.f, 1.f, 1.f };
    float noiseValue = { 1.f };
    
    // 1. 진행 비율 계산 (AppearRatio: 등장, DissolveProgress: 소멸)
    float LifeRatio = saturate(In.vLifeTime.x / In.vLifeTime.y);
    float AppearRatio = In.vLifeTime.x / (In.vLifeTime.y * g_Effect.g_AppearRatio);
    float DissolveProgress = saturate((LifeRatio - g_Effect.g_AppearRatio) / max(0.001f, 1.0f - g_Effect.g_AppearRatio));
   
     // ================     노이즈 텍스처     ===============

        // 3. 왜곡량(Offset) 계산
    float2 distortionOffset = float2(0.f, 0.f);
    
    if (Has(g_Effect.g_TextureFlags, NOISETEXTURE))
    {
        if (HasTextureScroll(SCROLL_NOISE))
        {
            float2 scrolledUV = In.vUV + g_Effect.g_UVOffset;
            scrolledUV += g_Effect.g_ScrollOffset * g_Effect.NoiseTexture_ScrollWeight;
            
            noiseSample = NoiseTextureSample(Get90DegreeRotatedUV(scrolledUV, g_Effect.g_RotationFlags, NOISETEXTURE));
            
            distortionOffset.x = (noiseSample.r - 0.5f) * g_Effect.g_DistortionScale.x;
            distortionOffset.y = (noiseSample.g - 0.5f) * g_Effect.g_DistortionScale.y;
        }
        else
        {
            noiseSample = NoiseTextureSample(Get90DegreeRotatedUV(In.vUV, g_Effect.g_RotationFlags, NOISETEXTURE));
            
            distortionOffset.x = (noiseSample.r - 0.5f) * g_Effect.g_DistortionScale.x;
            distortionOffset.y = (noiseSample.g - 0.5f) * g_Effect.g_DistortionScale.y;
        }
    }
    else
    {
        noiseSample = float4(1.f, 1.f, 1.f, 1.f);
        distortionOffset = float2(0.f, 0.f);
    }
    
    // 최종 왜곡 UV (왜곡량)
    float2 distortionUV = distortionOffset;
    
    // ================     메인 텍스처      ===============
    if (Has(g_Effect.g_TextureFlags, DEFAULTTEXTURE))
    {
        if (HasTextureSprite(g_Effect.DiffuseTexture_SpriteInfo))
        {
            float2 SpriteUV = GetStaticSpriteUV(In.vUV, g_Effect.DiffuseTexture_SpriteInfo);
            
            if (Has(g_Effect.g_TextureFlags, NOISETEXTURE))
            {
                SpriteUV += distortionUV;
                DiffuseSample = DefaultTextureSample(Get90DegreeRotatedUV(SpriteUV, g_Effect.g_RotationFlags, DEFAULTTEXTURE));
            }
            else
            {
                DiffuseSample = DefaultTextureSample(Get90DegreeRotatedUV(SpriteUV, g_Effect.g_RotationFlags, DEFAULTTEXTURE));
            }
        }
        else if (HasTextureScroll(SCROLL_DIFFUSE))
        {
            if (Has(g_Effect.g_TextureFlags, NOISETEXTURE))
            {
                float2 scrolledUV = In.vUV + g_Effect.g_UVOffset;
                scrolledUV += g_Effect.g_ScrollOffset * g_Effect.DiffuseTexture_ScrollWeight;
                scrolledUV += distortionUV;
                DiffuseSample = DefaultTextureSample(Get90DegreeRotatedUV(scrolledUV, g_Effect.g_RotationFlags, DEFAULTTEXTURE));
            }
            else
            {
                float2 scrolledUV = In.vUV + g_Effect.g_UVOffset;
                scrolledUV += g_Effect.g_ScrollOffset * g_Effect.DiffuseTexture_ScrollWeight;
                DiffuseSample = DefaultTextureSample(Get90DegreeRotatedUV(scrolledUV, g_Effect.g_RotationFlags, DEFAULTTEXTURE));
            }
        }
        else
        {
            DiffuseSample = DefaultTextureSample(Get90DegreeRotatedUV(In.vUV, g_Effect.g_RotationFlags, DEFAULTTEXTURE));
        }
    }
    else
        DiffuseSample = float4(1.f, 1.f, 1.f, 1.f);
    
    // ================     그라데이션 텍스처     ===============
    
    if (Has(g_Effect.g_TextureFlags, GRADATIONTEXTURE))
    {
        if (HasTextureScroll(SCROLL_GRADATION))
        {
            float2 scrolledUV = In.vUV + g_Effect.g_UVOffset;
            scrolledUV += g_Effect.g_ScrollOffset * g_Effect.GradationTexture_ScrollWeight;
            GradationSample = GradationTextureSample(Get90DegreeRotatedUV(scrolledUV, g_Effect.g_RotationFlags, GRADATIONTEXTURE));
        }
        else if (HasTextureSprite(g_Effect.GradationTexture_SpriteInfo))
        {
            float2 SpriteUV = GetStaticSpriteUV(In.vUV, g_Effect.GradationTexture_SpriteInfo);
            GradationSample = GradationTextureSample(Get90DegreeRotatedUV(SpriteUV, g_Effect.g_RotationFlags, GRADATIONTEXTURE));
        }
        else
        {
            GradationSample = GradationTextureSample(Get90DegreeRotatedUV(In.vUV, g_Effect.g_RotationFlags, GRADATIONTEXTURE));
        }

    }
    else
        GradationSample = float4(1.f, 1.f, 1.f, 1.f);
    
     // ================    GLOW 텍스처     ===============
    
    if (Has(g_Effect.g_TextureFlags, GLOWTEXTURE))
    {
        if (HasTextureScroll(SCROLL_GLOW))
        {
            float2 scrolledUV = In.vUV + g_Effect.g_UVOffset;
            scrolledUV += g_Effect.g_ScrollOffset * g_Effect.GlowTexture_ScrollWeight;
            GlowSample = GlowTextureSample(Get90DegreeRotatedUV(scrolledUV, g_Effect.g_RotationFlags, GLOWTEXTURE));
        }
        else if (HasTextureSprite(g_Effect.GlowTexture_SpriteInfo))
        {
            float2 SpriteUV = GetStaticSpriteUV(In.vUV, g_Effect.GlowTexture_SpriteInfo);
            GlowSample = GlowTextureSample(Get90DegreeRotatedUV(SpriteUV, g_Effect.g_RotationFlags, GLOWTEXTURE));
        }
        else
        {
            GlowSample = GlowTextureSample(Get90DegreeRotatedUV(In.vUV, g_Effect.g_RotationFlags, GLOWTEXTURE));
        }

    }
    else
        GlowSample = float4(0.f, 0.f, 0.f, 0.f);
    
    
     // ================    Mask 텍스처     ===============
    
    if (Has(g_Effect.g_TextureFlags, MASKINGTEXTURE))
    {
        if (HasTextureScroll(SCROLL_MASKING))
        {
            float2 scrolledUV = In.vUV + g_Effect.g_UVOffset;
            scrolledUV += g_Effect.g_ScrollOffset * g_Effect.MaskingTexture_ScrollWeight;
            MaskSample = MaskTextureSample(Get90DegreeRotatedUV(scrolledUV, g_Effect.g_RotationFlags, MASKINGTEXTURE));
        }
        else if (HasTextureSprite(g_Effect.MaskTexture_SpriteInfo))
        {
            float2 SpriteUV = GetStaticSpriteUV(In.vUV, g_Effect.MaskTexture_SpriteInfo);
            MaskSample = MaskTextureSample(Get90DegreeRotatedUV(SpriteUV, g_Effect.g_RotationFlags, MASKINGTEXTURE));
        }
        else
        {
            MaskSample = MaskTextureSample(Get90DegreeRotatedUV(In.vUV, g_Effect.g_RotationFlags, MASKINGTEXTURE));
        }

    }
    else
        MaskSample = float4(1.f, 1.f, 1.f, 1.f);
    
     // ================    DISSOLVE 텍스처     ===============
    
    float dissolveMask = 1.0f;
    float dissolveNoise = 0.f;
    
    if (Has(g_Effect.g_TextureFlags, DISSOLVETEXTURE))
    {
        if (HasTextureScroll(SCROLL_DISSOLVE))
        {
            float2 scrolledUV = In.vUV + g_Effect.g_UVOffset;
            scrolledUV += g_Effect.g_ScrollOffset * g_Effect.DissolveTexture_ScrollWeight;
            DissolveSample = DissolveTextureSample(Get90DegreeRotatedUV(scrolledUV, g_Effect.g_RotationFlags, DISSOLVETEXTURE));
            dissolveNoise = DissolveSample.r;
            dissolveMask = step(DissolveProgress, dissolveNoise);
        }
        else if (HasTextureSprite(g_Effect.DissolveTexture_SpriteInfo))
        {
            float2 SpriteUV = GetStaticSpriteUV(In.vUV, g_Effect.DissolveTexture_SpriteInfo);
            DissolveSample = DissolveTextureSample(Get90DegreeRotatedUV(SpriteUV, g_Effect.g_RotationFlags, DISSOLVETEXTURE));
        }
        else
        {
            DissolveSample = DissolveTextureSample(Get90DegreeRotatedUV(In.vUV, g_Effect.g_RotationFlags, DISSOLVETEXTURE));
            dissolveNoise = DissolveSample.r;
            dissolveMask = step(DissolveProgress, dissolveNoise);
        }

    }
    else
    {
        DissolveSample = float4(1.f, 1.f, 1.f, 1.f);
    }
    
         // ================    Curve 텍스처     ===============
    
    float CurvePowerStrength = 1.0f;
    
    if (Has(g_Effect.g_TextureFlags, CURVETEXTURE))
    {
        if (HasTextureSprite(g_Effect.CurveTexture_SpriteInfo))
        {
            float2 SpriteUV = GetStaticSpriteUV(In.vUV, g_Effect.CurveTexture_SpriteInfo);

            if (HasTextureScroll(SCROLL_CURVE))
            {
                SpriteUV += g_Effect.g_ScrollOffset * g_Effect.CurveTexture_ScrollWeight;
            }

            CurveSample = CurveTextureSample(Get90DegreeRotatedUV(SpriteUV, g_Effect.g_RotationFlags, CURVETEXTURE));
            CurvePowerStrength = CurveSample.r;
            CurvePowerStrength *= 2.5f;
        }
        else
        {
            CurveSample = CurveTextureSample(Get90DegreeRotatedUV(In.vUV, g_Effect.g_RotationFlags, CURVETEXTURE));
            CurvePowerStrength = CurveSample.r;
            CurvePowerStrength *= 2.5f;
        }

    }
    else
    {
        CurvePowerStrength = 1.f;
    }
    
    // ===================== SubMask 텍스처 ====================
    if (Has(g_Effect.g_TextureFlags, SUBMASKINGTEXTURE))
    {
        if (HasTextureScroll(SCROLL_SUBMASKING))
        {
            float2 scrolledUV = In.vUV + g_Effect.g_UVOffset;
            scrolledUV += g_Effect.g_ScrollOffset * g_Effect.SubMaskTexture_ScrollWeight;
            SubMaskSample = SubMaskTextureSample(Get90DegreeRotatedUV(scrolledUV, g_Effect.g_RotationFlags, SUBMASKINGTEXTURE));
        }
        else if (HasTextureSprite(g_Effect.SubMaskTexture_SpriteInfo))
        {
            float2 SpriteUV = GetStaticSpriteUV(In.vUV, g_Effect.SubMaskTexture_SpriteInfo);
            SubMaskSample = SubMaskTextureSample(Get90DegreeRotatedUV(SpriteUV, g_Effect.g_RotationFlags, SUBMASKINGTEXTURE));
        }
        else
        {
            SubMaskSample = SubMaskTextureSample(Get90DegreeRotatedUV(In.vUV, g_Effect.g_RotationFlags, SUBMASKINGTEXTURE));
        }

    }
    else
        SubMaskSample = float4(1.f, 1.f, 1.f, 1.f);
    
    // =================  계산식 사용  ================
    
        // 5. 최종 결합 (아틀라스 색상 * 캐릭터 고유 색상)
    float3 finalRGB = DiffuseSample.rgb * GradationSample.rgb * CurvePowerStrength;
    finalRGB += (GlowSample.rgb * g_Effect.g_GlowPower);
    finalRGB *= g_Effect.g_EffectColor.rgb;
    
    float lifeAlpha = 1.0f - DissolveProgress;
    float finalAlpha = DiffuseSample.a * MaskSample.r * SubMaskSample.r * dissolveMask * g_Effect.g_EffectColor.a/* * lifeAlpha*/;

    if (HasLifeDissolve())
        finalAlpha *= lifeAlpha;
    
    if (finalAlpha <= g_Effect.g_DiscardValue)
        discard;
    
    float3 srcRGB = finalRGB;
    float srcAlpha = finalAlpha;
    float w = pow(saturate(1.0f - In.vViewZ / 1000.0f), 3.0f); // 3승으로 변화율 조절
    w = clamp(w, 0.01f, 3000.0f); // 상한선을 적당히 열어주되, 하한선으로 방어

    Out.vAccum = float4(srcRGB * srcAlpha, srcAlpha) * w;
    Out.vReveal = srcAlpha;

    return Out;
}

float4 PS_DISTOTION(VS_OUT_INST_MESH_PARTICLE In) : SV_Target0
{
    if (In.vLifeTime.x < 0.0f)
        discard;
    
    // 1. 화면 좌표계(ScreenUV) 계산 - 현재 픽셀의 정확한 위치
    float2 ScreenUV = In.vProjPos.xy / In.vProjPos.w;
    ScreenUV.x = ScreenUV.x * 0.5f + 0.5f;
    ScreenUV.y = ScreenUV.y * -0.5f + 0.5f;

    // 2. 초기 깊이 판정 (원본 위치 기준)
    float fBackNDCZ, fBackViewZ;
    DecodeDepth(ScreenUV, fBackNDCZ, fBackViewZ); //

    float4 vMyViewPos = mul(In.vWorldPos, V);
    float fMyViewZ = vMyViewPos.z;

    // 캐릭터 등 이펙트보다 앞에 있는 물체는 왜곡하지 않음 (Z값이 작으면 앞)
    if (fBackViewZ > 0.0f && fBackViewZ < fMyViewZ - 0.1f)
    {
        discard;
    }

    // 3. 왜곡량(Offset) 계산
    float2 distortionOffset = float2(0.f, 0.f);
    if (Has(g_Effect.g_TextureFlags, NOISETEXTURE))
    {
        float2 noiseUV = Get90DegreeRotatedUV(In.vUV, g_Effect.g_RotationFlags, NOISETEXTURE);
        noiseUV += g_Effect.g_ScrollOffset; // 커브가 적용된 실시간 오프셋
        
        float4 noiseSample = NoiseTextureSample(noiseUV);
        
        // [공식 수정] 노이즈의 r, g 채널을 각각 X, Y 왜곡에 사용하여 단조로움 탈피
        // (값 - 0.5f)를 통해 양방향(-0.5 ~ 0.5)으로 흔들리게 설정
        distortionOffset.x = (noiseSample.r - 0.5f) * g_Effect.g_DistortionScale.x;
        distortionOffset.y = (noiseSample.g - 0.5f) * g_Effect.g_DistortionScale.y;

        // 마스킹 텍스처가 있다면 외곽선 왜곡을 부드럽게 감쇄
        if (Has(g_Effect.g_TextureFlags, MASKINGTEXTURE))
        {
            float4 MaskSample = MaskTextureSample(Get90DegreeRotatedUV(In.vUV, g_Effect.g_RotationFlags, MASKINGTEXTURE));
            distortionOffset *= MaskSample.r;
        }
    }

    // 최종 왜곡 UV (화면 좌표 + 왜곡량)
    float2 distortionUV = ScreenUV + distortionOffset;

    // 4. 경계 예외 처리 (화면 밖 픽셀 참조 방지)
    // 왜곡된 좌표가 0~1을 벗어나면 원본 위치를 참조하여 잘림 현상 방지
    if (any(distortionUV < 0.0f) || any(distortionUV > 1.0f))
    {
        distortionUV = ScreenUV;
    }

    // 5. 왜곡된 위치의 깊이 재검사 (앞 사물이 왜곡 안으로 들어오는 고스트 현상 방지)
    float fDistZ, fDistViewZ;
    DecodeDepth(distortionUV, fDistZ, fDistViewZ);
    if (fDistViewZ > 0.0f && fDistViewZ < fMyViewZ)
    {
        distortionUV = ScreenUV;
    }
    
    float4 refractionColor = g_RenderTargetSceneHDRCopyTexture.Sample(LinearClampSampler, distortionUV);

    float2 scrolledUV = In.vUV + g_Effect.g_UVOffset;
    scrolledUV += g_Effect.g_ScrollOffset;
    float4 mainTex = g_DefaultTextures[DEFAULTTEXTURE].Sample(LinearClampSampler, scrolledUV);
    // 수명에 따른 투명도 계산
    float lifeAlpha = 1.0f - (In.vLifeTime.x / In.vLifeTime.y);
    float finalAlpha = mainTex.a * lifeAlpha;

    if (finalAlpha < g_Effect.g_DiscardValue)
        discard;

    // 굴절된 배경색에 이펙트 고유 색상을 살짝 얹어줌
    return float4(refractionColor.rgb + (g_Effect.g_EffectColor.rgb * 0.2f), finalAlpha);
}

float4 PS_SWORDEFFECT(VS_OUT_INST_MESH_PARTICLE In) : SV_Target0
{
    if (In.vLifeTime.x < 0.0f)
        discard;
    
    float2 finalUV = In.vUV;
    float4 DiffuseSample = { 1.f, 1.f, 1.f, 1.f };
    
    // 1. 진행 비율 계산 (AppearRatio: 등장, DissolveProgress: 소멸)
    float LifeRatio = saturate(In.vLifeTime.x / In.vLifeTime.y);
    float AppearRatio = In.vLifeTime.x / (In.vLifeTime.y * g_Effect.g_AppearRatio);
    float DissolveProgress = saturate((LifeRatio - g_Effect.g_AppearRatio) / (1.0f - g_Effect.g_AppearRatio));

    // 2. [DNA 스타일] 이중 노이즈 레이어링
    float noiseValue = 1.0f;
    if (Has(g_Effect.g_TextureFlags, NOISETEXTURE))
    {
        // JSON 분석 수치: 큰 흐름(2x3)과 미세 흐름(5x5)을 섞음
        float2 uv1 = In.vUV * float2(2.0f, 3.0f) + g_Effect.g_ScrollOffset * 0.25f;
        float2 uv2 = In.vUV * float2(5.0f, 5.0f) + g_Effect.g_ScrollOffset * 0.25f;
        
        float n1 = NoiseTextureSample(uv1).g; // Turbulence G채널
        float n2 = NoiseTextureSample(uv2).r; // Turbulence R채널
        noiseValue = n1 * n2;
        
        // 일렁이는 왜곡 효과 (Distortion)
        finalUV += (noiseValue - 0.5f) * g_Effect.g_DistortionScale;
    }

    // 3. [모양 잡기] 메인 텍스처(칼날 마스크) 및 등장 로직
    if (Has(g_Effect.g_TextureFlags, DEFAULTTEXTURE))
    {
        // 오른쪽에서 왼쪽으로 슥- 나타나는 컷팅
        if ((1.0f - finalUV.x) > AppearRatio)
            discard;
            
        DiffuseSample = DefaultTextureSample(finalUV);
    }

    // 4. [색상 뽑기] 컬러 아틀라스(GradationTexture) 적용
    // V 좌표는 캐릭터별 인덱스(예: bx=20번 칸)를 사용함
    float targetRow = 20.0f; // 툴에서 캐릭터별로 받아올 변수
    float vCoord = (targetRow + 0.5f) / 100.0f;
    
    // 노이즈 밝기(X)와 캐릭터 인덱스(V)로 아틀라스에서 색상 추출
    float4 AtlasColor = GradationTextureSample(float2(noiseValue, vCoord));

    // 5. 최종 결합 (아틀라스 색상 * 캐릭터 고유 색상)
    float3 finalRGB = AtlasColor.rgb * g_Effect.g_EffectColor.rgb;
    float finalAlpha = DiffuseSample.a * AtlasColor.a * g_Effect.g_EffectColor.a;

    // 6. [소멸 로직] 아래에서 위로 밀려나며 사라지기
    if (AppearRatio > 1.0f)
    {
        finalUV.y += g_Effect.g_ScrollOffset.y; // 위로 스크롤
        if ((1.0f - In.vUV.y) < DissolveProgress)
            discard; // 아래부터 깎기
    }

    // 7. 휘도 컷팅 (깔끔한 마무리)
    float luminance = dot(finalRGB, float3(0.2126f, 0.7152f, 0.0722f));
    if (luminance < 0.1f)
        discard;

    return float4(finalRGB, finalAlpha);
}


technique11 T0
{
    pass Mesh_Effect
    {
        SetRasterizerState(RS_Default_CullNone);
        SetDepthStencilState(DS_ReadOnly, 0);
        SetBlendState(BS_WBOIT_Accumulate, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetVertexShader(CompileShader(vs_5_0, VS_DEFAULT()));
        GeometryShader = NULL;
        SetPixelShader(CompileShader(ps_5_0, PS_DefaultMesh()));
    }

    pass SPRITE_EFFECT
    {
        SetRasterizerState(RS_Default_CullNone);
        SetDepthStencilState(DS_ReadOnly, 0);
        SetBlendState(BS_WBOIT_Accumulate, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetVertexShader(CompileShader(vs_5_0, VS_DEFAULT()));
        GeometryShader = NULL;
        SetPixelShader(CompileShader(ps_5_0, PS_SPRITEMESH()));
    }
    
    pass Mesh_Effect_BloomHard
    {
        SetRasterizerState(RS_Default_CullNone);
        SetDepthStencilState(DS_ReadOnly, 0);
        SetBlendState(BS_WBOIT_Accumulate, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetVertexShader(CompileShader(vs_5_0, VS_DEFAULT()));
        GeometryShader = NULL;
        SetPixelShader(CompileShader(ps_5_0, PS_BloomHard()));
    }

    pass Distotion_EFFECT
    {
        SetRasterizerState(RS_Default_CullNone);
        SetDepthStencilState(DS_ReadOnly, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetVertexShader(CompileShader(vs_5_0, VS_DEFAULT()));
        GeometryShader = NULL;
        SetPixelShader(CompileShader(ps_5_0, PS_DISTOTION()));
    }

    pass BLOOM_SWORDEFFECT
    {
        SetRasterizerState(RS_Default_CullNone);
        SetDepthStencilState(DS_ReadOnly, 0);
        SetBlendState(BS_WBOIT_Accumulate, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetVertexShader(CompileShader(vs_5_0, VS_DEFAULT()));
        GeometryShader = NULL;
        SetPixelShader(CompileShader(ps_5_0, PS_UnityConvert()));
    }

    pass None_DepthDefault
    {
        SetRasterizerState(RS_Default_CullNone);
        SetDepthStencilState(DS_Disabled, 0);
        SetBlendState(BS_WBOIT_Accumulate, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetVertexShader(CompileShader(vs_5_0, VS_DEFAULT()));
        GeometryShader = NULL;
        SetPixelShader(CompileShader(ps_5_0, PS_DefaultMesh()));
    }
}
