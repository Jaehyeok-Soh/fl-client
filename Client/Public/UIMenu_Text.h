#pragma once
#include "UIText.h"
#include "DataStruct_UI.h"

NS_BEGIN(Client)
class CStatComponent;
class CUIMenu_Text final : public CUIText
{
	using Super = CUIText;
public:
	typedef struct tagUIMenuTextDesc : public UI_TEXT_DESC
	{
	}MENU_TEXT_DESC;

private:
	CUIMenu_Text(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUIMenu_Text(const CUIMenu_Text& rhs);
	virtual ~CUIMenu_Text() = default;

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

private:
	HRESULT Ready_Components(MENU_TEXT_DESC* pDesc);
	HRESULT Bind_ShaderResources();

private:
	virtual void OnUIEvent(ETriggerEventType eEvent, CGenericUI* pSender)override;

private:
	virtual void Initialize_Visible_Event()override;
	virtual void Initialize_InVisible_Event()override;

private:
	virtual _bool Tick_Visible_Event(const _float fTimeDelta)override;
	virtual _bool Tick_InVisible_Event(const _float fTimeDelta)override;

private:
	CStatComponent* m_pTargetStat = { nullptr };
public:
	static CUIMenu_Text* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGameObject* Clone(void* pArg);
	virtual void Free()override;
};

NS_END