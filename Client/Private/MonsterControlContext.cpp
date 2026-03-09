#include "pch.h"
#include "MonsterControlContext.h"
#include "Client_Defines.h"
#include "Monster_Base.h"
#include "MonsterActionState.h"
#include "PhysicsCCT.h"
#include "Engine_Utils.h"
#include "GameInstance.h"


CMonsterControlContext::CMonsterControlContext()
	: Super()
{
}

CMonsterControlContext::CMonsterControlContext(const CMonsterControlContext& rhs)
	: Super(rhs)
{
}

HRESULT CMonsterControlContext::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonsterControlContext::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	if (pArg)
	{
		MONSTER_CONTROLCONTEXT_DESC* pDesc = static_cast<MONSTER_CONTROLCONTEXT_DESC*>(pArg);
		m_tDesc = *pDesc;
	}

	return S_OK;
}

HRESULT CMonsterControlContext::Awake(const _uint iLevelIndex)
{
	Safe_Release(m_pTarget);
	m_pTarget = m_pGameInstance->Get_GameObject_Front(/* static */ 0, L"Player_Layer");
	if (m_pTarget != nullptr)
		Safe_AddRef(m_pTarget);
	m_iSubState = 0;
	return S_OK;
}

void CMonsterControlContext::Update_RuntimeDesc(const _float fTiemDelta)
{
	// 타겟의 유효성 체크
	if (m_pTarget == nullptr)
		return;

	if (m_pTarget->IsDead() == true)
	{
		Clear_RuntimeDesc();
		Safe_Release(m_pTarget);
		m_pTarget = nullptr;
		return;
	}

	// 오너의 유효성 체크
	CGameObject* pOwner = Get_Owner();
	if (pOwner == nullptr)
		return;

	if (pOwner->IsDead() == true)
	{
		Clear_RuntimeDesc();
		return;
	}

	CTransform* pOwnerTransform = pOwner->Get_Component<CTransform>();
	m_tRuntimeDesc.vOwnerPos = pOwnerTransform->Get_Info(TRANSFORM_INFO_STATE::POS);
	m_tRuntimeDesc.vOwnerLook = pOwnerTransform->Get_Info(TRANSFORM_INFO_STATE::LOOK);
	m_tRuntimeDesc.vOwnerRight = pOwnerTransform->Get_Info(TRANSFORM_INFO_STATE::RIGHT);

	if (m_tRuntimeDesc.vOwnerLook != Vec3::Zero)
		m_tRuntimeDesc.vOwnerLook.Normalize();
	if (m_tRuntimeDesc.vOwnerRight != Vec3::Zero)
		m_tRuntimeDesc.vOwnerRight.Normalize();

	m_tRuntimeDesc.bTargetValid = (m_pTarget != nullptr && (m_pTarget->IsDead() == false));

	if (m_tRuntimeDesc.bTargetValid)
	{
		CTransform* pTargetTr = m_pTarget->Get_Component<CTransform>();
		m_tRuntimeDesc.vTargetPos = pTargetTr->Get_Info(TRANSFORM_INFO_STATE::POS);
		m_tRuntimeDesc.vToTarget = m_tRuntimeDesc.vTargetPos - m_tRuntimeDesc.vOwnerPos;
		m_tRuntimeDesc.fDistance = m_tRuntimeDesc.vToTarget.Length();

		if (m_tRuntimeDesc.fDistance > g_XMEpsilon.f[0])
		{
			m_tRuntimeDesc.vToTargetDir = m_tRuntimeDesc.vToTarget / m_tRuntimeDesc.fDistance;
			m_tRuntimeDesc.fDotForward = m_tRuntimeDesc.vOwnerLook.Dot(m_tRuntimeDesc.vToTargetDir);
		}
		else
		{
			m_tRuntimeDesc.vToTargetDir = m_tRuntimeDesc.vOwnerLook;
			m_tRuntimeDesc.fDotForward = 1.f;
		}
	}
	else
		Clear_RuntimeDesc();
}

Vec3 CMonsterControlContext::Get_MoveDir()
{
	return m_vMoveDir;
}

