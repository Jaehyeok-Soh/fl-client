#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CGameObject;

class CLayer final : public CBase
{
	using Super = CBase;
private:
	CLayer();
	virtual ~CLayer() = default;
public:
	HRESULT Awake(const _uint iCurrentLevelID);
	void Update_Priority(const _float fTimeDelta);
	void Update(const _float fTimeDelta);
	void Update_Late(const _float fTimeDelta);
	void Ready_Before_Render(const _float fTimeDelta);
	CGameObject* Get_GameObject(CGameObject* pGo);
	void Delete_GameObject(class CGameObject* pGo);
	CGameObject* Add_GameObject(class CGameObject* pGO);
	CGameObject* Get_GameObject_Front();
	CGameObject* Get_GameObject_Back();
	list<CGameObject*>* Get_GameObject_List() { return &m_pGameObjects; }
private:
	CGameObject* Find_GameObject(CGameObject* pGo);
private:
	list<CGameObject*> m_pGameObjects;
public:
	static CLayer* Create();
	virtual void Free() override;
};

NS_END