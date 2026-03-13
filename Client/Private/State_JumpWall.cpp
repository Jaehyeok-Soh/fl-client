#include "pch.h"
#include "State_JumpWall.h"

#include "Player.h"

#include "PhysicsCCT.h"

CState_JumpWall::CState_JumpWall(CActionState* pOwnerComponent)
	: Super(pOwnerComponent, "JumpWall")
{
}

HRESULT CState_JumpWall::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_JumpWall::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_JumpWall::Start(void* pArg, _bool bForce)
{
	if (FAILED(Super::Start(pArg, bForce)))
		return E_FAIL;

	Set_ApplyGravity(false);

	//// 03/05 소재혁 추가
	//{
	//	CTransform* pPlayerTrans = Get_OwnerObject()->Get_Component<CTransform>();
	//	CPhysicsCCT* pCCT = Get_OwnerObject()->Get_Component<CPhysicsCCT>();

	//	Vec3 vUp = (pPlayerTrans->Get_Info(TRANSFORM_INFO_STATE::UP));

	//	Vec3 accelation = vUp;

	//	SetCCTImpuls(accelation);
	//	Set_ZeroVerticalVelocity();
	//}
	Jump_Impuls(0.5f);
	Look_Impuls(-2.f);
	Set_ZeroVerticalVelocity();

	return S_OK;
}

void CState_JumpWall::Update(const _float fTimeDelta)
{
	// 바닥 충돌 검사 후 change
	if (m_fStateElapsed > 0.6f &&
		Check_OnGround(0.3f))
	{
		Change_PlayerState(ENUM_TO_UINT(CPlayer::State::LAND));
		return;
	}


	Super::Update(fTimeDelta);
}

HRESULT CState_JumpWall::End()
{
	if (FAILED(Super::End()))
		return E_FAIL;

	Set_ApplyGravity(true);

	return S_OK;
}

void CState_JumpWall::Set_NextStateDesc(_uint iNextState)
{
	switch (iNextState)
	{
	case ENUM_TO_UINT(CPlayer::State::SLIDE):
		m_tNextStateDesc.iMainAnimIdx = 1; break; // skyslide

	default:
		m_tNextStateDesc.iMainAnimIdx = 0;
	}
}


CState_JumpWall* CState_JumpWall::Create(CActionState* pOwnerComponent, void* pArg)
{
	CState_JumpWall* pInstance = new CState_JumpWall(pOwnerComponent);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CState_JumpWall::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CState_JumpWall::Free()
{
	Super::Free();
}
