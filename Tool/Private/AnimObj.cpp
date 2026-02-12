#include "pch.h"
#include "AnimObj.h"

#include "Model.h"
#include "Shader.h"
#include "PhysicsCCT.h"
#include "GameInstance.h"

CAnimObj::CAnimObj(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(eType, pDevice, pDeviceContext)
{
}

CAnimObj::CAnimObj(const CAnimObj& rhs)
	: Super(rhs)
{
}

HRESULT CAnimObj::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CAnimObj::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	ANIMOBJ_DESC* pDesc = static_cast<ANIMOBJ_DESC*>(pArg);

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CAnimObj::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	return S_OK;
}

void CAnimObj::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CAnimObj::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
	Get_Component<CModel>()->Update_Animation(fTimeDelta, Get_Component<CTransform>(), Get_Component<CPhysicsCCT>());
}

void CAnimObj::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CAnimObj::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);

	m_pGameInstance->Push_RenderObject(RENDER_CATEGORY::NONEBLEND, this);
}

HRESULT CAnimObj::Render()
{
	CShader* pShader	= Get_Component<CShader>();
	CModel* pModel		= Get_Component<CModel>();
	_uint iMeshCount	= pModel->Get_MeshCount();

	pShader->Bind_TransformData(Get_Component<CTransform>()->Get_WorldMatrix());
	for (_uint i = 0; i < iMeshCount; ++i)
	{
		pModel->Bind_Material(pShader, i);
		//pModel->Bind_Bones(pShader, i);
		pShader->Apply();
		pModel->Render(i);
	}

	return S_OK;
}

_bool CAnimObj::Picking(OUT Vec3& vOut)
{
	return _bool();
}

void CAnimObj::Draw_ImGui()
{
}

void CAnimObj::Set_Dead(const wstring& wstrLayerTag)
{
	Super::Set_Dead(wstrLayerTag);
}

const CModel* CAnimObj::Get_ModelComPtr()
{
	return Get_Component<CModel>();
}

HRESULT CAnimObj::Change_ModelCom(_wstring wstrModelTag)
{
	Change_Component<CModel>(static_cast<CModel*>(m_pGameInstance->
		Clone_Prototype(EPrototypeType::COMPONENT, ENUM_TO_UINT(ELevelType::ANIMATION), wstrModelTag)));

	/* 만약 model이 없다면 */
	if (Get_Component<CModel>() == nullptr)
	{
		MSG_BOX("Fail to Change Model Component");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CAnimObj::Ready_Components(ANIMOBJ_DESC* pDesc)
{
	/* model componenet */
	if (FAILED(Add_Component<CModel>(ENUM_TO_UINT(ELevelType::ANIMATION), pDesc->wstrModelProtoTag, pDesc)))
		return E_FAIL;

	/* Prototype_Component_Shader_AnimMesh */
	if (FAILED(Add_Component<CShader>(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Shader_AnimMesh", nullptr)))
		return E_FAIL;

	return S_OK;
}

CAnimObj* CAnimObj::Create(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CAnimObj* pInstance = new CAnimObj(eType, pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX(" CAnimObj Is Failed To Create ");
		return nullptr;
	}

	return pInstance;
}

CGameObject* CAnimObj::Clone(void* pArg)
{
	CAnimObj* pInstance = new CAnimObj(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CAnimObj Is Failed To Create ");
		return nullptr;
	}

	return pInstance;
}

void CAnimObj::Free()
{
	Super::Free();
}