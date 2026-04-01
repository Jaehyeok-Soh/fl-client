#include "Struct_Defines.hlsl"
#include "Light_Defines.hlsl"
#include "Animation_Defines.hlsl"
#include "DissolveEffect_Defines.hlsl"

#define RENDERFX_EMISSIVE (1<<0)
#define RENDERFX_SHAKE (1<<1)


#define Citizen_Face_Eye    0
#define Citizen_Face_Mouth  1
#define Citizen_Face_END    2


bool Has_RenderFx(uint iFlags, uint iMask)
{
    return (iFlags & iMask) != 0;
}

float3 Apply_Shake(float3 vWorldPos)
{
    float3 cameraRight = normalize(InvV[0].xyz);
    float3 cameraUp = normalize(InvV[1].xyz);
    
    if (Has_RenderFx(renderFx.iFalgs, RENDERFX_SHAKE) == false)
        return vWorldPos;
    
    return vWorldPos
        + cameraRight * renderFx.fShakeAmpX
        + cameraUp * renderFx.fShakeAmpY;
}

void Apply_Dissolve_Discard_And_Alpha(
    in PS_IN_SKELETON input,
    inout float4 ioDiffuse)
{
    uint flags = g_DissolveEffect.g_iDissolveFlag;
    float amount = saturate(g_DissolveEffect.g_fDissolveAmount);

    if (!Is_Dissolve_Active())
        return;

    if (Should_Discard_By_Dissolve(input.vUV, flags, amount))
        discard;

    if (Has_DissolveFlag(flags, DF_USE_ALPHAFADE))
    {
        float alpha = Get_DissolveAlphaFactor(flags, amount);
        ioDiffuse.a *= alpha;

        if (ioDiffuse.a <= EPSILON)
            discard;
    }
}

float3 Get_DissolveEdgeEmissive(float2 uv)
{
    uint flags = g_DissolveEffect.g_iDissolveFlag;
    float amount = saturate(g_DissolveEffect.g_fDissolveAmount);

    if (!Is_Dissolve_Active())
        return float3(0.f, 0.f, 0.f);

    if (!Has_DissolveFlag(flags, DF_USEEDGE))
        return float3(0.f, 0.f, 0.f);

    float edge = Compute_DissolveEdge(
        uv,
        flags,
        amount,
        g_DissolveEffect.g_fDissolveEdgeWidth);

    return g_DissolveEffect.g_vDissolveEdgeColor * edge * 3.0f;
}

cbuffer CB_MAPPING_RGB
{
    float4 Color_R = { 1.f, 1.f, 1.f, 1.f };
    float4 Color_G = { 1.f, 1.f, 1.f, 1.f };
    float4 Color_B = { 1.f, 1.f, 1.f, 1.f };
};


struct CittzenFaceUV
{
    float2 vUVOffset; // 선택된 셀의 시작 UV (예: 0.25, 0.5)
    float2 vUVScale; // 한 셀의 가로세로 크기 (예: 1.0/MaxCol, 1.0/MaxRow)
};

cbuffer CB_CitizentFaceData
{
    CittzenFaceUV tCitizenFaceUV[Citizen_Face_END];
};

cbuffer CB_MonsterEmotion
{   
    float4 vMonsterSkinColor = float4(1.f, 1.f, 1.f, 1.f);
    float4 vMonsterEmotionColor = float4(1.f,1.f,1.f,1.f);
    CittzenFaceUV tMonsterEmotionUV;
};


VS_OUT_SKELETON VS_MAIN(VS_IN_SKELECTON input)
{
    VS_OUT_SKELETON output;
    float4x4 matBone = Get_BoneMatrix(input);
    
    float4 vWorldPosition = mul(float4(input.vPosition, 1.f), matBone);
    vWorldPosition = mul(vWorldPosition, W);
    float4 vViewPosition = mul(vWorldPosition, V);
    float4 vProjPosition = mul(vViewPosition, P);
    
    input.vNormal = normalize(mul(input.vNormal, (float3x3) matBone));
    input.vTangent = normalize(mul(input.vTangent, (float3x3) matBone));
    input.vBinormal = normalize(mul(input.vBinormal, (float3x3) matBone));
    
    output.vPosition = vProjPosition;
    output.vUV = input.vUV;
    output.vNormal = normalize(mul(input.vNormal, (float3x3) W));
    output.vTangent = normalize(mul(input.vTangent, (float3x3) W));
    output.vBinormal = normalize(mul(input.vBinormal, (float3x3) W));
    
    output.vWorldPos = vWorldPosition;
    output.vProjPos = vProjPosition;
    output.fViewZ = vViewPosition.z;
    return output;
}

