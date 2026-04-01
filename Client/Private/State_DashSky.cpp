#include "pch.h"
#include "State_DashSky.h"
#include "Player.h"

#include "ControlContext.h"

#include "GameInstance.h"

CState_DashSky::CState_DashSky(CActionState* pOwnerComponent)
	: Super(pOwnerComponent, "DashSky")
{
}

HRESULT CState_DashSky::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_DashSky::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_DashSky::Start(void* pArg, _bool bForce)
{
	CheckAni_WhenStart();

	STATE_START_DESC* pDesc = static_cast<STATE_START_DESC*>(pArg);
	pDesc->iMainAnimIdx = m_iMainAnimIdx;

	if (FAILED(Super::Start(pDesc, bForce)))
		return E_FAIL;

	switch (m_iMainAnimIdx)
	{
	case ENUM_TO_UINT(ANI::FRONT):
		m_fCapHitMoveTime = Get_MoveBoneTime(22.f);
		break;

	case ENUM_TO_UINT(ANI::BACK):
		m_fCapHitMoveTime = Get_MoveBoneTime(24.f);
		break;
	}

	Set_ApplyGravity(false);

	Set_YLerp(false);

	// 40ÆÛ¼¾Æ® È®·ü·Î ÀÜ»ó
	{
		_float fRate = m_pGameInstance->Rand_Float(0.f, 1.f);
		if (fRate <= 0.4f)
			Get_OwnerObject()->Play_GhostTrail();
	}

	return S_OK;
}

void CState_DashSky::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

	if (Get_AnimElpasedTimeSeconds() > 0.45f)
	{
		if(Check_OnGround(0.3f))
		{
			Change_PlayerState(ENUM_TO_UINT(CPlayer::State::LAND));
			return;
		}
		Set_ApplyGravity(true);
		Get_OwnerObject()->Stop_GhostTrail();
	}
}

HRESULT CState_DashSky::End()
{
	if (FAILED(Super::End()))
		return E_FAIL;

	m_FCollisions = 0;

	Set_ApplyGravity(true);

	Set_YLerp(true);

	Get_OwnerObject()->Stop_GhostTrail();

	return S_OK;
}

void CState_DashSky::Set_NextStateDesc(_uint iNextState)
{
	switch (iNextState)
	{
	case ENUM_TO_UINT(CPlayer::State::SLIDE):
		m_tNextStateDesc.iMainAnimIdx = 1; break; // skyslide

	default:
		Super::Set_NextStateDesc(iNextState);
	}
}

void CState_DashSky::CheckAni_WhenStart()
{
	if (Key_Input(ENUM_TO_UINT(CControlContext::CONTROL_KEY::MOVE)))
	{
		m_iMainAnimIdx = ENUM_TO_UINT(ANI::FRONT);
	}
	else
		m_iMainAnimIdx = ENUM_TO_UINT(ANI::BACK);
}

CState_DashSky* CState_DashSky::Create(CActionState* pOwnerComponent, void* pArg)
{
	CState_DashSky* pInstance = new CState_DashSky(pOwnerComponent);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CState_DashSky::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CState_DashSky::Free()
{
	Super::Free();
}