#pragma once
#include "GimmikController.h"

NS_BEGIN(Client)

class CXibi_GimmikController final : public CGimmikController
{
	using Super = CGimmikController;
public:
	enum class EGimmikType
	{
		TeleportRandom_Disappear,
		TeleportRandom_Appear,
		TeleportCenter_Disappear,
		TeleportCenter_Appear,
		COUNT
	};
private:
	CXibi_GimmikController();
	CXibi_GimmikController(const CXibi_GimmikController& rhs);
	virtual ~CXibi_GimmikController() = default;

	virtual HRESULT						Initialize_Prototype() override;
	virtual HRESULT						Initialize(void* pArg) override; 
public:
	HRESULT Awake(const _uint iCurLevelIndex);
private:
	//TODO - Åø·Î »©±â
	HRESULT Set_Event();
	void On_TeleportRandom_Disappear();
	void On_TeleportRandom_Appear();
	void On_TeleportCenter_Disappear();
	void On_TeleportCenter_Appear();
	virtual void On_ModelAnimNotify(const AnimNotifyKey& key) override;
public:
	static CXibi_GimmikController* Create();
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END