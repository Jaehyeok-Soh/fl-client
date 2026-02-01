#include "pch.h"
#include "Grid.h"
#include "GameInstance.h"
#include "Shader.h"
#include "VIBuffer_Line_Color.h"

CGrid::CGrid(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext), m_vColor{ 1.f,1.f,1.f,1.f }, m_iMaxLineCount{}
{
}

CGrid::CGrid(const CGrid& rhs)
	: CGameObject(rhs), m_vColor(rhs.m_vColor), m_iMaxLineCount{rhs.m_iMaxLineCount}
{
}

HRESULT CGrid::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	CGrid::GRID_DESC* pDesc = static_cast<CGrid::GRID_DESC*>(pArg);

	m_vColor = pDesc->vColor;

	if (FAILED(Ready_Buffer()))
		return E_FAIL;


	return S_OK;
}

HRESULT CGrid::Ready_Buffer()
{
	CVIBuffer_Line_Color::VIBUFFER_LINE_ORIGIN_DESC tDesc{};
	tDesc.vColor = m_vColor;
	tDesc.iMaxLineCount = m_iMaxLineCount;


	m_pBuffer =  CVIBuffer_Line_Color::Create(m_pDevice,m_pDeviceContext,&tDesc);
	if (!m_pBuffer)
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component<CShader>(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Shader_VtxCol", nullptr)))
		return E_FAIL;

	return S_OK;
}

void CGrid::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CGrid::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CGrid::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CGrid::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}


HRESULT CGrid::Render()
{
	if (m_pBuffer == nullptr) return E_FAIL;
	CShader* pShader = CGameObject::Get_Component<CShader>();
	if (pShader == nullptr) return E_FAIL;

	pShader->Bind_TransformData(Get_Component<CTransform>()->Get_WorldMatrix());

	pShader->Apply();
	m_pBuffer->Bind_Resource();
	m_pBuffer->Render();


	return S_OK;
}



HRESULT CGrid::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;



	return S_OK;
}

HRESULT CGrid::Awake(const _uint iCurrentLevelID)
{
	return S_OK;
}



CGrid* CGrid::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGrid* pGrid = new CGrid(pDevice,pContext);

	if (FAILED(pGrid->Initialize_Prototype()))
	{
		Safe_Release(pGrid);
		MSG_BOX("Grid is Failed to Create");
		return nullptr;
	}

	return pGrid;
}



CGameObject* CGrid::Clone(void* pArg)
{
	CGrid* pGrid = new CGrid(*this);

	if (FAILED(pGrid->Initialize(pArg)))
	{
		Safe_Release(pGrid);
		MSG_BOX("Grid is Failed to Clone");
		return nullptr;
	}




	return pGrid;
}



void CGrid::Free()
{
	Super::Free();


	Safe_Release(m_pBuffer);

}
