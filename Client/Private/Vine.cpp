#include "pch.h"
#include "Vine.h"



CVine::CVine(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CPlants(pDevice ,pDeviceContext)
{
}

CVine::CVine(const CVine& rhs)
	: CPlants(rhs)
{
}


HRESULT CVine::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CVine::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVine::Ready_Component(VINE_DESC* pDesc)
{

	return S_OK;
}

HRESULT CVine::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	return S_OK;
}

void CVine::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CVine::Update(const _float fTimeDelta)
{

	Super::Update(fTimeDelta);
}

void CVine::Update_Late(const _float fTimeelta)
{
	Super::Update_Late(fTimeelta);

}


void CVine::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CVine::Render()
{
	if (FAILED(Super::Render_Plnats(ENUM_TO_UINT(EMapObjectShaderPass::Vine))))
		return E_FAIL;


	return S_OK;
}

CVine* CVine::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CVine* pVine = new CVine(pDevice, pDeviceContext);

	if (FAILED(pVine->Initialize_Prototype()))
	{
		Safe_Release(pVine);
		MSG_BOX("Vine is failed to Create");
		return nullptr;
	}
	return pVine;
}

CGameObject* CVine::Clone(void* pArg)
{
	CVine* pVine = new CVine(*this);

	if (FAILED(pVine->Initialize(pArg)))
	{
		Safe_Release(pVine);
		MSG_BOX(" Vine is failed to Clone ");
		return nullptr;
	}

	return pVine;
}

void CVine::Free()
{
	Super::Free();
}
