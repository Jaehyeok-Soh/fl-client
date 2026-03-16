#pragma once
#include "ImGui_Layout.h"

NS_BEGIN(Client)

class CImGui_StateLayout final : public CImGui_Layout
{
	using Super = CImGui_Layout;
private:
	CImGui_StateLayout();
	virtual ~CImGui_StateLayout() = default;
public:
	virtual _bool Can_Render(CGameObject* pGo) override;
	virtual void Render(CGameObject* pGo) override;

public:
	static CImGui_StateLayout* Create();
	virtual void Free() override;
};

NS_END;