#pragma once
#include "ImGui_Base.h"

NS_BEGIN(Tool)

class CToolObject;

class CImGui_Layout abstract : public CImGui_Base
{
	using Super = CImGui_Base;
protected:
	CImGui_Layout(const _char* pLabel, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CImGui_Layout() = default;
public:
	virtual HRESULT Render(CToolObject*pGo) PURE;
public:
	virtual void Free() override;	
};

NS_END