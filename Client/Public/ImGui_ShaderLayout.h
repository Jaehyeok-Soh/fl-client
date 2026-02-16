#pragma once
#include "ImGui_Layout.h"

NS_BEGIN(Client)

class CImGui_ShaderLayout final : public CImGui_Layout
{
	using Super = CImGui_Layout;
private:
	CImGui_ShaderLayout();
	virtual ~CImGui_ShaderLayout() = default;
public:
	virtual _bool Can_Render(CGameObject* pGo) override { return true; }
	virtual void Render(CGameObject* pGo) override;
private:
	_bool m_bAutoApply{ true };
	_bool m_bDefaultCached{ false };
	SHADER_SSAOPARAM_DESC  m_defSSAO{};
	SHADER_HDRPARAM_DESC   m_defHDR{};
	SHADER_BLOOMPARAM_DESC m_defBloom{};
	SHADER_OUTLINE_DESC    m_defOutline{};
public:
	static CImGui_ShaderLayout* Create();
	virtual void Free() override;
};

NS_END