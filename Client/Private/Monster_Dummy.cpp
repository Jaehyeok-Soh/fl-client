#include "pch.h"
#include "Monster_Dummy.h"

#include "GameInstance.h"

#include "Monster_Body_Base.h"

#include "MonsterActionState.h"
#include "MonsterControlContext.h"
#include "StateBase_Monster.h"
#include "StatComponent.h"
#include "Model.h"
#include "PhysicsCCT.h"

#pragma region STATE
#include "StateMonster_Idle.h"
#include "StateMonster_Walk.h"
#include "StateMonster_Attack.h"
#pragma endregion

CMonster_Dummy::CMonster_Dummy(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
{
}

CMonster_Dummy::CMonster_Dummy(const CMonster_Dummy& rhs)
	: Super(rhs)
{
}

HRESULT CMonster_Dummy::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster_Dummy::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	//if (FAILED(Ready_Ability()))
	//	return E_FAIL;

	Set_Name("Monster_Dummy");

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_BaseStates()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster_Dummy::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;
	
	return S_OK;
}

void CMonster_Dummy::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CMonster_Dummy::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CMonster_Dummy::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CMonster_Dummy::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CMonster_Dummy::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	return S_OK;
}

void CMonster_Dummy::OnCollision(_uint iMyColliderLayer, CGameObject* pOther)
{
	Super::OnCollision(iMyColliderLayer, pOther);
}

void CMonster_Dummy::OnCollision_Enter(_uint iMyColliderLayer, CGameObject* pOther)
{
	Super::OnCollision_Enter(iMyColliderLayer, pOther);
}

void CMonster_Dummy::OnCollision_Exit(_uint iMyColliderLayer, CGameObject* pOther)
{
	Super::OnCollision_Exit(iMyColliderLayer, pOther);
}

void CMonster_Dummy::OnTrigger_Enter(_uint iMyColliderLayer, CGameObject* pOther)
{
	Super::OnTrigger_Enter(iMyColliderLayer, pOther);
}

void CMonster_Dummy::OnTrigger_Exit(_uint iMyColliderLayer, CGameObject* pOther)
{
	Super::OnTrigger_Exit(iMyColliderLayer, pOther);
}

