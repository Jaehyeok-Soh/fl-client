#include "Engine_pch.h"
#include "StateBase.h"
#include "ActionState.h"
#include "ContainerObject.h"

#include "GameInstance.h"
#include "Engine_Utils.h"

CStateBase::CStateBase(CActionState* pOwnerComponent, const string& strName)
	: m_pOwnerStateComp(pOwnerComponent)
	, m_strName(strName)
	, m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CStateBase::Initialize(void* pArg)
{
	if (pArg)
	{
		STATE_DESC* pDesc = static_cast<STATE_DESC*>(pArg);

		m_bBlend = pDesc->bBlend;
		m_bLoop = pDesc->bLoop;
		m_iAnimIndex = pDesc->iAnimIndex;

		m_FAniFlags = pDesc->FAniFlags;
		if (Engine_Utils::Has_Flag(m_FAniFlags, STATEANI_FLAG::SA_HasPreAni))
			m_vecPreAnims = std::move(pDesc->vecPreAnims);
		else
			m_vecPreAnims.clear();
	}

	return S_OK;
}

HRESULT CStateBase::Awake(const _uint iLevelIndex)
{
	return S_OK;
}

HRESULT CStateBase::Start(void *pArg, _bool bForce)
{
	if (pArg)
	{
		STATE_START_DESC *pDesc = static_cast<STATE_START_DESC*>(pArg);
		m_bMainForce = bForce;
	}

	// 만약 preAni가 있는 state라면
	// vecPreAnims 탐색 후 preAni 결정
	if (Engine_Utils::Has_Flag(m_FAniFlags, STATEANI_FLAG::SA_HasPreAni))
	{
		for (auto& pAniData : m_vecPreAnims)
		{
			// 만약 preidx가 없다면 무조건 업데이트
			if(pAniData.iPrevStateIdx == -1 || pAniData.iPrevStateIdx == Get_PrevState())
			{
				Request_ChangeAnimation(pAniData.iAnimationIdex, true, false, true); // 무조건 loop : false
				return S_OK;
			}
		}
	}

	// 만약 preAni가 없다면 내꺼 재생
	Request_ChangeAnimation(m_iAnimIndex, m_bBlend, m_bLoop, bForce);
	return S_OK;
}

void CStateBase::Update(const _float fTimeDelta)
{
	// 만약 preAni가 있는 state라면
	// preAni 끝났는지 확인 후 ->  PreAniDone flag On & animationi chnage. 
	if (Engine_Utils::Has_Flag(m_FAniFlags, STATEANI_FLAG::SA_HasPreAni) &&
		!Engine_Utils::Has_Flag(m_FAniFlags, STATEANI_FLAG::SA_PreAniDone))
	{
		if (Is_AnimFinished())
		{
			Engine_Utils::Add_Flag(m_FAniFlags, STATEANI_FLAG::SA_PreAniDone);
			Request_ChangeAnimation(m_iAnimIndex, m_bBlend, m_bLoop, m_bMainForce);
		}
	}

	m_fStateElapsed += fTimeDelta;
}

HRESULT CStateBase::End()
{
	m_fStateElapsed = 0.f;
	return S_OK;
}

HRESULT CStateBase::Request_ChangeAnimation(_uint iAnimationIndex, _bool bBlend, _bool bLoop, _bool bForce)
{
	if (m_pOwnerStateComp == nullptr)
		return E_FAIL;

	if (bForce)
		bBlend = false;

	return m_pOwnerStateComp->Request_ChangeAnimation(iAnimationIndex, bBlend, bLoop, bForce);
}

HRESULT CStateBase::Request_Change_State(_uint iIndex, void* pArg)
{
	if (m_pOwnerStateComp == nullptr)
		return E_FAIL;
	
	return m_pOwnerStateComp->Change_State(iIndex, false, pArg);
}

CGameObject* CStateBase::Get_OwnerObject()
{
	if (m_pOwnerStateComp == nullptr)
		return nullptr;

	return m_pOwnerStateComp->Get_Owner();
}

_float CStateBase::Get_AnimElpasedTimeSeconds()
{
	if (m_pOwnerStateComp == nullptr)
		return -1.f;

	return m_pOwnerStateComp->Get_AnimElpasedTimeSeconds();
}

_float CStateBase::Get_AnimNormalizedTime()
{
	if (m_pOwnerStateComp == nullptr)
		return -1.f;

	return m_pOwnerStateComp->Get_AnimNormalizedTime();
}

_bool CStateBase::Is_AnimTrackPositionAt(_float fRatio)
{
	if (m_pOwnerStateComp == nullptr)
		return false;

	return m_pOwnerStateComp->Is_AnimTrackPositionAt(fRatio);
}

_bool CStateBase::Is_AnimTrackPositionBetween(_float fStartRatio, _float EndRatio)
{
	if (m_pOwnerStateComp == nullptr)
		return false;

	return m_pOwnerStateComp->Is_AnimTrackPositionBetween(fStartRatio, EndRatio);
}

_bool CStateBase::Is_AnimFinished()
{
	if (m_pOwnerStateComp == nullptr)
		return false;

	return m_pOwnerStateComp->Is_AnimFinished();
}

