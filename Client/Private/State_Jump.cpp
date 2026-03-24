#include "pch.h"
#include "State_Jump.h"

#include "Player.h"

#include "PlayerImguiValues.h"

CState_Jump::CState_Jump(CActionState* pOwnerComponent)
	: Super(pOwnerComponent, "Jump")
{
}

HRESULT CState_Jump::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_Jump::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_Jump::Start(void* pArg, _bool bForce)
{
	if (FAILED(Super::Start(pArg, bForce)))
		return E_FAIL;

	Set_DoubleJumpCount(false);
	Set_ZeroVerticalVelocity();
	Jump_Impuls(0.6f);

	Set_ApplyGravity(false);
	Set_YLerp(false);

	Set_RootMotion_Apply(true);

	return S_OK;
}

void CState_Jump::Update(const _float fTimeDelta)
{
	// 바닥 충돌 검사 후 change
	if (m_fStateElapsed > 12.f / ANIMTIC &&
		(Check_OnGround(0.1f) || IsOn_CCTFlag(PxControllerCollisionFlag::Enum::eCOLLISION_SIDES)))
	{
		Change_PlayerState(ENUM_TO_UINT(CPlayer::State::LAND));
		return;
	}

	if (Get_AnimElpasedTimeSeconds() > 0.5f)
	{
		Set_ApplyGravity(true);

		Set_RootMotion_Apply(false);
	}

	Super::Update(fTimeDelta);
}

HRESULT CState_Jump::End()
{
	if (FAILED(Super::End()))
		return E_FAIL;

	Set_ApplyGravity(true);

	//Set_RootMotion_Apply(true);

	Set_YLerp(true);

	return S_OK;
}

void CState_Jump::Set_NextStateDesc(_uint iNextState)
{
	switch (iNextState)
	{
	case ENUM_TO_UINT(CPlayer::State::SLIDE):
		m_tNextStateDesc.iMainAnimIdx = 1; break; // skyslide

	default:
		Super::Set_NextStateDesc(iNextState);
	}
}

CState_Jump* CState_Jump::Create(CActionState* pOwnerComponent, void* pArg)
{
	CState_Jump* pInstance = new CState_Jump(pOwnerComponent);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CState_Jump::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CState_Jump::Free()
{
	Super::Free();
}
