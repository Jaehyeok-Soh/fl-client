#include "pch.h"
#include "PlayerControlContext.h"

#include "Client_Defines.h"
#include "Player.h"
#include "PlayerActionState.h"
#include "CameraMan.h"

#include "GameInstance.h"

CPlayerControlContext::CPlayerControlContext()
	: Super()
{
}

CPlayerControlContext::CPlayerControlContext(const CPlayerControlContext& rhs)
	: Super(rhs)
	, m_FKeys{ rhs.m_FKeys }
	, m_FPreKeys{ rhs.m_FPreKeys }
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

	PLAYER_CONTROLCONTEXT_DESC* pDesc = static_cast<PLAYER_CONTROLCONTEXT_DESC*>(pArg);
	m_FKeys						= pDesc->FKeys;

	return S_OK;
}

HRESULT CPlayerControlContext::Awake(const _uint iLevelIndex)
{
	return S_OK;
}

void CPlayerControlContext::Count_Time(const _float fTimeDelta)
{
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
	case Client::CPlayer::State::RUNLOOP:

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

void CPlayerControlContext::Set_CheckKey(KEYFLAGS FKey, _bool bOn)
{
	if (bOn)
		Engine_Utils::Add_Flag(m_FKeys, FKey);

	else
		Engine_Utils::RemoveHard_Flag(m_FKeys, FKey);
}

void CPlayerControlContext::Set_AllKeyFlag(_bool bOn)
{
	if (bOn)
		m_FKeys = MOVE | JUMP | DASH | SPECIAL | COMBO | SKILL1 | SKILL2 | INTERACT | GUN;

	else
	{
		m_FPreKeys = m_FKeys;
		m_FKeys = 0;
	}
}

void CPlayerControlContext::Set_PreKeyFlag()
{
	m_FKeys = m_FPreKeys;
}

void CPlayerControlContext::Set_AttackLanded()
{
	m_bIsAttackLanded = true;
}

_bool CPlayerControlContext::Is_LeftAttackPressed()
{
	if (Engine_Utils::Has_Flag(m_FKeys, KEYFLAGS::COMBO) &&
		m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::LB))
		return true;

	return false;
}

_bool CPlayerControlContext::Is_RightAttackPressed()
{
	if (Engine_Utils::Has_Flag(m_FKeys, KEYFLAGS::GUN) &&
		m_pGameInstance->Mouse_Pressing(MOUSEKEYSTATE::RB))
		return true;

	return false;
}

_bool CPlayerControlContext::Is_MovePressed()
{
	if (Engine_Utils::Has_Flag(m_FKeys, KEYFLAGS::MOVE) &&
		(m_pGameInstance->KeyButton_Pressing(DIK_W) ||
			m_pGameInstance->KeyButton_Pressing(DIK_A) ||
			m_pGameInstance->KeyButton_Pressing(DIK_S) ||
			m_pGameInstance->KeyButton_Pressing(DIK_D)))
		return true;

	return false;
}

_bool CPlayerControlContext::Is_JumpPressed()
{
	if (Engine_Utils::Has_Flag(m_FKeys, KEYFLAGS::JUMP) &&
		m_pGameInstance->KeyButton_Down(DIK_SPACE))
		return true;

	return false;
}

_bool CPlayerControlContext::Is_WirePressed()
{
	if (Engine_Utils::Has_Flag(m_FKeys, KEYFLAGS::INTERACT) &&
		m_pGameInstance->KeyButton_Down(DIK_F))
		return true;

	return false;
}

_bool CPlayerControlContext::Is_DodgePressed()
{
	if (Engine_Utils::Has_Flag(m_FKeys, KEYFLAGS::SPECIAL) &&
		(m_pGameInstance->KeyButton_Up(DIK_LCONTROL) || m_pGameInstance->KeyButton_None(DIK_LCONTROL)))
		return true;

	return false;
}

_bool CPlayerControlContext::Is_InteractionPressed()
{
	if (Engine_Utils::Has_Flag(m_FKeys, KEYFLAGS::INTERACT) &&
		m_pGameInstance->KeyButton_Down(DIK_F))
		return true;

	return false;
}

_bool CPlayerControlContext::Is_DashPressed()
{
	if (Engine_Utils::Has_Flag(m_FKeys, KEYFLAGS::DASH) &&
		m_pGameInstance->KeyButton_Down(DIK_LSHIFT))
	{
		static_cast<CPlayer*>(Get_Owner())->Count_Dash();

		return true;
	}

	return false;
}

_bool CPlayerControlContext::Is_SepcialMovePressed()
{
	if (Engine_Utils::Has_Flag(m_FKeys, KEYFLAGS::SPECIAL) &&
		m_pGameInstance->KeyButton_Down(DIK_LCONTROL))
		return true;

	return false;
}

