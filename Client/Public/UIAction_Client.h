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
	void Set_Visible(bool isVisible) override;
	void Set_TextureIndex(_uint index) override;
	const _string& Get_Tag() const override;

private:
	CGenericUI* m_pOwner = { nullptr };

public:
	static CUIAction_Client* Create(CGenericUI* pUI);
	virtual void Free()override;
};

NS_END