#include "pch.h"
#include "StateBase_Monster.h"

#include "MonsterState_Factory.h"

// has?
#include "Monster_Base.h"
#include "MonsterControlContext.h"

#include "ActionState.h"
#include "MonsterActionState.h"

// manager
#include "ControlContext.h"
#include "GameInstance.h"

CStateBase_Monster::CStateBase_Monster(CActionState* pOwnerComponent, const string& strName, _uint iStateIndex)
	: Super(pOwnerComponent, strName),
	m_iThisStateIndex(iStateIndex)
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
	m_tStateCoolDownTime.fTimeAcc = m_pDesc->tStateCoolDownTime.fMaxTime;
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

	if (FAILED(Bind_ConditionFeature()))
		return E_FAIL;

	if (FAILED(Bind_StartConditionFeature()))
		return E_FAIL;

	if (FAILED(Bind_EndConditionFeature()))
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

	Update_CooldownTime(0.f, true);

	// 혹시 dt관련 호출한다면 0.f 고정이오
	for (auto& conditionFeature : m_vecStartConditionFeature)
		if (conditionFeature.condition(conditionFeature.condParam))
			conditionFeature.feature(0.f, conditionFeature.featParam);

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
	if (Check_Transition(m_pDesc->vecGlobalStateTransition))
		return;
	
	// 로컬 상태 전이
	if (Check_Transition(m_pDesc->vecStateTransition))
		return;

	// 기능 실행
	for (auto& feat : m_vecFeature)
		feat.func(fTimeDelta, feat.tParam);

	for (auto& conditionfeature : m_vecConditionFeature)
	{
		if (conditionfeature.condition(conditionfeature.condParam))
			conditionfeature.feature(fTimeDelta, conditionfeature.featParam);
	}
}

HRESULT CStateBase_Monster::End()
{
	if (FAILED(Super::End()))
		return E_FAIL;

	// 여기도 dt 0.f 고정
	for (auto& cf : m_vecEndConditionFeature)
		if (cf.condition(cf.condParam))
			cf.feature(0.f, cf.featParam);

	if (m_tStateLifeTime.bTimeReset)
		m_tStateLifeTime.fTimeAcc = 0.f;

	return S_OK;
}

void CStateBase_Monster::Update_Time(TIME_COUNTER timer, _float fTimeDelta)
{
	if (timer.bCountTime)
		timer.CountTime(fTimeDelta);
}

void CStateBase_Monster::Update_CooldownTime(_float fTimeDelta, _bool bEntryStart)
{
	if (m_tStateCoolDownTime.bCountTime)
	{
		if (bEntryStart)
			m_tStateCoolDownTime.fTimeAcc = 0.f;
		else if(m_tStateCoolDownTime.fTimeAcc != m_tStateCoolDownTime.fMaxTime)
		{
			m_tStateCoolDownTime.fTimeAcc += fTimeDelta;
			if (m_tStateCoolDownTime.fTimeAcc >= m_tStateCoolDownTime.fMaxTime)
				m_tStateCoolDownTime.fTimeAcc = m_tStateCoolDownTime.fMaxTime;
		}
	}
}

void CStateBase_Monster::Change_MonsterState(_int eKey)
{
	_uint iNextState = eKey;
	Set_NextStateDesc(iNextState);		// next state에 대한 desc 작성
	Request_Change_State(iNextState, &m_tNextStateDesc);

	/* 플레이어가 이런 state를 이런 애니메이션으로 바꿨다 */
}

_bool CStateBase_Monster::IsCooldownTimeSatisfy()
{
	if (!m_tStateCoolDownTime.bCountTime)
		return true;

	return m_tStateCoolDownTime.fTimeAcc == m_tStateCoolDownTime.fMaxTime;
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
		auto itr = m_umapState.find(keyValue.first);
		if (itr == m_umapState.end())
			return E_FAIL;
		const auto &state = *itr;
		m_vecPreAnims.emplace_back(state.second, owner->Get_AnimationIndex(Engine_Utils::ToWString(keyValue.second)));
	}

	return S_OK;
}

