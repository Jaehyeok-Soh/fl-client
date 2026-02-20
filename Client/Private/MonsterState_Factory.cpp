#include "pch.h"
#include "MonsterState_Factory.h"

#include "StateBase_Monster.h"

IMPLEMENT_SINGLETON(CMonsterState_Factory)

CMonsterState_Factory::CMonsterState_Factory()
	: Super()
{
}

HRESULT CMonsterState_Factory::Initialize()
{
	if (FAILED(Ready_Condition()))
		return E_FAIL;

	if (FAILED(Ready_Feature()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonsterState_Factory::Ready_Condition()
{
	HRESULT result = {};

	result = RegisterCondition("cond_", [](class CStateBase_Monster* state)->_bool {

		});
	
	if (FAILED(result))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonsterState_Factory::Ready_Feature()
{
	HRESULT result = {};

	result = RegisterFeature("feat_", [](class CStateBase_Monster* state, const _float& fTimeDelta) {

		});

	if (FAILED(result))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonsterState_Factory::RegisterCondition(string name, std::function<_bool(class CStateBase_Monster*)> func)
{
	auto iter = m_mapCondition.find(name);
	if (iter != m_mapCondition.end())
		return E_FAIL;

	m_mapCondition.emplace(name, func);

	return S_OK;
}

std::function<_bool(class CStateBase_Monster*)> CMonsterState_Factory::GetCondition(string name)
{
	auto iter = m_mapCondition.find(name);
	if (iter == m_mapCondition.end())
		return nullptr;

	return (*iter).second;
}

HRESULT CMonsterState_Factory::RegisterFeature(string name, std::function<void(class CStateBase_Monster*, const _float& fTimeDelta)> func)
{
	auto iter = m_mapFeature.find(name);
	if (iter != m_mapFeature.end())
		return E_FAIL;

	m_mapFeature.emplace(name, func);

	return S_OK;
}

std::function<void(class CStateBase_Monster*, const _float& fTimeDelta)> CMonsterState_Factory::GetFeature(string name)
{
	auto iter = m_mapFeature.find(name);
	if (iter == m_mapFeature.end())
		return nullptr;

	return (*iter).second;
}

void CMonsterState_Factory::Free()
{
	for (auto& cond : m_mapCondition)
		Safe_Delete(cond.second);
	m_mapCondition.clear();

	for (auto& feat : m_mapFeature)
		Safe_Delete(feat.second);
	m_mapFeature.clear();
}
