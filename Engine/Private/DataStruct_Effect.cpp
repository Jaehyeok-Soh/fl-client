#include "Engine_pch.h"
#include "DataStruct_Effect.h"
#include "Engine_Utils.h"
#pragma push_macro("new")
#undef new
#include "json.hpp"
using json = nlohmann::json;
#pragma pop_macro("new")

NS_BEGIN(DTO)

// ==========  Container  ==========
void to_json(json& j, const TEFFECT_ContainerData& data)
{
	j = json
	{
		{ "Type", TEFFECT_ContainerData::eType },
		{ "strTag", data.strTag },
		{ "ContainerName", data.EffectContainerName },
		{ "SimulationType", data._Effect_SimulationType },
		{ "WorldMatrix", {
			data.vWorldMatrix._11, data.vWorldMatrix._12, data.vWorldMatrix._13, data.vWorldMatrix._14,
			data.vWorldMatrix._21, data.vWorldMatrix._22, data.vWorldMatrix._23, data.vWorldMatrix._24,
			data.vWorldMatrix._31, data.vWorldMatrix._32, data.vWorldMatrix._33, data.vWorldMatrix._34,
			data.vWorldMatrix._41, data.vWorldMatrix._42, data.vWorldMatrix._43, data.vWorldMatrix._44 
		}},
		{ "_ChildData", data._ChildData }
	};
}

void from_json(const json& j, TEFFECT_ContainerData& data)
{
	j.at("strTag").get_to(data.strTag);
	j.at("ContainerName").get_to(data.EffectContainerName);
	j.at("SimulationType").get_to(data._Effect_SimulationType);

	if (j.contains("WorldMatrix")) {
		const auto& m = j.at("WorldMatrix");
		data.vWorldMatrix._11 = m[0];  data.vWorldMatrix._12 = m[1];  data.vWorldMatrix._13 = m[2];  data.vWorldMatrix._14 = m[3];
		data.vWorldMatrix._21 = m[4];  data.vWorldMatrix._22 = m[5];  data.vWorldMatrix._23 = m[6];  data.vWorldMatrix._24 = m[7];
		data.vWorldMatrix._31 = m[8];  data.vWorldMatrix._32 = m[9];  data.vWorldMatrix._33 = m[10]; data.vWorldMatrix._34 = m[11];
		data.vWorldMatrix._41 = m[12]; data.vWorldMatrix._42 = m[13]; data.vWorldMatrix._43 = m[14]; data.vWorldMatrix._44 = m[15];
	}

	if (j.contains("_ChildData"))
		j.at("_ChildData").get_to(data._ChildData);
}