_bool CPlayerControlContext::Is_ComboAtt1Pressed()
{
	if (Engine_Utils::Has_Flag(m_FKeys, KEYFLAGS::COMBO) &&
		m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::LB))
		return true;

	return false;
}

_bool CPlayerControlContext::Is_ComboAtt2Pressed()
{
	if (Engine_Utils::Has_Flag(m_FKeys, KEYFLAGS::COMBO) &&
		m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::LB))
		return true;

	return false;
}

_bool CPlayerControlContext::Is_ComboAtt3Pressed()
{
	if (Engine_Utils::Has_Flag(m_FKeys, KEYFLAGS::COMBO) &&
		m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::LB))
		return true;

	return false;
}

_bool CPlayerControlContext::Is_ComboAtt4Pressed()
{
	if (Engine_Utils::Has_Flag(m_FKeys, KEYFLAGS::COMBO) &&
		m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::LB))
		return true;

	return false;
}

_bool CPlayerControlContext::Is_Skill1Pressed()
{
	if (Engine_Utils::Has_Flag(m_FKeys, KEYFLAGS::SKILL1) &&
		m_pGameInstance->KeyButton_Down(DIK_E))
		return true;

	return false;
}

_bool CPlayerControlContext::Is_Skill2Pressed()
{
	if (Engine_Utils::Has_Flag(m_FKeys, KEYFLAGS::SKILL2) &&
		m_pGameInstance->KeyButton_Down(DIK_Q))
		return true;

	return false;
}

_bool CPlayerControlContext::Is_ChargingAttackPressed()
{
	return m_pGameInstance->Mouse_Pressing(MOUSEKEYSTATE::LB);
}

_bool CPlayerControlContext::Is_AttackLanded()
{
	_bool result = m_bIsAttackLanded;
	m_bIsAttackLanded = { false };

	return result;
}

Vec3 CPlayerControlContext::Get_MoveDir()
{
	if (!m_pOwnerTargetCamera)
	{
		if (!(m_pOwnerTargetCamera = Get_Owner()->Get_CameraTargeter()))
			return Vec3::Zero;
	}

	_bool bGround = (Is_WallMode() == false);
	CTransform* pCameraTransform = m_pOwnerTargetCamera->Get_Component<CTransform>();
	
	CTransform* pTransform = Get_Owner()->Get_Component<CTransform>();
	if (!pCameraTransform || !pTransform)
		return Vec3::Zero;

	Vec3 vDesiredDir = Vec3::Zero;
	Vec3 vCameraLook = pCameraTransform->Get_Info(TRANSFORM_INFO_STATE::LOOK);
	Vec3 vCameraRight = pCameraTransform->Get_Info(TRANSFORM_INFO_STATE::RIGHT);
	Vec3 vCameraLookXZ = vCameraLook;
	vCameraLook.y = 0.0f;
	vCameraLook.Normalize();
	Vec3 vCameraRightXZ = vCameraRight;
	vCameraRight.y = 0.0f;
	vCameraRight.Normalize();

	if (bGround)
	{
		if (KEY_BUTTON_HOLD(DIK_W))  vDesiredDir += vCameraLookXZ;
		else if (KEY_BUTTON_HOLD(DIK_S)) vDesiredDir -= vCameraLookXZ;

		if (KEY_BUTTON_HOLD(DIK_D)) vDesiredDir += vCameraRightXZ;
		else if (KEY_BUTTON_HOLD(DIK_A)) vDesiredDir -= vCameraRightXZ;
	}
	else
	{
		Vec3 vPlayerUp = pTransform->Get_Info(TRANSFORM_INFO_STATE::UP);
		Vec3 vCamLookOnWall = vCameraLook - (vPlayerUp * vCameraLook.Dot(vPlayerUp));
		Vec3 vCamRightOnWall = vCameraRight - (vPlayerUp * vCameraRight.Dot(vPlayerUp));

		if (vCamLookOnWall.LengthSquared() <= g_XMEpsilon.f[0])
		{
			vCamLookOnWall = pTransform->Get_Info(TRANSFORM_INFO_STATE::LOOK);
			vCamRightOnWall = pTransform->Get_Info(TRANSFORM_INFO_STATE::RIGHT);
		}

		vCamLookOnWall.Normalize();
		vCamRightOnWall.Normalize();

		if (KEY_BUTTON_HOLD(DIK_W))  vDesiredDir += vCamLookOnWall;
		else if (KEY_BUTTON_HOLD(DIK_S)) vDesiredDir -= vCamLookOnWall;

		if (KEY_BUTTON_HOLD(DIK_D)) vDesiredDir += vCamRightOnWall;
		else if (KEY_BUTTON_HOLD(DIK_A)) vDesiredDir -= vCamRightOnWall;
	}

	return vDesiredDir;
}

void CPlayerControlContext::Clear_WhenChangeLevel()
{
	m_pOwnerTargetCamera = nullptr;
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