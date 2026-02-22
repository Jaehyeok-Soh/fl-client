#pragma once
#include "ImGui_Panel.h"

NS_BEGIN(Tool)

class CPanel_PresetMain final : public CImGui_Panel
{
	using Super = CImGui_Panel;
private:
	CPanel_PresetMain(CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CPanel_PresetMain() = default;
private:
	HRESULT	Initialize();
public:
	virtual HRESULT Render(CToolObject* pGo) override;
	virtual void Update(const _float fTimeDelta) override;
public:
	static			CPanel_PresetMain* Create(CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void	Free() override;
};

NS_END