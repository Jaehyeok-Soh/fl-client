#include "Client_Defines.h"
#include "Player.h"
#include "PlayerActionState.h"
#include "CameraMan.h"
#include "GameInstance.h"
#include "PlayerControlContext.h"

CPlayerControlContext::CPlayerControlContext()
	: Super()
{
}

CPlayerControlContext::CPlayerControlContext(const CControlContext& rhs)
	: Super(rhs)
{
}

HRESULT CPlayerControlContext::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerControlContext::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerControlContext::Awake(const _uint iLevelIndex)
{
	return S_OK;
}

_bool CPlayerControlContext::Is_FootRayEnabled()
{
	CGameObject* pOwner = Get_Owner();
	if (pOwner == nullptr)
		return true;

	CPlayerActionState* pActionState = pOwner->Get_Component<CPlayerActionState>();
	if (pActionState == nullptr)
		return true;

	const _int iIndex = pActionState->Get_CurrentStateIndex();
	if (iIndex <= -1)
		return true;

	CPlayer::State eState = static_cast<CPlayer::State>(iIndex);

	switch (eState)
	{
	case Client::CPlayer::State::IDLE:
	case Client::CPlayer::State::RUNSTART:
	case Client::CPlayer::State::RUN:
	case Client::CPlayer::State::RUNEND:
		return true;
	default:
		return true;
	}
}

void CPlayerControlContext::Set_Grounded(_bool bGrounded, const COLMESH_HITINFO* pHit)
{
	Super::Set_Grounded(bGrounded);
	if (bGrounded && pHit)
		m_CurrentGroundInfo = *pHit;
}

void CPlayerControlContext::Clear_Grounded()
{
	Super::Set_Grounded(false);
	m_CurrentGroundInfo = {};
}

_bool CPlayerControlContext::Is_LeftAttackPressed()
{
	return m_pGameInstance->Mouse_Pressing(MOUSEKEYSTATE::LB);
}

_bool CPlayerControlContext::Is_RightAttackPressed()
{
	return m_pGameInstance->Mouse_Pressing(MOUSEKEYSTATE::RB);
}

_bool CPlayerControlContext::Is_FrontDashPressed()
{
	return m_pGameInstance->KeyButton_Pressing(DIK_LSHIFT) && m_pGameInstance->KeyButton_Pressing(DIK_W);
}

_bool CPlayerControlContext::Is_LeftDashPressed()
{
	return m_pGameInstance->KeyButton_Pressing(DIK_LSHIFT) && m_pGameInstance->KeyButton_Pressing(DIK_A);
}

_bool CPlayerControlContext::Is_RightDashPressed()
{
	return m_pGameInstance->KeyButton_Pressing(DIK_LSHIFT) && m_pGameInstance->KeyButton_Pressing(DIK_D);
}

_bool CPlayerControlContext::Is_BackDashPressed()
{
	return m_pGameInstance->KeyButton_Pressing(DIK_LSHIFT) && m_pGameInstance->KeyButton_Pressing(DIK_S);
}

_bool CPlayerControlContext::Is_MovePressed()
{
	return m_pGameInstance->KeyButton_Pressing(DIK_W) ||
		m_pGameInstance->KeyButton_Pressing(DIK_A) ||
		m_pGameInstance->KeyButton_Pressing(DIK_S) ||
		m_pGameInstance->KeyButton_Pressing(DIK_D);
}

_bool CPlayerControlContext::Is_JumpPressed()
{
	return m_pGameInstance->KeyButton_Down(DIK_SPACE);
}

_bool CPlayerControlContext::Is_WirePressed()
{
	return m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::RB);
}

_bool CPlayerControlContext::Is_DodgePressed()
{
	return m_pGameInstance->KeyButton_Down(DIK_Q);
}

_bool CPlayerControlContext::Is_ChakraJumpPressed()
{
	return m_pGameInstance->KeyButton_Down(DIK_LCONTROL);
}

