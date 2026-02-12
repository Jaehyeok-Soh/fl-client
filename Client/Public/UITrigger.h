#pragma once
#include "GenericUI.h"
#include "DataStruct_UI.h"

NS_BEGIN(Client)
class CCanvas;
class CGenericUI;
class CUITrigger final : public CGenericUI
{
	using Super = CGenericUI;
public:
	typedef struct tagTriggerUIDesc : public GENERIC_UI_DESC
	{
		DTO::EUISubClassType eOwner;
		DTO::TUI_TriggerData tTriggerData;
	}UI_TRIGGER_DESC;

	enum class ETriggerEventType { HOVER_ENTER, HOVER_EXIT, PRESS_ENTER, PRESS_EXIT, END };

private:
	CUITrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUITrigger(const CUITrigger& rhs);
	virtual ~CUITrigger() = default;

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

	array<vector<CCanvas*>, ENUM_TO_UINT(ETriggerEventType::END)> m_pTriggerCanvas;
	array<vector<CGenericUI*>, ENUM_TO_UINT(ETriggerEventType::END)> m_pTriggerUI;

private:
	DTO::EUISubClassType m_eSubClassType = {};
	DTO::TUI_TriggerData m_tTriggerData = {};

public:
	static CUITrigger* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGameObject* Clone(void* pArg);
	virtual void Free()override;
};

NS_END