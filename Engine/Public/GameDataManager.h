#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CGameDataManager final : public CBase
{
	using Super = CBase;
private:
	CGameDataManager();
	virtual ~CGameDataManager() = default;

	HRESULT Initialize();
public:
	const MODELPARTS_PROTOTYPETAGS& Get_ModelPartsData() const { return m_tModelPartsData; }
	void Set_ModelPartsData(const MODELPARTS_PROTOTYPETAGS& tData) { m_tModelPartsData = tData; }
	HRESULT Add_Presets(const wstring& wstrFilePath);
	HRESULT Add_Previews(const wstring& wstrFilePath);
	const EFFECT_PRESET_SNAPSHOT& Get_PresetSnapShot(_uint iPresetID);
	const EFFECT_PRESET_SNAPSHOT& Get_PresetSnapShot(const string& strTag);
	const MAPOBJECT_SAVEDATA* Get_Preview(const wstring& wstrGroupTag, const string& strNameTag);
	const vector<MAPOBJECT_SAVEDATA> *Get_Previews(const wstring& wstrTag);
private:
	HRESULT Regist_PresetID(const string& strTag, _uint iID);
	void Regist_Preset(_uint iID, const EFFECT_PRESET_SNAPSHOT& data);
	_int Get_PresetID(const string& strTag);
private:
	void ModelPartsData_Clear();
	void Preset_Clear();
	void Preview_Clear();
private:
	MODELPARTS_PROTOTYPETAGS m_tModelPartsData = {};
	unordered_map<string, _uint> m_umapPresetNameToID;
	unordered_map<_uint, EFFECT_PRESET_SNAPSHOT> m_umapPresets;
	unordered_map<wstring, vector<MAPOBJECT_SAVEDATA>> m_umapPreviews;
private:
	class CGameInstance *m_pGameInstance = { nullptr };
public:
	static CGameDataManager* Create();
	virtual void Free() override;
};

NS_END