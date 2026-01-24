#include "StaticModel.h"
#include "GameInstance.h"
#include "Mesh.h"
#include "Model.h"

CStaticModel::CStaticModel(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CMapObject(eType, pDevice,pDeviceContext)
{
	m_eMapObjectType = EMapObject_Type::STATICMODEL;
}

CStaticModel::CStaticModel(const CStaticModel& rhs)
	: CMapObject(rhs)
{

}

HRESULT CStaticModel::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CStaticModel::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	STATICMODEL_DESC* pDesc = static_cast<STATICMODEL_DESC*>(pArg);

	if (FAILED(CStaticModel::Ready_Component()))
		return E_FAIL;

	return S_OK;
}

HRESULT CStaticModel::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;


	return S_OK;
}

void CStaticModel::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}


void CStaticModel::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

}

void CStaticModel::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);

}

void CStaticModel::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CStaticModel::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;


	return S_OK;
}

void CStaticModel::Draw_ImGui()
{
	Super::Draw_ImGui();
}

void CStaticModel::Set_Dead(const wstring& wstrLayerTag)
{

}

bool CStaticModel::IntsersectWithPlane(OUT Vec3& vOut)
{
	CModel* pModel = Get_Component<CModel>();
	if (pModel == nullptr)  return false;

	_uint iMeshCount = pModel->Get_MeshCount();
	for (_uint i = 0; i < iMeshCount; ++i)
	{
		if (pModel->Get_Mesh(i)->IntsersectWithPlane(vOut))
			return true;
	}
	return false;
}

_bool CStaticModel::Picking(OUT Vec3& vOut)
{
	const Matrix& matWorld = Get_Component<CTransform>()->Get_WorldMatrix();
	Matrix matLocal = {};

	matLocal = matWorld.Invert();
	m_pGameInstance->TransformRayToLocalSpace(matLocal);

	if (IntsersectWithPlane(vOut))
	{
		vOut = Vec3::Transform(vOut, matWorld);
		return true;
	}
	return false;
}

HRESULT CStaticModel::Export_Data(OUT MAPOBJECT_SAVEDATA& data)
{
	return S_OK;
}

CStaticModel* CStaticModel::Create(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CStaticModel* pStaticModel = new CStaticModel(eType,pDevice,pDeviceContext);

	if (FAILED(pStaticModel->Initialize_Prototype()))
	{
		Safe_Release(pStaticModel);
		MSG_BOX(" Static Model Is Failed To Create ");
		return nullptr;
	}
	return pStaticModel;
}

CGameObject* CStaticModel::Clone(void* pArg)
{
	CStaticModel* pStaticModel = new CStaticModel(*this);

	if (FAILED(pStaticModel->Initialize(pArg)))
	{
		Safe_Release(pStaticModel);
		MSG_BOX(" Static Model Is Failed To Create ");
		return nullptr;
	}

	return pStaticModel;
}


void CStaticModel::Free()
{
	Super::Free();
}
