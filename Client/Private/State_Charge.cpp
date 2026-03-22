#include "pch.h"
#include "State_Charge.h"

#include "Player.h"
#include "Weapon.h"

#include "PhysicsCCT.h"
#include "ActionState.h"

#include "GameInstance.h"

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
	if (FAILED(Start_AttackState(pArg)))
		return E_FAIL;

	Change_WeaponState(ENUM_TO_UINT(CPlayer::EWEAPON::MELEE), ENUM_TO_UINT(CWeapon::State::HAND));

	Start_Att(ENUM_TO_UINT(CPlayer::State::CHARGE));

	m_bLookMonsterYet = true;
	m_bImpulsYet =true;
	m_bShakeActived = false;

	return S_OK;
}

void CState_Charge::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

	// monster 추적
	if (m_bLookMonsterYet && Engine_Utils::Has_Flag(m_FAniFlags, STATEANI_FLAG::SA_PreAniDone))
	{
		LookAt_Monser();
		m_bLookMonsterYet = false;
	}

	// camera shaking
	if (m_bShakeActived == false && m_fStateElapsed >= (1.f / 1.2f))
	{
		CAMERA_SHAKE_DESC desc{};
		desc.fDuration = 0.2f;
		CGameInstance::GetInstance()->Request_MainCameraShake(desc);
		m_bShakeActived = true;
	}

	// weapon 별 업데이트
	Update_byWeapon(fTimeDelta);

	// weapon change time
	if (m_fHoldWeaponTime <= m_fStateElapsed)
	{
		Change_Weapon();
		Change_WeaponState(ENUM_TO_UINT(CPlayer::EWEAPON::MELEE), ENUM_TO_UINT(CWeapon::State::HOLD));
	}
}

HRESULT CState_Charge::End()
{
	if (FAILED(Super::End()))
		return E_FAIL;

	End_Att(ENUM_TO_UINT(CPlayer::State::CHARGE));

	Change_WeaponState(ENUM_TO_UINT(CPlayer::EWEAPON::MELEE), ENUM_TO_UINT(CWeapon::State::HOLD));

	return S_OK;
}

void CState_Charge::Go_Front(const _float fTimeDelta)
{
	CTransform* pPlayerTrans = Get_OwnerObject()->Get_Component<CTransform>();
	CPhysicsCCT* pCCT = Get_OwnerObject()->Get_Component<CPhysicsCCT>();

	Vec3 vLook = (pPlayerTrans->Get_Info(TRANSFORM_INFO_STATE::LOOK));
	vLook.Normalize();

	Move(vLook);
}

void CState_Charge::Update_byWeapon(const _float fTimeDelta)
{
	switch (m_iMainAnimIdx)
	{
	case ENUM_TO_UINT(CPlayer::MELEE::SWORD):
		Update_Sword(fTimeDelta);
		break;

	case ENUM_TO_UINT(CPlayer::MELEE::DUAL):
		Update_Dual(fTimeDelta);
		break;
	}
}

void CState_Charge::Update_Sword(const _float fTimeDelta)
{
	if (m_bImpulsYet && Engine_Utils::Has_Flag(m_FAniFlags, STATEANI_FLAG::SA_PreAniDone))
	{
		Look_Impuls(12.f);

		m_bImpulsYet = false;
	}
}

void CState_Charge::Update_Dual(const _float fTimeDelta)
{
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