_bool CPlayerControlContext::Is_RopePressed()
{
	return m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::RB);
}

_fvector CPlayerControlContext::Get_MoveDir()
{
	if (!m_pOwnerTargetCamera)
	{
		if (!(m_pOwnerTargetCamera = Get_Owner()->Get_CameraTargeter()))
			return ::XMVectorZero();
	}

	_bool bGround = (Is_WallMode() == false);
	CTransform* pCameraTransform = m_pOwnerTargetCamera->Get_Component<CTransform>();
	
	CTransform* pTransform = Get_Owner()->Get_Component<CTransform>();
	if (!pCameraTransform || !pTransform)
		return::XMVectorZero();

	_vector vDesiredDir = ::XMVectorZero();
	_vector vCameraLook = pCameraTransform->Get_Info(TRANSFORM_INFO_STATE::LOOK);
	_vector vCameraRight = pCameraTransform->Get_Info(TRANSFORM_INFO_STATE::RIGHT);
	_vector vCameraLookXZ = ::XMVector3Normalize(::XMVectorSetY(vCameraLook, 0.f));
	_vector vCameraRightXZ = ::XMVector3Normalize(::XMVectorSetY(vCameraRight, 0.f));
	if (bGround)
	{
		if (KEY_BUTTON_HOLD(DIK_W))  vDesiredDir += vCameraLookXZ;
		else if (KEY_BUTTON_HOLD(DIK_S)) vDesiredDir -= vCameraLookXZ;

		if (KEY_BUTTON_HOLD(DIK_D)) vDesiredDir += vCameraRightXZ;
		else if (KEY_BUTTON_HOLD(DIK_A)) vDesiredDir -= vCameraRightXZ;
	}
	else
	{
		_vector vPlayerUp = pTransform->Get_Info(TRANSFORM_INFO_STATE::UP);
		_vector vCamLookOnWall = vCameraLook - ::XMVector3Dot(vCameraLook, vPlayerUp) * vPlayerUp;
		_vector vCamRightOnWall = vCameraRight - ::XMVector3Dot(vCameraRight, vPlayerUp) * vPlayerUp;

		if (::XMVectorGetX(::XMVector3LengthSq(vCamLookOnWall)) <= g_XMEpsilon.f[0])
		{
			vCamLookOnWall = ::XMVector3Normalize(pTransform->Get_Info(TRANSFORM_INFO_STATE::LOOK));
			vCamRightOnWall = ::XMVector3Normalize(pTransform->Get_Info(TRANSFORM_INFO_STATE::RIGHT));
		}
		else
		{
			vCamLookOnWall = ::XMVector3Normalize(vCamLookOnWall);
			vCamRightOnWall = ::XMVector3Normalize(vCamRightOnWall);
		}

		if (KEY_BUTTON_HOLD(DIK_W))  vDesiredDir += vCamLookOnWall;
		else if (KEY_BUTTON_HOLD(DIK_S)) vDesiredDir -= vCamLookOnWall;

		if (KEY_BUTTON_HOLD(DIK_D)) vDesiredDir += vCamRightOnWall;
		else if (KEY_BUTTON_HOLD(DIK_A)) vDesiredDir -= vCamRightOnWall;
	}

	return vDesiredDir;
}

void CPlayerControlContext::OnChangeLockonTarget(CGameObject* pGo)
{
	if (pGo == m_pTarget)
		return;

	if (pGo)
		m_pTarget = pGo;
	else
		m_pTarget = nullptr;
}

CPlayerControlContext* CPlayerControlContext::Create()
{
	CPlayerControlContext* pInstance = new CPlayerControlContext();
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CPlayerControlContext::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CPlayerControlContext::Clone(void* pArg)
{
	CPlayerControlContext* pInstance = new CPlayerControlContext(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CPlayerControlContext::Clone, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerControlContext::Free()
{
	Super::Free();
}
