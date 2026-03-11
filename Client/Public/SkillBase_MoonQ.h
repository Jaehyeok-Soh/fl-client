#pragma once
#include "SkillBase.h"

NS_BEGIN(Engine)
class CSingleSkillSpawner;
NS_END

NS_BEGIN(Client)
class CSkillBase_MoonQ final : public CSkillBase
{
	using Super = CSkillBase;

private:
	CSkillBase_MoonQ();
	virtual ~CSkillBase_MoonQ() = default;

	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual void	Awake(const _uint iCurLevelIndex) override;
	virtual void	Update_Default(const _float fTimeDelta, CMyStat* pStatCom = nullptr) override;
	virtual void	Update(const _float fTimeDelta, CMyStat* pStatCom = nullptr) override;

public:
	virtual void ClearSkillBase_WhenChangeLevel() override;

public:
	virtual _bool Start_Skill(CMyStat* pStatCom = nullptr)override;
	virtual void End_Skill(CMyStat* pStatCom = nullptr)override;

	virtual _bool On_Collision(const _float fTimeDelta, CGameObject* pObj = nullptr)override;

	virtual void Set_ExtraAttack_Desc(EXTRA_ATTACK_DESC& tStat_ExtraDesc, CMyStat* pOwnerStat) override;

private:
	CSingleSkillSpawner* m_pAttackSkill_ObjSpawner{ nullptr };

private:
	_float		m_fAddAttackRate = { 0.15f };

	_bool		m_bSkillAttackOn = { false };
	TimeCount	m_TAttackSkillObj_Timer = { 0.f, /*1.85*/1.95f };

private:
	virtual void	Update_Skill(const _float fTimeDelta, CMyStat* pStatCom = nullptr) override;

private:
	HRESULT			Ready_Spawner();
	void			Spawn_Attack_SkillObj(CMyStat* pOwnerStat);

public:
	static CSkillBase_MoonQ* Create(void* pArg = nullptr);
	virtual void Free() override;
};

NS_END