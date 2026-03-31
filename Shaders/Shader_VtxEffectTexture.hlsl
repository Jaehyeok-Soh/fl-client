#include "Effect_Defines.hlsl"

VS_OUT_POS_GS_PARTICLE VS_Texture(VS_IN_POS_GS_PARTICLE In)
{
    VS_OUT_POS_GS_PARTICLE Out;
    
    matrix matInst = INSTANCE_OUTPUT[In.vInstID].matTransform;
    
    vector vPosition = mul(vector(In.vPosition, 1.f), matInst);
    vPosition = mul(vPosition, W);
    
    Out.vPosition = vPosition;
    
    float4 vViewPos = mul(vPosition, V);
    float4 vProj = mul(vViewPos, P);
    
    float2 pSize = float2(length(matInst._11_12_13), length(matInst._21_22_23));

    // 사이즈 계산

    Out.vWorldPos = vPosition;
    Out.vProjPos = vProj;
    Out.vPSize = float3(length(W._11_12_13) * pSize.x, length(W._21_22_23) * pSize.y, length(W._31_32_33));
    Out.vLifeTime = INSTANCE_OUTPUT[In.vInstID].vLifeTime;
    Out.vInstID = In.vInstID;
    Out.matTransform = W;
    Out.vViewZ = vViewPos.z;
    
    return Out;
}


[maxvertexcount(6)]
void GS_Texture(point VS_OUT_POS_GS_PARTICLE In[1], inout TriangleStream<GS_OUT_EFFECT_PARTICLE> OutStream)
{
    GS_OUT_EFFECT_PARTICLE Out[4];
    matrix matVP = mul(V, P);

    float3 vRight, vUp;
    
  

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
        // 회전 반영: 인스턴스 행렬에서 직접 X, Y축 추출
        matrix matInst = INSTANCE_OUTPUT[In[0].vInstID].matTransform;
        matrix matWorld = mul(matInst, W);
        
        vRight = normalize(matWorld[0].xyz);
        vUp = normalize(matWorld[2].xyz);
    }

    float3 vScaledRight = vRight * In[0].vPSize.x * 0.5f;
    float3 vScaledUp = vUp * In[0].vPSize.y * 0.5f;

    float3 vPos[4];
    vPos[0] = In[0].vPosition.xyz - vScaledRight + vScaledUp; // 좌상
    vPos[1] = In[0].vPosition.xyz + vScaledRight + vScaledUp; // 우상
    vPos[2] = In[0].vPosition.xyz - vScaledRight - vScaledUp; // 좌하
    vPos[3] = In[0].vPosition.xyz + vScaledRight - vScaledUp; // 우하

    float2 vUV[4] = { float2(0, 0), float2(1, 0), float2(0, 1), float2(1, 1) };

    [unroll]
    for (int i = 0; i < 4; ++i)
    {
        Out[i].vUV = vUV[i];
        Out[i].vLifeTime = In[0].vLifeTime;
        Out[i].vSpriteUV = float2(0, 0);
        Out[i].vViewZ = In[0].vViewZ;
        
        Out[i].vPosition = mul(float4(vPos[i], 1.f), matVP);
        Out[i].vProjPos = Out[i].vPosition;
        Out[i].vWorldPos = float4(vPos[i], 1.f);
        OutStream.Append(Out[i]);
    }
    OutStream.RestartStrip();
}

