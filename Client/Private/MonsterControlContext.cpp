#include "pch.h"
#include "MonsterControlContext.h"

#include "Client_Defines.h"
#include "Monster_Base.h"
#include "MonsterActionState.h"

#include "GameInstance.h"
#include "Engine_Utils.h"

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
	return S_OK;
}

Vec3 CMonsterControlContext::Get_MoveDir()
{
	return m_vMoveDir;
}

_bool CMonsterControlContext::IsTargetFound()
{
	CTransform* pOwnerTransform = Get_Owner()->Get_Component<CTransform>();
	CTransform* pTargetTransform = m_pTarget->Get_Component<CTransform>();

	Vec3 vOwnerPosition = pOwnerTransform->Get_Info(TRANSFORM_INFO_STATE::POS);
	Vec3 vTargetPosition = pTargetTransform->Get_Info(TRANSFORM_INFO_STATE::POS);

	Vec3 vToTarget = vTargetPosition - vOwnerPosition;
	_float fDistance = ::XMVectorGetX(::XMVector3Length(vToTarget));

	return fDistance <= m_tDesc.fDetectionRange;
}

_bool CMonsterControlContext::IsTargetLost()
{
	CTransform* pOwnerTransform = Get_Owner()->Get_Component<CTransform>();
	CTransform* pTargetTransform = m_pTarget->Get_Component<CTransform>();

	Vec3 vOwnerPosition = pOwnerTransform->Get_Info(TRANSFORM_INFO_STATE::POS);
	Vec3 vTargetPosition = pTargetTransform->Get_Info(TRANSFORM_INFO_STATE::POS);

	Vec3 vToTarget = vTargetPosition - vOwnerPosition;
	_float fDistance = ::XMVectorGetX(::XMVector3Length(vToTarget));

	return fDistance > m_tDesc.fDetectionRange;
}

_bool CMonsterControlContext::IsTargetAlive()
{
	return m_pTarget->IsDead() == false;
}

_bool CMonsterControlContext::IsTargetVisible()
{
	return _bool();
}

_bool CMonsterControlContext::IsTargetFOV()
{
	return _bool();
}

_bool CMonsterControlContext::IsTargetBehind()
{
	CTransform* pOwnerTransform = Get_Owner()->Get_Component<CTransform>();
	CTransform* pTargetTransform = m_pTarget->Get_Component<CTransform>();

	Vec3 vOwnerPosition = pOwnerTransform->Get_Info(TRANSFORM_INFO_STATE::POS);
	Vec3 vTargetPosition = pTargetTransform->Get_Info(TRANSFORM_INFO_STATE::POS);

	Vec3 vOwnerLook = pOwnerTransform->Get_Info(TRANSFORM_INFO_STATE::LOOK);
	vOwnerLook.Normalize();

	Vec3 vToTarget = vTargetPosition - vOwnerPosition;
	vToTarget.Normalize();

	return vOwnerLook.Dot(vToTarget) < 0;
}

_bool CMonsterControlContext::IsTargetSide()
{
	CTransform* pOwnerTransform = Get_Owner()->Get_Component<CTransform>();
	CTransform* pTargetTransform = m_pTarget->Get_Component<CTransform>();

	Vec3 vOwnerPosition = pOwnerTransform->Get_Info(TRANSFORM_INFO_STATE::POS);
	Vec3 vTargetPosition = pTargetTransform->Get_Info(TRANSFORM_INFO_STATE::POS);

	Vec3 vOwnerLook = pOwnerTransform->Get_Info(TRANSFORM_INFO_STATE::LOOK);
	vOwnerLook.Normalize();

	Vec3 vToTarget = vTargetPosition - vOwnerPosition;
	vToTarget.Normalize();

	_float dot = vOwnerLook.Dot(vToTarget);

	return dot >= 0.7f || dot <= -0.7f;
}

_bool CMonsterControlContext::IsTargetClose()
{
	CTransform* pOwnerTransform = Get_Owner()->Get_Component<CTransform>();
	CTransform* pTargetTransform = m_pTarget->Get_Component<CTransform>();

	Vec3 vOwnerPosition = pOwnerTransform->Get_Info(TRANSFORM_INFO_STATE::POS);
	Vec3 vTargetPosition = pTargetTransform->Get_Info(TRANSFORM_INFO_STATE::POS);

	Vec3 vToTarget = vTargetPosition - vOwnerPosition;
	_float fDistance = ::XMVectorGetX(::XMVector3Length(vToTarget));

	return fDistance <= m_tDesc.fCloseRange;
}

