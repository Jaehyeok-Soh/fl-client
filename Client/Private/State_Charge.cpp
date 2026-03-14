#include "pch.h"
#include "State_Charge.h"

#include "Player.h"
#include "Weapon.h"

#include "PhysicsCCT.h"
#include "ActionState.h"

CState_Charge::CState_Charge(CActionState* pOwnerComponent)
	: Super(pOwnerComponent, "Charge")
{
}

HRESULT CState_Charge::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_Charge::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_Charge::Start(void* pArg, _bool bForce)
{
	if (FAILED(Super::Start(pArg, bForce)))
		return E_FAIL;

	Change_Weapon(CPlayer::Part::SWORD, ENUM_TO_UINT(CWeapon::State::HAND));

	Start_Att(ENUM_TO_UINT(CPlayer::State::CHARGE));

	return S_OK;
}

void CState_Charge::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

	if(m_fHoldWeaponTime <= m_fStateElapsed)
		Change_Weapon(CPlayer::Part::SWORD, ENUM_TO_UINT(CWeapon::State::HOLD));

	else
	{
		Check_Monster();
	}
}

HRESULT CState_Charge::End()
{
	if (FAILED(Super::End()))
		return E_FAIL;

	End_Att(ENUM_TO_UINT(CPlayer::State::CHARGE));

	Change_Weapon(CPlayer::Part::SWORD, ENUM_TO_UINT(CWeapon::State::HOLD));

	return S_OK;
}

void CState_Charge::Go_Front(const _float fTimeDelta)
{
	CTransform* pPlayerTrans = Get_OwnerObject()->Get_Component<CTransform>();
	CPhysicsCCT* pCCT = Get_OwnerObject()->Get_Component<CPhysicsCCT>();

	Vec3 vLook = (pPlayerTrans->Get_Info(TRANSFORM_INFO_STATE::LOOK));

	Move(vLook);
}

CState_Charge* CState_Charge::Create(CActionState* pOwnerComponent, void* pArg)
{
	CState_Charge* pInstance = new CState_Charge(pOwnerComponent);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CState_Charge::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CState_Charge::Free()
{
	Super::Free();
}
