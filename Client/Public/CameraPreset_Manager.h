#pragma once
#include "Base.h"
#include "json_forward.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)

class CCameraPreset_Manager final : public CBase
{
	using Super = CBase;
	DECLARE_SINGLETON(CCameraPreset_Manager)
private:
	CCameraPreset_Manager();
	virtual ~CCameraPreset_Manager() = default;
public:
	HRESULT Initialize(const std::filesystem::path& rootDir);

	const std::filesystem::path& Get_RootDirectory() const { return m_rootPath; }

	HRESULT Load_AllPresets();
	HRESULT Load_PresetFile(const std::filesystem::path& loadFilePath);
	HRESULT Save_PresetFile(const CAMERA_SHOT_PRESET& preset, const std::filesystem::path& saveFilePath);
	HRESULT Save_PresetFileAs(const CAMERA_SHOT_PRESET& preset, const std::filesystem::path& saveFilePath);

	HRESULT Register_Preset(const CAMERA_SHOT_PRESET& preset);
	HRESULT Unregister_Preset(const string& strPresetTag);

	const CAMERA_SHOT_PRESET* Find_Preset(const string& strPresetTag) const;
	HRESULT Copy_Preset(const string& strPresetTag, OUT CAMERA_SHOT_PRESET& outPreset) const;
	vector<string> Get_PresetTags() const;
	const unordered_map<string, CAMERA_SHOT_PRESET>& Get_AllPresets() const { return m_mapPresets; }

	void Clear();
private:
	std::filesystem::path Make_FilePathFromTag(const string& strTag) const;
private:
	std::filesystem::path m_rootPath;
	CGameInstance* m_pGameInstance = { nullptr };
	unordered_map<string, CAMERA_SHOT_PRESET> m_mapPresets;
public:
	virtual void Free() override;
};

NS_END