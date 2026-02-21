#pragma once
#include "SkillComponent.h"

NS_BEGIN(Client)
class CSkillComp_MoonQ final : public CSkillComponent
{
	using Super = CSkillComponent;

private:
	CSkillComp_MoonQ();
	explicit CSkillComp_MoonQ(const CSkillComp_MoonQ& rhs);
	virtual ~CSkillComp_MoonQ() = default;

	virtual HRESULT Initialize_Prototype() override;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Update(const _float fTimeDelta) override;

public:
	virtual void Start_Skill(CStatComponent* pStatCom = nullptr);
	virtual void Update_Skill(const _float fTimeDelta);
	virtual void End_Skill(CStatComponent* pStatCom = nullptr);
	virtual void On_Collision_Monster(const _float fTimeDelta, CGameObject* pObj = nullptr);

private:
	_float m_fAddAttackRate = { 0.15f };

public:
	static CSkillComp_MoonQ* Create();
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END