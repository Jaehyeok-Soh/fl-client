#pragma once
#include "ImGui_Panel.h"

NS_BEGIN(Tool)
class CImGui_ToolManager;

class CUI_Hierachy final : public CImGui_Panel
{
	using Super = CImGui_Panel;

private:
	explicit CUI_Hierachy(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CUI_Hierachy() = default;

public:
	HRESULT Initialize_Prototype();

	virtual void Update(const _float fTimeDelta)override;
	virtual HRESULT Render(CToolObject* pGo) override;

private:
	CImGui_ToolManager* m_pToolManager = { nullptr };
	ImGuiWindowFlags m_Flag = {};

private:
	vector<_string> m_vecLayers;

public:
	static CUI_Hierachy* Create(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

NS_END