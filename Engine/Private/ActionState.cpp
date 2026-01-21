#include "StateBase.h"
#include "GameObject.h"
#include "ContainerObject.h"
#include "Navigation.h"
#include "CameraMan.h"
#include "Model.h"
#include "Transform.h"
#include "ControlContext.h"
#include "ActionState.h"

CActionState::CActionState()
{
}

CActionState::CActionState(const CActionState& rhs)
{
}

HRESULT CActionState::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CActionState::Initialize(void* pArg)
{
	if (!pArg)
		return E_FAIL;

	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	ACTIONSTATE_DESC* pDesc = static_cast<ACTIONSTATE_DESC*>(pArg);
	m_iStateCount = pDesc->iStateCount;
	m_vecStates.resize(m_iStateCount);
	m_pOwnerModel = pDesc->pOwnerModel;

	return S_OK;
}

HRESULT CActionState::Awake(_uint iLvelIndex)
{
	if (FAILED(Set_OwnerComponents()))
		return E_FAIL;

	for (auto& pState : m_vecStates)
	{
		if (pState)
		{
			if (FAILED(pState->Awake(iLvelIndex)))
				return E_FAIL;
		}
	}
	return S_OK;
}

void CActionState::Update(const _float fTimeDelta)
{
	if(m_vecStates[m_iCurrentState])
		m_vecStates[m_iCurrentState]->Update(fTimeDelta);
}

HRESULT CActionState::Add_State(_uint iIndex, CStateBase* pState)
{
	if (m_vecStates.size() <= iIndex)
		return E_FAIL;

	if (!pState)
		return E_FAIL;

	m_vecStates[iIndex] = pState;
	return S_OK;
}

HRESULT CActionState::Remove_State(_uint iIndex)
{
	if (m_vecStates.size() <= iIndex)
		return E_FAIL;

	if (!m_vecStates[iIndex])
		return S_OK;

	Safe_Release(m_vecStates[iIndex]);
	m_vecStates[iIndex] = nullptr;
	return S_OK;
}

HRESULT CActionState::Swap_State(_uint iIndex, CStateBase* pState, CStateBase** ppSwappedState)
{
	if (m_vecStates.size() <= iIndex)
		return E_FAIL;

	if (!m_vecStates[iIndex])
		return E_FAIL;

	*ppSwappedState = m_vecStates[iIndex];
	m_vecStates[iIndex] = pState;
	return S_OK;
}

HRESULT CActionState::Change_State(_uint iIndex, _bool bForce, void* pArg)
{
	if (iIndex >= m_vecStates.size() || m_vecStates[iIndex] == nullptr)
		return E_FAIL;

	if (m_iCurrentState == iIndex && bForce == false)
		return S_OK;

	if (m_iCurrentState >= 0 && m_iCurrentState < m_vecStates.size())
	{
		if (CStateBase* pPrevState = m_vecStates[m_iCurrentState])
		{
			if (FAILED(pPrevState->End()))
				return E_FAIL;
		}
	}

	if (FAILED(m_vecStates[iIndex]->Start(pArg, bForce)))
		return E_FAIL;
	m_iPrevState = m_iCurrentState;
	m_iCurrentState = iIndex;
	return S_OK;
}

CStateBase* CActionState::Get_State(_uint iIndex)
{
	if(iIndex >= m_vecStates.size())
		return nullptr;

	return m_vecStates[iIndex];
}

CStateBase* CActionState::Get_CurrentState()
{
	return m_vecStates[m_iCurrentState];
}

const char* CActionState::Get_CurrentStateName() const
{
	return m_vecStates[m_iCurrentState]->Get_Name();
}

void CActionState::Set_Navigation(CNavigation* pNavigation)
{
	if (pNavigation == nullptr)
		return;

	m_pOwnerNavigation = pNavigation;
}

HRESULT CActionState::Set_OwnerComponents()
{
	CGameObject* pOwner = Get_Owner();

	m_pOwnerControlContext = pOwner->Get_Component<CControlContext>();
	if (m_pOwnerControlContext == nullptr)
		return E_FAIL;

	m_pOwnerTransform = pOwner->Get_Component<CTransform>();
	if (m_pOwnerTransform == nullptr)
		return E_FAIL;

	m_pOwnerNavigation = pOwner->Get_Component<CNavigation>();
	return S_OK;
}

HRESULT CActionState::Request_ChangeAnimation(_uint iAnimationIndex, _bool bBlend, _bool bLoop, _bool bForce)
{
	if (m_pOwnerModel == nullptr)
		return E_FAIL;

	return m_pOwnerModel->Change_Animation(iAnimationIndex, bBlend, bLoop, bForce);
}

_float CActionState::Get_AnimElpasedTimeSeconds()
{
	if (m_pOwnerModel == nullptr)
		return -1.f;

	return m_pOwnerModel->Get_AnimElpasedTimeSeconds();
}

