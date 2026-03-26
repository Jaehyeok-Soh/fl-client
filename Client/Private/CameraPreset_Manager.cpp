#include "pch.h"
#include "CameraPreset_Manager.h"
#include "FileUtils.h"
#pragma push_macro("new")
#undef new
#include "json.hpp"
using json = nlohmann::json;
#pragma pop_macro("new")
#include "GameInstance.h"

#pragma region NOLMHAN

using ordered_json = nlohmann::ordered_json;

NS_BEGIN(Engine)

NLOHMANN_JSON_SERIALIZE_ENUM(Engine::ECameraShotEase,
{
	{Engine::ECameraShotEase::Linear,        "Linear"},
	{Engine::ECameraShotEase::SmoothStep,    "SmoothStep"},
	{Engine::ECameraShotEase::EaseOutQuad,   "EaseOutQuad"},
	{Engine::ECameraShotEase::EaseInOutQuad, "EaseInOutQuad"},
	{Engine::ECameraShotEase::EaseOutBack,   "EaseOutBack"},
	{Engine::ECameraShotEase::END,           "END"},
}
)

NLOHMANN_JSON_SERIALIZE_ENUM(Engine::ECameraShotStartMode,
{
	{Engine::ECameraShotStartMode::InheritCurrent,  "InheritCurrent"},
	{Engine::ECameraShotStartMode::FixedFromPivot,  "FixedFromPivot"},
	{Engine::ECameraShotStartMode::END,              "END"},
}
)

NLOHMANN_JSON_SERIALIZE_ENUM(Engine::ECameraBasisMode,
{
	{Engine::ECameraBasisMode::TARGET_TRANSFORM_YAW, "TARGET_TRANSFORM_YAW"},
	{Engine::ECameraBasisMode::WORLD,                "WORLD"},
	{Engine::ECameraBasisMode::END,                  "END"},
}
)

NLOHMANN_JSON_SERIALIZE_ENUM(Engine::ECameraShotRecoverTarget,
{
	{Engine::ECameraShotRecoverTarget::PreshotSnap,		"PreshotSnap"},
	{Engine::ECameraShotRecoverTarget::GameplaySolved,   "GameplaySolved"},
	{Engine::ECameraShotRecoverTarget::END,               "END"},
}
)

NLOHMANN_JSON_SERIALIZE_ENUM(Engine::ECameraShotRecoverMethod,
{
	{Engine::ECameraShotRecoverMethod::Snap,  "Snap"},
	{Engine::ECameraShotRecoverMethod::Blend, "Blend"},
	{Engine::ECameraShotRecoverMethod::END,   "END"},
}
)

NLOHMANN_JSON_SERIALIZE_ENUM(Engine::ECameraAnchorSource,
{
	{Engine::ECameraAnchorSource::ACTOR,  "ACTOR"},
	{Engine::ECameraAnchorSource::OBJECT, "OBJECT"},
	{Engine::ECameraAnchorSource::END,    "END"},
}
)

NLOHMANN_JSON_SERIALIZE_ENUM(Engine::ECameraAnchorResolve,
{
	{Engine::ECameraAnchorResolve::TRANSFORM,  "TRANSFORM"},
	{Engine::ECameraAnchorResolve::BONE,       "BONE"},
	{Engine::ECameraAnchorResolve::CAM_SOCKET, "CAM_SOCKET"},
	{Engine::ECameraAnchorResolve::END,        "END"},
}
)

NS_END

#pragma region Helper
namespace
{
	static ordered_json MakeJsonVec3(const Vec3& v)
	{
		return ordered_json{
			{"x", v.x},
			{"y", v.y},
			{"z", v.z}
		};
	}

	static HRESULT ReadJsonFile(const std::filesystem::path& filePath, OUT ordered_json& jOut)
	{
		Engine::CFileUtils* pFile = Engine::CFileUtils::Create();
		if (pFile == nullptr)
			return E_FAIL;

		std::string strText;
		HRESULT hr = S_OK;

		if (FAILED(pFile->Open(filePath.wstring(), Engine::FileMode::READ)))
		{
			Safe_Release(pFile);
			return E_FAIL;
		}

		if (FAILED(pFile->ReadAllText(strText)))
		{
			Safe_Release(pFile);
			return E_FAIL;
		}

		Safe_Release(pFile);

		try
		{
			jOut = ordered_json::parse(strText);
		}
		catch (...)
		{
			return E_FAIL;
		}

		return S_OK;
	}

