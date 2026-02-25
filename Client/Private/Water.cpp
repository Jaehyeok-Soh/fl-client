#include "pch.h"
#include "Water.h"


CWater::CWater(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CMapObject(pDevice ,pDeviceContext)
{
}

CWater::CWater(const CWater& rhs)
	: CMapObject(rhs)
{
}


HRESULT CWater::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWater::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CWater::Ready_Component(WATER_DESC* pDesc)
{

	return S_OK;
}

HRESULT CWater::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	return S_OK;
}

void CWater::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CWater::Update(const _float fTimeDelta)
{

	Super::Update(fTimeDelta);
}

void CWater::Update_Late(const _float fTimeelta)
{
	Super::Update_Late(fTimeelta);

}


void CWater::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CWater::Render()
{
	if (m_eMapObjectDrawType == EMapObject_DrawType::Instance)
	{
		if (FAILED(CMapObject::Render_Instance(ENUM_TO_UINT(EMapObjectShaderPass::Water))))
			return E_FAIL;
	}
	else
	{
		if (FAILED(CMapObject::Render_Default(ENUM_TO_UINT(EMapObjectShaderPass::Water))))
			return E_FAIL;
	}
		

	return S_OK;
}

CWater* CWater::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CWater* pWater = new CWater(pDevice, pDeviceContext);

	if (FAILED(pWater->Initialize_Prototype()))
	{
		Safe_Release(pWater);
		MSG_BOX("Water is failed to Create");
		return nullptr;
	}
	return pWater;
}

CGameObject* CWater::Clone(void* pArg)
{
	CWater* pWater = new CWater(*this);

	if (FAILED(pWater->Initialize(pArg)))
	{
		Safe_Release(pWater);
		MSG_BOX(" Water is failed to Clone ");
		return nullptr;
	}

	return pWater;
}

void CWater::Free()
{
	Super::Free();
}
