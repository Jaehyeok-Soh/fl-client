#pragma once
#include "SkillBase.h"

NS_BEGIN(Engine)
class CSingleSkillSpawner;
NS_END

NS_BEGIN(Client)
class CSkillBase_MoonE final : public CSkillBase
{
	using Super = CSkillBase;

private:
	CSkillBase_MoonE();
	virtual ~CSkillBase_MoonE() = default;

	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual void	Awake(const _uint iCurLevelIndex) override;
	virtual void	Update_Default(const _float fTimeDelta, CMyStat* pStatCom = nullptr) override;
	virtual void	Update(const _float fTimeDelta, CMyStat* pStatCom = nullptr) override;

public:
	virtual _bool Start_Skill(CMyStat* pStatCom = nullptr)override;
	virtual void End_Skill(CMyStat* pStatCom = nullptr)override;

	virtual _bool On_Collision(const _float fTimeDelta, CGameObject* pObj = nullptr)override;

	virtual void Set_ExtraAttack_Desc(EXTRA_ATTACK_DESC& tStat_ExtraDesc, CMyStat* pOwnerStat) override;

private:
	CSingleSkillSpawner* m_pSkillObjSpawner{ nullptr };

	_bool m_bSpawn_Second = { false };
	_bool m_bSpawn_First = { false };
	_float m_fAccTime = { 0.f };

private:
	virtual void Update_Skill(const _float fTimeDelta, CMyStat* pStatCom)override;

private:
	HRESULT Ready_Spawner();
	void	Spawn_SkillObj(CMyStat* pOwnerStat, _bool bFirst);

public:
	static CSkillBase_MoonE* Create(void* pArg = nullptr);
	virtual void Free() override;
};

NS_END