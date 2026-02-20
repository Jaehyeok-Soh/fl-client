#pragma once
#include "State_SkillBase.h"

NS_BEGIN(Client)
class CState_MoonSkill final : public CState_SkillBase
{
	using Super = CState_SkillBase;

private:
	CState_MoonSkill(CActionState* pOwnerComponent, const string& strName);
	virtual ~CState_MoonSkill() = default;

	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual HRESULT Awake(const _uint iLevelIndex) override;
	virtual HRESULT Start(void* pArg, _bool bForce = false) override;
	virtual void	Update(const _float fTimeDelta) override;
	virtual HRESULT End() override;

private:
	void SkillE_Update(const _float fTimeDelta);
	void SkillQ_Update(const _float fTimeDelta);
	
public:
	static CState_MoonSkill* Create(CActionState* pOwnerComponent, const string& strName, void* pArg = nullptr);
	virtual void Free() override;
};

NS_END