HRESULT CStateBase_Monster::Bind_MainAnims()
{
	if (m_umapState.size() == 0)
		return E_FAIL;

	CGameObject* owner = m_pOwnerStateComp->Get_Owner();

	m_vecMainAnims.reserve(m_pDesc->vecMainAnimNames.size());
	for (auto& animName : m_pDesc->vecMainAnimNames)
		m_vecMainAnims.push_back(owner->Get_AnimationIndex(Engine_Utils::ToWString(animName)));
	// WeaponAni가 있을때
	if (Engine_Utils::Has_Flag(m_FAniFlags, SA_WeaponAni))
	{
		m_vecWeaponAnims.reserve(m_pDesc->vecWeaponAnimNames.size());
		for (auto& animWeaponNames : m_pDesc->vecWeaponAnimNames)
			m_vecWeaponAnims.push_back(owner->Get_WeaponAnimationIndex(Engine_Utils::ToWString(animWeaponNames)));
	}
	return S_OK;
}

HRESULT CStateBase_Monster::Bind_Transition(vector<DTO::STATE_TRANSITION>& transition)
{
	auto factory = CMonsterState_Factory::GetInstance();

	for (auto& trans : transition)
	{
		trans.vecConditionIdx.clear();
		trans.mapRandomStatePoolIdx.clear();
		trans.fTotalWeight = 0.f;

		// CONDITION_ENTRY 순회 런타임 바인딩
		for (auto& entry : trans.vecConditionEntry)
		{
			auto func = factory->GetCondition(entry.strCondition);
			if (func == nullptr)
				return E_FAIL;

			_uint idx = (_uint)m_vecCondition.size();

			BOUND_CONDITION bound{};
			bound.func = std::bind(func, this, std::placeholders::_1);
			bound.tParam = entry.tParam;

			m_vecCondition.push_back(bound);
			trans.vecConditionIdx.push_back(idx);
		}

		// 전이 상태 id 매핑
		for (auto& value : trans.mapRandomStatePool)
		{
			trans.fTotalWeight += value.second;

			auto itr = m_umapState.find(value.first);
			if (itr == m_umapState.end())
				return E_FAIL;
			_int stateIdx = itr->second;
			trans.mapRandomStatePoolIdx.emplace(stateIdx, value.second);
		}
	}

	return S_OK;
}

HRESULT CStateBase_Monster::Bind_Feature()
{
	auto factory = CMonsterState_Factory::GetInstance();

	m_vecFeature.clear();
	m_vecFeature.reserve(m_pDesc->vecFeatureEntry.size());
	// Entry 순회
    for (auto& entry : m_pDesc->vecFeatureEntry)
    {
		// Feature 이름으로 가져오기
        auto func = factory->GetFeature(entry.strFeature);
        if (func == nullptr)
            return E_FAIL;

		// 파람 및 함수 바인딩 후 벡터에 밀어넣기
        BOUND_FEATURE bound{};
		// 원본 시그니쳐 = state, fTimeDelta, param
		// state(this)는 지금 고정
		// fTimeDelta(_1) 호출시 첫번째 인자로 받겠다.
		// param(_2) 호출시 두번째 인자로 받겠다.
        bound.func = std::bind(func, this, std::placeholders::_1, std::placeholders::_2);
        bound.tParam = entry.tParam;
		m_vecFeature.push_back(bound);
    }

    return S_OK;
}

HRESULT CStateBase_Monster::Bind_ConditionFeature()
{
	auto factory = CMonsterState_Factory::GetInstance();

	m_vecConditionFeature.clear();
	m_vecConditionFeature.reserve(m_pDesc->vecConditionFeature.size());
	for (auto& conditionfeature : m_pDesc->vecConditionFeature)
	{
		auto condFunc = factory->GetCondition(conditionfeature.cond.strCondition);
		auto featFunc = factory->GetFeature(conditionfeature.feat.strFeature);

		if (condFunc == nullptr || featFunc == nullptr)
			return E_FAIL;

		BOUND_CONDFEATURE bound{};
		bound.condParam = conditionfeature.cond.tParam;
		bound.featParam = conditionfeature.feat.tParam;

		bound.condition = std::bind(condFunc, this, std::placeholders::_1);
		bound.feature = std::bind(featFunc, this, std::placeholders::_1, std::placeholders::_2);

		m_vecConditionFeature.push_back(bound);
	}

	return S_OK;
}

