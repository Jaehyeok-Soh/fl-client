#pragma once
#include "ImGui_Panel.h"


NS_BEGIN(Tool)

class CPanel_MapObjectList : public CImGui_Panel
{
	using Super = CImGui_Panel;
protected:
	explicit CPanel_MapObjectList(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CPanel_MapObjectList() = default;
public:
	virtual HRESULT Render(CToolObject* pGo)override;
	virtual void Update(const _float fTimeDelta)override;

private:


public:
	static  CPanel_MapObjectList* Create(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

NS_END

