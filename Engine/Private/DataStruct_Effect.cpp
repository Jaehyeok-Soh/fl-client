#include "Engine_pch.h"
#include "DataStruct_Effect.h"
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

// ==========  TEFFECT_PartsData (to_json)  ==========
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

        // --- Resource Tags (Glow, Dissolve 포함) ---
        {"Model_Tag", Engine_Utils::ToString(data._Effect_Model_Tag)},
        {"DiffuseTexture_Tag", Engine_Utils::ToString(data._Effect_DiffuseTexture_Tag)},
        {"NoiseTexture_Tag", Engine_Utils::ToString(data._Effect_NoiseTexture_Tag)},
        {"MaskingTexture_Tag", Engine_Utils::ToString(data._Effect_MaskingTexture_Tag)},
        {"GradationTexture_Tag", Engine_Utils::ToString(data._Effect_GradationTexture_Tag)},
        {"CurveTexture_Tag", Engine_Utils::ToString(data._Effect_CurveTexture_Tag)},
        {"NormalTexture_Tag", Engine_Utils::ToString(data._Effect_NormalTexture_Tag)},
        {"DissolveTexture_Tag", Engine_Utils::ToString(data._Effect_DissolveTexture_Tag)},
        {"GlowTexture_Tag", Engine_Utils::ToString(data._Effect_GlowTexture_Tag)},
        {"Shader_Path", Engine_Utils::ToString(data._Effect_Shader_Path)},
        {"Shader_Tag", Engine_Utils::ToString(data._Effect_Shader_Tag)},
        {"Shader_Pass", data._Effect_ShaderPass},

        // --- Math & Transformation ---
        {"ScrollSpeed", {{"x", data._Effect_ScrollSpeed.x}, {"y", data._Effect_ScrollSpeed.y}}},
        {"DistortionScale", {{"x", data._Effect_DistortionScale.x}, {"y", data._Effect_DistortionScale.y}}},
        {"StartScale", {{"x", data._Effect_StartScale.x}, {"y", data._Effect_StartScale.y}, {"z", data._Effect_StartScale.z}}},
        {"EndScale", {{"x", data._Effect_EndScale.x}, {"y", data._Effect_EndScale.y}, {"z", data._Effect_EndScale.z}}},

        // --- Scale Curve Flags  ---
        {"UseScaleCurve", data._bUseScaleCurve},
        {"SeparateScaleAxes", data._bSeparateScaleAxes}, // 축별 개별 제어 여부

        {"Color", {{"x", data._Effect_Color.x}, {"y", data._Effect_Color.y}, {"z", data._Effect_Color.z}, {"w", data._Effect_Color.w}}},
        {"DiscardValue", data._Effect_DiscardValue},
        {"_Use_Effect_Particle_Burst", data._Use_Effect_Particle_Burst},
        {"Range", {{"x", data._Effect_Range.x}, {"y", data._Effect_Range.y}, {"z", data._Effect_Range.z}}},
        {"ParticleSize", {{"x", data._Effect_ParticleSize.x}, {"y", data._Effect_ParticleSize.y}}},
        {"SpiralData", {{"Radius", data._Effect_Spiral_Radius}, {"Speed", data._Effect_Spiral_Speed}}},
        {"UV_Offset", {{"x", data._Effect_UV_Offset.x}, {"y", data._Effect_UV_Offset.y}}},

        // --- Time & Animation ---
        {"TimeFlag", data._Effect_TimeFlag},
        {"StartDelay", data._Effect_StartDelay},
        {"LifeTime", data._Effect_LifeTime},
        {"Duration", data._Effect_Duration},
        {"Looping", data._Effect_Looping},
        {"_Use_Effect_Continue", data._Use_Effect_Continue},
        {"MaxParticle", data._Effect_MaxParticle},
       {"RandomFlags", static_cast<int>(data.iRandomFlags)},
        {"PlayBackSpeed", data._Effect_PlayBackSpeed},
        {"StartSpeed", data._Effect_StartSpeed},
        {"PlayAnimation", data._Effect_bPlayAnim},
        {"AnimationSpeed", data._Effect_AnimSpeed},
        {"AppearRatio", data._Effect_ApearRatio},

        // ---- Sprite 가중치 ----
        {"SpriteInfo", {
            {"Diffuse",   {{"x", data._Effect_DiffuseTexture_SpriteInfo.x}, {"y", data._Effect_DiffuseTexture_SpriteInfo.y}, {"z", data._Effect_DiffuseTexture_SpriteInfo.z}, {"w", data._Effect_DiffuseTexture_SpriteInfo.w}}},
            {"Noise",     {{"x", data._Effect_NoiseTexture_SpriteInfo.x},   {"y", data._Effect_NoiseTexture_SpriteInfo.y},   {"z", data._Effect_NoiseTexture_SpriteInfo.z},   {"w", data._Effect_NoiseTexture_SpriteInfo.w}}},
            {"Gradation", {{"x", data._Effect_GradationTexture_SpriteInfo.x}, {"y", data._Effect_GradationTexture_SpriteInfo.y}, {"z", data._Effect_GradationTexture_SpriteInfo.z}, {"w", data._Effect_GradationTexture_SpriteInfo.w}}},
            {"Dissolve",  {{"x", data._Effect_DissolveTexture_SpriteInfo.x},  {"y", data._Effect_DissolveTexture_SpriteInfo.y},  {"z", data._Effect_DissolveTexture_SpriteInfo.z},  {"w", data._Effect_DissolveTexture_SpriteInfo.w}}},
            {"Glow",      {{"x", data._Effect_GlowTexture_SpriteInfo.x},      {"y", data._Effect_GlowTexture_SpriteInfo.y},      {"z", data._Effect_GlowTexture_SpriteInfo.z},      {"w", data._Effect_GlowTexture_SpriteInfo.w}}},
             {"Curve",      {{"x", data._Effect_CurveTexture_SpriteInfo.x},      {"y", data._Effect_CurveTexture_SpriteInfo.y},      {"z", data._Effect_CurveTexture_SpriteInfo.z},      {"w", data._Effect_CurveTexture_SpriteInfo.w}}},
             {"Mask",      {{"x", data._Effect_MaskTexture_SpriteInfo.x},      {"y", data._Effect_MaskTexture_SpriteInfo.y},      {"z", data._Effect_MaskTexture_SpriteInfo.z},      {"w", data._Effect_MaskTexture_SpriteInfo.w}}}
        
         }},

        // --- 스크롤 가중치 & 개별 활성화 ---
        {"ScrollWeights", {
            {"Diffuse", {{"x", data._Effect_DiffuseTexture_ScrollWeight.x}, {"y", data._Effect_DiffuseTexture_ScrollWeight.y}}},
            {"Noise", {{"x", data._Effect_NoiseTexture_ScrollWeight.x}, {"y", data._Effect_NoiseTexture_ScrollWeight.y}}},
            {"Masking", {{"x", data._Effect_MaskingTexture_ScrollWeight.x}, {"y", data._Effect_MaskingTexture_ScrollWeight.y}}},
            {"Gradation", {{"x", data._Effect_GradationTexture_ScrollWeight.x}, {"y", data._Effect_GradationTexture_ScrollWeight.y}}},
            {"Dissolve", {{"x", data._Effect_DissolveTexture_ScrollWeight.x}, {"y", data._Effect_DissolveTexture_ScrollWeight.y}}},
            {"Glow", {{"x", data._Effect_GlowTexture_ScrollWeight.x}, {"y", data._Effect_GlowTexture_ScrollWeight.y}}},
            {"Curve", {{"x", data._Effect_CurveTexture_ScrollWeight.x}, {"y", data._Effect_CurveTexture_ScrollWeight.y}}}
        }},
        {"Tool_ScrollFlags", {
            {"Diffuse", data._Effect_Tool_UseScroll_Diffuse},
            {"Noise", data._Effect_Tool_UseScroll_Noise},
            {"Masking", data._Effect_Tool_UseScroll_Masking},
            {"Gradation", data._Effect_Tool_UseScroll_Gradation},
            {"Dissolve", data._Effect_Tool_UseScroll_Dissolve},
            {"Glow", data._Effect_Tool_UseScroll_Glow},
            {"Curve", data._Effect_Tool_UseScroll_Curve}
        }},

        // --- Physics & Gravity ---
        {"GravityBase", {{"Val", data._Effect_Gravity_Value}, {"Mod", data._Effect_GravityModifier}}},
        {"GravityDir", {{"x", data._Effect_GravityDir.x}, {"y", data._Effect_GravityDir.y}, {"z", data._Effect_GravityDir.z}}},
        {"UseGlobalGravityCurve", data._bUseGlobalGravityCurve},
        {"UseExternalForceCurve", data._bUseExternalForceCurve},
        {"ExternalForceStrength", data.fExternalForceStrength},

        // --- UV Scroll Curves  ---
        {"UseUVScrollCurve", data._bUseUVScrollCurve},

        // --- Rotation Settings ---
        {"StartRotation", {{"x", data._Effect_StartRotation.x}, {"y", data._Effect_StartRotation.y}, {"z", data._Effect_StartRotation.z}}},
        {"TargetRotation", {{"x", data._Effect_TargetRotation.x}, {"y", data._Effect_TargetRotation.y}, {"z", data._Effect_TargetRotation.z}}},
        {"UseStartRotation", data._bUseStartRotation},
        {"UseRotationCurve", data._bUseRotationCurve},
        {"_Use_Effect_UV_OverScroll", data._Use_Effect_UV_OverScroll},
        {"_Effect_Tool_UseLifeDissolve", data._Effect_Tool_UseLifeDissolve },
        {"SeparateAxes", data._bSeparateAxes},

        // --- Flags ---
        {"TextureFlag", data._Effect_TextureFlag},
        {"RenderFlag", data._Effect_RenderFlag},
        {"SamplerStateFlag", data._Effect_SamplerStateFlag},
        {"TexRotationFlag", data._Effect_TextureRotationFlag},
        {"TexOperatorFlag", data._Effect_TextureOperatorFlag},

        // --- Tool Settings ---
        {"Tool_Resource", {
            {"Diffuse", data._Effect_Tool_DiffuseTexture},
            {"Noise", data._Effect_Tool_NoiseTexture},
            {"Masking", data._Effect_Tool_MaskingTexture},
            {"Gradation", data._Effect_Tool_GradationTexture},
            {"Dissolve", data._Effect_Tool_DissolveTexture},
            {"Glow", data._Effect_Tool_GlowTexture},
            {"Curve", data._Effect_Tool_CurveTexture}
        }},
        {"Tool_Render", {
            {"Billboard", data._Effect_Tool_UseBillboard},
            {"DirBillboard", data._Effect_Tool_UseDirBillboard},
            {"Scroll", data._Effect_Tool_UseScroll},
            {"Right", data._Effect_Tool_RightScroll},
            {"Down", data._Effect_Tool_DownScroll}
        }},
        {"Tool_Samplers", {
            {"Diffuse", data._Effect_Tool_DiffuseSamplerState_Flag},
            {"Noise", data._Effect_Tool_NoiseSamplerState_Flag},
            {"Masking", data._Effect_Tool_MaskingSamplerState_Flag},
            {"Gradation", data._Effect_Tool_GradationSamplerState_Flag}
        }}
    };

    // 커브 데이터 직렬화 람다
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
    j["UVScrollCurveX"] = CurveToJson(data._vecUVScrollCurveX);
    j["UVScrollCurveY"] = CurveToJson(data._vecUVScrollCurveY);
    j["ScaleCurveX"] = CurveToJson(data._vecScaleCurveX);
    j["ScaleCurveY"] = CurveToJson(data._vecScaleCurveY);
    j["ScaleCurveZ"] = CurveToJson(data._vecScaleCurveZ);
}

