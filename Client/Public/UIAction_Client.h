#pragma once
#include "IUIActionForMe.h"

NS_BEGIN(Client)
class CGenericUI;
class CUIAction_Client final : public IUIActionForMe
{
	using Super = IUIActionForMe;
private:
	CUIAction_Client(CGenericUI* pUI);
	virtual ~CUIAction_Client() = default;
	

public:
	void Set_Visible(bool isVisible, const _float fDelay) override;
	void Set_TextureIndex(_uint index, const _float fDelay) override;
	const _string& Get_Tag() const override;
	void Start_Lerp_Movement(const Vec3& vTargetPos, const _float fTargetAlpha, const _float& fDuration, _bool isPin, const _float fDelay) override;
	void Start_Return_Lerp_Movement(const _float fDelay) override;

	void Start_Fade(const _float fStartAlpha, const _float fTargetAlpha, const _float fDuration, const _float fDelay) override;

private:
	CGenericUI* m_pOwner = { nullptr };

public:
	static CUIAction_Client* Create(CGenericUI* pUI);
	virtual void Free()override;
};

NS_END