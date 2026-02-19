#pragma once
#include "State_SkillBase.h"

NS_BEGIN(Client)
class CState_MoonESkill final : public CState_SkillBase
{
	using Super = CState_SkillBase;
public:
	typedef struct tagMoonESkillDesc
	{

	}MoonESkill_DESC;

private:
	CState_MoonESkill(CActionState* pOwnerComponent);
	virtual ~CState_MoonESkill() = default;

	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual HRESULT Awake(const _uint iLevelIndex) override;
	virtual HRESULT Start(void* pArg, _bool bForce = false) override;
	virtual void	Update(const _float fTimeDelta) override;
	virtual HRESULT End() override;

public:
	static CState_MoonESkill* Create(CActionState* pOwnerComponent, void* pArg = nullptr);
	virtual void Free() override;
};

NS_END