VS_OUT_SKELETON VS_WITHSHAKE(VS_IN_SKELECTON input)
{
    VS_OUT_SKELETON output;
    float4x4 matBone = Get_BoneMatrix(input);
    
    float4 vWorldPosition = mul(float4(input.vPosition, 1.f), matBone);
    vWorldPosition = mul(vWorldPosition, W);    
    vWorldPosition.xyz = Apply_Shake(vWorldPosition.xyz);    
    float4 vViewPosition = mul(vWorldPosition, V);
    float4 vProjPosition = mul(vViewPosition, P);
    
    input.vNormal = normalize(mul(input.vNormal, (float3x3) matBone));
    input.vTangent = normalize(mul(input.vTangent, (float3x3) matBone));
    input.vBinormal = normalize(mul(input.vBinormal, (float3x3) matBone));
    
    output.vPosition = vProjPosition;
    output.vUV = input.vUV;
    output.vNormal = normalize(mul(input.vNormal, (float3x3) W));
    output.vTangent = normalize(mul(input.vTangent, (float3x3) W));
    output.vBinormal = normalize(mul(input.vBinormal, (float3x3) W));
    
    output.vWorldPos = vWorldPosition;
    output.vProjPos = vProjPosition;
    output.fViewZ = vViewPosition.z;
    return output;
}

VS_OUT_SHADOW VS_SHADOW(VS_IN_SKELECTON input)
{
    VS_OUT_SHADOW output;
    int iCascadeIndex = (int)cascadeParam.fCascadeIndex;    
    float4x4 matBone = Get_BoneMatrix(input);
    float4 vWorldPosition = mul(float4(input.vPosition, 1.f), matBone);
    vWorldPosition = mul(vWorldPosition, W);    
    output.vPosition = mul(vWorldPosition, cascadeParam.matLightVP[iCascadeIndex]);
    return output;
}

PS_OUT_DEFFERED PS_MAIN(PS_IN_SKELETON input)
{
    PS_OUT_DEFFERED output;
    
    output.vDiffuse = 1.f;
    Compute_Diffse(output.vDiffuse, input.vUV);
    if(output.vDiffuse.a <= 0.5f)
        discard;
    
    // 디졸브 디스카드
    Apply_Dissolve_Discard_And_Alpha(input, output.vDiffuse);
    
    float3 vNormal = input.vNormal;
    Compute_Normal(vNormal, input.vTangent, input.vBinormal, input.vUV);
    output.vNormal = float4(vNormal * 0.5f + 0.5f, 1.f);
    
    float3 vSpecMask = float3(1.f, 1.f, 0.f);
    if (Has(g_iMaterialMask, METALNESS))
        vSpecMask = g_MaterialTextures[METALNESS].Sample(LinearSampler, input.vUV).xyz;
    output.vSpecularMask = float4(vSpecMask, 1.f); 
    output.vObjectInfo.r = PackObjectInfo(objectInfo.iObjectID, objectInfo.iFlags);
    output.vDepth = float4(input.vProjPos.z / input.vProjPos.w, input.vProjPos.w, 0.f, 0.f);
    float3 vEmissive = float3(0.f, 0.f, 0.f);
    
    
    // 텍스쳐 Emissive
    if (Has(g_iMaterialMask, EMISSIVE))
    {
        vEmissive = g_MaterialTextures[EMISSIVE].Sample(LinearSampler, input.vUV).xyz;
        float fMask = max(vEmissive.r, max(vEmissive.g, vEmissive.b));
        vEmissive = output.vDiffuse.rgb * fMask * 4.5f;
    }
    
    // 피격 Emissive
    if (Has(renderFx.iFalgs, RENDERFX_EMISSIVE))
    {
        float3 vV = normalize(CameraPosition() - input.vWorldPos.xyz);
        float fNdotV = saturate(dot(vNormal, vV));

        // 외곽
        float fRim = 1.f - fNdotV;
        fRim = smoothstep(0.35f, 0.80f, fRim);
        fRim = pow(fRim, 1.2f);

        float3 vFlash = renderFx.vEmissiveColor.rgb * fRim * renderFx.fEmissiveIntensity;

        vEmissive += vFlash;
    }
    
    // dissolve edge emissive
    vEmissive += Get_DissolveEdgeEmissive(input.vUV);
    
    output.vEmissive = float4(vEmissive, 1.f);
    return output;
}

