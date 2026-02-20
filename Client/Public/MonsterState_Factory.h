#pragma once
#include "Base.h"

NS_BEGIN(Client)

class CMonsterState_Factory final : public CBase
{
	DECLARE_SINGLETON(CMonsterState_Factory)

	using Super = CBase;

private:
	CMonsterState_Factory();
	virtual ~CMonsterState_Factory() = default;

	HRESULT Ready_Condition();
	HRESULT Ready_Feature();

public:
	HRESULT Initialize();

	HRESULT RegisterCondition(string name, std::function<_bool(class CStateBase_Monster*)> func);
	std::function<_bool(class CStateBase_Monster*)> GetCondition(string name);

	HRESULT RegisterFeature(string name, std::function<void(class CStateBase_Monster*, const _float& fTimeDelta)> func);
	std::function<void(class CStateBase_Monster*, const _float& fTimeDelta)> GetFeature(string name);

private:
	map<string, std::function<_bool(class CStateBase_Monster*)>> m_mapCondition;
	map<string, std::function<void(class CStateBase_Monster*, const _float& fTimeDelta)>> m_mapFeature;

public:
	virtual void Free() override;
};

NS_END