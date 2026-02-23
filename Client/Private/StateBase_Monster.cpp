#include "pch.h"
#include "StateBase_Monster.h"

#include "MonsterState_Factory.h"

// has?
#include "Monster_Base.h"
#include "MonsterControlContext.h"

#include "ActionState.h"
#include "MonsterActionState.h"

// manager
#include "GameInstance.h"
#include "ControlContext.h"
#include "Engine_Utils.h"

CStateBase_Monster::CStateBase_Monster(CActionState* pOwnerComponent, const string& strName)
	: Super(pOwnerComponent, strName)
{
}

HRESULT CStateBase_Monster::Initialize(void* pArg)
{
	m_pDesc = static_cast<DTO::MONSTER_STATEBASE_DESC*>(pArg);

	if (m_pDesc == nullptr)
		return E_FAIL;

	CStateBase::STATE_DESC baseDesc{};
	baseDesc.bBlend = m_pDesc->bBlend;
	baseDesc.bLoop = m_pDesc->bLoop;
	baseDesc.FAniFlags = m_pDesc->FAniFlags;

	if (FAILED(Super::Initialize(&baseDesc)))
		return E_FAIL;

	m_strName = m_pDesc->strName;

	m_tStateLifeTime.bCountTime = m_pDesc->tStateLifeTime.bCountTime;
	m_tStateLifeTime.bTimeReset = m_pDesc->tStateLifeTime.bTimeReset;
	m_tStateLifeTime.fMaxTime = m_pDesc->tStateLifeTime.fMaxTime;
	m_tStateLifeTime.fMinTime = m_pDesc->tStateLifeTime.fMinTime;

	m_tStateCoolDownTime.bCountTime = m_pDesc->tStateCoolDownTime.bCountTime;
	m_tStateCoolDownTime.bTimeReset = m_pDesc->tStateCoolDownTime.bTimeReset;
	m_tStateCoolDownTime.fMaxTime = m_pDesc->tStateCoolDownTime.fMaxTime;
	m_tStateCoolDownTime.fMinTime = m_pDesc->tStateCoolDownTime.fMinTime;

	if (FAILED(Bind_State()))
		return E_FAIL;

	if (FAILED(Bind_Transition(m_pDesc->vecStateTransition)))
		return E_FAIL;

	if (FAILED(Bind_Transition(m_pDesc->vecGlobalStateTransition)))
		return E_FAIL;

	if (FAILED(Bind_Feature()))
		return E_FAIL;

	return S_OK;
}

HRESULT CStateBase_Monster::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	if (m_pDesc == nullptr)
		return E_FAIL;

	if (FAILED(Bind_PreAnims()))
		return E_FAIL;

	if (FAILED(Bind_MainAnims()))
		return E_FAIL;

	m_iEndStateIdx = CMonster_Base::State::END;

	return S_OK;
}

HRESULT CStateBase_Monster::Start(void* pArg, _bool bForce)
{
	if (FAILED(Super::Start(pArg, bForce)))
		return E_FAIL;

	return S_OK;
}

void CStateBase_Monster::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

	// 만약 이전 애니메이션때 변화하기 싫은데 아직 preAni가 끝나지 않았다면 : key 입력 처리를 하지 않음
	if (Engine_Utils::Has_Flag(m_FAniFlags, STATEANI_FLAG::SA_PreNonEvent) &&
		!Engine_Utils::Has_Flag(m_FAniFlags, STATEANI_FLAG::SA_PreAniDone))
		return;

	// 글로벌 전이부터 검사 ( Die, Damage 등)
	Check_Transition(m_pDesc->vecGlobalStateTransition);

	// 로컬 상태 전이
	Check_Transition(m_pDesc->vecStateTransition);

	// 기능 실행
	for (auto& featIdx : m_pDesc->vecFeatureIdx)
		m_vecFeature[featIdx](fTimeDelta);
}

HRESULT CStateBase_Monster::End()
{
	if (FAILED(Super::End()))
		return E_FAIL;

	return S_OK;
}

void CStateBase_Monster::Change_MonsterState(_int eKey)
{
	_uint iNextState = eKey;
	Set_NextStateDesc(iNextState);		// next state에 대한 desc 작성
	Request_Change_State(iNextState, &m_tNextStateDesc);

	/* 플레이어가 이런 state를 이런 애니메이션으로 바꿨다 */
}

_bool CStateBase_Monster::Has_ChangeState(_int eKey)
{
	// state end 이면 state change를 안 한다
	return m_iEndStateIdx != eKey;
}