_bool CStateBase::Is_AnimTrackPositionHalf()
{
	if (m_pOwnerStateComp == nullptr)
		return false;

	return m_pOwnerStateComp->Is_AnimTrackPositionHalf();
}

_bool CStateBase::Align_Movement(const _float fTimeDelta)
{
	if (m_pOwnerStateComp == nullptr)
		return false;

	return m_pOwnerStateComp->Align_Movement(fTimeDelta);
}

void CStateBase::Follow_CameraLook(const _float fTimeDelta)
{
	if (m_pOwnerStateComp == nullptr)
		return;

	m_pOwnerStateComp->Align_Movement(fTimeDelta);
}

void CStateBase::Apply_Gravity(const _float fTimeDelta)
{
	m_pOwnerStateComp->Apply_Gravity(fTimeDelta);
}

void CStateBase::SetupLook_CameraLook()
{
	m_pOwnerStateComp->SetupLook_CameraLook();
}

void CStateBase::SetupLookAt(const Vec3& vPoint)
{
	m_pOwnerStateComp->SetupLookAt(vPoint);
}

void CStateBase::SetupLook_Target_XZ()
{
	m_pOwnerStateComp->SetupLook_Target_XZ();
}

_bool CStateBase::Is_Grounded() const
{
	return m_pOwnerStateComp->Is_Grounded();
}

_bool CStateBase::Is_ApplyGravity() const
{
	return m_pOwnerStateComp->Is_ApplyGravity();
}

_bool CStateBase::Is_AttackPressed() const
{
	return m_pOwnerStateComp->Is_AttackPressed();
}

void CStateBase::Chase_Target(Vec3 vTargetPosition, const _float fTimedelta, const _float fSpeedRatio)
{
	m_pOwnerStateComp->Chase_Target(vTargetPosition, fTimedelta, fSpeedRatio);
}

void CStateBase::Move_Up(const _float fTimeDelta, const _float fSpeedRatio)
{
	m_pOwnerStateComp->Move_Up(fTimeDelta , fSpeedRatio);
}

void CStateBase::Move_Left(const _float fTimeDelta, const _float fSpeedRatio)
{
	m_pOwnerStateComp->Move_Left(fTimeDelta , fSpeedRatio);
}

void CStateBase::Move_Right(const _float fTimeDelta, const _float fSpeedRatio)
{
	m_pOwnerStateComp->Move_Right(fTimeDelta, fSpeedRatio);
}

void CStateBase::Move_Front(const _float fTimeDelta, const _float fSpeedRatio)
{
	m_pOwnerStateComp->Move_Front(fTimeDelta, fSpeedRatio);
}

void CStateBase::Move_Backward(const _float fTimeDelta, const _float fSpeedRatio)
{
	m_pOwnerStateComp->Move_Backward(fTimeDelta, fSpeedRatio);
}

void CStateBase::StartForce_Front_ForAnimation(_float fForceAbs, _float fDragK)
{
	m_pOwnerStateComp->StartForce_Front_ForAnimation(fForceAbs, fDragK);
}

void CStateBase::StartForce_Backward_ForAnimation(_float fForceAbs, _float fDragK)
{
	m_pOwnerStateComp->StartForce_Backward_ForAnimation(fForceAbs, fDragK);
}

void CStateBase::StartForce_Left_ForAnimation(_float fForceAbs, _float fDragK)
{
	m_pOwnerStateComp->StartForce_Left_ForAnimation(fForceAbs, fDragK);
}

void CStateBase::StartForce_Right_ForAnimation(_float fForceAbs, _float fDragK)
{
	m_pOwnerStateComp->StartForce_Right_ForAnimation(fForceAbs, fDragK);
}

void CStateBase::Set_AttackCollider(_uint iPartIndex, _bool bActive, ATTACK_DESC* pDesc)
{
	m_pOwnerStateComp->Set_AttackCollider(iPartIndex, bActive, pDesc);
}

void CStateBase::Apply_ForceMove(const _float fTimeDelta)
{
	m_pOwnerStateComp->Apply_ForceMove(fTimeDelta);
}

void CStateBase::Clear_ForceMove()
{
	m_pOwnerStateComp->Clear_ForceMove();
}

_int CStateBase::Get_PrevState() const
{
	return m_pOwnerStateComp->Get_PrevState();
}

CGameObject* CStateBase::Get_Target()
{
	return m_pOwnerStateComp->Get_Target();
}

void CStateBase::Set_AnimationPlayRate(_float fSpeed)
{
	m_pOwnerStateComp->Set_AnimationPlayRate(m_iAnimIndex, fSpeed);
}

void CStateBase::Set_JumpCount(_uint iCount)
{
	m_pOwnerStateComp->Set_JumpCount(iCount);
}

_bool CStateBase::Align_Move(_uint iRunState)
{
	return m_pOwnerStateComp->Align_Move(iRunState);
}
_bool CStateBase::Align_Attack(_uint iState)
{
	return m_pOwnerStateComp->Align_Attack(iState);
}

void CStateBase::Free()
{
	Safe_Release(m_pGameInstance);
	Super::Free();
}