// ==========  Parts (from_json)  ==========
void from_json(const json& j, TEFFECT_PartsData& data)
{
    // 기본 태그 및 행렬 복구
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

    // 타입 설정
    j.at("EffectSystem").get_to(data.eEffectSystemType);
    j.at("ParticleSystem").get_to(data.eEffectParticleType);
    j.at("EffectType").get_to(data.eEffectType);
    j.at("ShapeType").get_to(data._Effect_ShapeType);
    if (j.contains("EmissionType")) j.at("EmissionType").get_to(data._Effect_EmissionType);

    // 리소스 태그 복구
    data._Effect_Model_Tag = Engine_Utils::ToWString(j.at("Model_Tag").get<string>());
    data._Effect_DiffuseTexture_Tag = Engine_Utils::ToWString(j.at("DiffuseTexture_Tag").get<string>());
    data._Effect_NoiseTexture_Tag = Engine_Utils::ToWString(j.at("NoiseTexture_Tag").get<string>());
    data._Effect_MaskingTexture_Tag = Engine_Utils::ToWString(j.at("MaskingTexture_Tag").get<string>());
    data._Effect_GradationTexture_Tag = Engine_Utils::ToWString(j.at("GradationTexture_Tag").get<string>());
    data._Effect_CurveTexture_Tag = Engine_Utils::ToWString(j.at("CurveTexture_Tag").get<string>());
    data._Effect_NormalTexture_Tag = Engine_Utils::ToWString(j.at("NormalTexture_Tag").get<string>());
    if (j.contains("DissolveTexture_Tag")) data._Effect_DissolveTexture_Tag = Engine_Utils::ToWString(j.at("DissolveTexture_Tag").get<string>());
    if (j.contains("GlowTexture_Tag")) data._Effect_GlowTexture_Tag = Engine_Utils::ToWString(j.at("GlowTexture_Tag").get<string>());
    if (j.contains("Shader_Path")) data._Effect_Shader_Path = Engine_Utils::ToWString(j.at("Shader_Path").get<string>());
    data._Effect_Shader_Tag = Engine_Utils::ToWString(j.at("Shader_Tag").get<string>());
    j.at("Shader_Pass").get_to(data._Effect_ShaderPass);

    // 수학 데이터 복구
    data._Effect_ScrollSpeed = { j.at("ScrollSpeed").at("x"), j.at("ScrollSpeed").at("y") };
    data._Effect_DistortionScale = { j.at("DistortionScale").at("x"), j.at("DistortionScale").at("y") };
    data._Effect_StartScale = { j.at("StartScale").at("x"), j.at("StartScale").at("y"), j.at("StartScale").at("z") };
    data._Effect_EndScale = { j.at("EndScale").at("x"), j.at("EndScale").at("y"), j.at("EndScale").at("z") };

    if (j.contains("UseScaleCurve")) j.at("UseScaleCurve").get_to(data._bUseScaleCurve);
    else data._bUseScaleCurve = false;
    if (j.contains("SeparateScaleAxes")) j.at("SeparateScaleAxes").get_to(data._bSeparateScaleAxes);
    else data._bSeparateScaleAxes = false;

    data._Effect_Color = { j.at("Color").at("x"), j.at("Color").at("y"), j.at("Color").at("z"), j.at("Color").at("w") };
    j.at("DiscardValue").get_to(data._Effect_DiscardValue);
    data._Effect_Range = { j.at("Range").at("x"), j.at("Range").at("y"), j.at("Range").at("z") };
    data._Effect_ParticleSize = { j.at("ParticleSize").at("x"), j.at("ParticleSize").at("y") };
    if (j.contains("UV_Offset")) data._Effect_UV_Offset = { j.at("UV_Offset").at("x"), j.at("UV_Offset").at("y") };
    if (j.contains("SpiralData")) {
        data._Effect_Spiral_Radius = j.at("SpiralData").at("Radius").get<float>();
        data._Effect_Spiral_Speed = j.at("SpiralData").at("Speed").get<float>();
    }

    // 시간 및 애니메이션 복구
    j.at("TimeFlag").get_to(data._Effect_TimeFlag);
    j.at("StartDelay").get_to(data._Effect_StartDelay);
    j.at("LifeTime").get_to(data._Effect_LifeTime);
    j.at("Duration").get_to(data._Effect_Duration);
    j.at("Looping").get_to(data._Effect_Looping);
    j.at("_Use_Effect_Continue").get_to(data._Use_Effect_Continue);
    j.at("MaxParticle").get_to(data._Effect_MaxParticle);

    if (j.contains("RandomFlags"))
    {
        // 정수형으로 읽어온 뒤 Enum 타입으로 강제 형변환
        data.iRandomFlags = static_cast<E_RANDOM_FLAG>(j.at("RandomFlags").get<int>());
    }
    // 하위 호환성 유지: 기존 데이터에 IsRandomSeed(bool)가 남아있을 경우
    else if (j.contains("IsRandomSeed"))
    {
        bool bOldSeed = j.at("IsRandomSeed").get<bool>();
        // 기존에 True였다면 모든 랜덤 속성을 다 켜주는 식으로 보정
        data.iRandomFlags = bOldSeed ? (RAND_POS | RAND_LIFE | RAND_SIZE) : RAND_NONE;
    }

    j.at("PlayBackSpeed").get_to(data._Effect_PlayBackSpeed);
    j.at("StartSpeed").get_to(data._Effect_StartSpeed);
    j.at("PlayAnimation").get_to(data._Effect_bPlayAnim);
    j.at("AnimationSpeed").get_to(data._Effect_AnimSpeed);
    if (j.contains("AppearRatio")) j.at("AppearRatio").get_to(data._Effect_ApearRatio);

    // 스크롤 가중치 복구
    if (j.contains("ScrollWeights")) {
        auto& sw = j.at("ScrollWeights");
        data._Effect_DiffuseTexture_ScrollWeight = { sw["Diffuse"]["x"], sw["Diffuse"]["y"] };
        data._Effect_NoiseTexture_ScrollWeight = { sw["Noise"]["x"], sw["Noise"]["y"] };
        data._Effect_MaskingTexture_ScrollWeight = { sw["Masking"]["x"], sw["Masking"]["y"] };
        data._Effect_GradationTexture_ScrollWeight = { sw["Gradation"]["x"], sw["Gradation"]["y"] };
        if (sw.contains("Dissolve")) data._Effect_DissolveTexture_ScrollWeight = { sw["Dissolve"]["x"], sw["Dissolve"]["y"] };
        if (sw.contains("Glow")) data._Effect_GlowTexture_ScrollWeight = { sw["Glow"]["x"], sw["Glow"]["y"] };
        if (sw.contains("Curve")) data._Effect_CurveTexture_ScrollWeight = { sw["Curve"]["x"], sw["Curve"]["y"] };
    }

    // 스프라이트 전용 값들

    if (j.contains("SpriteInfo")) {
        const auto& si = j.at("SpriteInfo");

        if (si.contains("Diffuse"))
            data._Effect_DiffuseTexture_SpriteInfo = { si["Diffuse"]["x"], si["Diffuse"]["y"], si["Diffuse"]["z"], si["Diffuse"]["w"] };

        if (si.contains("Noise"))
            data._Effect_NoiseTexture_SpriteInfo = { si["Noise"]["x"], si["Noise"]["y"], si["Noise"]["z"], si["Noise"]["w"] };

        if (si.contains("Gradation"))
            data._Effect_GradationTexture_SpriteInfo = { si["Gradation"]["x"], si["Gradation"]["y"], si["Gradation"]["z"], si["Gradation"]["w"] };

        if (si.contains("Dissolve"))
            data._Effect_DissolveTexture_SpriteInfo = { si["Dissolve"]["x"], si["Dissolve"]["y"], si["Dissolve"]["z"], si["Dissolve"]["w"] };

        if (si.contains("Glow"))
            data._Effect_GlowTexture_SpriteInfo = { si["Glow"]["x"], si["Glow"]["y"], si["Glow"]["z"], si["Glow"]["w"] };

        if (si.contains("Curve"))
            data._Effect_CurveTexture_SpriteInfo = { si["Curve"]["x"], si["Curve"]["y"], si["Curve"]["z"], si["Curve"]["w"] };
    
        if (si.contains("Mask"))
            data._Effect_MaskTexture_SpriteInfo = { si["Mask"]["x"], si["Mask"]["y"], si["Mask"]["z"], si["Mask"]["w"] };

    }

    // 툴 전용 스크롤 체크박스 상태 로드
    j.at("_Use_Effect_UV_OverScroll").get_to(data._Use_Effect_UV_OverScroll);
    j.at("_Use_Effect_Particle_Burst").get_to(data._Use_Effect_Particle_Burst);
    if (j.contains("Tool_ScrollFlags")) {
        auto& tf = j.at("Tool_ScrollFlags");
        tf.at("Diffuse").get_to(data._Effect_Tool_UseScroll_Diffuse);
        tf.at("Noise").get_to(data._Effect_Tool_UseScroll_Noise);
        tf.at("Masking").get_to(data._Effect_Tool_UseScroll_Masking);
        tf.at("Gradation").get_to(data._Effect_Tool_UseScroll_Gradation);
        if (tf.contains("Dissolve")) tf.at("Dissolve").get_to(data._Effect_Tool_UseScroll_Dissolve);
        if (tf.contains("_Effect_Tool_UseLifeDissolve")) tf.at("_Effect_Tool_UseLifeDissolve").get_to(data._Effect_Tool_UseLifeDissolve);
        if (tf.contains("Glow")) tf.at("Glow").get_to(data._Effect_Tool_UseScroll_Glow);
        if (tf.contains("Curve")) tf.at("Curve").get_to(data._Effect_Tool_UseScroll_Curve);
    }

    // 물리 및 커브 설정 로드
    data._Effect_Gravity_Value = j.at("GravityBase").at("Val").get<float>();
    data._Effect_GravityModifier = j.at("GravityBase").at("Mod").get<float>();
    data._Effect_GravityDir = { j.at("GravityDir").at("x"), j.at("GravityDir").at("y"), j.at("GravityDir").at("z") };
    j.at("UseGlobalGravityCurve").get_to(data._bUseGlobalGravityCurve);
    j.at("UseExternalForceCurve").get_to(data._bUseExternalForceCurve);
    j.at("ExternalForceStrength").get_to(data.fExternalForceStrength);
    if (j.contains("UseUVScrollCurve")) j.at("UseUVScrollCurve").get_to(data._bUseUVScrollCurve);

    // 회전 및 모든 커브 데이터 복구
    data._Effect_StartRotation = { j.at("StartRotation").at("x"), j.at("StartRotation").at("y"), j.at("StartRotation").at("z") };
    data._Effect_TargetRotation = { j.at("TargetRotation").at("x"), j.at("TargetRotation").at("y"), j.at("TargetRotation").at("z") };
    j.at("UseStartRotation").get_to(data._bUseStartRotation);
    j.at("UseRotationCurve").get_to(data._bUseRotationCurve);
    if (j.contains("SeparateAxes")) j.at("SeparateAxes").get_to(data._bSeparateAxes);

    auto JsonToCurve = [&](const string& key, auto& vec) {
        if (j.contains(key)) {
            vec.clear();
            for (const auto& item : j.at(key))
                vec.push_back({ item.at("Time").get<float>(), item.at("Val").get<float>() });
        }
        };

    JsonToCurve("GlobalGravityCurve", data._vecGlobalGravityCurve);
    JsonToCurve("ExternalForceCurve", data._vecExternalForceCurve);
    JsonToCurve("RotationCurveX", data._vecRotationCurveX);
    JsonToCurve("RotationCurveY", data._vecRotationCurveY);
    JsonToCurve("RotationCurveZ", data._vecRotationCurveZ);
    JsonToCurve("UVScrollCurveX", data._vecUVScrollCurveX);
    JsonToCurve("UVScrollCurveY", data._vecUVScrollCurveY);
    JsonToCurve("ScaleCurveX", data._vecScaleCurveX);
    JsonToCurve("ScaleCurveY", data._vecScaleCurveY);
    JsonToCurve("ScaleCurveZ", data._vecScaleCurveZ);

    // 플래그 및 Tool_Render/Tool_Samplers 복구
    j.at("TextureFlag").get_to(data._Effect_TextureFlag);
    j.at("RenderFlag").get_to(data._Effect_RenderFlag);
    j.at("SamplerStateFlag").get_to(data._Effect_SamplerStateFlag);
    if (j.contains("TexRotationFlag")) j.at("TexRotationFlag").get_to(data._Effect_TextureRotationFlag);
    if (j.contains("TexOperatorFlag")) j.at("TexOperatorFlag").get_to(data._Effect_TextureOperatorFlag);

    // T(Billboard, Scroll 등
    if (j.contains("Tool_Render")) {
        auto& tr = j.at("Tool_Render");
        tr.at("Billboard").get_to(data._Effect_Tool_UseBillboard);
        tr.at("DirBillboard").get_to(data._Effect_Tool_UseDirBillboard);
        tr.at("Scroll").get_to(data._Effect_Tool_UseScroll);
        tr.at("Right").get_to(data._Effect_Tool_RightScroll);
        tr.at("Down").get_to(data._Effect_Tool_DownScroll);
    }

    // Tool_Samplers 복구
    if (j.contains("Tool_Samplers")) {
        auto& ts = j.at("Tool_Samplers");
        ts.at("Diffuse").get_to(data._Effect_Tool_DiffuseSamplerState_Flag);
        ts.at("Noise").get_to(data._Effect_Tool_NoiseSamplerState_Flag);
        ts.at("Masking").get_to(data._Effect_Tool_MaskingSamplerState_Flag);
        ts.at("Gradation").get_to(data._Effect_Tool_GradationSamplerState_Flag);
    }

    // Tool_Resource 복구 (텍스처 체크박스)
    if (j.contains("Tool_Resource")) {
        auto& res = j.at("Tool_Resource");
        res.at("Diffuse").get_to(data._Effect_Tool_DiffuseTexture);
        res.at("Noise").get_to(data._Effect_Tool_NoiseTexture);
        res.at("Masking").get_to(data._Effect_Tool_MaskingTexture);
        res.at("Gradation").get_to(data._Effect_Tool_GradationTexture);
        if (res.contains("Dissolve")) res.at("Dissolve").get_to(data._Effect_Tool_DissolveTexture);
        if (res.contains("Glow")) res.at("Glow").get_to(data._Effect_Tool_GlowTexture);
        if (res.contains("Curve")) res.at("Curve").get_to(data._Effect_Tool_CurveTexture);
    }
}

NS_END;

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