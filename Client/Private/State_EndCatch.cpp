#include "pch.h"
#include "State_EndCatch.h"
#include "GameObject.h"
#include "PhysicsCCT.h"
#include "MonsterActionState.h"
#include "MonsterControlContext.h"
#include "CameraPreset_Manager.h"
#include "Boss_Lianhuo.h"
#include "Player.h"
#include "GameInstance.h"

CState_EndCatch::CState_EndCatch(CActionState* pOwnerComponent, _uint iStateIndex)
	: Super(pOwnerComponent, "EndCatch", iStateIndex)
{
}

HRESULT CState_EndCatch::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	CGameObject* pGo = Get_OwnerObject();
	m_pOwnerActionState = pGo->Get_Component<CMonsterActionState>();
	m_pOwnerControlContext = pGo->Get_Component<CMonsterControlContext>();
	return S_OK;
}

HRESULT CState_EndCatch::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_EndCatch::Start(void* pArg, _bool bForce)
{
	if (FAILED(Super::Start(pArg, bForce)))
		return E_FAIL;

	CBoss_Lianhuo* pOwner = static_cast<CBoss_Lianhuo*>(Get_OwnerObject());
	CGameObject* pTarget = Get_Target();
	CTransform* pOwnerTransform = pOwner->Get_Component<CTransform>();
	CTransform* pTargetTransform = pTarget->Get_Component<CTransform>();
	CPhysicsCCT* pOwnerCCT = pOwner->Get_Component<CPhysicsCCT>();
	CPhysicsCCT* pTargetCCT = Get_Target()->Get_Component<CPhysicsCCT>();
	// SetCapsule
	{
		static_cast<PxCapsuleController*>(pOwnerCCT->GetController())->setRadius(0.05f);
	}

	Matrix matPlayerLoc = Matrix::Identity;
	Vec3 vResultPosition = Vec3::One;
	const Matrix& matWorld = pOwnerTransform->Get_WorldMatrix();

	// SetBone
	{
		Vec3 vOwnerPosition = matWorld.Translation();
		Matrix matLoc = (*(pOwner->Get_PlayerLocBonePosition()));
		matPlayerLoc = matLoc * matWorld;
		Vec3 vLocPosition = matPlayerLoc.Translation();
		vLocPosition.y = vOwnerPosition.y;
		Vec3 vToLook = vLocPosition - vOwnerPosition;
		_float fDistance = vToLook.Length();
		fDistance += 2.2f;
		vToLook.Normalize();

		vResultPosition = vOwnerPosition + vToLook * fDistance;
	}
		
	pTargetTransform->Set_Info(TRANSFORM_INFO_STATE::POS, vResultPosition);
	pTargetCCT->SetFootPosition(vResultPosition);
	pTargetTransform->Look_At_XZ(pOwnerTransform->Get_Info(TRANSFORM_INFO_STATE::POS));
	Get_Target()->Get_Component<CActionState>()->Change_State(ENUM_TO_UINT(CPlayer::State::SPHIT_START));
	CCameraPreset_Manager::GetInstance()->Play_Preset("GimmikCamera02", pOwner);
	return S_OK;
}

void CState_EndCatch::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
	if (Is_AnimFinished())
	{
		Change_MonsterState(m_umapState["Idle"]);
		return;
	}
	if (Is_AnimTrackPositionBetweenRaw(27.f, 37.f))
	{
		Set_ApplyGravity(false);
		Set_ZeroDeAccelRate();
	}
	else if (Is_AnimTrackPositionBetweenRaw(54.f, 65.f))
	{
		Set_ApplyGravity(true);
		Reset_DeAccelRate();
	}
}

HRESULT CState_EndCatch::End()
{
	CPhysicsCCT* pCCT = Get_OwnerObject()->Get_Component<CPhysicsCCT>();
	static_cast<PxCapsuleController*>(pCCT->GetController())->setRadius(0.5f);
	pCCT->SetHeight(2.5f);
	Set_ApplyGravity(true);
	Reset_DeAccelRate();
	return Super::End();
}

CState_EndCatch* CState_EndCatch::Create(CActionState* pOwnerComponent, _uint iStateIndex, void* pArg)
{
	CState_EndCatch* pInstance = new CState_EndCatch(pOwnerComponent, iStateIndex);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CState_EndCatch::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CState_EndCatch::Free()
{
	Super::Free();
}
