#pragma once
#include "ImGui_Layout.h"

NS_BEGIN(Client)

class CImGui_MoveStateLayout final : public CImGui_Layout
{
	using Super = CImGui_Layout;
private:
	CImGui_MoveStateLayout();
	virtual ~CImGui_MoveStateLayout() = default;
public:
	virtual _bool Can_Render(CGameObject* pGo) override;
	virtual void Render(CGameObject* pGo) override;
private:
	_bool m_bInit{ false };

public:
	static CImGui_MoveStateLayout* Create();
	virtual void Free() override;
};

NS_END