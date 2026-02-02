#pragma once
#include "ImGui_Panel.h"

NS_BEGIN(Tool)
class CToolUI;
class CImGui_UIManager;
class CUIComponent_Inspector final : public CImGui_Panel
{
	using Super = CImGui_Panel;

private:
	explicit CUIComponent_Inspector(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CUIComponent_Inspector() = default;

public:
	HRESULT Initialize_Prototype();

	virtual void Update(const _float fTimeDelta)override;
	virtual HRESULT Render(CToolObject* pGo) override;

	void Edit_ImageComponent();
	void Edit_ButtonComponent();

private:
	CImGui_UIManager* m_pUIManager = { nullptr };
	CToolUI* m_pSelectedUI = { nullptr };
	ImGuiWindowFlags m_Flag = {};

public:
	static CUIComponent_Inspector* Create(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

NS_END