#pragma once
#include "UIObject.h"

NS_BEGIN(Client)

class CGenericUI final : public CUIObject
{
	using Super = CUIObject;

public:
	typedef struct tagGenericUIDesc : public UIOBJECT_DESC
	{
		uint32_t iUIType;
		uint32_t iRectTransformType;
		_wstring wstrTextureTag;

	}GENERIC_UI_DESC;

private:
	CGenericUI(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGenericUI(const CGenericUI& rhs);
	virtual ~CGenericUI() = default;

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

private:
	HRESULT Ready_Components(GENERIC_UI_DESC* pDesc);
	HRESULT Bind_ShaderResources();

public:
	static CGenericUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGameObject* Clone(void* pArg);
	virtual void Free()override;

};

NS_END