void CMonsterControlContext::Set_Groggy(_bool b)
{
	b == true
		? Engine_Utils::Add_Flag(m_iSubState, SUB_STATE::GROGGY)
		: Engine_Utils::RemoveHard_Flag(m_iSubState, SUB_STATE::GROGGY);
}

void CMonsterControlContext::Set_Dead()
{
	if (IsDeadProcessing())
		return;

	m_iSubState |= SUB_STATE::DEAD;
}

_bool CMonsterControlContext::IsTargetVisible()
{
	return _bool();
}

_bool CMonsterControlContext::IsTargetFOV()
{
	return _bool();
}

_bool CMonsterControlContext::IsCliffAhead()
{
	return _bool();
}

_bool CMonsterControlContext::IsPhaseTwo()
{
	return _bool();
}

_bool CMonsterControlContext::IsFalling()
{
	_bool result = m_iSubState & SUB_STATE::FALL;
	m_iSubState &= ~SUB_STATE::FALL;
	return result;
}

_bool CMonsterControlContext::IsDown()
{
	_bool result = m_iSubState & SUB_STATE::DOWN;
	m_iSubState &= ~SUB_STATE::DOWN;
	return result;
}

_bool CMonsterControlContext::IsHit()
{
	if (m_tHitDesc.attackDesc.pAttackPreset == nullptr)
		return false;

	return m_iSubState & SUB_STATE::HIT;
}

_bool CMonsterControlContext::IsHitAdditive()
{
	_bool result;
	if ((result = IsHit()) == false)
		return result;

	if (result = (m_tHitDesc.attackDesc.pAttackPreset->tCombat.eHitType == DTO::EHitType::Additive))
		m_iSubState &= ~SUB_STATE::HIT;

	return result;
}

_bool CMonsterControlContext::IsHitLight()
{
	_bool result;
	if ((result = IsHit()) == false)
		return result;

	if (result = (m_tHitDesc.attackDesc.pAttackPreset->tCombat.eHitType == DTO::EHitType::Light))
		m_iSubState &= ~SUB_STATE::HIT;

	return result;
}

_bool CMonsterControlContext::IsHitHeavy()
{
	_bool result;
	if ((result = IsHit()) == false)
		return result;

	if (result = (m_tHitDesc.attackDesc.pAttackPreset->tCombat.eHitType == DTO::EHitType::Heavy))
		m_iSubState &= ~SUB_STATE::HIT;

	return result;
}

_bool CMonsterControlContext::IsHitLaunch()
{
	_bool result;
	if ((result = IsHit()) == false)
		return result;

	if (result = (m_tHitDesc.attackDesc.pAttackPreset->tCombat.eHitType == DTO::EHitType::Launch))
		m_iSubState &= ~SUB_STATE::HIT;

	return result;
}

_bool CMonsterControlContext::IsHitKnockdown()
{
	_bool result;
	if ((result = IsHit()) == false)
		return result;

	if (result = (m_tHitDesc.attackDesc.pAttackPreset->tCombat.eHitType == DTO::EHitType::Knockdown))
		m_iSubState &= ~SUB_STATE::HIT;

	return result;
}

_bool CMonsterControlContext::IsDead()
{
	_bool result = m_iSubState & SUB_STATE::DEAD;
	return result;
}

_bool CMonsterControlContext::IsDeadProcessing()
{
	_bool result = m_iSubState & SUB_STATE::DEAD_PROCESS;
	return result;
}

_bool CMonsterControlContext::IsDamageRecently()
{
	return _bool();
}

_bool CMonsterControlContext::IsComboPossible()
{
	return _bool();
}

_bool CMonsterControlContext::CanAttackMelee()
{
	return false;
}

_bool CMonsterControlContext::CanAttackRanged()
{
	return false;
}

_bool CMonsterControlContext::IsSkillReady()
{
	return _bool();
}

_bool CMonsterControlContext::IsArrived()
{
	return _bool();
}

_bool CMonsterControlContext::IsPathBlocked()
{
	return _bool();
}

