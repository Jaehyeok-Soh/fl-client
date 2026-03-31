#include "pch.h"
#include "State_SpawnAttack.h"
#include "GameObject.h"
#include "MonsterActionState.h"
#include "MonsterControlContext.h"
#include "Lianhuo_GimmikController.h"
#include "GameInstance.h"

CState_SpawnAttack::CState_SpawnAttack(CActionState* pOwnerComponent, _uint iStateIndex)
	: Super(pOwnerComponent, "SpawnAttack", iStateIndex)
{

}

HRESULT CState_SpawnAttack::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_SpawnAttack::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	m_pOwnerGimmikController = Get_OwnerObject()->Get_Component<CLianhuo_GimmikController>();
	return S_OK;
}

HRESULT CState_SpawnAttack::Start(void* pArg, _bool bForce)
{
	if (FAILED(Super::Start(pArg, bForce)))
		return E_FAIL;

	CTransform* pTargetTransform = Get_Target()->Get_Component<CTransform>();
	m_pOwnerGimmikController->Trigger_XSpace(pTargetTransform->Get_Info(TRANSFORM_INFO_STATE::POS));
	return S_OK;
}

void CState_SpawnAttack::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
	if (Is_AnimFinished())
	{
		Change_MonsterState(m_umapState["Idle"]);
		return;
	}


}

HRESULT CState_SpawnAttack::End()
{
	return Super::End();
}

CState_SpawnAttack* CState_SpawnAttack::Create(CActionState* pOwnerComponent, _uint iStateIndex, void* pArg)
{
	CState_SpawnAttack* pInstance = new CState_SpawnAttack(pOwnerComponent, iStateIndex);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CState_SpawnAttack::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CState_SpawnAttack::Free()
{
	Super::Free();
}
