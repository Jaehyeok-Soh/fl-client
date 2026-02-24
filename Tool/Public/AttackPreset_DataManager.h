#pragma once
#include "Base.h"

NS_BEGIN(Tool)

class CAttackPreset_DataManager final : public CBase
{
	DECLARE_SINGLETON(CAttackPreset_DataManager)
	using Super = CBase;
private:
	CAttackPreset_DataManager();
	virtual ~CAttackPreset_DataManager() = default;
public:
	const map<_uint, DTO::TAttackPreset_Data>& Get_AllDatas() const { return m_mapDatas; }
	HRESULT Upsert_Data(const DTO::TAttackPreset_Data &inData);
	_bool Is_TagUnique(const std::string& strTag, _uint iIgnoreKey = 0) const;
	HRESULT Create_Data(DTO::EAttackPresetCategory eCategory, const string& strTag, OUT _uint& iOutKey);
	DTO::TAttackPreset_Data* Find(_uint iKey);

	HRESULT Remove(_uint iKey);
	void Clear();
private:
	_ushort Get_NextID(DTO::EAttackPresetCategory eCategory);
	void Update_NextID(const DTO::TAttackPreset_Data& data);
private:
	map<_uint, DTO::TAttackPreset_Data> m_mapDatas;
	unordered_map<string, _uint> m_umapTagToKey;
	map<DTO::EAttackPresetCategory, _ushort> m_mapNextID;
public:
	virtual void Free() override;
};

NS_END