PS_OUT PS_RED(PS_IN_SKELETON input)
{
    PS_OUT output;
    output.vColor = float4(1.f, 0.f, 0.f, 1.f);
    return output;
}

PS_OUT_DEFFERED PS_RGBMAPPING(PS_IN_SKELETON input)
{
    PS_OUT_DEFFERED output;
    
    float4 vDiffuse = 1.f;
    Compute_Diffse(vDiffuse, input.vUV);

    float3 final =
        (vDiffuse.r * Color_R.rgb) +
        (vDiffuse.g * Color_G.rgb) +
        (vDiffuse.b * Color_B.rgb);
     //saturate(vDiffuse.r * Color_R) +
    // saturate(vDiffuse.g * Color_G) +
     //saturate(vDiffuse.b * Color_B);
    
    float luminance = dot(final, float3(0.3, 0.59, 0.11));

    float3 finalRGB = final * luminance;
    
    float4 finalDiffuse = float4(saturate(finalRGB), vDiffuse.a);
    
    output.vDiffuse = finalDiffuse;
    
    // 디졸브 디스카드
    Apply_Dissolve_Discard_And_Alpha(input, output.vDiffuse);
    
    float3 vNormal = input.vNormal;
    Compute_Normal(vNormal, input.vTangent, input.vBinormal, input.vUV);
    output.vNormal = float4(vNormal * 0.5f + 0.5f, 1.f);
    
    float3 vSpecMask = float3(1.f, 1.f, 0.f);
    if (Has(g_iMaterialMask, METALNESS))
        vSpecMask = g_MaterialTextures[METALNESS].Sample(LinearSampler, input.vUV).xyz;
    output.vSpecularMask = float4(vSpecMask, 1.f);
    output.vObjectInfo.r = PackObjectInfo(objectInfo.iObjectID, objectInfo.iFlags);
    output.vDepth = float4(input.vProjPos.z / input.vProjPos.w, input.vProjPos.w, 0.f, 0.f);
    float3 vEmissive = float3(0.f, 0.f, 0.f);
    if (Has(g_iMaterialMask, EMISSIVE))
    {
        vEmissive = g_MaterialTextures[EMISSIVE].Sample(LinearSampler, input.vUV).xyz;
        float fMask = max(vEmissive.r, max(vEmissive.g, vEmissive.b));
        vEmissive = output.vDiffuse.rgb * fMask * 4.5f;
    }
    
    
    // dissolve edge emissive
    vEmissive += Get_DissolveEdgeEmissive(input.vUV);
    
    output.vEmissive = float4(vEmissive, 1.f);
    return output;
}

PS_OUT_SHADOW PS_SHADOW(VS_OUT_SHADOW input)
{
    PS_OUT_SHADOW output;
    output.vDepth = float4(input.vPosition.z, 0.f, 0.f, 1.f);
    return output;
}


