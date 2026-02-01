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

		// ========== Effect 타입 ==========
		{"EffectSystem", data.eEffectSystemType},
		{"ParticleSystem", data.eEffectParticleType},
		{"EffectType", data.eEffectType},
		{"ShapeType", data._Effect_ShapeType},

		// ========== Resource 값 ==========
		{"Model_Tag", Engine_Utils::ToString(data._Effect_Model_Tag)},
		{"DiffuseTexture_Tag", Engine_Utils::ToString(data._Effect_DiffuseTexture_Tag)},
		{"NoiseTexture_Tag", Engine_Utils::ToString(data._Effect_NoiseTexture_Tag)},
		{"MaskingTexture_Tag", Engine_Utils::ToString(data._Effect_MaskingTexture_Tag)},
		{"GradationTexture_Tag", Engine_Utils::ToString(data._Effect_GradationTexture_Tag)},
		{"TrailTexture_Tag", Engine_Utils::ToString(data._Effect_TrailTexture_Tag)},
		{"NormalTexture_Tag", Engine_Utils::ToString(data._Effect_NormalTexture_Tag)},

		// ========== Shader 값 ==========
		{"Shader_Tag", Engine_Utils::ToString(data._Effect_Shader_Tag)},
		{"Shader_Pass", data._Effect_ShaderPass},

		{"ScrollSpeed", {{"x", data._Effect_ScrollSpeed.x}, {"y", data._Effect_ScrollSpeed.y}}},
		{"DistotionScale", {{"x", data._Effect_DistortionScale.x}, {"y", data._Effect_DistortionScale.y}}},
		{"StartScale", {{"x", data._Effect_StartScale.x}, {"y", data._Effect_StartScale.y}, {"z", data._Effect_StartScale.z}}},
		{"EndScale", {{"x", data._Effect_EndScale.x}, {"y", data._Effect_EndScale.y}, {"z", data._Effect_EndScale.z}}},
		{"Color", {{"x", data._Effect_Color.x}, {"y", data._Effect_Color.y}, {"z", data._Effect_Color.z}, {"w", data._Effect_Color.w}}},
		{"Range", {{"x", data._Effect_Range.x}, {"y", data._Effect_Range.y}, {"z", data._Effect_Range.z}}},
		{"ParticleSize", {{"x", data._Effect_ParticleSize.x}, {"y", data._Effect_ParticleSize.y}}},

		// ========== Shader 내부 바인딩 값 ==========
		{"DiscardValue", data._Effect_DiscardValue},
		{"UseSprite", data._Effect_bUseSprite},
		{"TileCount", {{"x", data._Effect_TileCount.x}, {"y", data._Effect_TileCount.y}}},
		{"PlayAnimation", data._Effect_bPlayAnim},
		{"AnimationSpeed", data._Effect_AnimSpeed},
		{"SpriteNumber", data.m_iCurSpriteNumber},
		{"Duration", data._Effect_Duration},
		{"Looping", data._Effect_Looping},
		{"MaxParticle", data._Effect_MaxParticle},
		{"TextureFlag", data._Effect_TextureFlag},
		{"RenderFlag", data._Effect_RenderFlag},
		{"SampleStateFlag", data._Effect_SamplerStateFlag},

		// ==========  툴 전용 플래그 저장  ==========
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
}

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

	data._Effect_Model_Tag = Engine_Utils::ToWString(j.at("Model_Tag").get<string>());
	data._Effect_DiffuseTexture_Tag = Engine_Utils::ToWString(j.at("DiffuseTexture_Tag").get<string>());
	data._Effect_NoiseTexture_Tag = Engine_Utils::ToWString(j.at("NoiseTexture_Tag").get<string>());
	data._Effect_MaskingTexture_Tag = Engine_Utils::ToWString(j.at("MaskingTexture_Tag").get<string>());
	data._Effect_GradationTexture_Tag = Engine_Utils::ToWString(j.at("GradationTexture_Tag").get<string>());
	data._Effect_TrailTexture_Tag = Engine_Utils::ToWString(j.at("TrailTexture_Tag").get<string>());
	data._Effect_NormalTexture_Tag = Engine_Utils::ToWString(j.at("NormalTexture_Tag").get<string>());
	data._Effect_Shader_Tag = Engine_Utils::ToWString(j.at("Shader_Tag").get<string>());

	j.at("Shader_Pass").get_to(data._Effect_ShaderPass);

	// Math 복구
	data._Effect_ScrollSpeed.x = j.at("ScrollSpeed").at("x").get<float>();
	data._Effect_ScrollSpeed.y = j.at("ScrollSpeed").at("y").get<float>();
	data._Effect_DistortionScale.x = j.at("DistotionScale").at("x").get<float>();
	data._Effect_DistortionScale.y = j.at("DistotionScale").at("y").get<float>();
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
	data._Effect_TileCount.x = j.at("TileCount").at("x").get<_uint>();
	data._Effect_TileCount.y = j.at("TileCount").at("y").get<_uint>();
	data._Effect_ParticleSize.x = j.at("ParticleSize").at("x").get<float>();
	data._Effect_ParticleSize.y = j.at("ParticleSize").at("y").get<float>();
	data._Effect_Range.x = j.at("Range").at("x").get<float>();
	data._Effect_Range.y = j.at("Range").at("y").get<float>();
	data._Effect_Range.z = j.at("Range").at("z").get<float>();

	j.at("UseSprite").get_to(data._Effect_bUseSprite);
	j.at("PlayAnimation").get_to(data._Effect_bPlayAnim);
	j.at("AnimationSpeed").get_to(data._Effect_AnimSpeed);
	j.at("DiscardValue").get_to(data._Effect_DiscardValue);
	j.at("Duration").get_to(data._Effect_Duration);
	j.at("Looping").get_to(data._Effect_Looping);
	j.at("MaxParticle").get_to(data._Effect_MaxParticle);
	j.at("TextureFlag").get_to(data._Effect_TextureFlag);
	j.at("RenderFlag").get_to(data._Effect_RenderFlag);
	j.at("SampleStateFlag").get_to(data._Effect_SamplerStateFlag);

	// 툴 전용 플래그 복원
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