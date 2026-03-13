#include "Effect_Defines.hlsl"

VS_OUT_POS_GS_PARTICLE VS_Texture(VS_IN_POS_GS_PARTICLE In)
{
    VS_OUT_POS_GS_PARTICLE Out;
    
    matrix matInst = INSTANCE_OUTPUT[In.vInstID].matTransform;
    
    vector vPosition = mul(vector(In.vPosition, 1.f), matInst);
    vPosition = mul(vPosition, W);
    
    float2 pSize = float2(length(matInst._11_12_13), length(matInst._21_22_23));

    // 사이즈 계산
    Out.vPosition = vPosition;
    Out.vPSize = float3(length(W._11_12_13) * pSize.x, length(W._21_22_23) * pSize.y, length(W._31_32_33));
    Out.vLifeTime = INSTANCE_OUTPUT[In.vInstID].vLifeTime;
    Out.vInstID = In.vInstID;
    Out.matTransform = W;
    
    return Out;
}


[maxvertexcount(6)]
void GS_Texture(point VS_OUT_POS_GS_PARTICLE In[1], inout TriangleStream<GS_OUT_EFFECT_PARTICLE> OutStream)
{
    GS_OUT_EFFECT_PARTICLE Out[4];
    
    float3 vRight = float3(1.f, 0.f, 0.f);
    float3 vUp = float3(0.f, 1.f, 0.f);

    if (HasBillboard())
    {
        // 카메라를 향하는 기본 빌보드 축 생성
        float3 vLook = normalize(CameraPosition() - In[0].vPosition.xyz);
        vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook));
        vUp = normalize(cross(vLook, vRight));

        float fAngle = atan2(W._12, W._11);

        if (fAngle != 0.0f)
        {
            float s, c;
            sincos(fAngle, s, c);

            // 빌보드 축을 fAngle만큼 회전 (평면 회전)
            float3 vRotRight = vRight * c + vUp * s;
            float3 vRotUp = -vRight * s + vUp * c;

            vRight = vRotRight;
            vUp = vRotUp;
        }
    }
    else
    {
        // 빌보드가 아닐 경우 월드 행렬의 축을 그대로 사용
        vRight = normalize(W[0].xyz);
        vUp = normalize(W[1].xyz);
    }

    // 최종 크기 적용 (VS에서 넘어온 pSize 사용)
    vRight *= In[0].vPSize.x;
    vUp *= In[0].vPSize.y;
    
    matrix matVP = mul(V, P);
    
    // 정점 위치 계산 (중심점 In[0].vPosition 기준)
    float3 vFinalPos[4];
    vFinalPos[0] = In[0].vPosition.xyz - vRight + vUp; // 좌상
    vFinalPos[1] = In[0].vPosition.xyz + vRight + vUp; // 우상
    vFinalPos[2] = In[0].vPosition.xyz - vRight - vUp; // 좌하
    vFinalPos[3] = In[0].vPosition.xyz + vRight - vUp; // 우하

    // 기본 UV만 설정
    float2 vFinalUV[4] = { float2(0, 0), float2(1, 0), float2(0, 1), float2(1, 1) };

    for (int i = 0; i < 4; ++i)
    {
        Out[i].vPosition = mul(float4(vFinalPos[i], 1.f), matVP);
        Out[i].vUV = vFinalUV[i];
        Out[i].vSpriteUV = float2(0, 0); // 이제 사용 안함 (혹은 제거 가능)
        Out[i].vLifeTime = In[0].vLifeTime;
        OutStream.Append(Out[i]);
    }
    OutStream.RestartStrip();
}


