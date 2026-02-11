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

private:
	HRESULT Ready_Components(JUST_IMAGE_DESC* pDesc);
	HRESULT Bind_ShaderResources();

public:
	static CUIJust_Image* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGameObject* Clone(void* pArg);
	virtual void Free()override;
};

NS_END