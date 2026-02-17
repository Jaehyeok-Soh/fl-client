#pragma once
#include "GenericUI.h"
#include "DataStruct_UI.h"

NS_BEGIN(Client)
class CCanvas;
class CStatComponent;
class CUIJust_Image final : public CGenericUI
{
	using Super = CGenericUI;
public:
	typedef struct tagUIJustImageDesc : public GENERIC_UI_DESC
	{

	}JUST_IMAGE_DESC;

private:
	CUIJust_Image(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUIJust_Image(const CUIJust_Image& rhs);
	virtual ~CUIJust_Image() = default;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* pArg) override;

public:
	virtual HRESULT Awake(const _uint iCurrentLevelID) override;
	virtual void Update_Priority(const _float fTimeDelta) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual void Update_Late(const _float fTimeDelta) override;
	virtual void Ready_Before_Render(const _float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void OnUIEvent(ETriggerEventType eEvent, CGenericUI* pSender)override;

	virtual void Initialize_Visible_Event()override;
	virtual void Initialize_InVisible_Event()override;
	virtual _bool Tick_Visible_Event(const _float fTimeDelta)override;
	virtual _bool Tick_InVisible_Event(const _float fTimeDelta)override;

private:
	HRESULT Ready_Components(JUST_IMAGE_DESC* pDesc);
	HRESULT Bind_ShaderResources();

private:
	_float m_fTimeAcc = {};
	_float m_fOriginAlpha = {};


public:
	static CUIJust_Image* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGameObject* Clone(void* pArg);
	virtual void Free()override;
};

NS_END