	static HRESULT WriteJsonFile(const std::filesystem::path& filePath, const ordered_json& j)
	{
		Engine::CFileUtils* pFile = Engine::CFileUtils::Create();
		if (pFile == nullptr)
			return E_FAIL;

		std::string strText;
		HRESULT hr = S_OK;

		try
		{
			strText = j.dump(2);
		}
		catch (...)
		{
			Safe_Release(pFile);
			return E_FAIL;
		}

		if (FAILED(pFile->Open(filePath.wstring(), Engine::FileMode::WRITE)))
		{
			Safe_Release(pFile);
			return E_FAIL;
		}

		if (FAILED(pFile->WriteAllText(strText)))
		{
			Safe_Release(pFile);
			return E_FAIL;
		}

		Safe_Release(pFile);
		return S_OK;
	}

	static void ReadJsonVec3(const ordered_json& j, OUT Vec3& vOut)
	{
		vOut.x = j.value("x", 0.f);
		vOut.y = j.value("y", 0.f);
		vOut.z = j.value("z", 0.f);
	}

	template<typename T>
	static void ReadIfExists(const ordered_json& j, const char* pKey, OUT T& outValue)
	{
		auto it = j.find(pKey);
		if (it != j.end() && !it->is_null())
			outValue = it->get<T>();
	}
}
#pragma endregion

NS_BEGIN(Engine)

void to_json(ordered_json& j, const CAMERA_SHOT_KEY_1D& t)
{
	j = ordered_json{
		{"Time",  t.fTime},
		{"Value", t.fValue},
		{"Ease",  t.eEase}
	};

	// editor 전용이지만 저장해도 무방합니다.
	if (t.iEditorUID != 0)
		j["EditorUID"] = t.iEditorUID;
}

void from_json(const ordered_json& j, CAMERA_SHOT_KEY_1D& t)
{
	t = {};
	t.fTime = j.value("Time", 0.f);
	t.fValue = j.value("Value", 0.f);
	t.eEase = j.value("Ease", Engine::ECameraShotEase::SmoothStep);
	t.iEditorUID = j.value("EditorUID", 0u);
}

void to_json(ordered_json& j, const CAMERA_SHOT_CHANNEL_1D& t)
{
	j = ordered_json{
		{"Keys", t.vecKeys}
	};
}

void from_json(const ordered_json& j, CAMERA_SHOT_CHANNEL_1D& t)
{
	t = {};
	ReadIfExists(j, "Keys", t.vecKeys);
}

void to_json(ordered_json& j, const CAMERA_SHOT_START_DESC& t)
{
	j = ordered_json{
		{"Mode", t.eMode},
		{"LocalOffset", MakeJsonVec3(t.vLocaloffset)},
		{"ApplyStartPoseImmediately", t.bApplyStartPoseImmediately}
	};
}

void from_json(const ordered_json& j, CAMERA_SHOT_START_DESC& t)
{
	t = {};
	t.eMode = j.value("Mode", Engine::ECameraShotStartMode::FixedFromPivot);

	auto itLocal = j.find("LocalOffset");
	if (itLocal != j.end() && itLocal->is_object())
		ReadJsonVec3(*itLocal, t.vLocaloffset);

	t.bApplyStartPoseImmediately = j.value("ApplyStartPoseImmediately", true);
}

void to_json(ordered_json& j, const CAMERA_SHOT_RECOVER_DESC& t)
{
	j = ordered_json{
		{"Target",    t.eTarget},
		{"Method",    t.eMethod},
		{"BlendTime", t.fBlendTime},
		{"Ease",      t.eEase}
	};
}

void from_json(const ordered_json& j, CAMERA_SHOT_RECOVER_DESC& t)
{
	t = {};
	t.eTarget = j.value("Target", Engine::ECameraShotRecoverTarget::GameplaySolved);
	t.eMethod = j.value("Method", Engine::ECameraShotRecoverMethod::Blend);
	t.fBlendTime = j.value("BlendTime", 0.6f);
	t.eEase = j.value("Ease", Engine::ECameraShotEase::EaseInOutQuad);
}

