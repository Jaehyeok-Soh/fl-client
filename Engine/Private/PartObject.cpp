#include "Engine_pch.h"
#include "PartObject.h"
#include "CameraMan.h"
#include "Bounding_Sphere.h"
#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Collider.h"
#include "Model.h"
#include "GameInstance.h"

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

void CPartObject::Update_CombinedWorldMatrix(const Matrix* pMatParent)
{
	m_matCombinedWorld = Get_Component<CTransform>()->Get_WorldMatrix() * (*pMatParent);
}

void CPartObject::Update_CombinedWorldMatrix(Matrix matParent)
{
	m_matCombinedWorld = Get_Component<CTransform>()->Get_WorldMatrix() * matParent;
}


void CPartObject::Update_CombinedWorldMatrix_Bilboad(Matrix matParent)
{
	m_matCombinedWorld = Get_Component<CTransform>()->Get_WorldMatrix() * matParent;
	CCameraMan* pMainCamera = m_pGameInstance->Get_MainCamera();
	if (pMainCamera == nullptr)
		return;
	_float fScaleX = m_matCombinedWorld.Right().Length();
	_float fScaleY = m_matCombinedWorld.Up().Length();
	_float fScaleZ = m_matCombinedWorld.Backward().Length();
	Vec3 vPosition = m_matCombinedWorld.Translation() + Vec3{ 0.f, 0.3f, 0.f };
	Vec3 vCamPosition = pMainCamera->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::POS);
	Vec3 vLook = vPosition - vCamPosition;
	vLook.Normalize();
	Vec3 vRight = Vec3::Up.Cross(vLook);
	vRight.Normalize();
	Vec3 vUp = vLook.Cross(vRight);
	vUp.Normalize();
	
	m_matCombinedWorld.Right(vRight * fScaleX);
	m_matCombinedWorld.Up(vUp * fScaleY);
	m_matCombinedWorld.Backward(vLook * fScaleZ);
	m_matCombinedWorld.Translation(vPosition);
}

void CPartObject::Update_CombinedWorldMatrix_Bilboad(Matrix matParent, Vec2 vUIScale)
{
	m_matCombinedWorld = Get_Component<CTransform>()->Get_WorldMatrix() * matParent;
	CCameraMan* pMainCamera = m_pGameInstance->Get_MainCamera();
	if (pMainCamera == nullptr)
		return;
	Vec3 vPosition = m_matCombinedWorld.Translation() + Vec3{ 0.f, 0.3f, 0.f };
	Vec3 vCamPosition = pMainCamera->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::POS);
	Vec3 vLook = vPosition - vCamPosition;
	vLook.Normalize();
	Vec3 vRight = Vec3::Up.Cross(vLook);
	vRight.Normalize();
	Vec3 vUp = vLook.Cross(vRight);
	vUp.Normalize();

	m_matCombinedWorld.Right(vRight * vUIScale.x);
	m_matCombinedWorld.Up(vUp * vUIScale.y);
	m_matCombinedWorld.Backward(vLook);
	m_matCombinedWorld.Translation(vPosition);
}

void CPartObject::Free()
{
	Clear_HitTargets();
	Super::Free();
}
