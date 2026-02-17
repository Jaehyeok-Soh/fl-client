#pragma once
#include "UIDynamic_Image.h"
#include "DataStruct_UI.h"

NS_BEGIN(Client)
class CUIMenu_OutLine final : public  CUIDynamic_Image
{
	using Super = CUIDynamic_Image;
public:
	typedef struct tagUIMenuOutLine : public DIMAGE_DESC
	{
	}MENU_OUTLINE_DESC;

private:
	CUIMenu_OutLine(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUIMenu_OutLine(const CUIMenu_OutLine& rhs);
	virtual ~CUIMenu_OutLine() = default;
public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Awake(const _uint iCurrentLevelID)			override;
	virtual void	Update_Priority(const _float fTimeDelta)	override;
	virtual void	Update(const _float fTimeDelta)				override;
	virtual void	Update_Late(const _float fTimeDelta)		override;
	virtual void	Ready_Before_Render(const _float fTimeDelta)override;
	virtual HRESULT Render()									override;

private:
	HRESULT Ready_Components(MENU_OUTLINE_DESC* pDesc);
	HRESULT Bind_ShaderResources();

	virtual void OnUIEvent(ETriggerEventType eEvent, CGenericUI* pSender)override;
	virtual void Initialize_Visible_Event()override;
	virtual void Initialize_InVisible_Event()override;
	virtual _bool Tick_Visible_Event(const _float fTimeDelta)override;
	virtual _bool Tick_InVisible_Event(const _float fTimeDelta)override;

	void Emit_Light(const _float fTimeDelta );

private:
	_bool m_isTrigger_HoverEnter	= { false };
	_bool m_isTrigger_HoverExit		= { false };

	_float m_fEmit = { 4.f };

public:
	static CUIMenu_OutLine* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGameObject* Clone(void* pArg);
	virtual void Free()override;
};

NS_END