PS_OUT_DEFFERED PS_CITIZENMOUTH(PS_IN_SKELETON input)
{
    PS_OUT_DEFFERED output;
    
    output.vDiffuse = 1.f;
    
    float2 vUV = input.vUV * tCitizenFaceUV[Citizen_Face_Mouth].vUVScale + tCitizenFaceUV[Citizen_Face_Mouth].vUVOffset;
    
    
    Compute_Diffse(output.vDiffuse, vUV);
    if (output.vDiffuse.a <= 0.5f)
        discard;
    
    Apply_Dissolve_Discard_And_Alpha(input, output.vDiffuse);
    
    float3 vNormal = input.vNormal;
    Compute_Normal(vNormal, input.vTangent, input.vBinormal, vUV);
    output.vNormal = float4(vNormal * 0.5f + 0.5f, 1.f);
    
    float3 vSpecMask = float3(1.f, 1.f, 0.f);
    if (Has(g_iMaterialMask, METALNESS))
        vSpecMask = g_MaterialTextures[METALNESS].Sample(LinearSampler, vUV).xyz;
    output.vSpecularMask = float4(vSpecMask, 1.f);
    output.vObjectInfo.r = PackObjectInfo(objectInfo.iObjectID, objectInfo.iFlags);
    output.vDepth = float4(input.vProjPos.z / input.vProjPos.w, input.vProjPos.w, 0.f, 0.f);
    float3 vEmissive = float3(0.f, 0.f, 0.f);
    // 텍스쳐 Emissive
    if (Has(g_iMaterialMask, EMISSIVE))
    {
        vEmissive = g_MaterialTextures[EMISSIVE].Sample(LinearSampler,vUV ).xyz;
        float fMask = max(vEmissive.r, max(vEmissive.g, vEmissive.b));
        vEmissive = output.vDiffuse.rgb * fMask * 4.5f;
    }
    

    vEmissive += Get_DissolveEdgeEmissive(input.vUV);
    

    
    return output;
}

PS_OUT_DEFFERED PS_CITIZENEYE(PS_IN_SKELETON input)
{
    PS_OUT_DEFFERED output;
    
    output.vDiffuse = 1.f;
    
    float2 vUV = input.vUV * tCitizenFaceUV[Citizen_Face_Eye].vUVScale + tCitizenFaceUV[Citizen_Face_Eye].vUVOffset;
    
    
    Compute_Diffse(output.vDiffuse, vUV);
    if (output.vDiffuse.a <= 0.5f)
        discard;
    
    Apply_Dissolve_Discard_And_Alpha(input, output.vDiffuse);
    
    
    float3 vNormal = input.vNormal;
    Compute_Normal(vNormal, input.vTangent, input.vBinormal, vUV);
    output.vNormal = float4(vNormal * 0.5f + 0.5f, 1.f);
    
    float3 vSpecMask = float3(1.f, 1.f, 0.f);
    if (Has(g_iMaterialMask, METALNESS))
        vSpecMask = g_MaterialTextures[METALNESS].Sample(LinearSampler, vUV).xyz;
    output.vSpecularMask = float4(vSpecMask, 1.f);
    output.vObjectInfo.r = PackObjectInfo(objectInfo.iObjectID, objectInfo.iFlags);
    output.vDepth = float4(input.vProjPos.z / input.vProjPos.w, input.vProjPos.w, 0.f, 0.f);
    float3 vEmissive = float3(0.f, 0.f, 0.f);
    // 텍스쳐 Emissive
    if (Has(g_iMaterialMask, EMISSIVE))
    {
        vEmissive = g_MaterialTextures[EMISSIVE].Sample(LinearSampler, vUV).xyz;
        float fMask = max(vEmissive.r, max(vEmissive.g, vEmissive.b));
        vEmissive = output.vDiffuse.rgb * fMask * 4.5f;
    }
    
    
    vEmissive += Get_DissolveEdgeEmissive(input.vUV);
    
    return output;
}


PS_OUT_DEFFERED PS_MONSTERFACE(PS_IN_SKELETON input)
{
    PS_OUT_DEFFERED output;
    
    output.vDiffuse = 1.f;
    
    // UV 좌표 저장
    float2 vOriginUV = input.vUV;
    
    float2 vNoiseTileUV = vOriginUV * vMonsterSkinColor.a;

    
    float fWidthNoise = g_DissolveTexture[5].Sample(LinearSampler, vNoiseTileUV).r;
    float fHeightNoise = g_DissolveTexture[6].Sample(LinearSampler, vNoiseTileUV).r;
    
    float fMixNoise = saturate(fWidthNoise * fHeightNoise);

    
    if (fMixNoise < 0.1f)
        discard;
    
    output.vDiffuse = float4(vMonsterSkinColor.rgb, 1.f);

    Apply_Dissolve_Discard_And_Alpha(input, output.vDiffuse);
    
    
    
    
    float3 vNormal = input.vNormal;
    Compute_Normal(vNormal, input.vTangent, input.vBinormal, input.vUV);
    output.vNormal = float4(vNormal * 0.5f + 0.5f, 1.f);
    
    float3 vSpecMask = float3(1.f, 1.f, 0.f);
    if (Has(g_iMaterialMask, METALNESS))
        vSpecMask = g_MaterialTextures[METALNESS].Sample(LinearSampler,input.vUV).xyz;
    output.vSpecularMask = float4(vSpecMask, 1.f);
    output.vObjectInfo.r = PackObjectInfo(objectInfo.iObjectID, objectInfo.iFlags);
    output.vDepth = float4(input.vProjPos.z / input.vProjPos.w, input.vProjPos.w, 0.f, 0.f);
    float3 vEmissive = float3(0.f, 0.f, 0.f);
    // 텍스쳐 Emissive
    if (Has(g_iMaterialMask, EMISSIVE))
    {
        vEmissive = g_MaterialTextures[EMISSIVE].Sample(LinearSampler, input.vUV).xyz;
        float fMask = max(vEmissive.r, max(vEmissive.g, vEmissive.b));
        vEmissive = output.vDiffuse.rgb * fMask * 4.5f;
    }
    
    vEmissive += Get_DissolveEdgeEmissive(input.vUV);    
        
    return output;
}

