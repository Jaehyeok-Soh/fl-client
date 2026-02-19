#include "pch.h"
#include "MonsterActionState.h"

CMonsterActionState::CMonsterActionState()
	: Super()
{
}

CMonsterActionState::CMonsterActionState(const CMonsterActionState& rhs)
	: Super(rhs)
{
}

HRESULT CMonsterActionState::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonsterActionState::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

CMonsterActionState* CMonsterActionState::Create()
{
	CMonsterActionState* pInsatnce = new CMonsterActionState();
	if (FAILED(pInsatnce->Initialize_Prototype()))
	{
		MSG_BOX("CMonsterActionState::Create, Failed");
		Safe_Release(pInsatnce);
	}
	return pInsatnce;
}

CComponent* CMonsterActionState::Clone(void* pArg)
{
	CMonsterActionState* pClone = new CMonsterActionState(*this);
	if (FAILED(pClone->Initialize(pArg)))
	{
		MSG_BOX("CMonsterActionState::Clone, Failed");
		Safe_Release(pClone);
	}
	return pClone;
}

void CMonsterActionState::Free()
{
	Super::Free();
}
