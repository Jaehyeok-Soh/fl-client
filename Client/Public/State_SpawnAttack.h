#pragma once
#include "StateBase_Monster.h"

NS_BEGIN(Client)

class CLianhuo_GimmikController;

class CState_SpawnAttack : public CStateBase_Monster
{
	using Super = CStateBase_Monster;
private:
	CState_SpawnAttack(CActionState* pOwnerComponent, _uint iStateIndex);
	virtual ~CState_SpawnAttack() = default;

	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Awake(const _uint iLevelIndex) override;
	virtual HRESULT Start(void* pArg, _bool bForce = false) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual HRESULT End() override;
private:
	void Spawn_Skill();
private:
	CLianhuo_GimmikController* m_pOwnerGimmikController{ nullptr };
public:
	static CState_SpawnAttack* Create(CActionState* pOwnerComponent, _uint iStateIndex, void* pArg = nullptr);
	virtual void Free() override;
};

NS_END