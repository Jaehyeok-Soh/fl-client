#pragma once
#include "GenericUI.h"
#include "DataStruct_UI.h"

NS_BEGIN(Client)
class CStatComponent;
class CUIText abstract : public CGenericUI
{
	using Super = CGenericUI;
public:
	typedef struct tagUITextDesc : public GENERIC_UI_DESC
	{
		DTO::EUITextSubClassType eTextSubClass;
		_wstring wstrFontTag;
		_wstring wstrText;
		Vec4 vFontColor;
		_float fScale;
		_float fRotate;

	}UI_TEXT_DESC;

protected:
	CUIText(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUIText(const CUIText& rhs);
	virtual ~CUIText() = default;

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

protected:
	HRESULT Ready_Components(UI_TEXT_DESC* pDesc);
	HRESULT Bind_ShaderResources();

protected:
	DTO::EUITextSubClassType m_eTextSubClassType = {};
	_wstring m_wstrText		= {};
	_wstring m_wstrFontTag	= {};
	Vec2 m_vFontPos			= {};
	Vec4 m_vFontColor		= {};
	Vec4 m_vOriginFontColor = {};
	_float m_fFontScale		= {};
	_float m_fFontRotate	= {};

	_float m_fTimeAcc = {};
public:
	virtual void Free()override;
};

NS_END