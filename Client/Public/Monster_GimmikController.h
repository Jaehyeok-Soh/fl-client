#pragma once
#include "GimmikController.h"

NS_BEGIN(Engine)
class CSkillObjectSpawnerBase;
class CSingleSkillSpawner;
class CProjectileSpawner_Fan;
class CProjectileSpawner_Radial360;
class CSkillObjectSpawner_RandomXZ;
NS_END

NS_BEGIN(Client)

class CMonster_GimmikController :
    public CGimmikController
{
	using Super = CGimmikController;
private:
	CMonster_GimmikController();
	CMonster_GimmikController(const CMonster_GimmikController& rhs);
	virtual ~CMonster_GimmikController() = default;

	virtual HRESULT						Initialize_Prototype() override;
	virtual HRESULT						Initialize(void* pArg) override;

	virtual void On_ModelAnimNotify(const AnimNotifyKey& key) override {}

public:
	HRESULT Awake(const _uint iCurLevelIndex);
	void Update(const _float fTimeDelta);

	void AddSkillSpawner(CSkillObjectSpawnerBase* pSpawner);
	void SpawnTrigger(_uint iIndex);

private:
	vector<CSkillObjectSpawnerBase*> m_vecSpawner;

public:
	static CMonster_GimmikController* Create();
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END