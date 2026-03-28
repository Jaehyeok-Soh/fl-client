#pragma once
#include "GimmikController.h"

NS_BEGIN(Client)

class CLianhuo_GimmikController final : public CGimmikController
{
	using Super = CGimmikController;
private:
	enum class EGimmikType
	{
		END
	};
private:
	CLianhuo_GimmikController();
	CLianhuo_GimmikController(const CLianhuo_GimmikController& rhs);
	virtual ~CLianhuo_GimmikController() = default;

	virtual HRESULT						Initialize_Prototype() override;
	virtual HRESULT						Initialize(void* pArg) override;
private:
	HRESULT Awake(const _uint iCurLevelIndex);
	void Update(const _float fTimeDelta);
public:
	virtual HRESULT Bind_Events() override;
private:
protected:
	///////////////
	//// Event ////
	///////////////
	virtual void On_ModelAnimNotify(const AnimNotifyKey& key) override;
public:
	static CLianhuo_GimmikController* Create();
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END