#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class CModel;

class ENGINE_DLL CGimmikController abstract : public CComponent
{
	using Super = CComponent;
public:
	constexpr static EComponentType _ID = EComponentType::GIMMIKCTRL;
	typedef struct tagGimmikControllerDesc
	{
		CModel* pOwnerModel{ nullptr };
	}GIMMIKCTRL_DESC;
protected:
	CGimmikController();
	CGimmikController(const CGimmikController& rhs);
	virtual ~CGimmikController() = default;

	virtual HRESULT						Initialize_Prototype() override;
	virtual HRESULT						Initialize(void* pArg) override;
public:

protected:
	///////////////
	//// Event ////
	///////////////
	void Bind_ModelAnimNotify();
	void Unbind_ModelAnimNotify();
	virtual void On_ModelAnimNotify(const AnimNotifyKey& key) PURE;
protected:
	CModel* m_pOwnerModel{ nullptr };
	DelegateHandle m_hAnimNotifyHandle;
public:
	virtual CComponent* Clone(void* pArg) PURE;
	virtual void Free() override;
};

NS_END