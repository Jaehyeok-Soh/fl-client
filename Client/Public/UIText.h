#pragma once
#include "GenericUI.h"
#include "DataStruct_UI.h"

NS_BEGIN(Client)
class CStatComponent;
class CUIText final : public CGenericUI
{
	using Super = CGenericUI;
public:
	typedef struct tagUITextDesc : public GENERIC_UI_DESC
	{
		CStatComponent* pTargetStat;
		DTO::EUISubClassType eOwner;
		_wstring wstrText;
		Vec4 vFontColor;

	}UI_TEXT_DESC;

private:
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

private:
	HRESULT Ready_Components(UI_TEXT_DESC* pDesc);
	HRESULT Bind_ShaderResources();

private:
	CStatComponent* m_pTargetStat = { nullptr };
	DTO::EUISubClassType m_eSubClassType = {};

	_wstring m_wstrText = {};
	Vec2 m_vFontPos = {};
	Vec4 m_vFontColor = {};

public:
	static CUIText* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGameObject* Clone(void* pArg);
	virtual void Free()override;
};

NS_END