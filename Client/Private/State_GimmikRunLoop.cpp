#include "pch.h"
#include "State_GimmikRunLoop.h"
#include "GameObject.h"
#include "PhysicsCCT.h"
#include "Lianhuo_GimmikController.h"
#include "MonsterActionState.h"
#include "MonsterControlContext.h"
#include "GameInstance.h"

CState_GimmikRunLoop::CState_GimmikRunLoop(CActionState* pOwnerComponent, _uint iStateIndex)
	: Super(pOwnerComponent, "GimmikRunLoop", iStateIndex)
{
}

HRESULT CState_GimmikRunLoop::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_GimmikRunLoop::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	CGameObject* pGo = Get_OwnerObject();
	m_pOwnerActionState = pGo->Get_Component<CMonsterActionState>();
	m_pOwnerControlContext = pGo->Get_Component<CMonsterControlContext>();
	return S_OK;
}

HRESULT CState_GimmikRunLoop::Start(void* pArg, _bool bForce)
{
	if (FAILED(Super::Start(pArg, bForce)))
		return E_FAIL;
	
	CGameObject* pGo = Get_OwnerObject();
	CTransform* pTransform = pGo->Get_Component<CTransform>();
	CPhysicsCCT* pCCT = pGo->Get_Component<CPhysicsCCT>();
	if (pTransform == nullptr || pCCT == nullptr)
		return E_FAIL;

	m_iDashIndex = 0;
	m_ePhase = ERunPhase::Dash;
	m_bPathReady = false;
	m_fHoldAcc = 0.f;

	m_pOwnerActionState->Set_ZeroHorizontalVelocity();
	m_pOwnerControlContext->Set_RootMotion_Apply(false);

	CLianhuo_GimmikController* pGimmik = pGo->Get_Component<CLianhuo_GimmikController>();
	m_vFieldCenter = pGimmik->Get_BattleFieldCenter();
	m_fFieldRadius = pGimmik->Get_BattleFiledMaxRange() / 2.f;

	m_bPathReady = Build_DashLines(0);
	if (m_bPathReady == false)
		return E_FAIL;

	pCCT->SetFootPosition(m_arrDashLine[m_iDashIndex].vStart);
	Resolve_DashDirection();
	SetupLookAt(m_arrDashLine[m_iDashIndex].vEnd);
	Spawn_Line(m_arrDashLine[m_iDashIndex].vStart, m_arrDashLine[m_iDashIndex].vEnd);
	pGo->Set_Render(true);
	Get_OwnerObject()->Play_GhostTrail();
	return S_OK;
}

void CState_GimmikRunLoop::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
	CGameObject* pOwner = Get_OwnerObject();
	CTransform* pTransform = pOwner->Get_Component<CTransform>();
	CPhysicsCCT* pCCT = pOwner->Get_Component<CPhysicsCCT>();
	if (pTransform == nullptr || pCCT == nullptr)
		return;

	if (m_bPathReady == false)
	{
		Change_MonsterState(m_umapState["Idle"]);
		return;
	}

	switch (m_ePhase)
	{
	case ERunPhase::Dash:
	{
		Vec3 vCurPos = pTransform->Get_Info(TRANSFORM_INFO_STATE::POS);
		Vec3 vTarget = m_arrDashLine[m_iDashIndex].vEnd;
		Vec3 vToTarget = vTarget - vCurPos;
		vToTarget.y = 0.f;

		_float fRemain = vToTarget.Length();

		if (fRemain <= m_fArriveTolerance)
		{
			m_pOwnerActionState->Set_ZeroHorizontalVelocity();
			pCCT->SetFootPosition(vTarget); // 끝점 스냅
			pOwner->Set_Render(false);

			m_fHoldAcc = 0.f;
			m_ePhase = ERunPhase::Hold;
			return;
		}
		
		SetupLookAt(vTarget);
		if (vToTarget.LengthSquared() > g_XMEpsilon.f[0])
			vToTarget.Normalize();

		m_pOwnerActionState->SetCCTImpuls(vToTarget * m_fDashMovePerSec);
		break;
	}
	case ERunPhase::Hold:
	{
		m_fHoldAcc += fTimeDelta;

		if (m_fHoldAcc >= m_fHoldTime)
		{
			++m_iDashIndex;

			if (m_iDashIndex >= DASH_COUNT ||
				Build_DashLines(m_iDashIndex) == false)
			{
				Change_MonsterState(m_umapState["GimmikAttack"]);
				return;
			}

			pCCT->SetFootPosition(m_arrDashLine[m_iDashIndex].vStart);
			Resolve_DashDirection();
			SetupLookAt(m_arrDashLine[m_iDashIndex].vEnd);
			pOwner->Set_Render(true);
			Spawn_Line(m_arrDashLine[m_iDashIndex].vStart, m_arrDashLine[m_iDashIndex].vEnd);
			m_ePhase = ERunPhase::Dash;
			Get_OwnerObject()->Play_GhostTrail();
		}
		break;
	}
	}
}

HRESULT CState_GimmikRunLoop::End()
{
	Get_OwnerObject()->Set_Render(true);
	m_pOwnerActionState->Set_ZeroHorizontalVelocity();
	// m_pOwnerControlContext->Set_RootMotion_Apply(true);
	return Super::End();
}

