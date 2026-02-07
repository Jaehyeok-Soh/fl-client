#pragma once
#include "MonoBehaviour.h"
#include "UIAction_Registry.h"

#include "IUIActionForMe.h"
#include "IUIActionForTarget.h"

NS_BEGIN(Tool)

class CToolUI;
class CUIAction_Scheduler final : public CMonoBehaviour
{
	using Super = CMonoBehaviour;
public:
	typedef struct tagActionSchedulerDesc : public MONO_DESC
	{
		Engine::IUIActionForMe* pActionAgent = { nullptr };
		Engine::IUIActionForTarget* pActionTarget = { nullptr };
	}ACTION_SCHEDULER_DESC;

	typedef struct tagScheduleDesc
	{
		_float fRemain = {};
		Engine::CUIAction_Registry::ActionFunc Func;

	}SCHEDULE_DESC;

private:
	CUIAction_Scheduler();
	CUIAction_Scheduler(const CUIAction_Scheduler& rhs);
	virtual ~CUIAction_Scheduler() = default;

	HRESULT Initialize_Prototype() override;
	HRESULT Initialize_Prototype(ACTION_SCHEDULER_DESC* pDesc);
	HRESULT Initialize(void* pArg) override;

public:
	void Update(const _float fTimeDelta) override;

public:
	/* 복사 1회 발생 */
	void Push_Action_Scheduler(const _float fDelay, Engine::CUIAction_Registry::ActionFunc func);

private:
	vector<SCHEDULE_DESC> m_vecSchedules;

	/* 나를 향한 Action */
	Engine::IUIActionForMe* m_pActionAgent = { nullptr };
	/* 상대를 향한 Action */
	Engine::IUIActionForTarget* m_pActionTarget = { nullptr };

	Engine::CUIAction_Registry* m_pActionRegistry = { nullptr };

public:
	static CUIAction_Scheduler* Create(ACTION_SCHEDULER_DESC* pDesc);
	CComponent* Clone(void* pArg);
	virtual void Free() override;
};

NS_END
