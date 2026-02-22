#pragma once
#include "ImGui_Panel.h"

NS_BEGIN(Tool)

class CPanel_PresetList final : public CImGui_Panel
{
	using Super = CImGui_Panel;
private:
	CPanel_PresetList(CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CPanel_PresetList() = default;

private:
	HRESULT	Initialize();

public:
	virtual void Update(const _float fTimeDelta) override;
	virtual HRESULT Render(CToolObject* pGo) override;
public:
	static			CPanel_PresetList* Create(CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void	Free() override;
};

NS_END