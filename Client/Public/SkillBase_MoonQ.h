#pragma once
#include "SkillBase.h"

NS_BEGIN(Client)
class CSkillBase_MoonQ final : public CSkillBase
{
	using Super = CSkillBase;

private:
	CSkillBase_MoonQ();
	virtual ~CSkillBase_MoonQ() = default;

	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual void Update(const _float fTimeDelta, CMyStat* pStatCom = nullptr) override;

public:
	virtual _bool Start_Skill(CMyStat* pStatCom = nullptr)override;
	virtual void End_Skill(CMyStat* pStatCom = nullptr)override;

	virtual _bool On_Collision(const _float fTimeDelta, CGameObject* pObj = nullptr)override;

	virtual void Set_ExtraAttack_Desc(EXTRA_ATTACK_DESC& tStat_ExtraDesc, CMyStat* pOwnerStat) override;

private:
	_float m_fAddAttackRate = { 0.15f };

private:
	virtual void Update_Skill(const _float fTimeDelta)override;

public:
	static CSkillBase_MoonQ* Create(void* pArg = nullptr);
	virtual void Free() override;
};

NS_END