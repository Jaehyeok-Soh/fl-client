#include "Engine_pch.h"
#include "ActionState.h"
#include "StateBase.h"
#include "GameObject.h"
#include "ContainerObject.h"
#include "Navigation.h"
#include "CameraMan.h"
#include "Model.h"
#include "Transform.h"
#include "PhysicsCCT.h"

CActionState::CActionState()
{
}

CActionState::CActionState(const CActionState& rhs)
	:m_bApplyGravity(rhs.m_bApplyGravity)
	, m_bApplyYLerp(rhs.m_bApplyYLerp)
	, m_fGravityOffset(rhs.m_fGravityOffset)
{
}

HRESULT CActionState::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	CCTFlags |= PxControllerCollisionFlag::eCOLLISION_DOWN;

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
	if(m_iStateCount > 0)
		m_vecStates.resize(m_iStateCount);
	m_pOwnerModel = pDesc->pOwnerModel;
	m_pOwnerAnimECS = pDesc->pOwnerAnimECS;
	m_pOwnerWeaponModel = pDesc->pOwnerWeaponModel;
	m_pOwnerWeaponAnimECS = pDesc->pOwnerWeaponAnimECS;
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

	CPhysicsCCT* cct = { nullptr };
	if (cct = m_pOwner->Get_Component<CPhysicsCCT>())
	{
		cct->UpdateMove(fTimeDelta);
		CCTFlags = cct->GetCollisionState();
	}

}