PS_OUT_WBOIT PS_GHOST(PS_IN_SKELETON input)
{
    PS_OUT_WBOIT output;

    float3 vViewDir = normalize(CameraPosition().xyz - input.vWorldPos.xyz);
    float3 vNormal = normalize(input.vNormal);
    float fFresnel = pow(1.f - saturate(dot(vViewDir, vNormal)), 2.f);

    float3 srcRGB = ghostTrailParam.vColor.rgb + ghostTrailParam.vColor.rgb * fFresnel * 0.5f;
    float srcAlpha = ghostTrailParam.vColor.a * (0.5f + fFresnel * 0.2f);
    float w = pow(saturate(1.0f - input.fViewZ / 1000.0f), 3.0f); // 3승으로 변화율 조절
    w = clamp(w, 0.01f, 3000.0f); // 상한선을 적당히 열어주되, 하한선으로 방어

    output.vAccum = float4(srcRGB * srcAlpha, srcAlpha) * w;
    output.vReveal = srcAlpha;
    return output;
}


technique11 T0
{
    PASS_RS_DS_BS_VP(Default, RS_Default, DS_Default, BS_Default, VS_MAIN, PS_MAIN)
    PASS_RS_DS_BS_VP(Red, RS_Wire, DS_Default, BS_Default, VS_MAIN, PS_RED)

    // RGB mapping : weapon 쪽에서 쓰임
	PASS_RS_DS_BS_VP(RGBMapping, RS_Default, DS_Default, BS_Default, VS_MAIN, PS_RGBMAPPING)

    // Shake
    PASS_RS_DS_BS_VP(WithRenderFx, RS_Default, DS_Default, BS_Default, VS_WITHSHAKE, PS_MAIN)
    
    // Index - 4
    // Shadow - 이거추가되면 Render_Shadow에서 Set_Pass Index 바꿔줘야함
    PASS_RS_DS_BS_VP(Shadow, RS_Default, DS_Default, BS_Default, VS_SHADOW, PS_SHADOW)

	PASS_RS_DS_BS_VP(CitizenEye, RS_Default, DS_Default, BS_Default, VS_MAIN , PS_CITIZENEYE)
	PASS_RS_DS_BS_VP(CitizenMouth, RS_Default, DS_Default, BS_Default, VS_MAIN , PS_CITIZENMOUTH)
	PASS_RS_DS_BS_VP(CitizenCloth, RS_Default, DS_Default, BS_Default, VS_MAIN, PS_RGBMAPPING)
	PASS_RS_DS_BS_VP(CitizenBody, RS_Default, DS_Default, BS_Default, VS_MAIN, PS_MAIN)


    //흔들흔들열매 
	PASS_RS_DS_BS_VP(MonsterFace, RS_Default, DS_Default, BS_Default, VS_WITHSHAKE, PS_MONSTERFACE)

    //잔상 - index 10
    pass BLOOM_SWORDEFFECT
    {
        SetRasterizerState(RS_Default_CullNone);
        SetDepthStencilState(DS_ReadOnly, 0);
        SetBlendState(BS_WBOIT_Accumulate, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetVertexShader(CompileShader(vs_5_0, VS_MAIN()));
        GeometryShader = NULL;
        SetPixelShader(CompileShader(ps_5_0, PS_GHOST()));
    }
};