_float CActionState::Get_AnimNormalizedTime()
{
	if (m_pOwnerModel == nullptr)
		return -1.f;

	return m_pOwnerModel->Get_AnimNormalizedTime();
}

_bool CActionState::Is_AnimFinished()
{
	if (m_pOwnerModel == nullptr)
		return false;

	return m_pOwnerModel->Is_AnimFinished();
}

_bool CActionState::Is_AnimTrackPositionAt(_float fRatio)
{
	if (m_pOwnerModel == nullptr)
		return false;

	return m_pOwnerModel->Is_AnimTrackPositionAt(fRatio);
}

_bool CActionState::Is_AnimTrackPositionBetween(_float fStartRatio, _float EndRatio)
{
	if (m_pOwnerModel == nullptr)
		return false;

	return m_pOwnerModel->Is_AnimTrackPositionBetween(fStartRatio, EndRatio);
}

_bool CActionState::Is_AnimTrackPositionHalf()
{
	if (m_pOwnerModel == nullptr)
		return false;

	return m_pOwnerModel->Is_AnimTrackPositionAtHalf();
}

_bool CActionState::Align_Movement(const _float fTimeDelta)
{
	_fvector vTargetDir = m_pOwnerControlContext->Get_MoveDir();
	if (::XMVector3Equal(vTargetDir, ::XMVectorZero()))
		return false;

	m_pOwnerTransform->Turn_WorldYAxis(vTargetDir, fTimeDelta);
	m_pOwnerTransform->Go_Straight(fTimeDelta, m_pOwnerNavigation);
	return true;
}

void CActionState::Follow_CameraLook(const _float fTimeDelta)
{
	if (!m_pOwnerTargetCamera)
	{
		if (!(m_pOwnerTargetCamera = Get_Owner()->Get_CameraTargeter()))
			return;
	}
	_vector vTargetDir = m_pOwnerTargetCamera->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::LOOK);
	m_pOwnerTransform->Turn_WorldYAxis(vTargetDir, fTimeDelta);
}

void CActionState::Apply_Gravity(const _float fTimeDelta)
{
	if (Is_ApplyGravity() == false)
		return;

	m_fVerticalSpeed += m_fGravity * fTimeDelta;
	if (m_fVerticalSpeed < m_fMaxFallSpeed)
		m_fVerticalSpeed = m_fMaxFallSpeed;

	_float fDelta = m_fVerticalSpeed * fTimeDelta;

	_vector vPos = m_pOwnerTransform->Get_Info(TRANSFORM_INFO_STATE::POS);
	_vector vUp = ::XMVector3Normalize(m_pOwnerTransform->Get_Info(TRANSFORM_INFO_STATE::UP));
	vPos += vUp * fDelta;
	/*_float fCurrentY = ::XMVectorGetY(vPos);
	vPos = ::XMVectorSetY(vPos, fCurrentY + fDelta);*/
	m_pOwnerTransform->Set_Info(TRANSFORM_INFO_STATE::POS, vPos);
}

void CActionState::Apply_ForceMove(const _float fTimeDelta)
{
	m_pOwnerTransform->Apply_Force(fTimeDelta, m_pOwnerNavigation);
}

void CActionState::Clear_ForceMove()
{
	m_pOwnerTransform->Force_Clear();
}

void CActionState::SetupLook_CameraLook()
{
	if (!m_pOwnerTargetCamera)
	{
		if (!(m_pOwnerTargetCamera = Get_Owner()->Get_CameraTargeter()))
			return;
	}
	_vector vTarget = m_pOwnerTargetCamera->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::LOOK);
	vTarget = ::XMVector3Normalize(::XMVectorSetY(vTarget, 0.f));

	_float fTarget_X = ::XMVectorGetX(vTarget);
	_float fTarget_Z = ::XMVectorGetZ(vTarget);

	_float fRadian = std::atan2(fTarget_X, fTarget_Z);

	m_pOwnerTransform->Rotation(::XMVectorSet(0.f, 1.f, 0.f, 0.f), fRadian);
}

void CActionState::SetupLookAt(_fvector vPoint)
{
	m_pOwnerTransform->Look_At(vPoint);
}

void CActionState::SetupLook_Target_XZ()
{
	CGameObject* pTarget = { nullptr };
	if (Get_Target() == nullptr)
		return;
	CTransform* pTargetTransform = pTarget->Get_Component<CTransform>();

	_float3 vScale = m_pOwnerTransform->Get_Scaled();
	_vector vPosition = m_pOwnerTransform->Get_Info(TRANSFORM_INFO_STATE::POS);
	_vector vTargetPosition = pTargetTransform->Get_Info(TRANSFORM_INFO_STATE::POS);

	_vector vNewLookDir = ::XMVector3Normalize(::XMVectorSetY(vTargetPosition - vPosition, 0.f));
	_vector vNewRightDir = ::XMVector3Normalize(::XMVector3Cross(::XMVectorSet(0.f, 1.f, 0.f, 0.f), vNewLookDir));
	_vector vNewUpDir = ::XMVector3Normalize(::XMVector3Cross(vNewLookDir, vNewRightDir));

	m_pOwnerTransform->Set_Info(TRANSFORM_INFO_STATE::RIGHT, vNewRightDir * vScale.x);
	m_pOwnerTransform->Set_Info(TRANSFORM_INFO_STATE::UP, vNewUpDir * vScale.y);
	m_pOwnerTransform->Set_Info(TRANSFORM_INFO_STATE::LOOK, vNewLookDir * vScale.z);
}