void CActionState::Clear_WhenChangeLevel()
{
	m_pOwnerTargetCamera = nullptr;
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
	// 인덱스 방어
	if (iIndex >= m_vecStates.size() || m_vecStates[iIndex] == nullptr)
		return E_FAIL;

	// 같은 state 방어
	if (m_iCurrentState == iIndex && bForce == false)
		return S_OK;

	// 이전 state end 호출
	if (m_iCurrentState >= 0 && m_iCurrentState < m_vecStates.size())
	{
		if (CStateBase* pPrevState = m_vecStates[m_iCurrentState])
		{
			if (FAILED(pPrevState->End()))
				return E_FAIL;
		}
	}

	// pre state 업데이트 -> start에서 사용할지 몰라서 위에서 업데이트
	m_iPrevState = m_iCurrentState;

	// change state : start
	if (FAILED(m_vecStates[iIndex]->Start(pArg, bForce)))
		return E_FAIL;

	// cur state 업데이트
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

void CActionState::Set_ZeroVelocity()
{
	CPhysicsCCT* cct = { nullptr };
	if (cct = m_pOwner->Get_Component<CPhysicsCCT>())
		cct->SetZeroVelocity();
}

void CActionState::Set_ZeroHorizontalVelocity()
{
	CPhysicsCCT* cct = { nullptr };
	if (cct = m_pOwner->Get_Component<CPhysicsCCT>())
		cct->SetZeroHorizontalVelocity();
}

void CActionState::Set_ZeroVerticalVelocity()
{
	CPhysicsCCT* cct = { nullptr };
	if (cct = m_pOwner->Get_Component<CPhysicsCCT>())
		cct->SetZeroVerticalVelocity();
}

void CActionState::Set_ZeroDeAccelRate()
{
	CPhysicsCCT* cct = { nullptr };
	if (cct = m_pOwner->Get_Component<CPhysicsCCT>())
		cct->SetZeroDeAccelRate();
}

void CActionState::Set_DeAccelRate(_float fRate)
{
	CPhysicsCCT* cct = { nullptr };
	if (cct = m_pOwner->Get_Component<CPhysicsCCT>())
		cct->SetDeAccelRate(fRate);
}

void CActionState::Reset_DeAccelRate()
{
	CPhysicsCCT* cct = { nullptr };
	if (cct = m_pOwner->Get_Component<CPhysicsCCT>())
		cct->ResetDeAccelRate();
}

void CActionState::Set_Navigation(CNavigation* pNavigation)
{
	if (pNavigation == nullptr)
		return;

	m_pOwnerNavigation = pNavigation;
}

void CActionState::Set_ApplyGravity(_bool bApply)
{
	CPhysicsCCT* cct = { nullptr };
	if (cct = m_pOwner->Get_Component<CPhysicsCCT>())
		cct->SetApplyGravity(bApply);
}

void CActionState::Set_GravityOffset(_float fOffset)
{
	CPhysicsCCT* cct = { nullptr };
	if (cct = m_pOwner->Get_Component<CPhysicsCCT>())
		cct->Set_GravityOffset(fOffset);
}

_bool CActionState::IsOn_CCTFlag(PxControllerCollisionFlag::Enum eFlag)
{
	return (CCTFlags & eFlag);
}

HRESULT CActionState::Set_OwnerComponents()
{
	CGameObject* pOwner = Get_Owner();

	m_pOwnerControlContext = pOwner->Get_Component<CControlContext>();

	m_pOwnerTransform = pOwner->Get_Component<CTransform>();
	if (m_pOwnerTransform == nullptr)
		return E_FAIL;

	m_pOwnerNavigation = pOwner->Get_Component<CNavigation>();
	return S_OK;
}

CTransform* CActionState::Get_CamTransform()
{
	if(!m_pOwnerTargetCamera)
		return nullptr;

	CTransform* pCamTransform = m_pOwnerTargetCamera->Get_Component<CTransform>();

	if (!pCamTransform)
		return nullptr;

	return pCamTransform;
}

Vec3 CActionState::Get_MoveDir()
{
	if (!m_pOwnerControlContext)
		return Vec3::Zero;

	return m_pOwnerControlContext->Get_MoveDir();
}

void CActionState::Move(Vec3 vAccelation)
{
	CPhysicsCCT* cct = { nullptr };
	if (cct = m_pOwner->Get_Component<CPhysicsCCT>())
		cct->AddAccelation(vAccelation);
}

void CActionState::SetCCTInputDirection(Vec3 vInputDir)
{
	CPhysicsCCT* cct = { nullptr };
	if (cct = m_pOwner->Get_Component<CPhysicsCCT>())
		cct->SetInputDir(vInputDir);
}

void CActionState::SetCCTImpuls(Vec3 vImpuls)
{
	CPhysicsCCT* cct = { nullptr };
	if (cct = m_pOwner->Get_Component<CPhysicsCCT>())
		cct->SetImpulsAccelation(vImpuls);
}

HRESULT CActionState::Request_MixAnimation(_uint iVectorIdx, _int iAnimIdx)
{
	if (m_pOwnerModel == nullptr)
		return E_FAIL;

	m_pOwnerModel->Set_MixAnim_AnimIndex(iVectorIdx, iAnimIdx);

	return S_OK;
}

HRESULT CActionState::Request_ChangeAnimation(_uint iAnimationIndex, _bool bBlend, _bool bLoop, _bool bForce)
{
	if (m_pOwnerModel == nullptr)
		return E_FAIL;

	return m_pOwnerModel->Change_Animation(m_pOwnerAnimECS, iAnimationIndex, bBlend, bLoop, bForce);
}

HRESULT CActionState::Request_Change_WeaponAnimation(_uint iAnimationIndex, _bool bBlend, _bool bLoop, _bool bForce)
{
	if (m_pOwnerWeaponModel == nullptr || m_pOwnerWeaponAnimECS == nullptr)
		return E_FAIL;

	// TODO : 은비야 여기 Weapon Blend Off 다
	return m_pOwnerWeaponModel->Change_Animation(m_pOwnerWeaponAnimECS, iAnimationIndex, bBlend, bLoop, bForce);
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

_bool CActionState::Is_AnimTrackPositionAtRaw(_float fTrack)
{
	if (m_pOwnerModel == nullptr)
		return false;

	return m_pOwnerModel->Is_AnimTrackPositionAt(fTrack);
}

_bool CActionState::Is_AnimTrackPositionBetweenRaw(_float fTrackA, _float fTrackB)
{
	if (m_pOwnerModel == nullptr)
		return false;

	return m_pOwnerModel->Is_AnimTrackPositionBetweenRaw(fTrackA, fTrackB);
}

_bool CActionState::Is_AnimTrackPositionHalf()
{
	if (m_pOwnerModel == nullptr)
		return false;

	return m_pOwnerModel->Is_AnimTrackPositionAtHalf();
}

_bool CActionState::Align_Movement(const _float fTimeDelta)
{
	Vec3 vTargetDir = m_pOwnerControlContext->Get_MoveDir();
	if (::XMVector3Equal(vTargetDir, Vec3::Zero))
		return false;

	CPhysicsCCT* cct = { nullptr };
	if (cct = m_pOwner->Get_Component<CPhysicsCCT>())
	{
		m_pOwnerTransform->Turn_WorldYAxis(vTargetDir, fTimeDelta);
		Vec3 turnedLook = m_pOwnerTransform->Get_Info(TRANSFORM_INFO_STATE::LOOK);
		turnedLook.Normalize();

		SetCCTInputDirection(turnedLook);
	}
	else
	{
		m_pOwnerTransform->Turn_WorldYAxis(vTargetDir, fTimeDelta);
		m_pOwnerTransform->Go_Straight(fTimeDelta, m_pOwnerNavigation);
	}
	
	return true;
}

_bool CActionState::Align_Movement_MoveDir(const _float fTimeDelta)
{
	Vec3 vMoveDir = m_pOwnerControlContext->Get_MoveDir();
	if (::XMVector3Equal(vMoveDir, Vec3::Zero))
		return false;

	vMoveDir.y = 0.f;
	if (::XMVector3Equal(vMoveDir, Vec3::Zero))
		return false;

	vMoveDir.Normalize();

	CPhysicsCCT* cct = nullptr;
	if (cct = m_pOwner->Get_Component<CPhysicsCCT>())
		SetCCTInputDirection(vMoveDir);
	else
		m_pOwnerTransform->Go_Dir(vMoveDir, fTimeDelta, m_pOwnerNavigation);


	return true;
}

void CActionState::Follow_CameraLook(const _float fTimeDelta)
{
	if (!m_pOwnerTargetCamera)
	{
		if (!(m_pOwnerTargetCamera = Get_Owner()->Get_CameraTargeter()))
			return;
	}
	m_pOwnerTransform->Turn_WorldYAxis(m_pOwnerTargetCamera->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::LOOK), fTimeDelta);
}

void CActionState::Apply_Gravity(const _float fTimeDelta)
{
	//if (Is_ApplyGravity() == false)
	//	return;

	//m_fVerticalSpeed += m_fGravity * fTimeDelta;
	//if (m_fVerticalSpeed < m_fMaxFallSpeed)
	//	m_fVerticalSpeed = m_fMaxFallSpeed;

	//_float fDelta = m_fVerticalSpeed * fTimeDelta;

	//Vec3 vPos = m_pOwnerTransform->Get_Info(TRANSFORM_INFO_STATE::POS);
	//Vec3 vUp = m_pOwnerTransform->Get_Info(TRANSFORM_INFO_STATE::UP);
	//vUp.Normalize();
	//vPos += vUp * fDelta;
	///*_float fCurrentY = ::XMVectorGetY(vPos);
	//vPos = ::XMVectorSetY(vPos, fCurrentY + fDelta);*/
	//m_pOwnerTransform->Set_Info(TRANSFORM_INFO_STATE::POS, vPos);
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
	Vec3 vTarget = m_pOwnerTargetCamera->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::LOOK);
	vTarget.y = 0.f;
	vTarget.Normalize();

	_float fRadian = std::atan2(vTarget.x, vTarget.z);

	m_pOwnerTransform->Rotation(Vec3::Up, fRadian);
}