void to_json(ordered_json& j, const SCRIPTED_PIVOT_SHOT_DESC& t)
{
	j = ordered_json{
		{"Duration",         t.fDuration},
		{"FollowLivePivot",  t.bFollowLivePivot},
		{"FollowLiveLookAt", t.bFollowLiveLookAt},
		{"LookAtTarget",     t.bLookAtTarget},
		{"BasisMode",        t.eBasisMode},

		{"PivotOffsetX", t.PivotOffsetX},
		{"PivotOffsetY", t.PivotOffsetY},
		{"PivotOffsetZ", t.PivotOffsetZ},

		{"LookAtOffsetX", t.LookAtOffsetX},
		{"LookAtOffsetY", t.LookAtOffsetY},
		{"LookAtOffsetZ", t.LookAtOffsetZ},

		{"LocalX", t.LocalX},
		{"LocalY", t.LocalY},
		{"LocalZ", t.LocalZ},

		{"OrbitYawDeg", t.OrbitYawDeg}
	};
}

void from_json(const ordered_json& j, SCRIPTED_PIVOT_SHOT_DESC& t)
{
	t = {};
	t.fDuration = j.value("Duration", 0.7f);
	t.bFollowLivePivot = j.value("FollowLivePivot", true);
	t.bFollowLiveLookAt = j.value("FollowLiveLookAt", true);
	t.bLookAtTarget = j.value("LookAtTarget", true);
	t.eBasisMode = j.value("BasisMode", Engine::ECameraBasisMode::TARGET_TRANSFORM_YAW);

	ReadIfExists(j, "PivotOffsetX", t.PivotOffsetX);
	ReadIfExists(j, "PivotOffsetY", t.PivotOffsetY);
	ReadIfExists(j, "PivotOffsetZ", t.PivotOffsetZ);

	ReadIfExists(j, "LookAtOffsetX", t.LookAtOffsetX);
	ReadIfExists(j, "LookAtOffsetY", t.LookAtOffsetY);
	ReadIfExists(j, "LookAtOffsetZ", t.LookAtOffsetZ);

	ReadIfExists(j, "LocalX", t.LocalX);
	ReadIfExists(j, "LocalY", t.LocalY);
	ReadIfExists(j, "LocalZ", t.LocalZ);

	ReadIfExists(j, "OrbitYawDeg", t.OrbitYawDeg);
}

void to_json(ordered_json& j, const SCRIPTED_CONTROLLER_LAYER_DESC& t)
{
	j = ordered_json{
		{"FovDeltaDeg", t.FovDeltaDeg},

		{"RotYawDeg",   t.RotYawDeg},
		{"RotPitchDeg", t.RotPitchDeg},
		{"RotRollDeg",  t.RotRollDeg},

		{"LocalPosX", t.LocalPosX},
		{"LocalPosY", t.LocalPosY},
		{"LocalPosZ", t.LocalPosZ}
	};
}

void from_json(const ordered_json& j, SCRIPTED_CONTROLLER_LAYER_DESC& t)
{
	t = {};
	ReadIfExists(j, "FovDeltaDeg", t.FovDeltaDeg);

	ReadIfExists(j, "RotYawDeg", t.RotYawDeg);
	ReadIfExists(j, "RotPitchDeg", t.RotPitchDeg);
	ReadIfExists(j, "RotRollDeg", t.RotRollDeg);

	ReadIfExists(j, "LocalPosX", t.LocalPosX);
	ReadIfExists(j, "LocalPosY", t.LocalPosY);
	ReadIfExists(j, "LocalPosZ", t.LocalPosZ);
}

void to_json(ordered_json& j, const SCRIPTED_CAMERA_SHOT_BINDING_DESC& t)
{
	auto MakeBindTargetJson = [](const auto& target) -> ordered_json
		{
			return ordered_json{
				{"Source",     target.eSource},
				{"Resolve",    target.eResolve},
				{"PartIndex",  target.iPartIndex},
				{"AnchorTag",  target.strAnchorTag},
				{"LocalOffset", MakeJsonVec3(target.vLocalOffset)}
			};
		};

	j = ordered_json{
		{"Pivot",            MakeBindTargetJson(t.Pivot)},
		{"UseSeparateLookAt", t.bUseSeparateLookAt},
		{"LookAt",           MakeBindTargetJson(t.LookAt)}
	};
}

