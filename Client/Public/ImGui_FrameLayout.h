#pragma once
#include "ImGui_Layout.h"

NS_BEGIN(Client)

class CImGui_FrameLayout final : public CImGui_Layout
{
	using Super = CImGui_Layout;
private:
	CImGui_FrameLayout();
	virtual ~CImGui_FrameLayout() = default;
public:
	virtual _bool Can_Render(CGameObject* pGo) override { return true; }
	virtual void Render(CGameObject* pGo) override;
private:
	_float m_fAvgFrame{ 0.f };
	_float m_fMinFrame{ 0.f };
public:
	static CImGui_FrameLayout* Create();
	virtual void Free() override;
};

NS_END