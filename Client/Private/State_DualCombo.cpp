#include "pch.h"
#include "State_DualCombo.h"

#include "Player.h"
#include "PlayerActionState.h"
#include "PhysicsCCT.h"

#include "GameInstance.h"

CState_DualCombo::CState_DualCombo(CActionState* pOwnerComponent)
	: Super(pOwnerComponent,"DualCombo")
{
}

HRESULT CState_DualCombo::Initialize(void* pArg)
{
	DUALCOMBO_DESC* pDesc = static_cast<DUALCOMBO_DESC*>(pArg);

	PLAYER_COMBOBASE_DESC tMyDesc = {};
	tMyDesc.vCombo_CheckTimes = pDesc->vCombo_CheckTimes;
	tMyDesc.pOwnerGun = pDesc->pOwnerGun;

	tMyDesc.arrCombo_EndTimes = pDesc->arrCombo_EndTimes;
	tMyDesc.fSlide_CheckTime = pDesc->fSlide_CheckTime;

	tMyDesc.FCollis = CStateBase_Player::COLLISIONFLAGS::C_DOWN
		| CStateBase_Player::COLLISIONFLAGS::C_Strong
		| CStateBase_Player::COLLISIONFLAGS::C_Fly;

	tMyDesc.bBlend = true;
	tMyDesc.bLoop = false;
	tMyDesc.FAniFlags = 0;
	tMyDesc.vecPreAnims = {};
	tMyDesc.vecMainAnims = { pDesc->iFirstAnimIdx, pDesc->iSlideAnimIdx,pDesc->iSecondAnimIdx, pDesc->iThirdAnimIdx, pDesc->iFourthAnimIdx };

	TIME_COUNTER tKeyTimer	= {};
	tKeyTimer.bCountTime	= true;
	tKeyTimer.bTimeReset	= false;
	tKeyTimer.fMaxTime		= 1.5f;
	tMyDesc.tKeyTimer		= tKeyTimer;

	tMyDesc.FMoves = CStateBase_Player::MOVEFLAGS::PRESS_CHANGE | CStateBase_Player::MOVEFLAGS::LOOP_DONE;
	vector<_uint> vecChangeState_ByKey{};
	vecChangeState_ByKey.resize(ENUM_TO_SZET(CStateBase_Player::STATEKEY::END), pDesc->iEndStateIndex);

	//MOVE, SPACE, SHIFT, LCRTL_PRESS, LCRTL_UP, Q, E, LM, RM, CHARGE, LOOPDONE
	vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::MOVE)]			= ENUM_TO_UINT(CPlayer::State::WALK);
	vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SPACE)]			= ENUM_TO_UINT(CPlayer::State::JUMP);
	vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SHIFT)]			= ENUM_TO_UINT(CPlayer::State::DASHBACK);
	vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_PRESS)]	= ENUM_TO_UINT(CPlayer::State::CROUCH);
	vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_UP)]		= ENUM_TO_UINT(CPlayer::State::END);
	vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::E)]				= ENUM_TO_UINT(CPlayer::State::SKILL1);
	vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::Q)]				= ENUM_TO_UINT(CPlayer::State::SKILL2);
	vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LM)]				= ENUM_TO_UINT(CPlayer::State::END);
	vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::RM)]				= ENUM_TO_UINT(CPlayer::State::GUNATTACK);
	vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::CHARGE)]			= ENUM_TO_UINT(CPlayer::State::CHARGE);
	vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LOOPDONE)]		= ENUM_TO_UINT(CPlayer::State::IDLE);

	tMyDesc.vecChangeState_ByKey = vecChangeState_ByKey;

	if (FAILED(Super::Initialize(&tMyDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_DualCombo::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_DualCombo::Start(void* pArg, _bool bForce)
{
	if (FAILED(Super::Start(pArg, bForce)))
		return E_FAIL;

	return S_OK;
}

void CState_DualCombo::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

HRESULT CState_DualCombo::End()
{
	if (FAILED(Super::End()))
		return E_FAIL;

	Get_OwnerObject()->Set_Render(true);

	CPhysicsCCT* pCCT = Get_OwnerObject()->Get_Component<CPhysicsCCT>();
	pCCT->EnableCollision(true);

	return S_OK;
}

void CState_DualCombo::Start_Third()
{
	Get_OwnerObject()->Set_Render(false);
 
	// 좀 더 앞으로 셋
	CGameObject* pObj = Get_OwnerObject();
	CTransform* pTransform = pObj->Get_Component<CTransform>();
	CPhysicsCCT* pCCT = pObj->Get_Component<CPhysicsCCT>();
	if (pTransform && pCCT)
	{
		Vec3 vCurPos = pTransform->Get_Info(TRANSFORM_INFO_STATE::POS);
		Vec3 vLook = pTransform->Get_Info(TRANSFORM_INFO_STATE::LOOK);
		vLook.Normalize();

		if (m_bLookAtMonster)
		{
			pCCT->SetFootPosition(m_vMonsterPos - vLook * 2.f);
		}

		else
		{
			pCCT->SetFootPosition(vCurPos + vLook * 1.5f);
		}
	}
}

void CState_DualCombo::Update_Third(const _float fTimeDelta)
{
	if (m_fStateElapsed >= m_tKeyTimer.fMaxTime - 0.1f)
	{
		Get_OwnerObject()->Set_Render(true);
		CPhysicsCCT* pCCT = Get_OwnerObject()->Get_Component<CPhysicsCCT>();
		pCCT->EnableCollision(true);
	}

	// render onoff 관리
}

CState_DualCombo* CState_DualCombo::Create(CActionState* pOwnerComponent, void* pArg)
{
	CState_DualCombo* pInstance = new CState_DualCombo(pOwnerComponent);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CState_DualCombo::Create, Failed");
		Safe_Release(pInstance);
	} 
	return pInstance;
}

void CState_DualCombo::Free()
{
	Super::Free();
}