void CActionState::SetupLook_CameraLookLerp(const _float fTimeDelta, _float fLerpSpeed)
{
	if (!m_pOwnerTargetCamera)
	{
		if (!(m_pOwnerTargetCamera = Get_Owner()->Get_CameraTargeter()))
			return;
	}
	Vec3 vTarget = m_pOwnerTargetCamera->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::LOOK);
	vTarget.y = 0.f;
	vTarget.Normalize();

	_float fRadian = std::atan2(vTarget.x, vTarget.z);

	// 현재 각도
	Vec3 vCurLook = m_pOwnerTransform->Get_Info(TRANSFORM_INFO_STATE::LOOK);
	vCurLook.y = 0.f;
	vCurLook.Normalize();

	_float fCurRadian = std::atan2(vCurLook.x, vCurLook.z);

	// 현재 각도 : 179°
	// 목표 각도 : -179°

	// 2도만 돌면 되는데 358도를 돌게 됨 -> 따로 각도 보간 필요
	_float fDifff = (fRadian - fCurRadian);
	while (fDifff > XM_PI)  fDifff -= XM_PI * 2.f;
	while (fDifff < -XM_PI) fDifff += XM_PI * 2.f;
	_float fNewRadian = fCurRadian + fDifff * std::clamp(fLerpSpeed * fTimeDelta, 0.f, 1.f);

	m_pOwnerTransform->Rotation(Vec3::Up, fNewRadian);
}

