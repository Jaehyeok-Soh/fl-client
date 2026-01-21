#include "GameInstance.h"
#include "CameraMan.h"
#include "Bounding_Sphere.h"
#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Collider.h"
#include "Model.h"
#include "PartObject.h"

CPartObject::CPartObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
{
}

CPartObject::CPartObject(const CPartObject& rhs)
	: Super(rhs)
{
}

HRESULT CPartObject::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPartObject::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	PARTOBJ_DESC* pDesc = static_cast<PARTOBJ_DESC*>(pArg);
	m_pMatParent = pDesc->pMatParent;

	return S_OK;
}

HRESULT CPartObject::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	return S_OK;
}

void CPartObject::Update_Priority(_float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CPartObject::Update(_float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CPartObject::Update_Late(_float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CPartObject::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CPartObject::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	return S_OK;
}

void CPartObject::Set_AttackCollider(_bool bActive, ATTACK_DESC* pDesc)
{
	CCollider* pCollider = Get_Component<CCollider>();

	if (bActive)
	{
		Begin_AttackWindow(pDesc);
	}
	else
		End_AttackWindow();

	pCollider->Set_Active(bActive);
}

void CPartObject::OnCollision(_uint iMyColliderLayer, CCollider* pOther)
{
}

void CPartObject::OnCollision_Enter(_uint iMyColliderLayer, CCollider* pOther)
{
}

void CPartObject::OnCollision_Exit(_uint iMyColliderLayer, CCollider* pOther)
{
}

void CPartObject::Set_Parent(CGameObject* pGo)
{
	if (!pGo)
		return;

	m_pParentObject = pGo;
}

void CPartObject::Begin_AttackWindow(ATTACK_DESC* pDesc)
{
	::memcpy(&m_CurrentAttackDesc, pDesc, sizeof(ATTACK_DESC));
	Clear_HitTargets();
	m_bAttackWindow = true;
}

void CPartObject::End_AttackWindow()
{
	m_bAttackWindow = false;
	Clear_AttackDesc();
	Clear_HitTargets();
}

void CPartObject::Clear_AttackDesc()
{
	m_CurrentAttackDesc.iAttackType = -1;
	m_CurrentAttackDesc.fDragK = 0.f;
	m_CurrentAttackDesc.fForceAbs = 0.f;
	m_CurrentAttackDesc.iDamage = 0;
	m_CurrentAttackDesc.vColliderCenter = { 0.f, 0.f, 0.f };
}

void CPartObject::Update_CombinedWorldMatrix(const _float4x4* pMatParent)
{
	::XMStoreFloat4x4(&m_matCombinedWorld, ::XMLoadFloat4x4(&Get_Component<CTransform>()->Get_WorldMatrix()) * ::XMLoadFloat4x4(pMatParent));
}

void CPartObject::Update_CombinedWorldMatrix(_fmatrix matParent)
{
	::XMStoreFloat4x4(&m_matCombinedWorld, ::XMLoadFloat4x4(&Get_Component<CTransform>()->Get_WorldMatrix()) * matParent);
}


void CPartObject::Update_CombinedWorldMatrix_Bilboad(_fmatrix matParent)
{
	::XMStoreFloat4x4(&m_matCombinedWorld, ::XMLoadFloat4x4(&Get_Component<CTransform>()->Get_WorldMatrix()) * matParent);

	CCameraMan* pMainCamera = m_pGameInstance->Get_MainCamera();
	if (pMainCamera == nullptr)
		return;
	_matrix matWorld = ::XMLoadFloat4x4(&m_matCombinedWorld);
	_float fScaleX = ::XMVectorGetX(::XMVector3Length(matWorld.r[0]));
	_float fScaleY = ::XMVectorGetX(::XMVector3Length(matWorld.r[1]));
	_float fScaleZ = ::XMVectorGetX(::XMVector3Length(matWorld.r[2]));
	_vector vPosition = matWorld.r[3] + ::XMVectorSet(0.f, 0.3f, 0.f, 0.f);
	_vector vCamPosition = pMainCamera->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::POS);
	_vector vLook = ::XMVector3Normalize(vPosition - vCamPosition);
	_vector vRight = ::XMVector3Normalize(::XMVector3Cross(::XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));
	_vector vUp = ::XMVector3Normalize(::XMVector3Cross(vLook, vRight));
	matWorld.r[0] = ::XMVectorSetW(vRight * fScaleX, 0.f);
	matWorld.r[1] = ::XMVectorSetW(vUp * fScaleY, 0.f);
	matWorld.r[2] = ::XMVectorSetW(vLook * fScaleZ, 0.f);
	matWorld.r[3] = ::XMVectorSetW(vPosition, 1.f);

	::XMStoreFloat4x4(&m_matCombinedWorld, matWorld);
}

void CPartObject::Update_CombinedWorldMatrix_Bilboad(_fmatrix matParent, _float2 vUIScale)
{
	::XMStoreFloat4x4(&m_matCombinedWorld, ::XMLoadFloat4x4(&Get_Component<CTransform>()->Get_WorldMatrix()) * matParent);

	CCameraMan* pMainCamera = m_pGameInstance->Get_MainCamera();
	if (pMainCamera == nullptr)
		return;
	_matrix matWorld = ::XMLoadFloat4x4(&m_matCombinedWorld);
	_vector vPosition = matWorld.r[3] + ::XMVectorSet(0.f, 0.3f, 0.f, 0.f);
	_vector vCamPosition = pMainCamera->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::POS);
	_vector vLook = ::XMVector3Normalize(vPosition - vCamPosition);
	_vector vRight = ::XMVector3Normalize(::XMVector3Cross(::XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));
	_vector vUp = ::XMVector3Normalize(::XMVector3Cross(vLook, vRight));
	matWorld.r[0] = ::XMVectorSetW(vRight * vUIScale.x, 0.f);
	matWorld.r[1] = ::XMVectorSetW(vUp * vUIScale.y, 0.f);
	matWorld.r[2] = ::XMVectorSetW(vLook, 0.f);
	matWorld.r[3] = ::XMVectorSetW(vPosition, 1.f);

	::XMStoreFloat4x4(&m_matCombinedWorld, matWorld);
}

void CPartObject::Free()
{
	Clear_HitTargets();
	Super::Free();
}
