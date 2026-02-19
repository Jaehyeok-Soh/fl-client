#pragma once
#include "SkillComponent.h"

NS_BEGIN(Client)
class CSkillComp_MoonE final : public CSkillComponent
{
	using Super = CSkillComponent;

public:
	typedef struct tagSkillComponentDesc
	{

	}SKILLCOMP_DESC;

private:
	CSkillComp_MoonE();
	explicit CSkillComp_MoonE(const CSkillComp_MoonE& rhs);
	virtual ~CSkillComp_MoonE() = default;

	virtual HRESULT Initialize_Prototype() override;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Update(const _float fTimeDelta) override;

public:
	virtual void Start_Skill(CStatComponent* pStatCom = nullptr);
	virtual void Update_Skill(const _float fTimeDelta);
	virtual void End_Skill(CStatComponent* pStatCom = nullptr);
	virtual void On_Collision_Monster(const _float fTimeDelta, CGameObject* pObj = nullptr);

public:
	static CSkillComp_MoonE* Create();
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END