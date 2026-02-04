#pragma once
#include "ImGui_Layout.h"

NS_BEGIN(Client)

class CImGui_TransformLayout final : public CImGui_Layout
{
	using Super = CImGui_Layout;
private:
	CImGui_TransformLayout();
	virtual ~CImGui_TransformLayout() = default;
public:
	virtual _bool Can_Render(CGameObject* pGo) override;
	virtual void Render(CGameObject* pGo) override;
public:
	static CImGui_TransformLayout* Create();
	virtual void Free() override;
};

NS_END