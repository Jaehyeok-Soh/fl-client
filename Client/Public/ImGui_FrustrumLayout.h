#pragma once
#include "ImGui_Layout.h"

NS_BEGIN(Client)

class CImGui_FrustrumLayout final : public CImGui_Layout
{
	using Super = CImGui_Layout;
private:
	CImGui_FrustrumLayout();
	virtual ~CImGui_FrustrumLayout() = default;
public:
	virtual _bool Can_Render(CGameObject* pGo) override;
	virtual void Render(CGameObject* pGo) override;
public:
	static CImGui_FrustrumLayout* Create();
	virtual void Free() override;
};

NS_END