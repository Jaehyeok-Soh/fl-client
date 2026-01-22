#pragma once
#include "ImGui_Base.h"
#include "GameInstance.h"

NS_BEGIN(Engine)
class CLevel;
NS_END

NS_BEGIN(Tool)

class CImGui_Layout;
class CToolObject;

class CImGui_Panel abstract : public CImGui_Base
{
	using Super = CImGui_Base;
protected:
	explicit CImGui_Panel(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CImGui_Panel() = default;
public:
	virtual HRESULT Render(CToolObject* pGo) PURE;
public:
	HRESULT Add_Element(_uint _iType, CImGui_Layout* pElement);
	CImGui_Layout* Get_Layout(_uint _iType);
protected:
	CLevel* m_pOwnerLevel = { nullptr };
	vector<CImGui_Layout*> m_vecLayouts;
public:
	virtual void Free() override;
};

NS_END