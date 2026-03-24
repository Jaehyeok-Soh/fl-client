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

private:
	_float m_fFloat1 = { 0.f };
	_float m_fFloat2 = { 0.f };
	_float m_fFloat3 = { 0.f };

private:
	void Render_Float(CGameObject* pGo);

public:
	static CImGui_StateLayout* Create();
	virtual void Free() override;
};

NS_END;