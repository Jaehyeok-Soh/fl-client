#include "pch.h"
#include "UIAction_Scheduler.h"
#include "Tool_Defines.h"
#include "GameInstance.h"

CUIAction_Scheduler::CUIAction_Scheduler()
{
}

CUIAction_Scheduler::CUIAction_Scheduler(const CUIAction_Scheduler& rhs)
{
}

HRESULT CUIAction_Scheduler::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIAction_Scheduler::Initialize_Prototype(ACTION_SCHEDULER_DESC* pDesc)
{
	m_pActionAgent = pDesc->pActionAgent;
	m_pActionTarget = pDesc->pActionTarget;

	if (!m_pActionAgent || !m_pActionTarget)
		return E_FAIL;

	m_pActionRegistry = m_pGameInstance->Get_UIAction_Registry();
	Safe_AddRef(m_pActionRegistry);
	return S_OK;
}

HRESULT CUIAction_Scheduler::Initialize(void* pArg)
{
	return S_OK;
}

void CUIAction_Scheduler::Update(const _float fTimeDelta)
{
	for (size_t i = 0; i < m_vecSchedules.size(); )
	{
		auto& Schedule = m_vecSchedules[i];
		Schedule.fRemain -= fTimeDelta;

		if (Schedule.fRemain <= 0.f)
		{
			if (Schedule.Func)
			{
				Schedule.Func(m_pActionAgent, m_pActionTarget);
			}

			Schedule = std::move(m_vecSchedules.back());
			m_vecSchedules.pop_back();
			continue;
		}
		i++;
	}
}

void CUIAction_Scheduler::Push_Action_Scheduler(const _float fDelay, Engine::CUIAction_Registry::ActionFunc func)
{
	if (fDelay <= 0.f)
	{
		func(m_pActionAgent, m_pActionTarget);
		return;
	}
	m_vecSchedules.push_back(SCHEDULE_DESC{ fDelay, std::move(func) });
}

CUIAction_Scheduler* CUIAction_Scheduler::Create(ACTION_SCHEDULER_DESC* pDesc)
{
	CUIAction_Scheduler* pInstance = new CUIAction_Scheduler();
	if (FAILED(pInstance->Initialize_Prototype(pDesc)))
	{
		MSG_BOX("CUIAction_Scheduler::Create,CreateFailed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CUIAction_Scheduler::Clone(void* pArg)
{
	CUIAction_Scheduler* pInstance = new CUIAction_Scheduler(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUIAction_Scheduler::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUIAction_Scheduler::Free()
{
	Safe_Release(m_pActionRegistry);
	Super::Free();
}