// ==========   Parts   ==========
void to_json(json& j, const TEFFECT_PartsData& data)
{
    j = json
    {
        { "Type", TEFFECT_PartsData::eType },
        { "strTag", data.strTag },
        { "PartsName", data.EffectPartsName },
        { "ParentsName", data.ParentsName },
        { "WorldMatrix", {
            data.vWorldMatrix._11, data.vWorldMatrix._12, data.vWorldMatrix._13, data.vWorldMatrix._14,
            data.vWorldMatrix._21, data.vWorldMatrix._22, data.vWorldMatrix._23, data.vWorldMatrix._24,
            data.vWorldMatrix._31, data.vWorldMatrix._32, data.vWorldMatrix._33, data.vWorldMatrix._34,
            data.vWorldMatrix._41, data.vWorldMatrix._42, data.vWorldMatrix._43, data.vWorldMatrix._44
        }},

        // --- Effect Types & Shapes ---
        {"EffectSystem", data.eEffectSystemType},
        {"ParticleSystem", data.eEffectParticleType},
        {"EffectType", data.eEffectType},
        {"ShapeType", data._Effect_ShapeType},
        {"EmissionType", data._Effect_EmissionType},

        // --- Resource Tags ---
        {"Model_Tag", Engine_Utils::ToString(data._Effect_Model_Tag)},
        {"DiffuseTexture_Tag", Engine_Utils::ToString(data._Effect_DiffuseTexture_Tag)},
        {"NoiseTexture_Tag", Engine_Utils::ToString(data._Effect_NoiseTexture_Tag)},
        {"MaskingTexture_Tag", Engine_Utils::ToString(data._Effect_MaskingTexture_Tag)},
        {"GradationTexture_Tag", Engine_Utils::ToString(data._Effect_GradationTexture_Tag)},
        {"TrailTexture_Tag", Engine_Utils::ToString(data._Effect_TrailTexture_Tag)},
        {"NormalTexture_Tag", Engine_Utils::ToString(data._Effect_NormalTexture_Tag)},
        {"Shader_Tag", Engine_Utils::ToString(data._Effect_Shader_Tag)},
        {"Shader_Pass", data._Effect_ShaderPass},

        // --- Math & Transformation ---
        {"ScrollSpeed", {{"x", data._Effect_ScrollSpeed.x}, {"y", data._Effect_ScrollSpeed.y}}},
        {"DistortionScale", {{"x", data._Effect_DistortionScale.x}, {"y", data._Effect_DistortionScale.y}}},
        {"StartScale", {{"x", data._Effect_StartScale.x}, {"y", data._Effect_StartScale.y}, {"z", data._Effect_StartScale.z}}},
        {"EndScale", {{"x", data._Effect_EndScale.x}, {"y", data._Effect_EndScale.y}, {"z", data._Effect_EndScale.z}}},
        {"Color", {{"x", data._Effect_Color.x}, {"y", data._Effect_Color.y}, {"z", data._Effect_Color.z}, {"w", data._Effect_Color.w}}},
        {"DiscardValue", data._Effect_DiscardValue},
        {"Range", {{"x", data._Effect_Range.x}, {"y", data._Effect_Range.y}, {"z", data._Effect_Range.z}}},
        {"ParticleSize", {{"x", data._Effect_ParticleSize.x}, {"y", data._Effect_ParticleSize.y}}},
        {"SpiralData", {{"Radius", data._Effect_Spiral_Radius}, {"Speed", data._Effect_Spiral_Speed}}},

        // --- Time & Animation ---
        {"TimeFlag", data._Effect_TimeFlag},
        {"StartDelay", data._Effect_StartDelay},
        {"LifeTime", data._Effect_LifeTime},
        {"Duration", data._Effect_Duration},
        {"Looping", data._Effect_Looping},
        {"MaxParticle", data._Effect_MaxParticle},
        {"IsRandomSeed", data._Effect_IsRandomSeed},
        {"PlayBackSpeed", data._Effect_PlayBackSpeed},
        {"StartSpeed", data._Effect_StartSpeed},
        {"UseSprite", data._Effect_bUseSprite},
        {"TileCount", {{"x", data._Effect_TileCount.x}, {"y", data._Effect_TileCount.y}}},
        {"PlayAnimation", data._Effect_bPlayAnim},
        {"AnimationSpeed", data._Effect_AnimSpeed},
        {"SpriteNumber", data.m_iCurSpriteNumber},

        // -- 스크롤 속도 배율 (Weight) 저장 ---
        {"DiffuseTexture_ScrollWeight", {{"x", data._Effect_DiffuseTexture_ScrollWeight.x}, {"y", data._Effect_DiffuseTexture_ScrollWeight.y}}},
        {"NoiseTexture_ScrollWeight", {{"x", data._Effect_NoiseTexture_ScrollWeight.x}, {"y", data._Effect_NoiseTexture_ScrollWeight.y}}},
        {"MaskingTexture_ScrollWeight", {{"x", data._Effect_MaskingTexture_ScrollWeight.x}, {"y", data._Effect_MaskingTexture_ScrollWeight.y}}},
        {"GradationTexture_ScrollWeight", {{"x", data._Effect_GradationTexture_ScrollWeight.x}, {"y", data._Effect_GradationTexture_ScrollWeight.y}}},

        // --- 툴 UI 전용 스크롤 체크박스 상태 저장 ---
        {"Tool_UseScroll_Diffuse", data._Effect_Tool_UseScroll_Diffuse},
        {"Tool_UseScroll_Noise", data._Effect_Tool_UseScroll_Noise},
        {"Tool_UseScroll_Masking", data._Effect_Tool_UseScroll_Masking},
        {"Tool_UseScroll_Gradation", data._Effect_Tool_UseScroll_Gradation},

        // --- Physics & Gravity (커브 포함) ---
        {"GravityBase", {{"Val", data._Effect_Gravity_Value}, {"Mod", data._Effect_GravityModifier}}},
        {"GravityDir", {{"x", data._Effect_GravityDir.x}, {"y", data._Effect_GravityDir.y}, {"z", data._Effect_GravityDir.z}}},
        {"UseGlobalGravityCurve", data._bUseGlobalGravityCurve},
        {"UseExternalForceCurve", data._bUseExternalForceCurve},
        {"ExternalForceStrength", data.fExternalForceStrength},

        // --- Rotation Settings (커브 포함) ---
        {"StartRotation", {{"x", data._Effect_StartRotation.x}, {"y", data._Effect_StartRotation.y}, {"z", data._Effect_StartRotation.z}}},
        {"TargetRotation", {{"x", data._Effect_TargetRotation.x}, {"y", data._Effect_TargetRotation.y}, {"z", data._Effect_TargetRotation.z}}},
        {"UseStartRotation", data._bUseStartRotation},
        {"UseRotationCurve", data._bUseRotationCurve},
        {"SeparateAxes", data._bSeparateAxes},

        // --- Flags ---
        {"TextureFlag", data._Effect_TextureFlag},
        {"RenderFlag", data._Effect_RenderFlag},
        {"SamplerStateFlag", data._Effect_SamplerStateFlag},
        {"TexRotationFlag", data._Effect_TextureRotationFlag},
        {"TexOperatorFlag", data._Effect_TextureOperatorFlag},

        // --- Tool Settings ---
        {"Tool_DiffuseTexture", data._Effect_Tool_DiffuseTexture},
        {"Tool_NoiseTexture", data._Effect_Tool_NoiseTexture},
        {"Tool_MaskingTexture", data._Effect_Tool_MaskingTexture},
        {"Tool_GradationTexture", data._Effect_Tool_GradationTexture},
        {"Tool_UseBillboard", data._Effect_Tool_UseBillboard},
        {"Tool_UseScroll", data._Effect_Tool_UseScroll},
        {"Tool_RightScroll", data._Effect_Tool_RightScroll},
        {"Tool_DownScroll", data._Effect_Tool_DownScroll},
        {"Tool_DiffuseSampler_Flag", data._Effect_Tool_DiffuseSamplerState_Flag},
        {"Tool_NoiseSampler_Flag", data._Effect_Tool_NoiseSamplerState_Flag},
        {"Tool_MaskingSampler_Flag", data._Effect_Tool_MaskingSamplerState_Flag},
        {"Tool_GradationSampler_Flag", data._Effect_Tool_GradationSamplerState_Flag}
    };

    // 커브 배열 직렬화
    auto CurveToJson = [](const auto& vec) {
        json arr = json::array();
        for (const auto& key : vec) arr.push_back({ {"Time", key.fTimeKey}, {"Val", key.fValue} });
        return arr;
        };
    j["GlobalGravityCurve"] = CurveToJson(data._vecGlobalGravityCurve);
    j["ExternalForceCurve"] = CurveToJson(data._vecExternalForceCurve);
    j["RotationCurveX"] = CurveToJson(data._vecRotationCurveX);
    j["RotationCurveY"] = CurveToJson(data._vecRotationCurveY);
    j["RotationCurveZ"] = CurveToJson(data._vecRotationCurveZ);
}

