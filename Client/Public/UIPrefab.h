#pragma once
#include "GenericUI.h"
#include "DataStruct_UI.h"

NS_BEGIN(Client)
class CUIPrefab abstract : public CGenericUI
{
	using Super = CGenericUI;
public:
	typedef struct tagPrefabMonsterNamplate : public GENERIC_UI_DESC
	{
	}PREFAB_MONSTER_NAMEPLATE_DESC;

protected:
	CUIPrefab(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUIPrefab(const CUIPrefab& rhs);
	virtual ~CUIPrefab() = default;
public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Awake(const _uint iCurrentLevelID) override;
	virtual void Update_Priority(const _float fTimeDelta) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual void Update_Late(const _float fTimeDelta) override;
	virtual void Ready_Before_Render(const _float fTimeDelta) override;
	virtual HRESULT Render() override;
public:
	void Add_Child(const _wstring& wstr) { m_vecPoolTag.push_back(wstr); }
protected:
	HRESULT Ready_Components(PREFAB_MONSTER_NAMEPLATE_DESC* pDesc);
	HRESULT Bind_ShaderResources();
protected:
	vector<_wstring> m_vecPoolTag;

public:
	virtual void Free()override;
};

NS_END