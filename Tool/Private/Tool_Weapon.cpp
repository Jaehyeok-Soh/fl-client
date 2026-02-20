#include "pch.h"
#include "Tool_Weapon.h"
#include "Model.h"
#include "Shader.h"
#include "ComputeShader.h"
#include "GameInstance.h"

CTool_Weapon::CTool_Weapon(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:Tool_PartObject(eType, pDevice, pDeviceContext)
{
}

CTool_Weapon::CTool_Weapon(const CTool_Weapon& rhs)
	:Tool_PartObject(rhs)
{
}

HRESULT CTool_Weapon::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTool_Weapon::Initialize(void* pArg)
{
	if (!pArg)
		return E_FAIL;

	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	WEAPON_DESC* pDesc = static_cast<WEAPON_DESC*>(pArg);
	m_pMatHandSocket = pDesc->pMatHandSocket;
	m_pMatSocket = pDesc->pMatSocket;
	m_eModleType = pDesc->eModel;
	m_bMainWeapon = pDesc->bMianWeapon;

	if (FAILED(Ready_Component(pDesc)))
		return E_FAIL;

	// model과 desc 정보 다를때를 위한 방어
	if (Get_Component<CModel>()->Get_Type() == EModelType::NONANIM)
		m_eModleType = Weapon_ModelType::STATIC;

	switch (m_eModleType)
	{
	case Weapon_ModelType::STATIC:
		break;

	case Weapon_ModelType::ANIM:
		if (FAILED(Ready_ComputeShaders()))
			return E_FAIL;
		break;
	}

	if (m_bMainWeapon)
		m_eState = State::HOLD;
	else
		m_eState = State::NONE;

	Set_Flag(OF_Outline, true);
	return S_OK;
}

HRESULT CTool_Weapon::Ready_Component(WEAPON_DESC* pArg)
{
	if (FAILED(Add_Component<CModel>(0/*static*/, pArg->wstrModelPrototypeName, pArg)))
		return E_FAIL;

	if (FAILED(Add_Component<CShader>(0/*static*/, L"Prototype_Component_Shader_VtxMesh", pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTool_Weapon::Ready_ComputeShaders()
{
	return S_OK;
}

HRESULT CTool_Weapon::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	return S_OK;
}

void CTool_Weapon::Update_Priority(const _float fDT)
{
}

void CTool_Weapon::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

	// scale이 죽었을때 다시 살리기 위함
	//if (CCollider* pCollider = Get_Component<CCollider>())
	//	pCollider->Update(Matrix::CreateScale(100.f, 100.f, 100.f) * m_matCombinedWorld);

	switch (m_eModleType)
	{
	case Weapon_ModelType::STATIC:
		break;

	case Weapon_ModelType::ANIM:
	/*	Play_Anim(fTimeDelta);*/
		break;
	}
}

void CTool_Weapon::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CTool_Weapon::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);

	// none일때는 그리지 않음
	if (m_eState != State::NONE)
		m_pGameInstance->Push_RenderObject(RENDER_CATEGORY::NONEBLEND, this);

	// state에 따른 combineworld 업데이트
	switch (m_eState)
	{
	case State::HAND:
		Super::Update_CombinedWorldMatrix((*m_pMatHandSocket) * (*m_pMatParent));
		break;
	default:
		Super::Update_CombinedWorldMatrix((*m_pMatSocket) * (*m_pMatParent));
		//Update_HoldingPos();
		break;
	}

}

HRESULT CTool_Weapon::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	HRESULT hr = {};
	switch (m_eModleType)
	{
	case Weapon_ModelType::STATIC:
		Render_StaticWeap();
		break;

	case Weapon_ModelType::ANIM:
		Render_AnimWeap();
		break;
	}

	return S_OK;
}


HRESULT CTool_Weapon::Render_StaticWeap()
{
	CShader* pShader = Get_Component<CShader>();
	CModel* pModel = Get_Component<CModel>();
	_uint iMeshCount = pModel->Get_MeshCount();

	pShader->Bind_ObjectInfoData(m_tObjectInfoDesc);
	pShader->Bind_TransformData(m_CombineWorldMatrix);
	for (_uint i = 0; i < iMeshCount; ++i)
	{
		pModel->Bind_Material(pShader, i);
		pModel->Bind_MaterialInstance(pShader, i);
		pShader->Apply();
		pModel->Render(i);
	}

	return S_OK;
}


HRESULT CTool_Weapon::Render_AnimWeap()
{
	CShader* pShader = Get_Component<CShader>();
	CModel* pModel = Get_Component<CModel>();
	_uint				iMeshCount = pModel->Get_MeshCount();
	CComputeShader* pBoneMeshCS = static_cast<CComputeShader*>(Get_Script_Component(TEXT("ComputeShader_BoneMesh")));
	CComputeShader* pBoneCombineCS = static_cast<CComputeShader*>(Get_Script_Component(TEXT("ComputeShader_BoneCombine")));

	pShader->Bind_ObjectInfoData(m_tObjectInfoDesc);
	pShader->Bind_TransformData(m_CombineWorldMatrix);
	for (_uint i = 0; i < iMeshCount; ++i)
	{
		pModel->Bind_Material(pShader, i);
		pModel->Bind_Bones(pShader, i, pBoneMeshCS, pBoneCombineCS);
		pShader->Apply();
		pModel->Render(i);
	}

	return S_OK;
}

void CTool_Weapon::Draw_ImGui()
{
}

CTool_Weapon* CTool_Weapon::Create(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CTool_Weapon* pInstance = new CTool_Weapon(eType, pDevice, pDeviceContext);

	if (pInstance == nullptr)
	{
		MSG_BOX("Create to Fail : CTool_Weapon");
		Safe_Release(pInstance);
		return nullptr;
	}

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Initialize to Fail : CTool_Weapon");
		return nullptr;
	}

	return pInstance;
}

CGameObject* CTool_Weapon::Clone(void* pArg)
{
	CTool_Weapon* pClone = new CTool_Weapon(*this);
	if (FAILED(pClone->Initialize(pArg)))
	{
		MSG_BOX("CTool_Weapon::Clone, Failed");
		Safe_Release(pClone);
	}
	return pClone;
}
void CTool_Weapon::Free()
{
	Super::Free();
}