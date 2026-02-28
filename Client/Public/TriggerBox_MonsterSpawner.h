#pragma once
#include "TriggerBox.h"
#include "DataStruct_Map.h"

NS_BEGIN(Client)

class CTriggerBox_MonsterSpawner : public CTriggerBox
{
	using Super = CTriggerBox;
public:
	typedef struct tagTriggerBox_MonsterSpawner_Desc : public CTriggerBox::TRIGGERBOX_DESC
	{
		vector<MonsterSpawnData>		vecMonsterSpawnData{};
	}TRIGGERBOX_MONSTERSPAWNER_DESC;
protected:
	CTriggerBox_MonsterSpawner(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CTriggerBox_MonsterSpawner(const CTriggerBox_MonsterSpawner& rhs);
	virtual ~CTriggerBox_MonsterSpawner() = default;
public:
	virtual HRESULT			Initialize_Prototype()							override;
	virtual HRESULT			Initialize(void* pArg)							override;
	HRESULT					Ready_Component(TRIGGERBOX_MONSTERSPAWNER_DESC* pDesc);
public:
	virtual HRESULT			Awake(const _uint iCurrentLevelID)				override;
	virtual void			Update_Priority(const _float fTimeDelta)		override;
	virtual void			Update(const _float fTimeDelta)					override;
	virtual void			Update_Late(const _float fTimeDelta)			override;
	virtual void			Ready_Before_Render(const _float fTimeDelta)	override;
	virtual HRESULT			Render()										override;

public:
	virtual void			OnCollision(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual void			OnCollision_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo) override;
	virtual void			OnCollision_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual void			OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)override;
	virtual void			OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
public:
	HRESULT					SpawnMonster();
protected:
	vector<MonsterSpawnData>		m_vecMonsterSpawnData{};

public:
	static CTriggerBox_MonsterSpawner*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual	CGameObject*			Clone(void* pArg)override;
	virtual	void					Free()override;
};

NS_END