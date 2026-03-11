#pragma once
#include "GimmikController.h"

NS_BEGIN(Engine)
class CSingleSkillSpawner;
class CProjectileSpawner_Fan;
class CProjectileSpawner_Radial360;
class CSkillObjectSpawner_RandomXZ;
NS_END

NS_BEGIN(Client)

class CXibi_GateSpawner;

class CXibi_GimmikController final : public CGimmikController
{
	using Super = CGimmikController;
public:
	enum class EGimmikType
	{
		TeleportRandom_Disappear,
		TeleportRandom_Appear,
		TeleportCenter_Disappear,
		TeleportCenter_Appear,
		SpawnThunderRandom,
		SpawnThunder3way,
		Spawn360Circle,
		Spawn360Thunder,
		Spawn8Gate,
		COUNT
	};
private:
	CXibi_GimmikController();
	CXibi_GimmikController(const CXibi_GimmikController& rhs);
	virtual ~CXibi_GimmikController() = default;

	virtual HRESULT						Initialize_Prototype() override;
	virtual HRESULT						Initialize(void* pArg) override; 
public:
	HRESULT Awake(const _uint iCurLevelIndex);
	void Update(const _float fTimeDelta);
public:
	virtual HRESULT Bind_Events() override;
private:
	//TODO - Åø·Î »©±â
	HRESULT Set_Event();
	void On_TeleportRandom_Disappear();
	void On_TeleportRandom_Appear();
	void On_TeleportCenter_Disappear();
	void On_TeleportCenter_Appear();
	virtual void On_ModelAnimNotify(const AnimNotifyKey& key) override;
private:
	void Teleport_To(const Vec3& vPos);
	Vec3 Compute_RandomTeleportPosition(const Vec3& vCurrentPos);
	void SpawnSingleThunder();
	void On_SpawnThunderRandom();
	void On_SpawnThunder3way();
	void On_SpawnCircle360();
	void On_SpawnThunder360();
	void On_Spawn8Gate();
private:
	HRESULT Ready_Spawner();
private:
	// TODO - BattleField·Î »©±â
	Vec3 m_vSpawnPosition{Vec3::Zero};
	_float m_fFieldMaxRange{ 5.f };
	_float m_fMinTargetDist = { 1.3f };
	_float m_fRandomTeleportRange{ 3.f };

	CSingleSkillSpawner* m_pOneshotThunderSpawner{ nullptr };
	CSkillObjectSpawner_RandomXZ* m_pRandomThunderSpawner{ nullptr };
	CProjectileSpawner_Fan* m_p3wayThunderSpawner{ nullptr };
	CProjectileSpawner_Radial360* m_p360CircleSpawner{ nullptr };
	CProjectileSpawner_Radial360* m_p360ThunderSpawner{ nullptr };
	CXibi_GateSpawner* m_pGateSpawner{ nullptr };
public:
	static CXibi_GimmikController* Create();
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END