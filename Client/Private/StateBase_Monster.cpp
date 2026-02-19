#include "pch.h"
#include "StateBase_Monster.h"

// has?
#include "Monster_Base.h"
#include "MonsterControlContext.h"

#include "ActionState.h"

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
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	m_pDesc = static_cast<DTO::MONSTER_STATEBASE_DESC*>(pArg);
	
	m_strName = m_pDesc->strName;

	if (FAILED(Bind_State(m_pDesc->setStates)))
		return E_FAIL;

	if (FAILED(Bind_Condition(m_pDesc->mapTransferCondition)))
		return E_FAIL;
	
	if (FAILED(Bind_Feature()))
		return E_FAIL;
	

	return S_OK;
}

HRESULT CStateBase_Monster::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	if (FAILED(Bind_PreAnims(m_pDesc->mapPreAnimNames)))
		return E_FAIL;

	if (FAILED(Bind_MainAnims(m_pDesc->vecMainAnimNames)))
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

	// keyCount를 하지 않거나, coolTime이 다 되었다면 : key 입력을 처리하자
	if (!(m_tStateLifeTime.bCountTime)
		|| m_tStateLifeTime.CountMinTime(fTimeDelta) == 1.f
		|| m_tStateLifeTime.CountTime(fTimeDelta) == 1.f)
	{
		if (!m_bLoop && Is_MainAnimFinished())		// loop가 아닌데 애니메이션이 끝났다면 : pre animation이랑 잘 해야될듯..?
		{
			Change_MonsterState(CMonster_Base::State::Enum::LOOPDONE);			// 다음 state로 change
			return;
		}
	}
}

HRESULT CStateBase_Monster::End()
{
	if (FAILED(Super::End()))
		return E_FAIL;

	return S_OK;
}

void CStateBase_Monster::Change_MonsterState(CMonster_Base::State::Enum eKey)
{
	_uint iNextState = eKey;
	Set_NextStateDesc(iNextState);		// next state에 대한 desc 작성
	Request_Change_State(iNextState, &m_tNextStateDesc);

	/* 플레이어가 이런 state를 이런 애니메이션으로 바꿨다 */
}

_bool CStateBase_Monster::Has_ChangeState(CMonster_Base::State::Enum eKey)
{
	// state end 이면 state change를 안 한다
	return m_iEndStateIdx != eKey;
}

HRESULT CStateBase_Monster::Bind_State(std::set<string> states)
{
	m_umapState.clear();

	m_umapState.reserve(states.size());
	for (auto iter = states.begin(); iter != states.end(); iter++)
	{
		m_umapState.emplace(*iter, std::distance(states.begin(), iter));
		iter++;
	}

	return S_OK;
}

HRESULT CStateBase_Monster::Bind_PreAnims(map<string, string> stateAnimMap)
{
	if (m_umapState.size() == 0)
		return E_FAIL;

	CGameObject* owner = m_pOwnerStateComp->Get_Owner();
	

	for (auto& keyValue : stateAnimMap)
	{
		auto state = m_umapState.find(keyValue.first);
		m_vecPreAnims.emplace_back((*state).second, owner->Get_AnimationIndex(Engine_Utils::ToWString(keyValue.second)));
	}

	return S_OK;
}

HRESULT CStateBase_Monster::Bind_MainAnims(vector<string> mainAnimNames)
{
	if (m_umapState.size() == 0)
		return E_FAIL;

	CGameObject* owner = m_pOwnerStateComp->Get_Owner();

	for (auto& animName : mainAnimNames)
		m_vecMainAnims.push_back(owner->Get_AnimationIndex(Engine_Utils::ToWString(animName)));

	return S_OK;
}

HRESULT CStateBase_Monster::Bind_Condition(map<string, string> transferConditionMap)
{
	return S_OK;
}

HRESULT CStateBase_Monster::Bind_Feature()
{


	return S_OK;
}

void CStateBase_Monster::Free()
{
	Super::Free();
}
