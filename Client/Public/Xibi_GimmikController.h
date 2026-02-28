#pragma once
#include "GimmikController.h"

NS_BEGIN(Client)

class CXibi_GimmikController final : public CGimmikController
{
	using Super = CGimmikController;
public:
	enum class EGimmikType
	{
		TeleportRandom,
		TeleportCenter,
		COUNT
	};
private:
	CXibi_GimmikController();
	CXibi_GimmikController(const CXibi_GimmikController& rhs);
	virtual ~CXibi_GimmikController() = default;

	virtual HRESULT						Initialize_Prototype() override;
	virtual HRESULT						Initialize(void* pArg) override; 
public:

private:
	void On_TeleportRandom();
	void On_TeleportCenter();
	virtual void On_ModelAnimNotify(const AnimNotifyKey& key) override;
public:
	static CXibi_GimmikController* Create();
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END