[maxvertexcount(6)]
void Line_GS_Texture(point VS_OUT_POS_GS_PARTICLE In[1], inout TriangleStream<GS_OUT_EFFECT_PARTICLE> OutStream)
{
    GS_OUT_EFFECT_PARTICLE Out;
    matrix matVP = mul(V, P);

    float3 vStart = g_LineEffect.g_StartPos;
    float3 vEnd = g_LineEffect.g_EndPos;
    float fHalfWidth = g_LineEffect.g_HalfWidth;

    float3 vLine = vEnd - vStart;
    float fLength = length(vLine);

    // 길이가 너무 짧으면 그리지 않음
    if (fLength < 0.0001f)
        return;

    float3 vLineDir = vLine / fLength;

    // 중심점 기준으로 카메라 방향 계산
    float3 vCenter = (vStart + vEnd) * 0.5f;
    float3 vUp = float3(0.f, 1.f, 0.f);

    // 선에 직교하는 폭 방향
    float3 vSideDir = cross(vUp, vLineDir);
    float fSideLen = length(vSideDir);

    // 카메라 방향과 선 방향이 거의 평행하면 fallback
    if (fSideLen < 0.0001f)
    {
        vSideDir = cross(float3(0.f, 1.f, 0.f), vLineDir);
        fSideLen = length(vSideDir);

        if (fSideLen < 0.0001f)
        {
            vSideDir = cross(float3(1.f, 0.f, 0.f), vLineDir);
            fSideLen = length(vSideDir);

            if (fSideLen < 0.0001f)
                return;
        }
    }

    vSideDir /= fSideLen;
    float3 vOffset = vSideDir * fHalfWidth;

    // 사각형 4점
    float3 vPos0 = vStart + vOffset; // Start Left
    float3 vPos1 = vStart - vOffset; // Start Right
    float3 vPos2 = vEnd + vOffset; // End Left
    float3 vPos3 = vEnd - vOffset; // End Right

    float2 vUV0 = float2(0.f, 0.f);
    float2 vUV1 = float2(1.f, 0.f);
    float2 vUV2 = float2(0.f, 1.f);
    float2 vUV3 = float2(1.f, 1.f);

    float fViewZ = mul(float4(vCenter, 1.f), V).z;

    // Triangle 1 : vPos0, vPos2, vPos1
    Out.vPosition = mul(float4(vPos0, 1.f), matVP);
    Out.vUV = vUV0;
    Out.vLifeTime = In[0].vLifeTime;
    Out.vSpriteUV = float2(0.f, 0.f);
    Out.vViewZ = fViewZ;
    Out.vProjPos = In[0].vProjPos;
    Out.vWorldPos = In[0].vWorldPos;
    OutStream.Append(Out);

    Out.vPosition = mul(float4(vPos2, 1.f), matVP);
    Out.vUV = vUV2;
    Out.vLifeTime = In[0].vLifeTime;
    Out.vSpriteUV = float2(0.f, 0.f);
    Out.vViewZ = fViewZ;
    Out.vProjPos = In[0].vProjPos;
    Out.vWorldPos = In[0].vWorldPos;
    OutStream.Append(Out);

    Out.vPosition = mul(float4(vPos1, 1.f), matVP);
    Out.vUV = vUV1;
    Out.vLifeTime = In[0].vLifeTime;
    Out.vSpriteUV = float2(0.f, 0.f);
    Out.vViewZ = fViewZ;
    Out.vProjPos = In[0].vProjPos;
    Out.vWorldPos = In[0].vWorldPos;
    OutStream.Append(Out);

    OutStream.RestartStrip();

    // Triangle 2 : vPos1, vPos2, vPos3
    Out.vPosition = mul(float4(vPos1, 1.f), matVP);
    Out.vUV = vUV1;
    Out.vLifeTime = In[0].vLifeTime;
    Out.vSpriteUV = float2(0.f, 0.f);
    Out.vViewZ = fViewZ;
    Out.vProjPos = In[0].vProjPos;
    Out.vWorldPos = In[0].vWorldPos;
    OutStream.Append(Out);

    Out.vPosition = mul(float4(vPos2, 1.f), matVP);
    Out.vUV = vUV2;
    Out.vLifeTime = In[0].vLifeTime;
    Out.vSpriteUV = float2(0.f, 0.f);
    Out.vViewZ = fViewZ;
    Out.vProjPos = In[0].vProjPos;
    Out.vWorldPos = In[0].vWorldPos;
    OutStream.Append(Out);

    Out.vPosition = mul(float4(vPos3, 1.f), matVP);
    Out.vUV = vUV3;
    Out.vLifeTime = In[0].vLifeTime;
    Out.vSpriteUV = float2(0.f, 0.f);
    Out.vViewZ = fViewZ;
    Out.vProjPos = In[0].vProjPos;
    Out.vWorldPos = In[0].vWorldPos;
    OutStream.Append(Out);
    
        

    OutStream.RestartStrip();
}

