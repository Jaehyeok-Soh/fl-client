#include "pch.h"
#include "State_MoonQSkill.h"

CState_MoonQSkill::CState_MoonQSkill(CActionState* pOwnerComponent)
	:Super(pOwnerComponent,"Skill_Q")
{
}

HRESULT CState_MoonQSkill::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_MoonQSkill::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_MoonQSkill::Start(void* pArg, _bool bForce)
{
	if (FAILED(Super::Start(pArg, bForce)))
		return E_FAIL;

	return S_OK;
}

void CState_MoonQSkill::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

HRESULT CState_MoonQSkill::End()
{
	if (FAILED(Super::End()))
		return E_FAIL;

	return S_OK;
}

CState_MoonQSkill* CState_MoonQSkill::Create(CActionState* pOwnerComponent, void* pArg)
{
	CState_MoonQSkill* pInstance = new CState_MoonQSkill(pOwnerComponent);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CState_MoonQSkill::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CState_MoonQSkill::Free()
{
	__super::Free();
}
