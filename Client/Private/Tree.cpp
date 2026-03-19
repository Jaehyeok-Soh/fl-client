#include "pch.h"
#include "Tree.h"



CTree::CTree(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CPlants(pDevice ,pDeviceContext)
{
	m_ePlantsType = CPlants::Type::Tree;
	m_bBakedShadow = true;
}

CTree::CTree(const CTree& rhs)
	: CPlants(rhs)
{
	m_bBakedShadow = true;
}


HRESULT CTree::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTree::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTree::Ready_Component(TREE_DESC* pDesc)
{

	return S_OK;
}

HRESULT CTree::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	return S_OK;
}

void CTree::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CTree::Update(const _float fTimeDelta)
{

	Super::Update(fTimeDelta);
}

void CTree::Update_Late(const _float fTimeelta)
{
	Super::Update_Late(fTimeelta);

}


void CTree::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CTree::Render()
{
	if (FAILED(Super::Render_Plnats(ENUM_TO_UINT(EMapObjectShaderPass::Tree))))
		return E_FAIL;

	return S_OK;
}

CTree* CTree::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CTree* pTree = new CTree(pDevice, pDeviceContext);

	if (FAILED(pTree->Initialize_Prototype()))
	{
		Safe_Release(pTree);
		MSG_BOX("Tree is failed to Create");
		return nullptr;
	}
	return pTree;
}

CGameObject* CTree::Clone(void* pArg)
{
	CTree* pTree = new CTree(*this);

	if (FAILED(pTree->Initialize(pArg)))
	{
		Safe_Release(pTree);
		MSG_BOX(" Tree is failed to Clone ");
		return nullptr;
	}

	return pTree;
}

void CTree::Free()
{
	Super::Free();
}