void from_json(const ordered_json& j, SCRIPTED_CAMERA_SHOT_BINDING_DESC& t)
{
	t = {};

	auto ReadBindTarget = [](const ordered_json& jTarget, OUT auto& outTarget)
		{
			outTarget = {};
			outTarget.eSource = jTarget.value("Source", Engine::ECameraAnchorSource::ACTOR);
			outTarget.eResolve = jTarget.value("Resolve", Engine::ECameraAnchorResolve::CAM_SOCKET);
			outTarget.iPartIndex = jTarget.value("PartIndex", 0u);
			outTarget.pObject = nullptr;

			const std::string strTag = jTarget.value("AnchorTag", std::string{});
			outTarget.strAnchorTag = strTag;

			auto itLocal = jTarget.find("LocalOffset");
			if (itLocal != jTarget.end() && itLocal->is_object())
				ReadJsonVec3(*itLocal, outTarget.vLocalOffset);
		};

	auto itPivot = j.find("Pivot");
	if (itPivot != j.end() && itPivot->is_object())
		ReadBindTarget(*itPivot, t.Pivot);

	t.bUseSeparateLookAt = j.value("UseSeparateLookAt", false);

	auto itLookAt = j.find("LookAt");
	if (itLookAt != j.end() && itLookAt->is_object())
		ReadBindTarget(*itLookAt, t.LookAt);
}

void to_json(ordered_json& j, const SCRIPTED_CAMERA_SHOT_DESC& t)
{
	j = ordered_json{
		{"Name",               t.strName},
		{"Start",              t.Start},
		{"Pivot",              t.Pivot},
		{"Controller",         t.Controller},
		{"Recover",            t.Recover},
		{"ClientStartAction",  t.iClientStartAction},
		{"ClientFinishAction", t.iClientFinishAction}
	};
}

void from_json(const ordered_json& j, SCRIPTED_CAMERA_SHOT_DESC& t)
{
	t = {};
	t.strName = j.value("Name", std::string{});

	ReadIfExists(j, "Start", t.Start);
	ReadIfExists(j, "Pivot", t.Pivot);
	ReadIfExists(j, "Controller", t.Controller);
	ReadIfExists(j, "Recover", t.Recover);

	t.iClientStartAction = j.value("ClientStartAction", 0u);
	t.iClientFinishAction = j.value("ClientFinishAction", 0u);
}
NS_END

NS_BEGIN(Client)
void to_json(ordered_json& j, const CAMERA_SHOT_PRESET& t)
{
	j = ordered_json{
		{"Version",   1},
		{"PresetTag", t.strPresetTag},
		{"ShotDesc",  t.tShotDesc},
		{"Binding",   t.tBinding}
	};
}

void from_json(const ordered_json& j, CAMERA_SHOT_PRESET& t)
{
	t = {};
	t.strPresetTag = j.value("PresetTag", std::string{});

	ReadIfExists(j, "ShotDesc", t.tShotDesc);
	ReadIfExists(j, "Binding", t.tBinding);
}
NS_END
#pragma endregion

IMPLEMENT_SINGLETON(CCameraPreset_Manager)

CCameraPreset_Manager::CCameraPreset_Manager()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CCameraPreset_Manager::Initialize(const std::filesystem::path& rootDir)
{
	m_rootPath = rootDir;
	std::filesystem::create_directories(m_rootPath);

	return Load_AllPresets();
}

