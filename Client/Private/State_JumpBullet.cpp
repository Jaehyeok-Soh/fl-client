#include "pch.h"
#include "State_JumpBullet.h"

#include "Player.h"
#include "CameraMan.h"
#include "ControlContext.h"

#include "PhysicsCCT.h"

CState_JumpBullet::CState_JumpBullet(CActionState* pOwnerComponent)
	: Super(pOwnerComponent, "JumpBullet")
{
}

HRESULT CState_JumpBullet::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_JumpBullet::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_JumpBullet::Start(void* pArg, _bool bForce)
{
	if (FAILED(Super::Start(pArg, bForce)))
		return E_FAIL;

	Set_ApplyGravity(false);

	//CStateBase::SetupLook_CameraSameLook();

	// 03/05 º“¿Á«ı √ﬂ∞°
	{

		CPlayer* pOwner = static_cast<CPlayer*>(Get_OwnerObject());
		if (pOwner == nullptr)
			return E_FAIL;

		CTransform* pPlayerTransform = pOwner->Get_Component<CTransform>();
		CPhysicsCCT* pCCT = pOwner->Get_Component<CPhysicsCCT>();
		CTransform* pCamTransform = (pOwner->Get_CameraTargeter())->Get_Component<CTransform>();


		if (pPlayerTransform == nullptr ||
			pCCT == nullptr ||
			pCamTransform == nullptr)
			return E_FAIL;

		Vec3 vLook	= (pCamTransform->Get_Info(TRANSFORM_INFO_STATE::LOOK));
		Vec3 vPos	= pPlayerTransform->Get_Info(TRANSFORM_INFO_STATE::POS);

		vLook.Normalize();

		pPlayerTransform->Look_At_Dir(vLook);

		m_vDir = vLook;
		SetCCTInputDirection(m_vDir);
		SetCCTImpuls(m_vDir * 10.f);

		// ∏∂¬˚¿ª æ¯æ÷¡‹
		Set_ZeroDeAccelRate();
	}

	return S_OK;
}

void CState_JumpBullet::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

	if (Get_AnimElpasedTimeSeconds() > 0.8f)
	{
		Set_ApplyGravity(true);

		Set_RootMotion_Apply(false);

		CPlayer* pOwner = static_cast<CPlayer*>(Get_OwnerObject());
		if (pOwner == nullptr)
			return;

		CTransform* pPlayerTransform = pOwner->Get_Component<CTransform>();
		m_vDir.y = 0.f;
		pPlayerTransform->Look_At_Dir(m_vDir);

		if(Check_OnGround(0.3f))
		{
			//Get_OwnerObject()->Get_Component<CTransform>()->Is_OnGround(0.1f);

			if (Key_Input(ENUM_TO_UINT(CControlContext::CONTROL_KEY::MOVE)))
			{
				Change_PlayerState(ENUM_TO_UINT(CPlayer::State::WALK));
				return;
			}

			Change_PlayerState(ENUM_TO_UINT(CPlayer::State::LAND));
			return;
		}
	}

	//else
	//{
	//	Move(m_vDir * 10.f);
	//}
}

HRESULT CState_JumpBullet::End()
{
	if (FAILED(Super::End()))
		return E_FAIL;

	//CStateBase::SetupLook_CameraLook();

	CPlayer* pOwner = static_cast<CPlayer*>(Get_OwnerObject());
	if (pOwner == nullptr)
		return E_FAIL;

	CTransform* pPlayerTransform = pOwner->Get_Component<CTransform>();
	m_vDir.y = 0.f;
	pPlayerTransform->Look_At_Dir(m_vDir);

	//CStateBase::SetupLook_CameraLookLerp(0.7f, 10.f);

	Set_RootMotion_Apply(true);
	Set_ApplyGravity(true);
	Reset_DeAccelRate();

	return S_OK;
}

void CState_JumpBullet::Set_NextStateDesc(_uint iNextState)
{
	switch (iNextState)
	{
	case ENUM_TO_UINT(CPlayer::State::SLIDE):
		m_tNextStateDesc.iMainAnimIdx = 1; break; // skyslide

	default:
		Super::Set_NextStateDesc(iNextState);
	}
}


CState_JumpBullet* CState_JumpBullet::Create(CActionState* pOwnerComponent, void* pArg)
{
	CState_JumpBullet* pInstance = new CState_JumpBullet(pOwnerComponent);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CState_JumpBullet::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CState_JumpBullet::Free()
{
	Super::Free();
}
