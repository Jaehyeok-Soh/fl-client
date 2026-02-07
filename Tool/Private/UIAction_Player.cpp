#include "pch.h"
#include "UIAction_Player.h"
#include "Tool_Defines.h"
#include "ToolUI.h"
#include "GameInstance.h"

CUIAction_Player::CUIAction_Player()
{
}

CUIAction_Player::CUIAction_Player(const CUIAction_Player& rhs)
{
}

HRESULT CUIAction_Player::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIAction_Player::Initialize_Prototype(ACTION_PLAYER_DESC* pDesc)
{
	m_pOwner = pDesc->pOwner;
	if (nullptr == m_pOwner)
		return E_FAIL;

	return S_OK;
}

HRESULT CUIAction_Player::Initialize(void* pArg)
{
	return S_OK;
}

void CUIAction_Player::Update(const _float fTimeDelta)
{
	Lerp_Movement(fTimeDelta);
}

void CUIAction_Player::Start_Lerp_Movement(MOVE_DESC* pDesc)
{
	if (m_isPlaying_Lerp_Movement)
		return;

	m_isPlaying_Lerp_Movement = TRUE;

	m_vLerpMovement_StartPos	= pDesc->vStartPos;
	m_vLerpMovement_TargetPos	= pDesc->vTargetPos;
	m_fLerpMovement_TargetAlpha = pDesc->fAlpha;
	m_fLerpMovement_Duration	= pDesc->fDuration;
	m_fLerpMovement_TimeAcc		= 0.f;
}

void CUIAction_Player::Lerp_Movement(const _float fTimeDelta)
{
	if (!m_isPlaying_Lerp_Movement)
		return;

	if (m_fLerpMovement_Duration <= 0.f)
	{
		m_vMoveOffset = m_vLerpMovement_TargetPos - m_vLerpMovement_StartPos;
		m_isPlaying_Lerp_Movement = false;
		return;
	}

	m_fLerpMovement_TimeAcc += fTimeDelta;

	_float t = m_fLerpMovement_TimeAcc / m_fLerpMovement_Duration;
	if (t >= 1.f) 
		t = 1.f;
	else if (t <= 0.f) 
		t = 0.f;

	_float s = t;
	if (m_fLerpMovement_TargetAlpha > 0.f)
		s = 1.f - powf(1.f - t, m_fLerpMovement_TargetAlpha);

	m_vMoveOffset = (m_vLerpMovement_TargetPos - m_vLerpMovement_StartPos) * s;
	Apply_Lerp_Movement();

	if (t >= 1.f)
	{
		m_vMoveOffset = m_vLerpMovement_TargetPos - m_vLerpMovement_StartPos;
		m_isPlaying_Lerp_Movement = false;
	}
}

void CUIAction_Player::Apply_Lerp_Movement()
{
	m_pOwner->Set_MoveOffset(m_vMoveOffset);
}

void CUIAction_Player::Start_Progress(PROGRESS_DESC* pDesc)
{

}

CUIAction_Player* CUIAction_Player::Create(ACTION_PLAYER_DESC* pDesc)
{
	CUIAction_Player* pInstance = new CUIAction_Player();
	if (FAILED(pInstance->Initialize_Prototype(pDesc)))
	{
		MSG_BOX("CUIAction_Player::Create,CreateFailed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CUIAction_Player::Clone(void* pArg)
{
	CUIAction_Player* pInstance = new CUIAction_Player(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUIAction_Player::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUIAction_Player::Free()
{
	Super::Free();
}