HRESULT CMonster_Dummy::Ready_BaseStates()
{
	CMonsterActionState* pActionState = { nullptr };
	CModel* pModel = Get_Part<CMonster_Body_Base>(Part::BODY)->Get_Component<CModel>();
	if (!pModel)
		return E_FAIL;

	if (!(pActionState = Get_Component<CMonsterActionState>()))
		return E_FAIL;

	TIME_COUNTER tStateLifeTime = {};
	TIME_COUNTER tStateCoolDownTime = {};

	// Idle
	{
		CStateBase_Monster::MONSTER_STATEBASE_DESC  desc = {};
		desc.FAniFlags = CStateBase::STATEANI_FLAG::SA_HasPreAni;
		desc.vecPreAnims = {
								{State::WALK, Get_AnimationIndex(L"Animation_Monster_Dog_Attack_01")}
								,{State::IDLE, Get_AnimationIndex(L"Animation_Monster_Dog_Idle")}
		};
		desc.vecMainAnims = { Get_AnimationIndex(L"Animation_PlayerMoon_Idle") };
		desc.bBlend = false;
		desc.bLoop = true;

		tStateLifeTime.bCountTime = false;
		tStateLifeTime.bTimeReset = false;
		tStateLifeTime.fMaxTime = 0.05f;
		tStateLifeTime.fMinTime = 0.05f;

		tStateCoolDownTime.bCountTime = false;
		tStateCoolDownTime.bTimeReset = false;
		tStateCoolDownTime.fMaxTime = 0.05f;
		tStateCoolDownTime.fMinTime = 0.05f;

		desc.tStateLifeTime = tStateLifeTime;
		desc.tStateCoolDownTime = tStateCoolDownTime;

		if (FAILED(pActionState->Add_State(State::IDLE, CStateMonster_Idle::Create(pActionState, &desc))))
			return E_FAIL;
	}

	// Walk
	{
		CStateBase_Monster::MONSTER_STATEBASE_DESC  desc = {};
		desc.FAniFlags = CStateBase::STATEANI_FLAG::SA_HasPreAni;
		desc.vecPreAnims = {
								{State::RUN, Get_AnimationIndex(L"Animation_Monster_Dog_Run_Loop")}
		};
		desc.vecMainAnims = { Get_AnimationIndex(L"Animation_Monster_Dog_Idle") };
		desc.bBlend = false;
		desc.bLoop = true;

		tStateLifeTime.bCountTime = false;
		tStateLifeTime.bTimeReset = false;
		tStateLifeTime.fMaxTime = 0.05f;
		tStateLifeTime.fMinTime = 0.05f;

		tStateCoolDownTime.bCountTime = false;
		tStateCoolDownTime.bTimeReset = false;
		tStateCoolDownTime.fMaxTime = 0.05f;
		tStateCoolDownTime.fMinTime = 0.05f;

		desc.tStateLifeTime = tStateLifeTime;
		desc.tStateCoolDownTime = tStateCoolDownTime;

		if (FAILED(pActionState->Add_State(State::WALK, CStateMonster_Walk::Create(pActionState, &desc))))
			return E_FAIL;
	}

	// Attack
	{
		CStateBase_Monster::MONSTER_STATEBASE_DESC  desc = {};
		desc.FAniFlags = CStateBase::STATEANI_FLAG::SA_HasPreAni;
		desc.vecPreAnims = {
								{State::WALK, Get_AnimationIndex(L"Animation_Monster_Dog_Run_Loop")}
								,{State::IDLE, Get_AnimationIndex(L"Animation_Monster_Dog_Idle")}
		};
		desc.vecMainAnims = { Get_AnimationIndex(L"Animation_Monster_Dog_Attack_01") };
		desc.bBlend = false;
		desc.bLoop = true;

		tStateLifeTime.bCountTime = false;
		tStateLifeTime.bTimeReset = false;
		tStateLifeTime.fMaxTime = 0.05f;
		tStateLifeTime.fMinTime = 0.05f;

		tStateCoolDownTime.bCountTime = false;
		tStateCoolDownTime.bTimeReset = false;
		tStateCoolDownTime.fMaxTime = 0.05f;
		tStateCoolDownTime.fMinTime = 0.05f;

		desc.tStateLifeTime = tStateLifeTime;
		desc.tStateCoolDownTime = tStateCoolDownTime;

		if (FAILED(pActionState->Add_State(State::IDLE, CStateMonster_Walk::Create(pActionState, &desc))))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CMonster_Dummy::Ready_PartObjects()
{
	return S_OK;
}

HRESULT CMonster_Dummy::Ready_Components()
{
	if (FAILED(Add_Component<CMonsterControlContext>(0 /*static*/, L"Prototype_Component_ControlContext_Monster", nullptr)))
		return E_FAIL;

	return S_OK;
}

CMonster_Dummy* CMonster_Dummy::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CMonster_Dummy* pInsatnce = new CMonster_Dummy(pDevice, pDeviceContext);
	if (FAILED(pInsatnce->Initialize_Prototype()))
	{
		MSG_BOX("CMonster_Dummy::Create, Failed");
		Safe_Release(pInsatnce);
	}

	return pInsatnce;
}

CGameObject* CMonster_Dummy::Clone(void* pArg)
{
	CMonster_Dummy* pClone = new CMonster_Dummy(*this);
	if (FAILED(pClone->Initialize(pArg)))
	{
		MSG_BOX("CMonster_Dummy::Clone, Failed");
		Safe_Release(pClone);
	}
	return pClone;
}

void CMonster_Dummy::Free()
{
	Super::Free();
}
