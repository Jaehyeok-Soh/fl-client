#pragma once
#include "ImGui_Layout.h"

NS_BEGIN(Tool)

class CToolObject;

class CImGui_Layout_Transform final : public CImGui_Layout
{
	using Super = CImGui_Layout;
private:
	CImGui_Layout_Transform(const _char* pLabel, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CImGui_Layout_Transform() = default;
public:
	virtual HRESULT Render(CToolObject* pGo) override;
private:
	CToolObject* m_pPrev = { nullptr };
public:
	static CImGui_Layout_Transform* Create(const _char* pLabel, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

NS_END