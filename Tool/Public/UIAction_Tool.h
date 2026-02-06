#pragma once
#include "IUIActionForMe.h"

NS_BEGIN(Tool)
class CToolUI;
class CUIAction_Tool final : public IUIActionForMe
{
	using Super = IUIActionForMe;
private:
	CUIAction_Tool(CToolUI* pUI);
	virtual ~CUIAction_Tool() = default;
	
public:
	void Set_Visible(bool isVisible) override;
	void Set_TextureIndex(_uint index) override;
	const _string& Get_Tag() const override;
	void Start_Lerp_Movement(const Vec3& vTargetPos, const _float fTargetAlpha, const _float& fDuration, _bool isPin) override;


private:
	CToolUI* m_pOwner = { nullptr };

public:
	static CUIAction_Tool* Create(CToolUI* pUI);
	virtual void Free()override;
};

NS_END