PS_OUT_WBOIT PS_Texture(GS_OUT_EFFECT_PARTICLE In) : SV_TARGET0
{
    PS_OUT_WBOIT Out = (PS_OUT_WBOIT)0;
    
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
    float4 SubMaskSample = { 1.f, 1.f, 1.f, 1.f};
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
            CurvePowerStrength *= 1.6f;
        }
        else
        {
            CurveSample = CurveTextureSample(Get90DegreeRotatedUV(In.vUV, g_Effect.g_RotationFlags, CURVETEXTURE));
            CurvePowerStrength = CurveSample.r;
            CurvePowerStrength *= 1.6f;
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


PS_OUT_WBOIT PS_TextureBloomHard(GS_OUT_EFFECT_PARTICLE In)
{
    PS_OUT_WBOIT Out = (PS_OUT_WBOIT) 0;
    
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

PS_OUT_WBOIT PS_GLowTexture(GS_OUT_EFFECT_PARTICLE In)
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
            if (Has(g_Effect.g_TextureFlags, NOISETEXTURE))
            {
                float2 scrolledUV = In.vUV + g_Effect.g_UVOffset;
                scrolledUV += g_Effect.g_ScrollOffset * g_Effect.GlowTexture_ScrollWeight;
                scrolledUV += distortionUV;
                GlowSample = GlowTextureSample(Get90DegreeRotatedUV(scrolledUV, g_Effect.g_RotationFlags, GLOWTEXTURE));
            }
            else
            {
                float2 scrolledUV = In.vUV + g_Effect.g_UVOffset;
                scrolledUV += g_Effect.g_ScrollOffset * g_Effect.GlowTexture_ScrollWeight;
                GlowSample = GlowTextureSample(Get90DegreeRotatedUV(scrolledUV, g_Effect.g_RotationFlags, GLOWTEXTURE));
            }
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
            if (Has(g_Effect.g_TextureFlags, NOISETEXTURE))
            {
                float2 scrolledUV = In.vUV + g_Effect.g_UVOffset;
                scrolledUV += g_Effect.g_ScrollOffset * g_Effect.MaskingTexture_ScrollWeight;
                scrolledUV += distortionUV;
                GlowSample = MaskTextureSample(Get90DegreeRotatedUV(scrolledUV, g_Effect.g_RotationFlags, MASKINGTEXTURE));
            }
            else
            {
                float2 scrolledUV = In.vUV + g_Effect.g_UVOffset;
                scrolledUV += g_Effect.g_ScrollOffset * g_Effect.MaskingTexture_ScrollWeight;
                MaskSample = MaskTextureSample(Get90DegreeRotatedUV(scrolledUV, g_Effect.g_RotationFlags, MASKINGTEXTURE));
            }
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

float4 PS_DISTOTION(GS_OUT_EFFECT_PARTICLE In) : SV_Target0
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
    
    
    float4 DiffuseSample = float4(1.f, 1.f, 1.f, 1.f);
    
    if (Has(g_Effect.g_TextureFlags, DEFAULTTEXTURE))
    {
        if (HasTextureSprite(g_Effect.DiffuseTexture_SpriteInfo))
        {
            float2 SpriteUV = GetStaticSpriteUV(In.vUV, g_Effect.DiffuseTexture_SpriteInfo);
            
            DiffuseSample = DefaultTextureSample(Get90DegreeRotatedUV(SpriteUV, g_Effect.g_RotationFlags, DEFAULTTEXTURE));
        }
        else if (HasTextureScroll(SCROLL_DIFFUSE))
        {
            float2 scrolledUV = In.vUV + g_Effect.g_UVOffset;
            scrolledUV += g_Effect.g_ScrollOffset * g_Effect.DiffuseTexture_ScrollWeight;
            DiffuseSample = DefaultTextureSample(Get90DegreeRotatedUV(scrolledUV, g_Effect.g_RotationFlags, DEFAULTTEXTURE));
        }
        else
        {
            DiffuseSample = DefaultTextureSample(Get90DegreeRotatedUV(In.vUV, g_Effect.g_RotationFlags, DEFAULTTEXTURE));
        }
    }
    else
    {
        DiffuseSample = DefaultTextureSample(Get90DegreeRotatedUV(In.vUV, g_Effect.g_RotationFlags, DEFAULTTEXTURE));
    }
    
    float lifeAlpha = 1.0f - (In.vLifeTime.x / In.vLifeTime.y);
    float finalAlpha = DiffuseSample.a * lifeAlpha;

    if (finalAlpha < g_Effect.g_DiscardValue)
        discard;

    // 굴절된 배경색에 이펙트 고유 색상을 살짝 얹어줌
    return float4(refractionColor.rgb + (g_Effect.g_EffectColor.rgb * 0.2f), finalAlpha);
}

PS_OUT_WBOIT PS_CampFire(GS_OUT_EFFECT_PARTICLE In)
{
    PS_OUT_WBOIT Out = (PS_OUT_WBOIT) 0;
    
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
    
    float3 srcRGB = finalRGB * pow(lifeAlpha, 10); // 끝을 검게 주고 싶은데
    float srcAlpha = finalAlpha;
    
    float w = pow(saturate(1.0f - In.vViewZ / 1000.0f), 3.0f); // 3승으로 변화율 조절
    w = clamp(w, 0.01f, 3000.0f); // 상한선을 적당히 열어주되, 하한선으로 방어

    Out.vAccum = float4(srcRGB * srcAlpha, srcAlpha) * w;
    Out.vReveal = saturate(srcAlpha);

    return Out;
}

technique11 T0
{
    pass Texture_Effect
    {
        SetRasterizerState(RS_Default_CullNone);
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetVertexShader(CompileShader(vs_5_0, VS_Texture()));
        SetGeometryShader(CompileShader(gs_5_0, GS_Texture()));
        SetPixelShader(CompileShader(ps_5_0, PS_Texture()));
    }

    pass Texture_BlendEffect
    {
        SetRasterizerState(RS_Default_CullNone);
        SetDepthStencilState(DS_ReadOnly, 0);
        SetBlendState(BS_WBOIT_Accumulate, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetVertexShader(CompileShader(vs_5_0, VS_Texture()));
        SetGeometryShader(CompileShader(gs_5_0, GS_Texture()));
        SetPixelShader(CompileShader(ps_5_0, PS_Texture()));
    }

    pass Texture_BloomHard
    {
        SetRasterizerState(RS_Default_CullNone);
        SetDepthStencilState(DS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetVertexShader(CompileShader(vs_5_0, VS_Texture()));
        SetGeometryShader(CompileShader(gs_5_0, GS_Texture()));
        SetPixelShader(CompileShader(ps_5_0, PS_TextureBloomHard()));
    }

    pass Texture_BlendBloomHard
    {
        SetRasterizerState(RS_Default_CullNone);
        SetDepthStencilState(DS_ReadOnly, 0);
        SetBlendState(BS_WBOIT_Accumulate, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetVertexShader(CompileShader(vs_5_0, VS_Texture())); 
        SetGeometryShader(CompileShader(gs_5_0, GS_Texture()));
        SetPixelShader(CompileShader(ps_5_0, PS_TextureBloomHard()));
    }

    pass None_DepthDefault
    {
        SetRasterizerState(RS_Default_CullNone);
        SetDepthStencilState(DS_Disabled, 0);
        SetBlendState(BS_WBOIT_Accumulate, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetVertexShader(CompileShader(vs_5_0, VS_Texture()));
        SetGeometryShader(CompileShader(gs_5_0, GS_Texture()));
        SetPixelShader(CompileShader(ps_5_0, PS_Texture()));
    }

    pass GlowTextureSample
    {
        SetRasterizerState(RS_Default_CullNone);
        SetDepthStencilState(DS_ReadOnly, 0);
        SetBlendState(BS_WBOIT_Accumulate, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetVertexShader(CompileShader(vs_5_0, VS_Texture()));
        SetGeometryShader(CompileShader(gs_5_0, GS_Texture()));
        SetPixelShader(CompileShader(ps_5_0, PS_GLowTexture()));
    }

    pass NoneDepth_GlowTextureSample
    {
        SetRasterizerState(RS_Default_CullNone);
        SetDepthStencilState(DS_Disabled, 0);
        SetBlendState(BS_WBOIT_Accumulate, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetVertexShader(CompileShader(vs_5_0, VS_Texture()));
        SetGeometryShader(CompileShader(gs_5_0, GS_Texture()));
        SetPixelShader(CompileShader(ps_5_0, PS_GLowTexture()));
    }

    pass Blend_Line_Texture
    {
        SetRasterizerState(RS_Default_CullNone);
        SetDepthStencilState(DS_ReadOnly, 0);
        SetBlendState(BS_WBOIT_Accumulate, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetVertexShader(CompileShader(vs_5_0, VS_Texture()));
        SetGeometryShader(CompileShader(gs_5_0, Line_GS_Texture()));
        SetPixelShader(CompileShader(ps_5_0, PS_Texture()));
    }

    pass NoneDepth_Line_Texture
    {
        SetRasterizerState(RS_Default_CullNone);
        SetDepthStencilState(DS_Disabled, 0);
        SetBlendState(BS_WBOIT_Accumulate, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetVertexShader(CompileShader(vs_5_0, VS_Texture()));
        SetGeometryShader(CompileShader(gs_5_0, Line_GS_Texture()));
        SetPixelShader(CompileShader(ps_5_0, PS_GLowTexture()));
    }

    pass Distotion_Texture
    {
        SetRasterizerState(RS_Default_CullNone);
        SetDepthStencilState(DS_ReadOnly, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetVertexShader(CompileShader(vs_5_0, VS_Texture()));
        SetGeometryShader(CompileShader(gs_5_0, GS_Texture()));
        SetPixelShader(CompileShader(ps_5_0, PS_DISTOTION()));
    }

    pass CampFire_Texture
    {
        SetRasterizerState(RS_Default_CullNone);
        SetDepthStencilState(DS_ReadOnly, 0);
        SetBlendState(BS_WBOIT_Accumulate, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetVertexShader(CompileShader(vs_5_0, VS_Texture()));
        SetGeometryShader(CompileShader(gs_5_0, GS_Texture()));
        SetPixelShader(CompileShader(ps_5_0, PS_CampFire()));
    }
}