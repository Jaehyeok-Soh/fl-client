#pragma once
#include "ImGui_Base.h"

NS_BEGIN(Tool)

class CToolObject;

class CImGui_Dockspace_MenuBar final : public CImGui_Base
{
	using Super = CImGui_Base;
private:
	CImGui_Dockspace_MenuBar(const _char *pLabel, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CImGui_Dockspace_MenuBar() = default;
public:
	virtual HRESULT Render(CToolObject* pGo = nullptr);
private:
	void Open_FileDialog();
	void Save_FileDialog();
public:
	static CImGui_Dockspace_MenuBar* Create(const _char* pLabel, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

NS_END