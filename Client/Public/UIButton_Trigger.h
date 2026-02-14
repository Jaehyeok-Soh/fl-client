#pragma once
#include "GenericUI.h"
#include "DataStruct_UI.h"

NS_BEGIN(Client)
class CCanvas;
class CGenericUI;
class CUIButton_Trigger final : public CGenericUI
{
	using Super = CGenericUI;
public:
	typedef struct tagTriggerUIDesc : public GENERIC_UI_DESC
	{
		DTO::EUISubClassType eOwner;
		DTO::TUI_TriggerData tTriggerData;
	}UI_TRIGGER_DESC;

private:
	CUIButton_Trigger(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUIButton_Trigger(const CUIButton_Trigger& rhs);
	virtual ~CUIButton_Trigger() = default;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* pArg) override;

	HRESULT Attach_Personal_Info();
	HRESULT Bind_Cache();

public:
	virtual HRESULT Awake(const _uint iCurrentLevelID) override;
	virtual void Update_Priority(const _float fTimeDelta) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual void Update_Late(const _float fTimeDelta) override;
	virtual void Ready_Before_Render(const _float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Components(UI_TRIGGER_DESC* pDesc);
	HRESULT Bind_ShaderResources();

	vector<CCanvas*> m_pTriggerCanvas;
	vector<CGenericUI*> m_pTriggerUI;

private:
	DTO::EUISubClassType m_eSubClassType = {};
	DTO::TUI_TriggerData m_tTriggerData = {};

public:
	static CUIButton_Trigger* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGameObject* Clone(void* pArg);
	virtual void Free()override;
};

NS_END