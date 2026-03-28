#include "pch.h"
#include "State_GimmikAttack.h"
#include "GameObject.h"
#include "PhysicsCCT.h"
#include "MonsterActionState.h"
#include "MonsterControlContext.h"
#include "GameInstance.h"

CState_GimmikAttack::CState_GimmikAttack(CActionState* pOwnerComponent, _uint iStateIndex)
	: Super(pOwnerComponent, "GimmikAttack", iStateIndex)
{
}

HRESULT CState_GimmikAttack::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_GimmikAttack::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	CGameObject* pGo = Get_OwnerObject();
	m_pOwnerActionState = pGo->Get_Component<CMonsterActionState>();
	m_pOwnerControlContext = pGo->Get_Component<CMonsterControlContext>();
	return S_OK;
}

HRESULT CState_GimmikAttack::Start(void* pArg, _bool bForce)
{
	if (FAILED(Super::Start(pArg, bForce)))
		return E_FAIL;

	CGameObject* pTarget = Get_Target();
	if (pTarget == nullptr)
		return E_FAIL;

	CTransform* pOwnerTransform = Get_OwnerObject()->Get_Component<CTransform>();
	CPhysicsCCT* pOwnerCCT = Get_OwnerObject()->Get_Component<CPhysicsCCT>();
	CTransform* pTargetTransform = pTarget->Get_Component<CTransform>();
	if (pOwnerTransform == nullptr || pOwnerCCT == nullptr || pTargetTransform == nullptr)
		return E_FAIL;

	Vec3 vPlayerPos = pTargetTransform->Get_Info(TRANSFORM_INFO_STATE::POS);

	const _float fSpawnDist = 14.f;
	_int iRand = m_pGameInstance->Rand_Int(0, 3);
	Vec3 vRandDir{Vec3::Zero};
	if (iRand == 0)
	{
		vRandDir.x = 1.f;
	}
	else if (iRand == 1)
	{
		vRandDir.x = -1.f;
	}
	else if (iRand == 2)
	{
		vRandDir.z = 1.f;
	}
	else if (iRand == 3)
	{
		vRandDir.z = -1.f;
	}
	Vec3 vSpawnPos = vPlayerPos - vRandDir * fSpawnDist;
	vSpawnPos.y = vPlayerPos.y;

	pOwnerTransform->Set_Info(TRANSFORM_INFO_STATE::POS, vSpawnPos);
	pOwnerCCT->SetFootPosition(vSpawnPos);
	SetupLookAt(vPlayerPos);
	m_iFlag = 0;
	return S_OK;
}

void CState_GimmikAttack::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
	if (Is_AnimFinished())
	{
		Change_MonsterState(m_umapState["Idle"]);
		return;
	}

	if ((Engine_Utils::Has_Flag(m_iFlag, 1) == false) && Is_AnimTrackPositionBetweenRaw(17.f, 20.f))
	{
		Engine_Utils::Add_Flag(m_iFlag, 1);
		Set_ApplyGravity(false);
		Set_ZeroDeAccelRate();
	}

	if ((Engine_Utils::Has_Flag(m_iFlag, 1 << 1) == false) && Is_AnimTrackPositionBetweenRaw(55.f, 65.f))
	{
		Engine_Utils::Add_Flag(m_iFlag, 1 << 1);
		Set_ApplyGravity(true);
		Set_ZeroDeAccelRate();
	}
}

HRESULT CState_GimmikAttack::End()
{
	Set_ApplyGravity(true);
	Reset_DeAccelRate();
	return Super::End();
}

CState_GimmikAttack* CState_GimmikAttack::Create(CActionState* pOwnerComponent, _uint iStateIndex, void* pArg)
{
	CState_GimmikAttack* pInstance = new CState_GimmikAttack(pOwnerComponent, iStateIndex);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CState_GimmikAttack::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CState_GimmikAttack::Free()
{
	Super::Free();
}
