#pragma once
#include "SkillBase.h"

NS_BEGIN(Client)
class CSkillComp_MoonE final : public CSkillBase
{
	using Super = CSkillBase;

private:
	CSkillComp_MoonE();
	virtual ~CSkillComp_MoonE() = default;

	virtual HRESULT Initialize(void* pArg) override;
public:

	virtual void	Update(const _float fTimeDelta) override;

public:
	virtual _bool Start_Skill(CMyStat* pStatCom = nullptr)override;
	virtual void End_Skill(CMyStat* pStatCom = nullptr)override;

	virtual _bool On_Collision(const _float fTimeDelta, CGameObject* pObj = nullptr)override;

private:
	virtual void Update_Skill(const _float fTimeDelta)override;

public:
	static CSkillComp_MoonE* Create(void* pArg = nullptr);
	virtual void Free() override;
};

NS_END