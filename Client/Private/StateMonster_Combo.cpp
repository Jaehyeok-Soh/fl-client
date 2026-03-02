#include "pch.h"
#include "StateMonster_Combo.h"

#include "Monster_Base.h"
#include "MonsterControlContext.h"

#include "ActionState.h"

#include "Engine_Utils.h"

CStateMonster_Combo::CStateMonster_Combo(CActionState* pOwnerComponent, _uint iStateIndex)
	: Super(pOwnerComponent, "MonsterCombo", iStateIndex)
{
}

HRESULT CStateMonster_Combo::Initialize(void* pArg)
{
	MONSTERCOMBO_DESC* tMyDesc = static_cast<MONSTERCOMBO_DESC*>(pArg);

	{
		CGameObject* owner = m_pOwnerStateComp->Get_Owner();

		//for (auto& animName : tMyDesc->vecComboAnimName)
		//	tMyDesc->vecMainAnims.push_back(owner->Get_AnimationIndex(Engine_Utils::ToWString(animName)));
	}
	
	if (FAILED(Super::Initialize(&tMyDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CStateMonster_Combo::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CStateMonster_Combo::Start(void* pArg, _bool bForce)
{
	if (FAILED(Super::Start(pArg, bForce)))
		return E_FAIL;

	return S_OK;
}

void CStateMonster_Combo::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

HRESULT CStateMonster_Combo::End()
{
	if (FAILED(Super::End()))
		return E_FAIL;

	return S_OK;
}

CStateMonster_Combo* CStateMonster_Combo::Create(CActionState* pOwnerComponent, _uint iStateIndex, void* pArg)
{
	CStateMonster_Combo* pInstance = new CStateMonster_Combo(pOwnerComponent, iStateIndex);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CStateMonster_Combo::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CStateMonster_Combo::Free()
{
	Super::Free();
}
