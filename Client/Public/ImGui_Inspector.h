#pragma once
#include "ImGuiBase.h"

NS_BEGIN(Engine)
class CGameObject;
NS_END

NS_BEGIN(Client)

class CImGui_Layout;

class CImGui_Inspector abstract : public CImGuiBase
{
	using Super = CImGuiBase;
protected:
	CImGui_Inspector(std::string_view label, _uint iLayoutCount);
	virtual ~CImGui_Inspector() = default;
public:
	virtual void Render(CGameObject* pGo) PURE;
protected:
	HRESULT Add_Layout(_uint iIndex, CImGui_Layout* pLayout);
	void Render_Layout(_uint iIndex, CGameObject* pGo);
protected:
	vector<CImGui_Layout*> m_vecLayouts;
public:
	virtual void Free() override;
};

NS_END