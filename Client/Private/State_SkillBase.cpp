#include "pch.h"
#include "State_SkillBase.h"

CState_SkillBase::CState_SkillBase(CActionState* pOwnerComponent, const string& strName)
	: Super(pOwnerComponent, strName)
{
}

HRESULT CState_SkillBase::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_SkillBase::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_SkillBase::Start(void* pArg, _bool bForce)
{
	if (FAILED(Super::Start(pArg, bForce)))
		return E_FAIL;

	Start_Att(m_iPlayerState);

	return S_OK;
}

void CState_SkillBase::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

HRESULT CState_SkillBase::End()
{
	if (FAILED(Super::End()))
		return E_FAIL;

	return S_OK;
}

void CState_SkillBase::Free()
{
	Super::Free();
}
