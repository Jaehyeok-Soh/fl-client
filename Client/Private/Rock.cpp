#include "pch.h"
#include "Rock.h"



CRock::CRock(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CMapObject(pDevice ,pDeviceContext)
{
}

CRock::CRock(const CRock& rhs)
	: CMapObject(rhs)
{
}


HRESULT CRock::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRock::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRock::Ready_Component(ROCK_DESC* pDesc)
{

	return S_OK;
}

HRESULT CRock::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	return S_OK;
}

void CRock::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CRock::Update(const _float fTimeDelta)
{

	Super::Update(fTimeDelta);
}

void CRock::Update_Late(const _float fTimeelta)
{
	Super::Update_Late(fTimeelta);

}


void CRock::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CRock::Render()
{
	return S_OK;
}

CRock* CRock::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CRock* pRock = new CRock(pDevice, pDeviceContext);

	if (FAILED(pRock->Initialize_Prototype()))
	{
		Safe_Release(pRock);
		MSG_BOX("Rock is failed to Create");
		return nullptr;
	}
	return pRock;
}

CGameObject* CRock::Clone(void* pArg)
{
	CRock* pRock = new CRock(*this);

	if (FAILED(pRock->Initialize(pArg)))
	{
		Safe_Release(pRock);
		MSG_BOX(" Rock is failed to Clone ");
		return nullptr;
	}

	return pRock;
}

void CRock::Free()
{
	Super::Free();
}
