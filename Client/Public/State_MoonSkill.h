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

public:
	virtual _uint	Get_Capabilities() const override;

protected:
	virtual _bool Can_Captablity_Move() const override;

private:
	_bool SkillE_Update(const _float fTimeDelta);
	_bool SkillQ_Update(const _float fTimeDelta);
	
public:
	static CState_MoonSkill* Create(CActionState* pOwnerComponent, const string& strName, void* pArg = nullptr);
	virtual void Free() override;
};

NS_END