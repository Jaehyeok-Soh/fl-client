#pragma once
#include "UITrigger.h"
#include "DataStruct_UI.h"

NS_BEGIN(Client)
class CCanvas;
class CGenericUI;
class CUIMenu_Trigger final : public CUITrigger
{
	using Super = CUITrigger;
public:
	typedef struct tagUIMenuTriggerDesc : public UI_TRIGGER_DESC
	{
	}UI_MENU_TRIGGER_DESC;


private:
	CUIMenu_Trigger(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUIMenu_Trigger(const CUIMenu_Trigger& rhs);
	virtual ~CUIMenu_Trigger() = default;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* pArg) override;

	HRESULT Attach_Personal_Info();

public:
	virtual HRESULT Awake(const _uint iCurrentLevelID) override;
	virtual void Update_Priority(const _float fTimeDelta) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual void Update_Late(const _float fTimeDelta) override;
	virtual void Ready_Before_Render(const _float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Fire_ToTargets(ETriggerEventType eEvent);

	virtual void OnUIEvent(ETriggerEventType eEvent, CGenericUI* pSender)override;
	virtual void Initialize_Interactable_Event()override;
	virtual void Initialize_NonInteractable_Event()override;
	virtual _bool Tick_Interactable_Event(const _float fTimeDelta)override;
	virtual _bool Tick_NonInteractable_Event(const _float fTimeDelta)override;

private:
	HRESULT Ready_Components(UI_TRIGGER_DESC* pDesc);
	HRESULT Bind_ShaderResources();

private:
	_float m_fDelayTimeAcc = {};

public:
	static CUIMenu_Trigger* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGameObject* Clone(void* pArg);
	virtual void Free()override;
};

NS_END