void CMonsterControlContext::UpdateWalk(const _float fTimeDelta)
{
	CTransform* pOwnerTransform = Get_Owner()->Get_Component<CTransform>();

	Vec3 vOwnerLook = pOwnerTransform->Get_Info(TRANSFORM_INFO_STATE::LOOK);
	vOwnerLook.Normalize();
	vOwnerLook* fTimeDelta;

	m_vMoveDir = vOwnerLook;
}

void CMonsterControlContext::UpdateChase(const _float fTimeDelta)
{
	CTransform* pOwnerTransform = Get_Owner()->Get_Component<CTransform>();
	CTransform* pTargetTransform = m_pTarget->Get_Component<CTransform>();

	Vec3 vOwnerPosition = pOwnerTransform->Get_Info(TRANSFORM_INFO_STATE::POS);
	Vec3 vTargetPosition = pTargetTransform->Get_Info(TRANSFORM_INFO_STATE::POS);

	Vec3 vToTarget = vTargetPosition - vOwnerPosition;
	vToTarget.Normalize();

	m_vMoveDir = vToTarget;
}

void CMonsterControlContext::Update_TurnToTarget_XZ(const _float fTimeDelta)
{
	CGameObject* pTarget = Get_Target();
	if (pTarget == nullptr)
		return;

	CTransform* pTransform = Get_Owner()->Get_Component<CTransform>();
	Vec3 vTargetPos = pTarget->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::POS);

	pTransform->Tunr_ToPoint_YAxis(vTargetPos, fTimeDelta);
}

void CMonsterControlContext::Update_8Dir_LocalAxisXZ(const _float fTimeDelta, _float fForward, _float fRight)
{
	CTransform* pOwnerTransform = Get_Owner()->Get_Component<CTransform>();
	Vec3 vLook = pOwnerTransform->Get_Info(TRANSFORM_INFO_STATE::LOOK);
	Vec3 vRight = pOwnerTransform->Get_Info(TRANSFORM_INFO_STATE::RIGHT);

	Vec3 vDir = vLook * fForward + vRight * fRight;
	vDir.Normalize();
	m_vMoveDir = vDir;
}

void CMonsterControlContext::UpdateRun(const _float fTimeDelta)
{
}

void CMonsterControlContext::UpdateFly(const _float fTimeDelta)
{
}

void CMonsterControlContext::UpdateFall(const _float fTimeDelta)
{
}

void CMonsterControlContext::UpdateJump(const _float fTimeDelta)
{
}

void CMonsterControlContext::UpdateDash(const _float fTimeDelta)
{
}

void CMonsterControlContext::UpdateCircleMove(const _float fTimeDelta)
{
}

void CMonsterControlContext::UpdateSideWalk(const _float fTimeDelta)
{
}

void CMonsterControlContext::UpdateTurn90(const _float fTimeDelta)
{
}

void CMonsterControlContext::UpdateTrun180(const _float fTimeDelta)
{
}

void CMonsterControlContext::Set_CCT_Collision_Disable()
{
	Get_Owner()->Get_Component<CPhysicsCCT>()->EnableCollision(false);
}

void CMonsterControlContext::Set_CCT_Collision_Enable()
{
	Get_Owner()->Get_Component<CPhysicsCCT>()->EnableCollision(true);
}

void CMonsterControlContext::Clear_RuntimeDesc()
{
	m_tRuntimeDesc.vTargetPos = {};
	m_tRuntimeDesc.vToTarget = {};
	m_tRuntimeDesc.vToTargetDir = {};
	m_tRuntimeDesc.fDistance = FLT_MAX;
	m_tRuntimeDesc.fDotForward = 0.f;
}

CMonsterControlContext* CMonsterControlContext::Create()
{
	CMonsterControlContext* pInstance = new CMonsterControlContext();
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CMonsterControlContext::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CMonsterControlContext::Clone(void* pArg)
{
	CMonsterControlContext* pInstance = new CMonsterControlContext(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CMonsterControlContext::Clone, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMonsterControlContext::Free()
{
	Safe_Release(m_pTarget);

	Super::Free();
}
