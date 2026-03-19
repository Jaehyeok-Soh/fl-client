#include "Effect_Defines.hlsl"
// =========== VS In  ==============

VS_OUT_POS_GS_PARTICLE VS_Particle(VS_IN_POS_GS_PARTICLE In)
{
    VS_OUT_POS_GS_PARTICLE Out;
    
    matrix matInst = INSTANCE_OUTPUT[In.vInstID].matTransform;
    
    vector vPosition = mul(vector(In.vPosition, 1.f), matInst);
    vPosition = mul(vPosition, W);
    float4 vViewPos = mul(vPosition, V);
    
    float2 pSize = float2(length(matInst._11_12_13), length(matInst._21_22_23));

    // 사이즈 계산
    Out.vPosition = vPosition;
    Out.vPSize = float3(length(W._11_12_13) * pSize.x, length(W._21_22_23) * pSize.y, length(W._31_32_33));
    Out.vLifeTime = INSTANCE_OUTPUT[In.vInstID].vLifeTime;
    Out.vInstID = In.vInstID;
    Out.matTransform = W;
    Out.vViewZ = vViewPos.z;
    
    return Out;
}

[maxvertexcount(4)]
void GS_Particle(point VS_OUT_POS_GS_PARTICLE In[1], inout TriangleStream<GS_OUT_POS_PARTICLE> OutStream)
{
    GS_OUT_POS_PARTICLE Out[4];
    matrix matVP = mul(V, P);

    float3 vRight, vUp;

    if (HasBillboard())
    {
        // 일반 빌보드: 카메라를 바라보는 축 계산
        float3 vLook = normalize(CameraPosition() - In[0].vPosition.xyz);
        vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook));
        vUp = normalize(cross(vLook, vRight));
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
        Out[i].vPosition = mul(float4(vPos[i], 1.f), matVP);
        Out[i].vUV = vUV[i];
        Out[i].vLifeTime = In[0].vLifeTime;
        Out[i].vViewZ = In[0].vViewZ;
        OutStream.Append(Out[i]);
    }
    OutStream.RestartStrip();
}

PS_OUT_WBOIT PS_Particle(GS_OUT_POS_PARTICLE In) : SV_TARGET0
{
    PS_OUT_WBOIT Out;
    
    // 클라에서 넘겨준 색상 그대로
    if (In.vLifeTime.x < 0.0f)
        discard;
    
        // ================    Curve 텍스처     ===============
    float4 CurveSample = { 1.f, 1.f, 1.f, 1.f };
    float4 GlowSample = { 1.f, 1.f, 1.f, 1.f };
    float CurvePowerStrength = 1.0f;
    
    // 1. 진행 비율 계산 (AppearRatio: 등장, DissolveProgress: 소멸)
    float LifeRatio = saturate(In.vLifeTime.x / In.vLifeTime.y);
    float AppearRatio = In.vLifeTime.x / (In.vLifeTime.y * g_Effect.g_AppearRatio);
    float DissolveProgress = saturate((LifeRatio - g_Effect.g_AppearRatio) / max(0.001f, 1.0f - g_Effect.g_AppearRatio));
   
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
            CurvePowerStrength *= 3.f;
        }
        else
        {
            CurveSample = CurveTextureSample(Get90DegreeRotatedUV(In.vUV, g_Effect.g_RotationFlags, CURVETEXTURE));
            CurvePowerStrength = CurveSample.r;
            CurvePowerStrength *= 3.f;
        }

    }
    else
    {
        CurvePowerStrength = 1.f;
    }
    
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
    
    
    float lifeAlpha = 1.0f - DissolveProgress;
    float finalAlpha = g_Effect.g_EffectColor.a;

    if (HasLifeDissolve())
        finalAlpha *= lifeAlpha;

    float3 finalRGB = g_Effect.g_EffectColor * CurvePowerStrength * GlowSample.r;
    // ===========  라이프타임에 따른 투명도 적용  =============

    if (finalAlpha < g_Effect.g_DiscardValue)
        discard;
    
    float3 srcRGB = finalRGB;
    float srcAlpha = finalAlpha;

    float w = pow(saturate(1.0f - In.vViewZ / 1000.0f), 3.0f); // 3승으로 변화율 조절
    w = clamp(w, 0.01f, 3000.0f); // 상한선을 적당히 열어주되, 하한선으로 방어

    Out.vAccum = float4(srcRGB * srcAlpha, srcAlpha) * w;
    Out.vReveal = saturate(srcAlpha);

    return Out;
}


technique11 T0
{
    pass ParticleEffect
    {
        SetRasterizerState(RS_Default_CullNone);
        SetDepthStencilState(DS_ReadOnly, 0);
        SetBlendState(BS_WBOIT_Accumulate, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetVertexShader(CompileShader(vs_5_0, VS_Particle()));
        SetGeometryShader(CompileShader(gs_5_0, GS_Particle()));
        SetPixelShader(CompileShader(ps_5_0, PS_Particle()));
    }

    pass None_DepthDefault
    {
        SetRasterizerState(RS_Default_CullNone);
        SetDepthStencilState(DS_Disabled, 0);
        SetBlendState(BS_WBOIT_Accumulate, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetVertexShader(CompileShader(vs_5_0, VS_Particle()));
        SetGeometryShader(CompileShader(gs_5_0, GS_Particle()));
        SetPixelShader(CompileShader(ps_5_0, PS_Particle()));
    }
}