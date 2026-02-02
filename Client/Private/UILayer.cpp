#include "pch.h"
#include "UILayer.h"
#include "Client_Defines.h"

//=================
// Component
//=================
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"

#include "GenericUI.h"

#include "GameInstance.h"

CUILayer::CUILayer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIObject(pDevice, pDeviceContext)
{
}

CUILayer::CUILayer(const CUILayer& rhs)
	:CUIObject(rhs)
{
}

HRESULT CUILayer::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUILayer::Initialize(void* pArg)
{
	UILAYER_DESC* pDesc = static_cast<UILAYER_DESC*>(pArg);

	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUILayer::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;
	return S_OK;
}

void CUILayer::Transmit_for_UI()
{
	for (auto* pUI : m_vecUIs)
	{
		if (m_isVisible) pUI->Set_Visible();
		else pUI->Set_Invisible();
	}
}

void CUILayer::Update_Priority(const _float fTimeDelta)
{
	Transmit_for_UI();
	Super::Update_Priority(fTimeDelta);
}

void CUILayer::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CUILayer::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUILayer::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CUILayer::Render()
{
	return S_OK;
}

HRESULT CUILayer::Ready_Components(UILAYER_DESC* pDesc)
{
	return S_OK;
}

HRESULT CUILayer::Bind_ShaderResources()
{
	return S_OK;
}

CUILayer* CUILayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUILayer* pInstance = new CUILayer(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUILayer::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUILayer::Clone(void* pArg)
{
	CUILayer* pInstance = new CUILayer(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUILayer::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUILayer::Free()
{
	Super::Free();
}
