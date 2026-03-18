#include "pch.h"
#include "MonsterControlContext.h"
#include "Client_Defines.h"
#include "Monster_Base.h"
#include "MyStat.h"
#include "MonsterActionState.h"
#include "PhysicsCCT.h"
#include "PhysicsRagdoll.h"
#include "Engine_Utils.h"
#include "GameInstance.h"
#include "Monster_Body_Base.h"
#include "MainPlayer.h"

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

	m_iOwnerID = Get_Owner()->Get_ID();

	m_bPhaseTwo = false;
	return S_OK;
}

void CMonsterControlContext::Consume_GroggyRequest()
{
	// 요청이 없었다면
	if (Engine_Utils::Has_Flag(m_iSubState, SUB_STATE::GROGGY_REQ) == false)
		return;

	Engine_Utils::RemoveHard_Flag(m_iSubState, SUB_STATE::GROGGY_REQ);
	Engine_Utils::Add_Flag(m_iSubState, SUB_STATE::GROGGY_ACTIVE);
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
	
	Update_Groggy(fTiemDelta);

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

void CMonsterControlContext::Update_Groggy(const _float fTimeDelta)
{
	if (Engine_Utils::Has_Flag(m_iSubState, SUB_STATE::GROGGY_ACTIVE) == false)
		return;


	if (m_tGroggyCounter.Tick(fTimeDelta))
		End_Groggy();
}

void CMonsterControlContext::End_Groggy()
{
	if (Engine_Utils::Has_Flag(m_iSubState, SUB_STATE::GROGGY_ACTIVE) == false)
		return;

	Engine_Utils::RemoveHard_Flag(m_iSubState, SUB_STATE::GROGGY_ACTIVE);
	m_tGroggyCounter.Clear();
	m_eCurrentGroggyState = { EGroggyState::None };
}

void CMonsterControlContext::Set_RootMotion_Apply(_bool bApply)
{
	static_cast<CMonster_Base*>(Get_Owner())->Set_RootMotion_Apply(bApply);
}

void CMonsterControlContext::Set_Target_Offset(_float fX, _float fY, _float fZ, _float fTimeDelta)
{
	Vec3 vOffsetPos = m_tRuntimeDesc.vOwnerPos;

	if (fX > 1e-5f)
		vOffsetPos.x = m_tRuntimeDesc.vTargetPos.x + fX;

	if (fY > 1e-5f)
		vOffsetPos.y = m_tRuntimeDesc.vTargetPos.y + fY;

	if (fZ > 1e-5f)
		vOffsetPos.z = m_tRuntimeDesc.vTargetPos.z + fZ;
	
	vOffsetPos = vOffsetPos - m_tRuntimeDesc.vOwnerPos;

	_float fSpeed = 5.f;
	vOffsetPos *= fSpeed * fTimeDelta;

	if (vOffsetPos.Length() > 0.01f)
	{
		Get_Owner()->Get_Component<CPhysicsCCT>()->AddFixedMove(vOffsetPos);
	}
}

void CMonsterControlContext::Auto_Teleport_Chase(_float fMaxLength)
{
	Vec3 diff = m_tRuntimeDesc.vTargetPos - m_tRuntimeDesc.vOwnerPos;
	_float length = diff.Length();

	if (length > fMaxLength)
	{
		Vec3 vOffsetPos(m_tRuntimeDesc.vTargetPos.x + 1.f,
			m_tRuntimeDesc.vTargetPos.y + 1.f,
			m_tRuntimeDesc.vTargetPos.z + 1.f);
		Get_Owner()->Get_Component<CPhysicsCCT>()->SetFootPosition(vOffsetPos);
	}
}

void CMonsterControlContext::Genimon_Smart_Chase(_float fX, _float fY, _float fZ, _float fMaxLength, _float fTimeDelta)
{
	Set_Target_Offset(fX, fY, fZ, fTimeDelta);
	Auto_Teleport_Chase(fMaxLength);
}

Vec3 CMonsterControlContext::Get_MoveDir()
{
	return m_vMoveDir;
}

_bool CMonsterControlContext::Set_Groggy(EGroggyState eState, _bool bRequest, _float fGroggyDuration)
{
	const _bool bAleradyRequested = Engine_Utils::Has_Flag(m_iSubState, SUB_STATE::GROGGY_REQ);
	const _bool bActive = Engine_Utils::Has_Flag(m_iSubState, SUB_STATE::GROGGY_ACTIVE);

	// 그로기 요청이라면
	if (bRequest == true)
	{
		// 이미 활성화 된상태 요청은 받지 않음
		if (bAleradyRequested || bActive)
			return false;

		Engine_Utils::Add_Flag(m_iSubState, SUB_STATE::GROGGY_REQ);

		m_eCurrentGroggyState = eState;
		m_tGroggyCounter.Start(fGroggyDuration);
	}
	// 아니라면
	else
	{
		// 요청 취소
		if (bAleradyRequested == false)
			return false;

		Engine_Utils::RemoveHard_Flag(m_iSubState, SUB_STATE::GROGGY_REQ);
	}

	// 요청 성공
	return true;
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

_bool CMonsterControlContext::IsAlive()
{
	return Get_Owner()->IsAlive();
}

_bool CMonsterControlContext::IsDying()
{
	return Get_Owner()->IsDying();
}

_bool CMonsterControlContext::IsAttackLanded()
{
	_bool result = { false };

	if (result = Engine_Utils::Has_Flag(m_iSubState, SUB_STATE::ATTACK_LANDED))
		Engine_Utils::RemoveHard_Flag(m_iSubState, SUB_STATE::ATTACK_LANDED);

	return result;
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
	m_vMoveDir = m_tRuntimeDesc.vOwnerLook;
	if (m_vMoveDir.LengthSquared() <= g_XMEpsilon.f[0])
		m_vMoveDir = Vec3(0.f, 0.f, 1.f);
}

void CMonsterControlContext::UpdateChase(const _float fTimeDelta)
{
	if (m_tRuntimeDesc.bTargetValid == false)
	{
		m_vMoveDir = Vec3::Zero;
		return;
	}

	m_vMoveDir = m_tRuntimeDesc.vToTargetDir;
}

void CMonsterControlContext::Update_TurnToTarget_XZ(const _float fTimeDelta)
{
	if (m_tRuntimeDesc.bTargetValid == false)
		return;

	CTransform* pTransform = Get_Owner()->Get_Component<CTransform>();
	pTransform->Tunr_ToPoint_YAxis(m_tRuntimeDesc.vTargetPos, fTimeDelta);
}

void CMonsterControlContext::Update_8Dir_LocalAxisXZ(const _float fTimeDelta, _float fForward, _float fRight)
{
	Vec3 vDir = m_tRuntimeDesc.vOwnerLook * fForward + m_tRuntimeDesc.vOwnerRight * fRight;

	if (vDir.LengthSquared() <= g_XMEpsilon.f[0])
		vDir = Vec3(0.f, 0.f, 1.f);
	else
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

//struct Part
//{
//	enum Enum : _uint
//	{
//		BODY = 0,
//		SWORD,
//		GUN,
//		END
//	};
//};

void CMonsterControlContext::Set_On_Ragdoll()
{
	auto body = static_cast<CMonster_Base*>(Get_Owner())->Get_Part<CMonster_Body_Base>(CMonster_Base::Part::BODY);
	auto pRagdoll = body->Get_Component<CPhysicsRagdoll>();
	if (pRagdoll)
		m_pGameInstance->RagdollRequestStart(body->Get_ID());
}

void CMonsterControlContext::Set_Off_Ragdoll()
{
	auto body = static_cast<CMonster_Base*>(Get_Owner())->Get_Part<CMonster_Body_Base>(CMonster_Base::Part::BODY);
	auto pRagdoll = body->Get_Component<CPhysicsRagdoll>();
	if (pRagdoll)
		m_pGameInstance->RagdollFinish(body->Get_ID());
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
