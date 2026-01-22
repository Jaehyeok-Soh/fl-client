#pragma once
#include "ImGui_Panel.h"

NS_BEGIN(Tool)

class CUI_Inspector final : public CImGui_Panel
{
	using Super = CImGui_Panel;
private:
	explicit CUI_Inspector(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CUI_Inspector() = default;

public:	
	HRESULT Initialize_Prototype();

	virtual void Update(const _float fTimeDelta)override;
	virtual HRESULT Render(CToolObject* pGo) override;

public:
	static CUI_Inspector* Create(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

NS_END