#include "pch.h"
#include "Grass.h"

CGrass::CGrass(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CPlants(pDevice ,pDeviceContext)
{
	m_ePlantsType = CPlants::Type::Grass;
}

CGrass::CGrass(const CGrass& rhs)
	: CPlants(rhs)
{
}


HRESULT CGrass::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGrass::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CGrass::Ready_Component(GRASS_DESC* pDesc)
{
	/* 생성된 Component 를 땔수있나 */


	return S_OK;
}

HRESULT CGrass::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	return S_OK;
}

void CGrass::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CGrass::Update(const _float fTimeDelta)
{

	Super::Update(fTimeDelta);
}

void CGrass::Update_Late(const _float fTimeelta)
{
	Super::Update_Late(fTimeelta);

}


void CGrass::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CGrass::Render()
{

	if (FAILED(Super::Render_Plnats(ENUM_TO_UINT(EMapObjectShaderPass::Grass))))
		return E_FAIL;



	return S_OK;
}

CGrass* CGrass::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CGrass* pGrass = new CGrass(pDevice, pDeviceContext);

	if (FAILED(pGrass->Initialize_Prototype()))
	{
		Safe_Release(pGrass);
		MSG_BOX("Grass is failed to Create");
		return nullptr;
	}
	return pGrass;
}

CGameObject* CGrass::Clone(void* pArg)
{
	CGrass* pGrass = new CGrass(*this);

	if (FAILED(pGrass->Initialize(pArg)))
	{
		Safe_Release(pGrass);
		MSG_BOX(" Grass is failed to Clone ");
		return nullptr;
	}

	return pGrass;
}

void CGrass::Free()
{
	Super::Free();
}
