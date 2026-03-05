#pragma once
#include "ImGui_Base.h"

NS_BEGIN(Tool)

class CToolObject;

class CImGui_Dockspace_TabBar : public CImGui_Base
{
	using Super = CImGui_Base;
private:
	CImGui_Dockspace_TabBar(const _char* pLabel, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, ELevelType eStartLevel);
	virtual ~CImGui_Dockspace_TabBar() = default;
public:
	virtual HRESULT Render(CToolObject* pGo = nullptr);
private:
	HRESULT Change_Scene(ELevelType eLevel);
private:
	ELevelType	m_eLastSelectedTab = { ELevelType::END };
	_bool		m_isNo = false;

public:
	static CImGui_Dockspace_TabBar* Create(const _char* pLabel, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, ELevelType eStartLevel);
	virtual void Free() override;
};

NS_END