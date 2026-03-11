#include "pch.h"
#include "State_JumpBullet.h"

#include "Player.h"

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

	CStateBase::SetupLook_CameraSameLook();

	// 03/05 ¼ÒÀçÇõ Ãß°¡
	{
		CTransform* pCamTransform = Get_CamTransform();
		CTransform* pPlayerTransform = Get_OwnerObject()->Get_Component<CTransform>();
		CPhysicsCCT* pCCT = Get_OwnerObject()->Get_Component<CPhysicsCCT>();

		Vec3 vLook = (pCamTransform->Get_Info(TRANSFORM_INFO_STATE::LOOK));
		Vec3 vPos = pPlayerTransform->Get_Info(TRANSFORM_INFO_STATE::POS);

		pPlayerTransform->Look_At(vPos + vLook);

		m_vDir = vLook;
		SetCCTInputDirection(m_vDir);
		SetCCTImpuls(m_vDir * 5.f);

		// ¸¶ÂûÀ» ¾ø¾ÖÁÜ
		Set_ZeroDeAccelRate();
	}

	return S_OK;
}

void CState_JumpBullet::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

	if (Get_AnimElpasedTimeSeconds() > 0.8f)
	{
		CStateBase::SetupLook_CameraLookLerp(fTimeDelta,10.f);

		Set_ApplyGravity(true);

		Set_RootMotion_Apply(false);

		if(Check_OnGround(0.3f))
		{
			//Get_OwnerObject()->Get_Component<CTransform>()->Is_OnGround(0.1f);
			Change_PlayerState(ENUM_TO_UINT(CPlayer::State::IDLE));
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

	Set_ApplyGravity(true);
	Reset_DeAccelRate();

	CStateBase::SetupLook_CameraLook();

	return S_OK;
}

void CState_JumpBullet::Set_NextStateDesc(_uint iNextState)
{
	switch (iNextState)
	{
	case ENUM_TO_UINT(CPlayer::State::SLIDE):
		m_tNextStateDesc.iMainAnimIdx = 1; break; // skyslide

	default:
		m_tNextStateDesc.iMainAnimIdx = 0;
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
