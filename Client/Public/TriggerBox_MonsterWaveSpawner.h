#pragma once
#include "TriggerBox.h"
#include "DataStruct_Map.h"

NS_BEGIN(Client)

class CTriggerBox_MonsterWaveSpawner final : public CTriggerBox
{
	using Super = CTriggerBox;
public:
	typedef struct tagMonsterSpawner_Desc : public CTriggerBox::TRIGGERBOX_DESC
	{
		TRIGGERBOX_MONSTERWAVESPAWNER_DESC		tWaveData{};
	}MONSTERWAVESPAWNER_DESC;
protected:
	CTriggerBox_MonsterWaveSpawner(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CTriggerBox_MonsterWaveSpawner(const CTriggerBox_MonsterWaveSpawner& rhs);
	virtual ~CTriggerBox_MonsterWaveSpawner() = default;
public:
	virtual HRESULT			Initialize_Prototype()							override;
	virtual HRESULT			Initialize(void* pArg)							override;
	HRESULT					Ready_Component(MONSTERWAVESPAWNER_DESC* pDesc);
	HRESULT					Ready_SpawnPool(MONSTERWAVESPAWNER_DESC* pDesc);
	HRESULT					Register_Pool(_uint iLevelId, _uint iFindPrototypeIndex, DTO::EMakeMonsterType eMakeMonsterType, _int numPool);
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
	virtual void			OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)override;
	virtual void			OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;

public:
	virtual void			QuestEnter() override;
	virtual void			QuestExit() override;

public:
	HRESULT					SpawnMonster(MonsterWaveInfo& waveInfo);
	TRIGGERBOX_MONSTERWAVESPAWNER_DESC* GetWaveData() { return &m_tWaveData; };

	void					Update_WaveLoop(_float fTimeDelta);
	void					Update_WaveAllKill(_float fTimeDelta);
	void					Update_WaveTimer(_float fTimeDelta);

protected:
	TRIGGERBOX_MONSTERWAVESPAWNER_DESC	m_tWaveData{};

	_bool m_bIsAction = { false };

public:
	static CTriggerBox_MonsterWaveSpawner* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual	CGameObject* Clone(void* pArg)override;
	virtual	void					Free()override;
};

NS_END