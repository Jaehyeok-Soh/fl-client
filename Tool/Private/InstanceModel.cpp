#include "pch.h"
#include "InstanceModel.h"
#include "Shader.h"
#include "Model.h"

CInstanceModel::CInstanceModel(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMapObject(eType,pDevice,pContext)
{

}

CInstanceModel::CInstanceModel(const CInstanceModel& rhs)
	: CMapObject(rhs)
{
}

HRESULT CInstanceModel::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CInstanceModel::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;


	if (FAILED(CInstanceModel::Ready_Component()))
		return E_FAIL;

	return S_OK;
}

HRESULT CInstanceModel::Ready_Component()
{
	return E_FAIL;
}

HRESULT CInstanceModel::Awake(const _uint iCurrentLevelID)
{
	/* Instance Model 위치 값이 들어갈 예정 */



	return S_OK;
}

void CInstanceModel::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CInstanceModel::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CInstanceModel::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CInstanceModel::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);

}

HRESULT CInstanceModel::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	CShader* pShader			= CGameObject::Get_Component<CShader>();
	CModel* pModel				= CGameObject::Get_Component<CModel>();
	CTransform* pTransform		= CGameObject::Get_Component<CTransform>();

	if (pShader == nullptr || pModel == nullptr || pTransform == nullptr ) return E_FAIL;

	return S_OK;
}

void CInstanceModel::Draw_ImGui()
{
}

CInstanceModel* CInstanceModel::Create(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CInstanceModel* pInstanceModel = new CInstanceModel(eType, pDevice, pContext);

	if (FAILED(pInstanceModel->Initialize_Prototype()))
	{
		Safe_Release(pInstanceModel);
		MSG_BOX(" Instance Model is Failed To Create ");
		return nullptr;
	}

	return pInstanceModel;
}


CGameObject* CInstanceModel::Clone(void* pArg)
{
	CInstanceModel* pInstanceModel = new CInstanceModel(*this);

	if (FAILED(pInstanceModel->Initialize(pArg)))
	{
		Safe_Release(pInstanceModel);
		MSG_BOX("Instance Model Is Failed To Clone");
		return nullptr;
	}

	return pInstanceModel;
}

void CInstanceModel::Free()
{


	Super::Free();
}

_bool CInstanceModel::Picking(OUT Vec3& vOut)
{
	return false;
}