Vec3 CState_GimmikRunLoop::NormalizeXZ(Vec3 v)
{
	v.y = 0.f;
	if (v.LengthSquared() > g_XMEpsilon.f[0])
		v.Normalize();
	return v;
}

_float CState_GimmikRunLoop::LengthXZ(const Vec3& v)
{
	Vec3 vT = v;
	vT.y = 0.f;
	return vT.Length();
}

_float CState_GimmikRunLoop::DistanceXZ(const Vec3& vA, const Vec3& vB)
{
	Vec3 vResult = vA - vB;
	vResult.y = 0.f;
	return vResult.Length();
}

_float CState_GimmikRunLoop::AngleXZ(const Vec3& vFrom, const Vec3& vTo)
{
	Vec3 v = vTo - vFrom;
	v.y = 0.f;
	return atan2f(v.z, v.x);
}

Vec3 CState_GimmikRunLoop::PointOnCircleXZ(const Vec3& vCenter, _float fRadius, _float fAngle)
{
	Vec3 v = vCenter;
	v.x += cosf(fAngle) * fRadius;
	v.z += sinf(fAngle) * fRadius;
	return v;
}

void CState_GimmikRunLoop::Resolve_DashDirection()
{
	m_vDashDir = m_arrDashLine[m_iDashIndex].vEnd - m_arrDashLine[m_iDashIndex].vStart;
	m_vDashDir.y = 0.f;

	if (m_vDashDir.LengthSquared() > g_XMEpsilon.f[0])
		m_vDashDir.Normalize();
}

void CState_GimmikRunLoop::Spawn_Line(const Vec3& vStart, const Vec3& vEnd)
{
	_float fDistance = 0.2f;
	Vec3 vS = vStart;
	Vec3 vE = vEnd;
	vS.y += fDistance;
	vE.y += fDistance;
	_uint iLevelID = ENUM_TO_UINT(ELevelType::LIANHUO);
	EFFECT_LINE_DESC desc{};
	desc.Set_LinePosition(vS, vE);
	m_pGameInstance->Request_AddObject(
		iLevelID,
		L"POOL_Boss_LianHuo_DashPanel",
		iLevelID,
		&desc);
}

Vec3 CState_GimmikRunLoop::Sample_PlayerPosition()
{
	CGameObject* pTarget = Get_Target();
	if (pTarget == nullptr)
		return m_vFieldCenter;
	CTransform* pTargetTransform = pTarget->Get_Component<CTransform>();
	return pTargetTransform->Get_Info(TRANSFORM_INFO_STATE::POS);
}

_bool CState_GimmikRunLoop::Build_DashLines(_uint iIndex)
{
	_float fAxisAngle = 0.f;
	const _float kPlayerTrackChance = 0.7f;

	_bool bTrackPlayer = (m_pGameInstance->Rand_Float(0.f, 1.f) < kPlayerTrackChance);
	if (bTrackPlayer)
	{
		// 플레이어 방향 기준 축
		Vec3 vPlayer = Sample_PlayerPosition();
		fAxisAngle = AngleXZ(m_vFieldCenter, vPlayer);

		// 조준 지터: 너무 정확하면 단조로우니 25 이내로 흔들기
		fAxisAngle += XMConvertToRadians(
			m_pGameInstance->Rand_Float(-25.f, 25.f));
	}
	else
	{
		// 완전 랜덤 축
		fAxisAngle = XMConvertToRadians(
			m_pGameInstance->Rand_Float(0.f, 180.f));
	}

	// 축은 [0, PI) 범위로 정규화 (방향 아닌 축이므로)
	while (fAxisAngle >= XM_PI)  fAxisAngle -= XM_PI;
	while (fAxisAngle < 0.f)    fAxisAngle += XM_PI;

	// 시작/끝 반지름: 경계 위 or 살짝 바깥
	// 0.95 ~ 1.10 범위 → 원 안쪽으로 파고들지 않음
	_float fStartRadius = m_fFieldRadius *
		m_pGameInstance->Rand_Float(0.95f, 1.10f);
	_float fEndRadius = m_fFieldRadius *
		m_pGameInstance->Rand_Float(0.95f, 1.10f);

	// 반대쪽 끝점에도 소량 지터 → 완전한 지름이 아닌 비대칭 현(chord)
	_float fEndJitter = XMConvertToRadians(
		m_pGameInstance->Rand_Float(-15.f, 15.f));

	m_arrDashLine[iIndex].vStart =
		PointOnCircleXZ(m_vFieldCenter, fStartRadius, fAxisAngle);
	m_arrDashLine[iIndex].vEnd =
		PointOnCircleXZ(m_vFieldCenter, fEndRadius, fAxisAngle + XM_PI + fEndJitter);

	return true;
}

CState_GimmikRunLoop* CState_GimmikRunLoop::Create(CActionState* pOwnerComponent, _uint iStateIndex, void* pArg)
{
	CState_GimmikRunLoop* pInstance = new CState_GimmikRunLoop(pOwnerComponent, iStateIndex);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CState_GimmikRunLoop::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CState_GimmikRunLoop::Free()
{
	Super::Free();
}