HRESULT CStateBase_Monster::Bind_State()
{
	m_umapState = static_cast<CMonsterActionState*>(m_pOwnerStateComp)->GetUmapState();

	return S_OK;
}

HRESULT CStateBase_Monster::Bind_PreAnims()
{
	if (m_umapState.size() == 0)
		return E_FAIL;

	CGameObject* owner = m_pOwnerStateComp->Get_Owner();

	for (auto& keyValue : m_pDesc->mapPreAnimNames)
	{
		auto state = m_umapState.find(keyValue.first);
		m_vecPreAnims.emplace_back((*state).second, owner->Get_AnimationIndex(Engine_Utils::ToWString(keyValue.second)));
	}

	return S_OK;
}

HRESULT CStateBase_Monster::Bind_MainAnims()
{
	if (m_umapState.size() == 0)
		return E_FAIL;

	CGameObject* owner = m_pOwnerStateComp->Get_Owner();

	for (auto& animName : m_pDesc->vecMainAnimNames)
		m_vecMainAnims.push_back(owner->Get_AnimationIndex(Engine_Utils::ToWString(animName)));

	return S_OK;
}

HRESULT CStateBase_Monster::Bind_Transition(vector<DTO::STATE_TRANSITION>& transition)
{
	for (auto& trans : transition)
	{
		if (FAILED(Bind_Condition(trans.vecCondition)))
			return E_FAIL;

		// 조건 id 매핑
		trans.vecConditionIdx.reserve(trans.vecCondition.size());
		for (auto& cond : trans.vecCondition)
		{
			auto iter = m_umapCondition.find(cond);
			if (iter == m_umapCondition.end())
				continue;

			trans.vecConditionIdx.push_back((*iter).second);
		}

		// 전이 상태 id 매핑
		for (auto& value : trans.mapRandomStatePool)
		{
			trans.fTotalWeight += value.second;

			_int stateIdx = (*m_umapState.find(value.first)).second;
			trans.mapRandomStatePoolIdx.emplace(stateIdx, value.second);
		}
	}

	return S_OK;
}

HRESULT CStateBase_Monster::Bind_Condition(vector<string> conds)
{
	auto factory = CMonsterState_Factory::GetInstance();

	m_vecCondition.reserve(conds.size());

	for (auto& cond : conds)
	{
		auto func = factory->GetCondition(cond);
		if (func == nullptr)
			return E_FAIL;

		size_t idx = m_vecCondition.size();

		auto iter = m_umapCondition.find(cond);
		if (iter != m_umapCondition.end())
			continue;

		m_umapCondition.emplace(cond, idx);
		m_vecCondition.push_back(std::bind(func, this));
	}

	return S_OK;
}

HRESULT CStateBase_Monster::Bind_Feature()
{
	auto factory = CMonsterState_Factory::GetInstance();

	m_vecFeature.reserve(m_pDesc->vecFeature.size());
	for (auto& feat : m_pDesc->vecFeature)
	{
		auto func = factory->GetFeature(feat);
		if (func == nullptr)
			return E_FAIL;

		size_t idx = m_vecFeature.size();

		auto iter = m_umapFeature.find(feat);
		if (iter != m_umapFeature.end())
			continue;

		m_umapFeature.emplace(feat, idx);
		m_vecFeature.push_back(std::bind(func, this, std::placeholders::_1));
	}

	m_pDesc->vecFeatureIdx.reserve(m_pDesc->vecFeature.size());
	for (auto& feat : m_pDesc->vecFeature)
	{
		auto iter = m_umapFeature.find(feat);
		if (iter == m_umapFeature.end())
			continue;

		m_pDesc->vecFeatureIdx.push_back((*iter).second);
	}

	return S_OK;
}

void CStateBase_Monster::Check_Transition(vector<DTO::STATE_TRANSITION>& transition)
{
	for (auto& trans : transition)
	{
		_bool allClear = { true };
		for (auto& condIdx : trans.vecConditionIdx)
		{
			if (m_vecCondition[condIdx]() == false)
			{
				allClear = false;
				break;
			}
		}

		// 모든 조건 통과
		if (allClear)
		{
			_float randomValue = (rand() % (_int)trans.fTotalWeight);

			_float curWeight = {};
			for (auto& to : trans.mapRandomStatePoolIdx)
			{
				curWeight += to.second;
				if (randomValue < curWeight)
				{
					Change_MonsterState(to.first); // 다음 state로 change
					return;
				}
			}
		}
	}
}

void CStateBase_Monster::Free()
{
	Super::Free();
}
