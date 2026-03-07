#include "pch.h"
#include "StatCom_Boss.h"
#include "GameInstance.h"

CStatCom_Boss::CStatCom_Boss()
	: Super()
{
}

CStatCom_Boss::CStatCom_Boss(const CStatCom_Boss& rhs)
	: Super(rhs)
{
}

HRESULT CStatCom_Boss::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CStatCom_Boss::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	BOSS_STAT_DESC* pDesc = static_cast<BOSS_STAT_DESC*>(pArg);
	m_tExtra_AttackDesc.vecCompute_Order = std::move(pDesc->vecExtraComputeOrder);
	m_fCriticalRate = pDesc->fCriticalRate;
	m_fCirticalAttack = pDesc->fCriticalAttack;

	return S_OK;
}

CStatCom_Boss* CStatCom_Boss::Create()
{
	CStatCom_Boss* pInstance = new CStatCom_Boss();
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CStatCom_Boss::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CStatCom_Boss::Clone(void* pArg)
{
	CStatCom_Boss* pInstance = new CStatCom_Boss(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CStatCom_Boss::Clone, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CStatCom_Boss::Free()
{
	Super::Free();
}