HRESULT CStateBase_Monster::Bind_StartConditionFeature()
{
	auto factory = CMonsterState_Factory::GetInstance();

	m_vecStartConditionFeature.clear();
	m_vecStartConditionFeature.reserve(m_pDesc->vecStartConditionFeature.size());

	for (auto& conditionfeature : m_pDesc->vecStartConditionFeature)
	{
		// feature는 필수
		auto featFunc = factory->GetFeature(conditionfeature.feat.strFeature);
		if (!featFunc)
			return E_FAIL;

		// cond는 비면 always로 치환
		string condName = conditionfeature.cond.strCondition.empty()
			? "condition_true_always"
			: conditionfeature.cond.strCondition;
		auto condFunc = factory->GetCondition(condName);
		if (!condFunc) return E_FAIL;

		BOUND_CONDFEATURE bound{};
		bound.condParam = conditionfeature.cond.tParam;
		bound.featParam = conditionfeature.feat.tParam;

		bound.condition = std::bind(condFunc, this, std::placeholders::_1);
		bound.feature = std::bind(featFunc, this, std::placeholders::_1, std::placeholders::_2);

		m_vecStartConditionFeature.push_back(bound);
	}
	return S_OK;
}

HRESULT CStateBase_Monster::Bind_EndConditionFeature()
{
	auto factory = CMonsterState_Factory::GetInstance();

	m_vecEndConditionFeature.clear();
	m_vecEndConditionFeature.reserve(m_pDesc->vecEndConditionFeature.size());

	for (auto& conditionfeature : m_pDesc->vecEndConditionFeature)
	{
		// feature는 필수
		auto featFunc = factory->GetFeature(conditionfeature.feat.strFeature);
		if (!featFunc)
			return E_FAIL;

		// cond는 비면 always로 치환
		string condName = conditionfeature.cond.strCondition.empty()
			? "condition_true_always"
			: conditionfeature.cond.strCondition;
		auto condFunc = factory->GetCondition(condName);
		if (!condFunc) return E_FAIL;

		BOUND_CONDFEATURE bound{};
		bound.condParam = conditionfeature.cond.tParam;
		bound.featParam = conditionfeature.feat.tParam;

		bound.condition = std::bind(condFunc, this, std::placeholders::_1);
		bound.feature = std::bind(featFunc, this, std::placeholders::_1, std::placeholders::_2);

		m_vecEndConditionFeature.push_back(bound);
	}
	return S_OK;
}

_bool CStateBase_Monster::Check_Transition(vector<DTO::STATE_TRANSITION>& transition)
{
	for (auto& trans : transition)
	{
		_bool allClear = { true };
		for (auto& condIdx : trans.vecConditionIdx)
		{
			if (m_vecCondition[condIdx].func(m_vecCondition[condIdx].tParam) == false)
			{
				allClear = false;
				break;
			}
		}

		// 모든 조건 통과
		if (allClear)
		{
			if (trans.fTotalWeight <= 0.f || trans.mapRandomStatePoolIdx.empty())
				continue;

			//_int total = (_int)trans.fTotalWeight;
			//if (total <= 0)
			//	continue;

			_int total = {};
			for (auto& to : trans.mapRandomStatePoolIdx)
			{
				if (static_cast<CMonsterActionState*>(m_pOwnerStateComp)->IsStateReady(to.first) == true)
					total += (_int)to.second;
			}

			if (total <= 0)
				continue;

			_float randomValue = (_float)(rand() % total);

			_float curWeight = {};
			for (auto& to : trans.mapRandomStatePoolIdx)
			{
				if (static_cast<CMonsterActionState*>(m_pOwnerStateComp)->IsStateReady(to.first) == false)
					continue;

				curWeight += to.second;
				if (randomValue < curWeight)
				{
					Change_MonsterState(to.first); // 다음 state로 change
#ifdef _DEBUG
					if (m_iThisStateIndex != to.first)
					{
						string toState{};
						for (auto pool : m_umapState)
						{
							if (pool.second == to.first)
							{
								toState = pool.first;
							}
						}
						wstring logInfo{};
						wstring separate{ L"\n//////////////////////////" };
						wstring owneName = m_pOwnerStateComp->Get_Owner()->Get_WName();
						wstring toInfo{ L"\nchange state to : " };
						wstring fromInfo{ L"\nfrom state : " };

						logInfo += separate;
						logInfo += (L"\ncharacter name : [" + owneName + L"]");
						logInfo += toInfo;
						logInfo += Engine_Utils::ToWString(toState);
						logInfo += fromInfo;
						logInfo += Engine_Utils::ToWString(m_strName);
						logInfo += separate;
						CLOG_INFO(logInfo);
					}
#endif // _DEBUG
					return m_iThisStateIndex != to.first;
				}
			}
		}
	}

	return false;
}

void CStateBase_Monster::Free()
{
	Super::Free();
}