HRESULT CCameraPreset_Manager::Load_AllPresets()
{
	m_mapPresets.clear();

	std::error_code ec;
	if (!std::filesystem::exists(m_rootPath, ec))
	{
		std::filesystem::create_directories(m_rootPath, ec);
		return S_OK;
	}

	std::vector<std::filesystem::path> vecFiles;
	for (const auto& entry : std::filesystem::directory_iterator(m_rootPath, ec))
	{
		if (!entry.is_regular_file())
			continue;

		if (entry.path().extension() == ".json")
			vecFiles.push_back(entry.path());
	}

	std::sort(vecFiles.begin(), vecFiles.end());

	for (const auto& path : vecFiles)
	{
		if (FAILED(Load_PresetFile(path)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CCameraPreset_Manager::Load_PresetFile(const std::filesystem::path& loadFilePath)
{
	try
	{
		ordered_json j;
		if (FAILED(ReadJsonFile(loadFilePath, j)))
			return E_FAIL;

		CAMERA_SHOT_PRESET tDoc = j.get<CAMERA_SHOT_PRESET>();

		if (tDoc.strPresetTag.empty())
			tDoc.strPresetTag = loadFilePath.stem().string();

		if (tDoc.tShotDesc.strName.empty())
			tDoc.tShotDesc.strName = tDoc.strPresetTag;

		m_mapPresets[tDoc.strPresetTag] = std::move(tDoc);
		return S_OK;
	}
	catch (...)
	{
		return E_FAIL;
	}
}

HRESULT CCameraPreset_Manager::Save_PresetFile(const CAMERA_SHOT_PRESET& preset, const std::filesystem::path& saveFilePath)
{
	try
	{
		CAMERA_SHOT_PRESET tSaveDoc = preset;

		if (tSaveDoc.strPresetTag.empty())
			tSaveDoc.strPresetTag = saveFilePath.stem().string();

		if (tSaveDoc.tShotDesc.strName.empty())
			tSaveDoc.tShotDesc.strName = tSaveDoc.strPresetTag;

		std::error_code ec;
		std::filesystem::create_directories(saveFilePath.parent_path(), ec);

		ordered_json j = tSaveDoc;

		if (FAILED(WriteJsonFile(saveFilePath, j)))
			return E_FAIL;

		m_mapPresets[tSaveDoc.strPresetTag] = std::move(tSaveDoc);
		return S_OK;
	}
	catch (...)
	{
		return E_FAIL;
	}
}

HRESULT CCameraPreset_Manager::Save_PresetFileAs(const CAMERA_SHOT_PRESET& preset, const std::filesystem::path& saveFilePath)
{
	try
	{
		CAMERA_SHOT_PRESET tSaveDoc = preset;

		if (tSaveDoc.strPresetTag.empty())
			tSaveDoc.strPresetTag = saveFilePath.stem().string();

		if (tSaveDoc.tShotDesc.strName.empty())
			tSaveDoc.tShotDesc.strName = tSaveDoc.strPresetTag;

		std::error_code ec;
		std::filesystem::create_directories(saveFilePath.parent_path(), ec);

		ordered_json j = tSaveDoc;

		if (FAILED(WriteJsonFile(saveFilePath, j)))
			return E_FAIL;

		m_mapPresets[tSaveDoc.strPresetTag] = std::move(tSaveDoc);
		return S_OK;
	}
	catch (...)
	{
		return E_FAIL;
	}
}

HRESULT CCameraPreset_Manager::Register_Preset(const CAMERA_SHOT_PRESET& preset)
{
	string strTag = preset.strPresetTag;

	if (strTag.empty())
		strTag = preset.tShotDesc.strName;

	if (strTag.empty())
		return E_FAIL;

	CAMERA_SHOT_PRESET tCopy = preset;
	tCopy.strPresetTag = strTag;

	m_mapPresets[strTag] = std::move(tCopy);
	return S_OK;
}

HRESULT CCameraPreset_Manager::Unregister_Preset(const string& strPresetTag)
{
	if (strPresetTag.empty())
		return E_FAIL;

	size_t iErased = m_mapPresets.erase(strPresetTag);
	return iErased > 0 ? S_OK : E_FAIL;
}

const CAMERA_SHOT_PRESET* CCameraPreset_Manager::Find_Preset(const string& strPresetTag) const
{
	auto it = m_mapPresets.find(strPresetTag);
	if (it == m_mapPresets.end())
		return nullptr;

	return &it->second;
}

HRESULT CCameraPreset_Manager::Copy_Preset(const string& strPresetTag, OUT CAMERA_SHOT_PRESET& outPreset) const
{
	const CAMERA_SHOT_PRESET* pFound = Find_Preset(strPresetTag);
	if (pFound == nullptr)
		return E_FAIL;

	outPreset = *pFound;
	return S_OK;
}

vector<string> CCameraPreset_Manager::Get_PresetTags() const
{
	std::vector<std::string> vecTags;
	vecTags.reserve(m_mapPresets.size());

	for (const auto& pair : m_mapPresets)
		vecTags.push_back(pair.first);

	std::sort(vecTags.begin(), vecTags.end());
	return vecTags;
}

void CCameraPreset_Manager::Clear()
{
	m_mapPresets.clear();
}

std::filesystem::path CCameraPreset_Manager::Make_FilePathFromTag(const string& strTag) const
{
	return m_rootPath / (strTag + ".json");
}

void CCameraPreset_Manager::Free()
{
	m_mapPresets.clear();
	Safe_Release(m_pGameInstance);
	Super::Free();
}