_bool CMonsterControlContext::IsTargetAhead()
{
	CTransform* pOwnerTransform = Get_Owner()->Get_Component<CTransform>();
	CTransform* pTargetTransform = m_pTarget->Get_Component<CTransform>();

	Vec3 vOwnerPosition = pOwnerTransform->Get_Info(TRANSFORM_INFO_STATE::POS);
	Vec3 vTargetPosition = pTargetTransform->Get_Info(TRANSFORM_INFO_STATE::POS);

	Vec3 vOwnerLook = pOwnerTransform->Get_Info(TRANSFORM_INFO_STATE::LOOK);
	vOwnerLook.Normalize();

	Vec3 vToTarget = vTargetPosition - vOwnerPosition;
	vToTarget.Normalize();

	_float dot = vOwnerLook.Dot(vToTarget);

	return dot > 0.7f;
}

_bool CMonsterControlContext::IsCliffAhead()
{
	return _bool();
}

_bool CMonsterControlContext::IsPhaseTwo()
{
	return _bool();
}

_bool CMonsterControlContext::IsTargetInMeleeRange()
{
	CTransform* pOwnerTransform = Get_Owner()->Get_Component<CTransform>();
	CTransform* pTargetTransform = m_pTarget->Get_Component<CTransform>();

	Vec3 vOwnerPosition = pOwnerTransform->Get_Info(TRANSFORM_INFO_STATE::POS);
	Vec3 vTargetPosition = pTargetTransform->Get_Info(TRANSFORM_INFO_STATE::POS);

	Vec3 vToTarget = vTargetPosition - vOwnerPosition;
	_float fDistance = ::XMVectorGetX(::XMVector3Length(vToTarget));

	return fDistance <= m_tDesc.fMeleeRange;
}

_bool CMonsterControlContext::IsTargetInAttackRange()
{
	CTransform* pOwnerTransform = Get_Owner()->Get_Component<CTransform>();
	CTransform* pTargetTransform = m_pTarget->Get_Component<CTransform>();

	Vec3 vOwnerPosition = pOwnerTransform->Get_Info(TRANSFORM_INFO_STATE::POS);
	Vec3 vTargetPosition = pTargetTransform->Get_Info(TRANSFORM_INFO_STATE::POS);

	Vec3 vToTarget = vTargetPosition - vOwnerPosition;
	_float fDistance = ::XMVectorGetX(::XMVector3Length(vToTarget));

	return fDistance <= m_tDesc.fAttackRange;
}

_bool CMonsterControlContext::IsTargetOutOfMeleeRange()
{
	return IsTargetFound() && IsTargetInMeleeRange() == false;
}

_bool CMonsterControlContext::IsTargetOutOfAttackRange()
{
	return IsTargetFound() && IsTargetInAttackRange() == false;
}

_bool CMonsterControlContext::IsFalling()
{
	return _bool();
}

_bool CMonsterControlContext::IsGrounded()
{
	return _bool();
}

_bool CMonsterControlContext::IsDown()
{
	return _bool();
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

void CMonsterControlContext::UpdateWalk(const _float& fTimeDelta)
{
	CTransform* pOwnerTransform = Get_Owner()->Get_Component<CTransform>();

	Vec3 vOwnerLook = pOwnerTransform->Get_Info(TRANSFORM_INFO_STATE::LOOK);
	vOwnerLook.Normalize();
	vOwnerLook* fTimeDelta;

	m_vMoveDir = vOwnerLook;
}

void CMonsterControlContext::UpdateChase(const _float& fTimeDelta)
{
	CTransform* pOwnerTransform = Get_Owner()->Get_Component<CTransform>();
	CTransform* pTargetTransform = m_pTarget->Get_Component<CTransform>();

	Vec3 vOwnerPosition = pOwnerTransform->Get_Info(TRANSFORM_INFO_STATE::POS);
	Vec3 vTargetPosition = pTargetTransform->Get_Info(TRANSFORM_INFO_STATE::POS);

	Vec3 vToTarget = vTargetPosition - vOwnerPosition;
	vToTarget.Normalize();

	m_vMoveDir = vToTarget;
}

void CMonsterControlContext::UpdateRun(const _float& fTimeDelta)
{
}

void CMonsterControlContext::UpdateFly(const _float& fTimeDelta)
{
}

void CMonsterControlContext::UpdateFall(const _float& fTimeDelta)
{
}

void CMonsterControlContext::UpdateJump(const _float& fTimeDelta)
{
}

void CMonsterControlContext::UpdateDash(const _float& fTimeDelta)
{
}

void CMonsterControlContext::UpdateCircleMove(const _float& fTimeDelta)
{
}

void CMonsterControlContext::UpdateSideWalk(const _float& fTimeDelta)
{
}

void CMonsterControlContext::UpdateTurn90(const _float& fTimeDelta)
{
}

void CMonsterControlContext::UpdateTrun180(const _float& fTimeDelta)
{
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
	Super::Free();
}