// ==========  Parts (from_json)  ==========
void from_json(const json& j, TEFFECT_PartsData& data)
{
    j.at("strTag").get_to(data.strTag);
    j.at("PartsName").get_to(data.EffectPartsName);
    j.at("ParentsName").get_to(data.ParentsName);

    if (j.contains("WorldMatrix")) {
        const auto& m = j.at("WorldMatrix");
        data.vWorldMatrix._11 = m[0]; data.vWorldMatrix._12 = m[1]; data.vWorldMatrix._13 = m[2]; data.vWorldMatrix._14 = m[3];
        data.vWorldMatrix._21 = m[4]; data.vWorldMatrix._22 = m[5]; data.vWorldMatrix._23 = m[6]; data.vWorldMatrix._24 = m[7];
        data.vWorldMatrix._31 = m[8]; data.vWorldMatrix._32 = m[9]; data.vWorldMatrix._33 = m[10]; data.vWorldMatrix._34 = m[11];
        data.vWorldMatrix._41 = m[12]; data.vWorldMatrix._42 = m[13]; data.vWorldMatrix._43 = m[14]; data.vWorldMatrix._44 = m[15];
    }

    j.at("EffectSystem").get_to(data.eEffectSystemType);
    j.at("ParticleSystem").get_to(data.eEffectParticleType);
    j.at("EffectType").get_to(data.eEffectType);
    j.at("ShapeType").get_to(data._Effect_ShapeType);
    if (j.contains("EmissionType")) j.at("EmissionType").get_to(data._Effect_EmissionType);

    data._Effect_Model_Tag = Engine_Utils::ToWString(j.at("Model_Tag").get<string>());
    data._Effect_DiffuseTexture_Tag = Engine_Utils::ToWString(j.at("DiffuseTexture_Tag").get<string>());
    data._Effect_NoiseTexture_Tag = Engine_Utils::ToWString(j.at("NoiseTexture_Tag").get<string>());
    data._Effect_MaskingTexture_Tag = Engine_Utils::ToWString(j.at("MaskingTexture_Tag").get<string>());
    data._Effect_GradationTexture_Tag = Engine_Utils::ToWString(j.at("GradationTexture_Tag").get<string>());
    data._Effect_TrailTexture_Tag = Engine_Utils::ToWString(j.at("TrailTexture_Tag").get<string>());
    data._Effect_NormalTexture_Tag = Engine_Utils::ToWString(j.at("NormalTexture_Tag").get<string>());
    data._Effect_Shader_Tag = Engine_Utils::ToWString(j.at("Shader_Tag").get<string>());
    j.at("Shader_Pass").get_to(data._Effect_ShaderPass);

    // Math Recovery
    data._Effect_ScrollSpeed.x = j.at("ScrollSpeed").at("x").get<float>();
    data._Effect_ScrollSpeed.y = j.at("ScrollSpeed").at("y").get<float>();
    data._Effect_DistortionScale.x = j.at("DistortionScale").at("x").get<float>();
    data._Effect_DistortionScale.y = j.at("DistortionScale").at("y").get<float>();
    data._Effect_StartScale.x = j.at("StartScale").at("x").get<float>();
    data._Effect_StartScale.y = j.at("StartScale").at("y").get<float>();
    data._Effect_StartScale.z = j.at("StartScale").at("z").get<float>();
    data._Effect_EndScale.x = j.at("EndScale").at("x").get<float>();
    data._Effect_EndScale.y = j.at("EndScale").at("y").get<float>();
    data._Effect_EndScale.z = j.at("EndScale").at("z").get<float>();
    data._Effect_Color.x = j.at("Color").at("x").get<float>();
    data._Effect_Color.y = j.at("Color").at("y").get<float>();
    data._Effect_Color.z = j.at("Color").at("z").get<float>();
    data._Effect_Color.w = j.at("Color").at("w").get<float>();
    data._Effect_Range.x = j.at("Range").at("x").get<float>();
    data._Effect_Range.y = j.at("Range").at("y").get<float>();
    data._Effect_Range.z = j.at("Range").at("z").get<float>();
    data._Effect_ParticleSize.x = j.at("ParticleSize").at("x").get<float>();
    data._Effect_ParticleSize.y = j.at("ParticleSize").at("y").get<float>();
    if (j.contains("SpiralData")) {
        data._Effect_Spiral_Radius = j.at("SpiralData").at("Radius").get<float>();
        data._Effect_Spiral_Speed = j.at("SpiralData").at("Speed").get<float>();
    }

    // Time & Animation
    j.at("TimeFlag").get_to(data._Effect_TimeFlag);
    j.at("StartDelay").get_to(data._Effect_StartDelay);
    j.at("LifeTime").get_to(data._Effect_LifeTime);
    j.at("Duration").get_to(data._Effect_Duration);
    j.at("Looping").get_to(data._Effect_Looping);
    j.at("MaxParticle").get_to(data._Effect_MaxParticle);
    j.at("IsRandomSeed").get_to(data._Effect_IsRandomSeed);
    j.at("PlayBackSpeed").get_to(data._Effect_PlayBackSpeed);
    j.at("StartSpeed").get_to(data._Effect_StartSpeed);
    j.at("DiscardValue").get_to(data._Effect_DiscardValue);
    j.at("UseSprite").get_to(data._Effect_bUseSprite);
    data._Effect_TileCount.x = j.at("TileCount").at("x").get<_uint>();
    data._Effect_TileCount.y = j.at("TileCount").at("y").get<_uint>();
    j.at("PlayAnimation").get_to(data._Effect_bPlayAnim);
    j.at("AnimationSpeed").get_to(data._Effect_AnimSpeed);

    if (j.contains("DiffuseTexture_ScrollWeight")) {
        data._Effect_DiffuseTexture_ScrollWeight.x = j.at("DiffuseTexture_ScrollWeight").at("x").get<float>();
        data._Effect_DiffuseTexture_ScrollWeight.y = j.at("DiffuseTexture_ScrollWeight").at("y").get<float>();
    }
    if (j.contains("NoiseTexture_ScrollWeight")) {
        data._Effect_NoiseTexture_ScrollWeight.x = j.at("NoiseTexture_ScrollWeight").at("x").get<float>();
        data._Effect_NoiseTexture_ScrollWeight.y = j.at("NoiseTexture_ScrollWeight").at("y").get<float>();
    }
    if (j.contains("MaskingTexture_ScrollWeight")) {
        data._Effect_MaskingTexture_ScrollWeight.x = j.at("MaskingTexture_ScrollWeight").at("x").get<float>();
        data._Effect_MaskingTexture_ScrollWeight.y = j.at("MaskingTexture_ScrollWeight").at("y").get<float>();
    }
    if (j.contains("GradationTexture_ScrollWeight")) {
        data._Effect_GradationTexture_ScrollWeight.x = j.at("GradationTexture_ScrollWeight").at("x").get<float>();
        data._Effect_GradationTexture_ScrollWeight.y = j.at("GradationTexture_ScrollWeight").at("y").get<float>();
    }

    // --- 툴 UI 체크박스 상태 복구 ---
    if (j.contains("Tool_UseScroll_Diffuse")) j.at("Tool_UseScroll_Diffuse").get_to(data._Effect_Tool_UseScroll_Diffuse);
    if (j.contains("Tool_UseScroll_Noise")) j.at("Tool_UseScroll_Noise").get_to(data._Effect_Tool_UseScroll_Noise);
    if (j.contains("Tool_UseScroll_Masking")) j.at("Tool_UseScroll_Masking").get_to(data._Effect_Tool_UseScroll_Masking);
    if (j.contains("Tool_UseScroll_Gradation")) j.at("Tool_UseScroll_Gradation").get_to(data._Effect_Tool_UseScroll_Gradation);

    // Physics & Rotation Settings
    data._Effect_Gravity_Value = j.at("GravityBase").at("Val").get<float>();
    data._Effect_GravityModifier = j.at("GravityBase").at("Mod").get<float>();
    data._Effect_GravityDir.x = j.at("GravityDir").at("x").get<float>();
    data._Effect_GravityDir.y = j.at("GravityDir").at("y").get<float>();
    data._Effect_GravityDir.z = j.at("GravityDir").at("z").get<float>();
    j.at("UseGlobalGravityCurve").get_to(data._bUseGlobalGravityCurve);
    j.at("UseExternalForceCurve").get_to(data._bUseExternalForceCurve);
    j.at("ExternalForceStrength").get_to(data.fExternalForceStrength);

    data._Effect_StartRotation.x = j.at("StartRotation").at("x").get<float>();
    data._Effect_StartRotation.y = j.at("StartRotation").at("y").get<float>();
    data._Effect_StartRotation.z = j.at("StartRotation").at("z").get<float>();
    data._Effect_TargetRotation.x = j.at("TargetRotation").at("x").get<float>();
    data._Effect_TargetRotation.y = j.at("TargetRotation").at("y").get<float>();
    data._Effect_TargetRotation.z = j.at("TargetRotation").at("z").get<float>();
    j.at("UseStartRotation").get_to(data._bUseStartRotation);
    j.at("UseRotationCurve").get_to(data._bUseRotationCurve);
    j.at("SeparateAxes").get_to(data._bSeparateAxes);

    // Curves Recovery
    auto JsonToCurve = [&](const string& key, auto& vec) {
        if (j.contains(key)) {
            vec.clear();
            for (const auto& item : j.at(key)) vec.push_back({ item.at("Time").get<float>(), item.at("Val").get<float>() });
        }
        };
    JsonToCurve("GlobalGravityCurve", data._vecGlobalGravityCurve);
    JsonToCurve("ExternalForceCurve", data._vecExternalForceCurve);
    JsonToCurve("RotationCurveX", data._vecRotationCurveX);
    JsonToCurve("RotationCurveY", data._vecRotationCurveY);
    JsonToCurve("RotationCurveZ", data._vecRotationCurveZ);

    // Flags & Tool Settings
    j.at("TextureFlag").get_to(data._Effect_TextureFlag);
    j.at("RenderFlag").get_to(data._Effect_RenderFlag);
    j.at("SamplerStateFlag").get_to(data._Effect_SamplerStateFlag);
    if (j.contains("TexRotationFlag")) j.at("TexRotationFlag").get_to(data._Effect_TextureRotationFlag);
    if (j.contains("TexOperatorFlag")) j.at("TexOperatorFlag").get_to(data._Effect_TextureOperatorFlag);

    j.at("Tool_DiffuseTexture").get_to(data._Effect_Tool_DiffuseTexture);
    j.at("Tool_NoiseTexture").get_to(data._Effect_Tool_NoiseTexture);
    j.at("Tool_MaskingTexture").get_to(data._Effect_Tool_MaskingTexture);
    j.at("Tool_GradationTexture").get_to(data._Effect_Tool_GradationTexture);
    j.at("Tool_UseBillboard").get_to(data._Effect_Tool_UseBillboard);
    j.at("Tool_UseScroll").get_to(data._Effect_Tool_UseScroll);
    j.at("Tool_RightScroll").get_to(data._Effect_Tool_RightScroll);
    j.at("Tool_DownScroll").get_to(data._Effect_Tool_DownScroll);
    j.at("Tool_DiffuseSampler_Flag").get_to(data._Effect_Tool_DiffuseSamplerState_Flag);
    j.at("Tool_NoiseSampler_Flag").get_to(data._Effect_Tool_NoiseSamplerState_Flag);
    j.at("Tool_MaskingSampler_Flag").get_to(data._Effect_Tool_MaskingSamplerState_Flag);
    j.at("Tool_GradationSampler_Flag").get_to(data._Effect_Tool_GradationSamplerState_Flag);
}

NS_END


NS_BEGIN(Engine)

json CEFFECT_CONTAINER::ToJson() const
{
	return json(m_Data);
}

HRESULT CEFFECT_CONTAINER::FromJson(const json& j)
{
	m_Data = j.get<DTO::TEFFECT_ContainerData>();
	return S_OK;
}
NS_END;