#pragma once
#include "MonsterState_Factory.h"

inline HRESULT RegisterMonsterCondition(string name, std::function<_bool(class CStateBase_Monster*)> func)
{
	return CMonsterState_Factory::GetInstance()->RegisterCondition(name, func);
}

inline HRESULT RegisterMonsterFeature(string name, std::function<void(class CStateBase_Monster*, const _float& fTimeDelta)> func)
{
	return CMonsterState_Factory::GetInstance()->RegisterFeature(name, func);
}