float4 PS_Texture(GS_OUT_EFFECT_PARTICLE In) : SV_TARGET0
{
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
        GlowSample = float4(1.f, 1.f, 1.f, 1.f);
    
    
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

    // =================  계산식 사용  ================
    
        // 5. 최종 결합 (아틀라스 색상 * 캐릭터 고유 색상)
    float3 finalRGB = DiffuseSample.rgb * GradationSample.rgb * g_Effect.g_EffectColor.rgb * CurvePowerStrength;
    
    float lifeAlpha = 1.0f - DissolveProgress;
    float finalAlpha = DiffuseSample.a * GlowSample.r * MaskSample.r * dissolveMask * g_Effect.g_EffectColor.a /** lifeAlpha*/;

    if (HasLifeDissolve())
        finalAlpha *= lifeAlpha;
    
    if (finalAlpha <= g_Effect.g_DiscardValue)
        discard;
    
    return float4(finalRGB, finalAlpha);
}


PS_OUT_WBOIT PS_TextureBloomHard(GS_OUT_EFFECT_PARTICLE In)
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
        GlowSample = float4(1.f, 1.f, 1.f, 1.f);
    
    
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

    // =================  계산식 사용  ================
    
        // 5. 최종 결합 (아틀라스 색상 * 캐릭터 고유 색상)
    float3 finalRGB = DiffuseSample.rgb * GradationSample.rgb * g_Effect.g_EffectColor.rgb * CurvePowerStrength;
    
    float lifeAlpha = 1.0f - DissolveProgress;
    float finalAlpha = DiffuseSample.a * GlowSample.r * MaskSample.r * dissolveMask * g_Effect.g_EffectColor.a /** lifeAlpha*/;

    if (HasLifeDissolve())
        finalAlpha *= lifeAlpha;

    // 7. 휘도 컷팅 (깔끔한 마무리)
    //float luminance = dot(finalRGB, float3(0.2126f, 0.7152f, 0.0722f));
    //if (luminance < 0.1f)
    //    discard;
    
    if (finalAlpha <= g_Effect.g_DiscardValue)
        discard;
    
    Out.vAccum = float4(finalRGB * finalAlpha, finalAlpha);
    Out.vReveal = finalAlpha;
    return Out;

}


technique11 T0
{
    pass Texture_Effect
    {
        SetRasterizerState(RS_Default_CullNone);
        SetDepthStencilState(DS_Default, 0);
        //SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetVertexShader(CompileShader(vs_5_0, VS_Texture()));
        SetGeometryShader(CompileShader(gs_5_0, GS_Texture()));
        SetPixelShader(CompileShader(ps_5_0, PS_Texture()));
    }

    pass Texture_BlendEffect
    {
        SetRasterizerState(RS_Default_CullNone);
        SetDepthStencilState(DS_Default, 0);
        //SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetVertexShader(CompileShader(vs_5_0, VS_Texture()));
        SetGeometryShader(CompileShader(gs_5_0, GS_Texture()));
        SetPixelShader(CompileShader(ps_5_0, PS_Texture()));
    }

    pass Texture_BloomHard
    {
        SetRasterizerState(RS_Default_CullNone);
        SetDepthStencilState(DS_Default, 0);
        //SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetVertexShader(CompileShader(vs_5_0, VS_Texture()));
        SetGeometryShader(CompileShader(gs_5_0, GS_Texture()));
        SetPixelShader(CompileShader(ps_5_0, PS_TextureBloomHard()));
    }

    pass Texture_BlendBloomHard
    {
        SetRasterizerState(RS_Default_CullNone);
        SetDepthStencilState(DS_ReadOnly, 0);
        //SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetVertexShader(CompileShader(vs_5_0, VS_Texture())); 
        SetGeometryShader(CompileShader(gs_5_0, GS_Texture()));
        SetPixelShader(CompileShader(ps_5_0, PS_TextureBloomHard()));
    }

    pass None_DepthDefault
    {
        SetRasterizerState(RS_Default_CullNone);
        SetDepthStencilState(DS_Disabled, 0);
        //SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetVertexShader(CompileShader(vs_5_0, VS_Texture()));
        SetGeometryShader(CompileShader(gs_5_0, GS_Texture()));
        SetPixelShader(CompileShader(ps_5_0, PS_Texture()));
    }
}