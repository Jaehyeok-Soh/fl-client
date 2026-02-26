#pragma once
#include "ImGui_Layout.h"

NS_BEGIN(Client)

class CImGui_ShaderDebugLayout final : public CImGui_Layout
{
	using Super = CImGui_Layout;
private:
	CImGui_ShaderDebugLayout();
	virtual ~CImGui_ShaderDebugLayout() = default;

	HRESULT Initialize();
public:
	virtual _bool Can_Render(CGameObject* pGo) override { return true; }
	virtual void Render(CGameObject* pGo) override;
public:
	static CImGui_ShaderDebugLayout* Create();
	virtual void Free() override;
};

NS_END