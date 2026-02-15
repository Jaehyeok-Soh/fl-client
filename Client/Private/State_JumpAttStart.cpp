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
	if (FAILED(Super::Start(pArg, bForce)))
		return E_FAIL;

	Change_Weapon(CPlayer::Part::SWORD, ENUM_TO_UINT(CWeapon::State::HAND));

	return S_OK;
}

void CState_JumpAttStart::Update(const _float fTimeDelta)
{
	CStateBase::Update(fTimeDelta);

	if (Check_OnGround(0.2f))
		Change_PlayerState(ENUM_TO_UINT(CPlayer::State::JUMPATTEND));

	OwnMove(fTimeDelta);
}

HRESULT CState_JumpAttStart::End()
{
	if (FAILED(Super::End()))
		return E_FAIL;

	return S_OK;
}

void CState_JumpAttStart::OwnMove(const _float fTimeDelta)
{
	// 30도로 빠르게 떨어지기
	CStateBase::Turn_byCam(fTimeDelta);

	CTransform* pPlayerTrans = Get_OwnerObject()->Get_Component<CTransform>();
	CPhysicsCCT* pCCT = Get_OwnerObject()->Get_Component<CPhysicsCCT>();
	_float moveps = pPlayerTrans->Get_MovePerSec(); // 속도


	Vec3 vLook = (pPlayerTrans->Get_Info(TRANSFORM_INFO_STATE::LOOK));
	Vec3 vUp = (pPlayerTrans->Get_Info(TRANSFORM_INFO_STATE::UP));

	vLook.Normalize();
	vUp.Normalize();

	Vec3 vDir = vLook * 1.f + vUp * (-1.f);
	Vec3 disp = vDir * moveps * fTimeDelta * 3.f;

	pCCT->Move(disp, 0.01f, fTimeDelta);

	Vec3 finalPos = pCCT->GetFootPosition();

	pPlayerTrans->Set_Info(TRANSFORM_INFO_STATE::POS, finalPos);
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
