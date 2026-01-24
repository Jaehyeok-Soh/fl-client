#include "pch.h"
#include "Client_Defines.h"
#include "Collider.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"
#include "Weapon.h"

CWeapon::CWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
{
}

CWeapon::CWeapon(const CWeapon& rhs)
	: Super(rhs)
{
}

HRESULT CWeapon::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWeapon::Initialize(void* pArg)
{
	if (!pArg)
		return E_FAIL;

	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	WEAPON_DESC* pDesc = static_cast<WEAPON_DESC*>(pArg);
	m_pMatHandSocket = pDesc->pMatHandSocket;
	m_pMatSocket = pDesc->pMatSocket;

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	//Get_Component<CTransform>()->Set_Scale(0.1f, 0.1f, 0.1f);
	//Get_Component<CTransform>()->Rotation(0.f, ::XMConvertToRadians(90.f), 0.f);
	//Get_Component<CTransform>()->Set_Info(TRANSFORM_INFO_STATE::POS, ::XMVectorSet(0.8f, 0.f, 0.f, 1.f));

	return S_OK;
}

HRESULT CWeapon::Awake(const _uint iCurrentLevelIndex)
{
	if (FAILED(Super::Awake(iCurrentLevelIndex)))
		return E_FAIL;

	return S_OK;
}

void CWeapon::Update_Priority(_float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);	
	//_matrix matSocket = ::XMLoadFloat4x4(m_pMatSocket);
	//// Right, Up, Look 노멀라이즈로 스케일 죽이기
	//// 예시코드에서 Socket에 적용된 스케일때문에 외부 Mesh를 붙였을때 이상하게 출력
	//// 이렇게 직접 컨트롤이 가능하다는 예시
	//for (_uint i = 0; i < 3; ++i)
	//	matSocket.r[i] = ::XMVector3Normalize(matSocket.r[i]);
	//_matrix matParent = matSocket * ::XMLoadFloat4x4(m_pMatParent);
	//Super::Update_CombinedWorldMatrix(matParent);
}

void CWeapon::Update(_float fTimeDelta)
{
	Super::Update(fTimeDelta);
	if (CCollider* pCollider = Get_Component<CCollider>())
		pCollider->Update(Matrix::CreateScale(100.f, 100.f, 100.f) * m_matCombinedWorld);
}

void CWeapon::Update_Late(_float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CWeapon::Ready_Before_Render(_float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
	m_pGameInstance->Push_RenderObject(RENDER_CATEGORY::NONEBLEND, this);
	switch (m_eState)
	{
	case Client::CWeapon::HAND:
		Super::Update_CombinedWorldMatrix((*m_pMatHandSocket) * (*m_pMatParent));
		break;
	default:
		Super::Update_CombinedWorldMatrix((*m_pMatSocket) * (*m_pMatParent));
		break;
	}

#ifdef _DEBUG
	m_pGameInstance->Push_DebugComponent(Get_Component<CCollider>());
#endif
}

void CWeapon::OnCollision(_uint iMyColliderLayer, CCollider* pOther)
{
	Get_Parent()->OnCollision(iMyColliderLayer, pOther);
}

void CWeapon::OnCollision_Enter(_uint iMyColliderLayer, CCollider* pOther)
{
	Get_Parent()->OnCollision_Enter(iMyColliderLayer, pOther);
}

void CWeapon::OnCollision_Exit(_uint iMyColliderLayer, CCollider* pOther)
{
	Get_Parent()->OnCollision_Exit(iMyColliderLayer, pOther);
}

HRESULT CWeapon::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	CShader* pShader = Get_Component<CShader>();
	CModel* pModel = Get_Component<CModel>();
	_uint iMeshCount = pModel->Get_MeshCount();
	pShader->Bind_TransformData(m_matCombinedWorld);
	for (_uint i = 0; i < iMeshCount; ++i)
	{
		pModel->Bind_Material(pShader, i);
		pModel->Bind_MaterialInstance(pShader, i);
		pShader->Apply();
		pModel->Render(i);
	}

	return S_OK;
}

void CWeapon::Set_HandSocket()
{
	if (m_eState == State::HAND)
		return;

	m_eState = State::HAND;
}

void CWeapon::Set_DefaultSocket()
{
	if (m_eState == State::NONE)
		return;

	m_eState = State::NONE;
}

HRESULT CWeapon::Ready_Components(WEAPON_DESC* pDesc)
{
	if (FAILED(Add_Component<CModel>(0/*static*/, pDesc->wstrModelPrototypeName, pDesc)))
		return E_FAIL;

	if (FAILED(Add_Component<CShader>(0/*static*/, L"Prototype_Component_Shader_VtxMesh", pDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CWeapon::Bind_ShaderResources()
{
	return S_OK;
}

CWeapon* CWeapon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CWeapon* pInstance = new CWeapon(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CWeapon::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CWeapon::Clone(void* pArg)
{
	CWeapon* pInstance = new CWeapon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CWeapon::Clone, Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWeapon::Free()
{
	Super::Free();
}

