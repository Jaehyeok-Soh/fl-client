#pragma once
#include "State_SkillBase.h"

NS_BEGIN(Client)
class CState_MoonQSkill final : public CState_SkillBase
{
	using Super = CState_SkillBase;
public:
	typedef struct tagMoonQSkillDesc
	{

	}MoonQSkill_DESC;

private:
	CState_MoonQSkill(CActionState* pOwnerComponent);
	virtual ~CState_MoonQSkill() = default;

	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual HRESULT Awake(const _uint iLevelIndex) override;
	virtual HRESULT Start(void* pArg, _bool bForce = false) override;
	virtual void	Update(const _float fTimeDelta) override;
	virtual HRESULT End() override;

public:
	static CState_MoonQSkill* Create(CActionState* pOwnerComponent, void* pArg = nullptr);
	virtual void Free() override;
};

NS_END