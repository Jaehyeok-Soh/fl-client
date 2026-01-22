#pragma once
#include "ImGui_ToolManager.h"
#include "Base.h"

NS_BEGIN(Tool)

class CToolObject;

class CImGui_Base abstract : public CBase
{
	using Super = CBase;
protected:
	CImGui_Base(const _char* pLabel, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CImGui_Base() = default;
public:
	virtual void Update(const _float fTimeDelta);
	virtual HRESULT Render(CToolObject* pGo) PURE;
public:
	const string& Get_Label() const { return m_strLabel; }
protected:
	string m_strLabel = { "" };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pDeviceContext = { nullptr };
public:
	virtual void Free() override;
};

NS_END