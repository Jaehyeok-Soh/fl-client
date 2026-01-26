#pragma once
#include "ImGui_Panel.h"

NS_BEGIN(Tool)

class CPanel_MapDataController : public CImGui_Panel
{
	using Super = CImGui_Panel;
protected:
	explicit CPanel_MapDataController(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CPanel_MapDataController() = default;
public:
	virtual HRESULT Render(CToolObject* pGo)override;
	virtual void Update(const _float fTimeDelta)override;
private:
	const wstring m_wstrMapDatPath{L"../../Resources/Data/MapData/"};
public:
	static  CPanel_MapDataController* Create(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

NS_END

