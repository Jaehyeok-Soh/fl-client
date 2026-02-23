#include "pch.h"
#include "WorldUI.h"

CWorldUI::CWorldUI(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CWorldUI::CWorldUI(const CWorldUI& rhs)
	:CGameObject(rhs)
{
}

HRESULT CWorldUI::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CWorldUI::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CWorldUI::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

void CWorldUI::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CWorldUI::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CWorldUI::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CWorldUI::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CWorldUI::Render()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CWorldUI::Ready_Components(WORLDUI_DESC* pDesc)
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

CWorldUI* CWorldUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CWorldUI* pInstance = new CWorldUI(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CWorldUI::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CWorldUI::Clone(void* pArg)
{
	CWorldUI* pInstance = new CWorldUI(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CWorldUI::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CWorldUI::Free()
{
	Super::Free();
}
