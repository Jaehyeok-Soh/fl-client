#include "pch.h"
#include "State_JumpAttStart.h"

#include "Player.h"
#include "Weapon.h"
#include "Transform.h"
#include "PhysicsCCT.h"

CState_JumpAttStart::CState_JumpAttStart(CActionState* pOwnerComponent)
	: Super(pOwnerComponent, "JumpAttStart")
{
}

HRESULT CState_JumpAttStart::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_JumpAttStart::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_JumpAttStart::Start(void* pArg, _bool bForce)
{
	if (IsRemotePlayer())
	{
		if (FAILED(Super::Start(pArg, bForce)))
			return E_FAIL;

		return S_OK;
	}

	if (FAILED(Start_AttackState(pArg)))
		return E_FAIL;

	CStateBase::SetupLook_CameraLook();

	Change_WeaponState(ENUM_TO_UINT(CPlayer::EWEAPON::MELEE), ENUM_TO_UINT(CWeapon::State::HAND));

	{
		CTransform* pPlayerTrans = Get_OwnerObject()->Get_Component<CTransform>();
		CPhysicsCCT* pCCT = Get_OwnerObject()->Get_Component<CPhysicsCCT>();

		Vec3 vLook = (pPlayerTrans->Get_Info(TRANSFORM_INFO_STATE::LOOK));
		Vec3 vUp = (pPlayerTrans->Get_Info(TRANSFORM_INFO_STATE::UP));

		vLook.Normalize();
		vUp.Normalize();

		//Vec3 vDir = vLook + (vUp * (-1.5f));
		//vDir.Normalize();

		Vec3 vDir = vLook * 1.5f - vUp * 2.f;

		//Vec3 accelation = vDir * 50.f;

		SetCCTImpuls(vDir * 20.f);
		Set_ZeroDeAccelRate();
	}

	Set_YLerp(false);

	return S_OK;
}

void CState_JumpAttStart::Update(const _float fTimeDelta)
{
	if (IsRemotePlayer())
	{
		CStateBase::Update(fTimeDelta);
		return;
	}

	CStateBase::Update(fTimeDelta);

	CTransform* pPlayerTrans = Get_OwnerObject()->Get_Component<CTransform>();
	CPhysicsCCT* pCCT = Get_OwnerObject()->Get_Component<CPhysicsCCT>();

	Vec3 vLook = (pPlayerTrans->Get_Info(TRANSFORM_INFO_STATE::LOOK));
	Vec3 vUp = (pPlayerTrans->Get_Info(TRANSFORM_INFO_STATE::UP));

	vLook.Normalize();
	vUp.Normalize();

	//Vec3 vDir = vLook + (vUp * (-1.5f));
	//vDir.Normalize();

	Vec3 vDir = vLook * 4.f - vUp * 3.f;

	Move(vDir);

	// 벽이랑 충돌했는지 먼저 검사
	if (IsOn_CCTFlag(PxControllerCollisionFlag::Enum::eCOLLISION_SIDES) ||
		Check_OnGround(0.7f))
	{
		Change_PlayerState(ENUM_TO_UINT(CPlayer::State::JUMPATTEND));
		return;
	}
}

HRESULT CState_JumpAttStart::End()
{
	if (FAILED(Super::End()))
		return E_FAIL;

	Reset_DeAccelRate();
	Set_ZeroHorizontalVelocity();

	Set_YLerp(true);

	return S_OK;
}

void CState_JumpAttStart::OwnMove(const _float fTimeDelta)
{
	
}

CState_JumpAttStart* CState_JumpAttStart::Create(CActionState* pOwnerComponent, void* pArg)
{
	CState_JumpAttStart* pInstance = new CState_JumpAttStart(pOwnerComponent);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CState_JumpAttStart::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CState_JumpAttStart::Free()
{
	Super::Free();
}
