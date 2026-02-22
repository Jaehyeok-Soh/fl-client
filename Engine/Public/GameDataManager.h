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

public:
	static ID3D11ShaderResourceView* Make_ShaderResourceViewColor(_uint A, _uint R, _uint G, _uint B,ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
public:
	////////////////////
	/// AttackPreset ///
	////////////////////
	const DTO::TAttackPreset_Data* Find_AttackPrseet(_uint iPresetKey) const;
	const DTO::TAttackPreset_Data* Find_AttackPresetByTag(const string& strTag) const;
	HRESULT Upsert_AttackPresetData(const DTO::TAttackPreset_Data& inData);
	const unordered_map<_uint, DTO::TAttackPreset_Data>& Get_AttackPresetsData_ForDebug() const { return m_umapAttackPresetDatas; }
public:
	void Clear_AttackPreset();
private:

private:
	class CGameInstance *m_pGameInstance = { nullptr };
	////////////////////
	/// AttackPreset ///
	////////////////////
	_bool m_bAttackPresetLoaded{ false };
	unordered_map<_uint, DTO::TAttackPreset_Data> m_umapAttackPresetDatas;
	unordered_map<string, _uint> m_umapAttackPresetTagToKey;
public:
	static CGameDataManager* Create();
	virtual void Free() override;
};

NS_END