CGameObject* CActionState::Get_Target()
{
	return m_pOwnerControlContext->Get_Target();
}

_bool CActionState::Align_Move(_uint iState)
{
	if (m_pOwnerControlContext == nullptr)
		return false;

	if (m_pOwnerControlContext->Is_MovePressed())
	{
		Change_State(iState);
		return true;
	}
	return false;
}

_bool CActionState::Align_Attack(_uint iState)
{
	if (m_pOwnerControlContext == nullptr)
		return false;

	if (m_pOwnerControlContext->Is_LeftAttackPressed())
	{
		Change_State(iState);
		return true;
	}

	return false;
}

void CActionState::Set_AnimationPlayRate(_uint iIndex, _float fSpeed)
{
	m_pOwnerModel->Set_AnimationPlayRate(iIndex, fSpeed);
}

void CActionState::Set_JumpCount(_uint iCount)
{
	m_pOwnerControlContext->Set_JumpCount(iCount);
}

_bool CActionState::Is_Grounded() const
{
	return m_pOwnerControlContext->Is_Grounded();
}

_bool CActionState::Is_ApplyGravity() const
{
	return m_pOwnerControlContext->Is_Gravity();
}

_bool CActionState::Is_AttackPressed() const
{
	return m_pOwnerControlContext->Is_LeftAttackPressed();
}

void CActionState::Chase_Target(_fvector vTargetPosition, const _float fTimedelta, const _float fSpeedRatio)
{
	m_pOwnerTransform->Chase(vTargetPosition, 1.f, fTimedelta * fSpeedRatio, m_pOwnerNavigation);
}

void CActionState::Move_Up(const _float fTimeDelta, const _float fSpeedRatio)
{
	m_pOwnerTransform->Go_Up(fTimeDelta * fSpeedRatio, m_pOwnerNavigation);
}

void CActionState::Move_Left(const _float fTimeDelta, const _float fSpeedRatio)
{
	m_pOwnerTransform->Go_Left(fTimeDelta * fSpeedRatio, m_pOwnerNavigation);
}

void CActionState::Move_Right(const _float fTimeDelta, const _float fSpeedRatio)
{
	m_pOwnerTransform->Go_Right(fTimeDelta * fSpeedRatio, m_pOwnerNavigation);
}

void CActionState::Move_Front(const _float fTimeDelta, const _float fSpeedRatio)
{
	m_pOwnerTransform->Go_Straight(fTimeDelta * fSpeedRatio, m_pOwnerNavigation);
}

void CActionState::Move_Backward(const _float fTimeDelta, const _float fSpeedRatio)
{
	m_pOwnerTransform->Go_BackWard(fTimeDelta * fSpeedRatio, m_pOwnerNavigation);
}

void CActionState::StartForce_Front_ForAnimation(_float fForceAbs, _float fDragK)
{
	m_pOwnerTransform->Start_Force(::XMVector3Normalize(m_pOwnerTransform->Get_Info(TRANSFORM_INFO_STATE::LOOK)), fForceAbs, fDragK);
}

void CActionState::StartForce_Backward_ForAnimation(_float fForceAbs, _float fDragK)
{
	m_pOwnerTransform->Start_Force(-1.f * ::XMVector3Normalize(m_pOwnerTransform->Get_Info(TRANSFORM_INFO_STATE::LOOK)), fForceAbs, fDragK);
}

void CActionState::StartForce_Left_ForAnimation(_float fForceAbs, _float fDragK)
{
	m_pOwnerTransform->Start_Force(-1.f * ::XMVector3Normalize(m_pOwnerTransform->Get_Info(TRANSFORM_INFO_STATE::RIGHT)), fForceAbs, fDragK);
}

void CActionState::StartForce_Right_ForAnimation(_float fForceAbs, _float fDragK)
{
	m_pOwnerTransform->Start_Force(::XMVector3Normalize(m_pOwnerTransform->Get_Info(TRANSFORM_INFO_STATE::RIGHT)), fForceAbs, fDragK);
}

void CActionState::Set_AttackCollider(_uint iPartIndex, _bool bActive, ATTACK_DESC* pDesc)
{
	static_cast<CContainerObject*>(Get_Owner())->Set_AttackCollider(iPartIndex, bActive, pDesc);
}

void CActionState::Free()
{
	for (auto& pState : m_vecStates)
	{
		Safe_Release(pState);
	}
	m_iCurrentState = -1;
	Super::Free();
}
