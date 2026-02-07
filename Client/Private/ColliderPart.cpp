#include "pch.h"
#include "Client_Defines.h"
#include "Collider.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"
#include "ColliderPart.h"

CColliderPart::CColliderPart(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
{
}

CColliderPart::CColliderPart(const CColliderPart& rhs)
	: Super(rhs)
{
}

HRESULT CColliderPart::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CColliderPart::Initialize(void* pArg)
{
	if (!pArg)
		return E_FAIL;

	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	COLLIDERPART_DESC* pDesc = static_cast<COLLIDERPART_DESC*>(pArg);
	m_pMatSocket = pDesc->pMatSocket;

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	//Get_Component<CTransform>()->Set_Scale(0.1f, 0.1f, 0.1f);
	//Get_Component<CTransform>()->Rotation(0.f, ::XMConvertToRadians(90.f), 0.f);
	//Get_Component<CTransform>()->Set_Info(TRANSFORM_INFO_STATE::POS, ::XMVectorSet(0.8f, 0.f, 0.f, 1.f));
	return S_OK;
}

HRESULT CColliderPart::Awake(const _uint iCurrentLevelIndex)
{
	if (FAILED(Super::Awake(iCurrentLevelIndex)))
		return E_FAIL;

	return S_OK;
}

void CColliderPart::Update_Priority(_float fTimeDelta)
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

void CColliderPart::Update(_float fTimeDelta)
{
	Super::Update(fTimeDelta);

	Super::Update_CombinedWorldMatrix((*m_pMatSocket) * (*m_pMatParent));

	if (CCollider* pCollider = Get_Component<CCollider>())
		pCollider->Update(Matrix::CreateScale(100.f, 100.f, 100.f) * m_matCombinedWorld);
}

void CColliderPart::Update_Late(_float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CColliderPart::Ready_Before_Render(_float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
#ifdef _DEBUG
	m_pGameInstance->Push_DebugComponent(Get_Component<CCollider>());
#endif
}

void CColliderPart::OnCollision(_uint iMyColliderLayer, CGameObject* pOther)
{
	Get_Parent()->OnCollision(iMyColliderLayer, pOther);
}

void CColliderPart::OnCollision_Enter(_uint iMyColliderLayer, CGameObject* pOther)
{
	Get_Parent()->OnCollision_Enter(iMyColliderLayer, pOther);
}

void CColliderPart::OnCollision_Exit(_uint iMyColliderLayer, CGameObject* pOther)
{
	Get_Parent()->OnCollision_Exit(iMyColliderLayer, pOther);
}

void CColliderPart::OnTrigger_Enter(_uint iMyColliderLayer, CGameObject* pOther)
{
	Get_Parent()->OnTrigger_Enter(iMyColliderLayer, pOther);
}

void CColliderPart::OnTrigger_Exit(_uint iMyColliderLayer, CGameObject* pOther)
{
	Get_Parent()->OnTrigger_Exit(iMyColliderLayer, pOther);
}

HRESULT CColliderPart::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CColliderPart::Ready_Components(COLLIDERPART_DESC* pDesc)
{
	return S_OK;
}

CColliderPart* CColliderPart::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CColliderPart* pInstance = new CColliderPart(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CColliderPart::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CColliderPart::Clone(void* pArg)
{
	CColliderPart* pInstance = new CColliderPart(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CColliderPart::Clone, Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CColliderPart::Free()
{
	m_pGameInstance->Unregister_Collider(Get_Component<CCollider>());
	Super::Free();
}

