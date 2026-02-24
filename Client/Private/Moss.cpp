#include "pch.h"
#include "Moss.h"



CMoss::CMoss(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CPlants(pDevice ,pDeviceContext)
{
}

CMoss::CMoss(const CMoss& rhs)
	: CPlants(rhs)
{
}


HRESULT CMoss::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMoss::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMoss::Ready_Component(MOSS_DESC* pDesc)
{

	return S_OK;
}

HRESULT CMoss::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	return S_OK;
}

void CMoss::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CMoss::Update(const _float fTimeDelta)
{

	Super::Update(fTimeDelta);
}

void CMoss::Update_Late(const _float fTimeelta)
{
	Super::Update_Late(fTimeelta);

}


void CMoss::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CMoss::Render()
{
	return S_OK;
}

CMoss* CMoss::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CMoss* pMoss = new CMoss(pDevice, pDeviceContext);

	if (FAILED(pMoss->Initialize_Prototype()))
	{
		Safe_Release(pMoss);
		MSG_BOX("Moss is failed to Create");
		return nullptr;
	}
	return pMoss;
}

CGameObject* CMoss::Clone(void* pArg)
{
	CMoss* pMoss = new CMoss(*this);

	if (FAILED(pMoss->Initialize(pArg)))
	{
		Safe_Release(pMoss);
		MSG_BOX(" Moss is failed to Clone ");
		return nullptr;
	}

	return pMoss;
}

void CMoss::Free()
{
	Super::Free();
}
