#include "pch.h"
#include "StatCom_Boss.h"
#include "GameObject.h"
#include "MonsterControlContext.h"
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

	Reset_GroggyStats();
	return S_OK;
}

EGroggyState CStatCom_Boss::Sub_Groggy(_float fValue)
{
	const _float fPrev = m_vGroggy.x;
	m_vGroggy.x = (std::clamp)(m_vGroggy.x - fValue, GROGGY_MIN, GROGGY_MAX);
	const _float fAfter = m_vGroggy.x;
	const _float fThreshold = GROGGY_MAX * 0.5f;

	// 마지막 그로기가 안되었을 때
	if (Engine_Utils::Has_Flag(m_iGroggyFlag, ENUM_TO_UINT(EGroggyState::Final)) == false)
	{
		const _bool bCrossed = (fPrev > GROGGY_MIN) && (fAfter <= GROGGY_MIN);
		if (bCrossed)
		{
			Engine_Utils::Add_Flag(m_iGroggyFlag, ENUM_TO_UINT(EGroggyState::Final));
			Engine_Utils::Add_Flag(m_iGroggyFlag, ENUM_TO_UINT(EGroggyState::Middle));
			return EGroggyState::Final;
		}
	}
	
	// 중간 그로기가 안되었을 때
	if (Engine_Utils::Has_Flag(m_iGroggyFlag, ENUM_TO_UINT(EGroggyState::Middle)) == false)
	{
		const _bool bCrossed_50 = (fPrev > fThreshold) && (fAfter <= fThreshold);
		if (bCrossed_50)
		{
			Engine_Utils::Add_Flag(m_iGroggyFlag, ENUM_TO_UINT(EGroggyState::Middle));
			return EGroggyState::Middle;
		}
	}

	return EGroggyState::None;
}

void CStatCom_Boss::Sub_Hp(_float fHealth)
{
	m_vHealth.x += fHealth;

	if (m_vHealth.x < 0)
		m_vHealth.x = 0;

	else if ((m_vHealth.x / m_vHealth.y <= 0.5f))
	{
		CMonsterControlContext* pCC = Get_Owner()->Get_Component<CMonsterControlContext>();
		if (pCC!= nullptr && pCC->IsPhaseTwo() == false)
			pCC->Set_PhaseTwo();
	}
}

void CStatCom_Boss::Reset_GroggyStats()
{
	Engine_Utils::RemoveHard_Flag(m_iGroggyFlag, ENUM_TO_UINT(EGroggyState::Final));
	Engine_Utils::RemoveHard_Flag(m_iGroggyFlag, ENUM_TO_UINT(EGroggyState::Middle));
	m_vGroggy = { GROGGY_MAX, GROGGY_MAX };
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
