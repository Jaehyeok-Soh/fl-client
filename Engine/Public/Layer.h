#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CGameObject;

class CLayer final : public CBase
{
	using Super = CBase;
public:
	enum class ETimeDomain : _uint
	{
		Unscaled = 0,
		Scaled
	};
private:
	CLayer();
	virtual ~CLayer() = default;
public:
	_float Get_TimeScale() const { return m_fTimeScale; }
	void Set_TimeScale(_float fScale) { m_fTimeScale = fScale; }
	_bool Is_ScaledDomain() const {return m_eTimeDomain == ETimeDomain::Scaled; }
	void Set_ScaledDomain() { m_eTimeDomain = ETimeDomain::Scaled; }
	void Set_UnscaledDomain() { m_eTimeDomain = ETimeDomain::Unscaled; }
	HRESULT Awake(const _uint iCurrentLevelID);
	void Update_Priority(const _float fTimeDelta);
	void Update(const _float fTimeDelta);
	void Update_Late(const _float fTimeDelta);
	void Ready_Before_Render(const _float fTimeDelta);
	CGameObject* Get_GameObject(_uint iIndex);
	void Delete_GameObject(class CGameObject* pGo);
	HRESULT Add_GameObject(class CGameObject* pGO);
	CGameObject* Get_GameObject_Front();
	CGameObject* Get_GameObject_Back();
	list<CGameObject*>* Get_GameObject_List() { return &m_pGameObjects; }
private:
	CGameObject* Find_GameObject(_uint iIndex);
private:
	ETimeDomain m_eTimeDomain{ ETimeDomain::Scaled };
	_float m_fTimeScale{ 1.f };
	list<CGameObject*> m_pGameObjects;
public:
	static CLayer* Create();
	virtual void Free() override;
};

NS_END