void CActionState::SetupLook_CameraSameLook()
{
	//if (!m_pOwnerTargetCamera)
	//{
	//	if (!(m_pOwnerTargetCamera = Get_Owner()->Get_CameraTargeter()))
	//		return;
	//}
	//Vec3 vTarget = m_pOwnerTargetCamera->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::LOOK);
	//_float fPitch = asin(vTarget.y);
	//vTarget.y = 0.f;
	//vTarget.Normalize();

	//_float fRadian = std::atan2(vTarget.x, vTarget.z);

	//m_pOwnerTransform->Rotation(Vec3::Up, fRadian);
	//m_pOwnerTransform->Rotation(TRANSFORM_INFO_STATE::RIGHT, fPitch);
}

void CActionState::SetupLookAt(const Vec3& vPoint)
{
	m_pOwnerTransform->Look_At(vPoint);
}

void CActionState::SetupLook_Target_XZ()
{
	CGameObject* pTarget = Get_Target();
	if (pTarget == nullptr) 
		return;
	CTransform* pTargetTransform = pTarget->Get_Component<CTransform>();

	Vec3 vScale = m_pOwnerTransform->Get_Scaled();
	Vec3 vPosition = m_pOwnerTransform->Get_Info(TRANSFORM_INFO_STATE::POS);
	Vec3 vTargetPosition = pTargetTransform->Get_Info(TRANSFORM_INFO_STATE::POS);

	Vec3 vNewLookDir = vTargetPosition - vPosition;
	vNewLookDir.y = 0.f;
	vNewLookDir.Normalize();
	Vec3 vNewRightDir = Vec3::Up.Cross(vNewLookDir);
	vNewRightDir.Normalize();
	Vec3 vNewUpDir = vNewLookDir.Cross(vNewRightDir);
	vNewUpDir.Normalize();

	m_pOwnerTransform->Set_Info(TRANSFORM_INFO_STATE::RIGHT, vNewRightDir * vScale.x);
	m_pOwnerTransform->Set_Info(TRANSFORM_INFO_STATE::UP, vNewUpDir * vScale.y);
	m_pOwnerTransform->Set_Info(TRANSFORM_INFO_STATE::LOOK, vNewLookDir * vScale.z);
}

CGameObject* CActionState::Get_Target()
{
	return m_pOwnerControlContext->Get_Target();
}

