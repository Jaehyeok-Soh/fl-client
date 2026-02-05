#pragma once
#include "ImGuiBase.h"

NS_BEGIN(Engine)
class CGameObject;
NS_END

NS_BEGIN(Client)

class CImGui_Layout abstract : public CImGuiBase
{
	using Super = CImGuiBase;
protected:
	CImGui_Layout(std::string_view label);
	virtual ~CImGui_Layout() = default;
public:
	virtual _bool Can_Render(CGameObject* pGo) PURE;
	virtual void Render(CGameObject* pGo) PURE;
public:
	virtual void Free() override;
};

NS_END