_bool CActionState::Align_Move(_uint iState, _bool bForce , void* pArg)
{
	if (m_pOwnerControlContext == nullptr)
		return false;

	if (m_pOwnerControlContext->Is_MovePressed())
	{
		Change_State(iState, bForce, pArg);
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

_bool CActionState::Key_Input(CControlContext::CONTROL_KEY eKey)
{
	switch (eKey)
	{
	case CControlContext::CONTROL_KEY::MOVE:
		return m_pOwnerControlContext->Is_MovePressed();

	case CControlContext::CONTROL_KEY::DASH:
		return m_pOwnerControlContext->Is_DashPressed();

	case CControlContext::CONTROL_KEY::WALK:
		return m_pOwnerControlContext->Is_WalkPressed();

	case CControlContext::CONTROL_KEY::SPECIALMV:
		return m_pOwnerControlContext->Is_SepcialMovePressed();

	case CControlContext::CONTROL_KEY::JUMP:
		return m_pOwnerControlContext->Is_JumpPressed();

	case CControlContext::CONTROL_KEY::WIRE:
		return m_pOwnerControlContext->Is_WirePressed();

	case CControlContext::CONTROL_KEY::DODGE:
		return m_pOwnerControlContext->Is_DodgePressed();

	case CControlContext::CONTROL_KEY::LATT:
		return m_pOwnerControlContext->Is_LeftAttackPressed();

	case CControlContext::CONTROL_KEY::RATT:
		return m_pOwnerControlContext->Is_RightAttackPressed();

	case CControlContext::CONTROL_KEY::CHARGATT:
		return m_pOwnerControlContext->Is_ChargingAttackPressed();

	case CControlContext::CONTROL_KEY::COMBO1:
		return m_pOwnerControlContext->Is_ComboAtt1Pressed();

	case CControlContext::CONTROL_KEY::COMBO2:
		return m_pOwnerControlContext->Is_ComboAtt2Pressed();

	case CControlContext::CONTROL_KEY::COMBO3:
		return m_pOwnerControlContext->Is_ComboAtt3Pressed();

	case CControlContext::CONTROL_KEY::COMBO4:
		return m_pOwnerControlContext->Is_ComboAtt4Pressed();

	case CControlContext::CONTROL_KEY::SKILL1:
		return m_pOwnerControlContext->Is_Skill1Pressed();

	case CControlContext::CONTROL_KEY::SKILL2:
		return m_pOwnerControlContext->Is_Skill2Pressed();
	}

	return false;
}

void CActionState::Set_AnimationPlayRate(_uint iIndex, _float fSpeed)
{
	m_pOwnerModel->Set_AnimationPlayRate(iIndex, fSpeed);
}

void CActionState::Additive_MixOn(_bool bOn)
{
	if (m_pOwnerModel)
		m_pOwnerModel->Set_Apply_AdditiveAnim(bOn);
}

void CActionState::Additive_DataSetting(_bool bAdditive, _int iRefIdx, _int iPosIdx, _float fMixOffset)
{
	if (m_pOwnerModel)
		m_pOwnerModel->Set_AdditiveData(bAdditive, iRefIdx,  iPosIdx,  fMixOffset);
}

void CActionState::Additive_DataSetting(_bool bAdditive, _int iPosIdx, _float fMixOffset)
{
	if (m_pOwnerModel)
		m_pOwnerModel->Set_AdditiveData(bAdditive, iPosIdx, fMixOffset);
}

void CActionState::Set_JumpCount(_uint iCount)
{
	m_pOwnerControlContext->Set_JumpCount(iCount);
}

void CActionState::Turn_byCam(const _float fTimeDelta)
{
	Vec3 vTargetDir = m_pOwnerControlContext->Get_MoveDir();
	if (::XMVector3Equal(vTargetDir, Vec3::Zero))
		return;

	CPhysicsCCT* cct = { nullptr };
	if (cct = m_pOwner->Get_Component<CPhysicsCCT>())
	{
		m_pOwnerTransform->Turn_WorldYAxis(vTargetDir, fTimeDelta);
	}
	else
	{
		m_pOwnerTransform->Turn_WorldYAxis(vTargetDir, fTimeDelta);
	}
}

_bool CActionState::Is_Grounded() const
{
	return m_pOwnerControlContext->Is_Grounded();
}

_bool CActionState::Is_ApplyGravity() const
{
	return m_bApplyGravity;
}

_bool CActionState::Is_AttackPressed() const
{
	return m_pOwnerControlContext->Is_LeftAttackPressed();
}

void CActionState::Chase_Target(const Vec3 &vTargetPosition, const _float fTimedelta, const _float fSpeedRatio)
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

void CActionState::Move_Down(const _float fTimeDelta, const _float fSpeedRatio)
{
	m_pOwnerTransform->Go_Down(fTimeDelta, m_pOwnerNavigation);
}

void CActionState::StartForce_Front_ForAnimation(_float fForceAbs, _float fDragK)
{
	m_pOwnerTransform->Start_Force(m_pOwnerTransform->Get_Info(TRANSFORM_INFO_STATE::LOOK), fForceAbs, fDragK);
}

void CActionState::StartForce_Backward_ForAnimation(_float fForceAbs, _float fDragK)
{
	m_pOwnerTransform->Start_Force(-1.f * m_pOwnerTransform->Get_Info(TRANSFORM_INFO_STATE::LOOK), fForceAbs, fDragK);
}

void CActionState::StartForce_Left_ForAnimation(_float fForceAbs, _float fDragK)
{
	m_pOwnerTransform->Start_Force(-1.f * m_pOwnerTransform->Get_Info(TRANSFORM_INFO_STATE::RIGHT), fForceAbs, fDragK);
}

void CActionState::StartForce_Right_ForAnimation(_float fForceAbs, _float fDragK)
{
	m_pOwnerTransform->Start_Force(m_pOwnerTransform->Get_Info(TRANSFORM_INFO_STATE::RIGHT), fForceAbs, fDragK);
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