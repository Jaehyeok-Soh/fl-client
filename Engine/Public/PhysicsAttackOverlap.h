#pragma once
#include "Component.h"
#include "AttackOverlap_Desc.h"

NS_BEGIN(Engine)

class CActiveAttackOverlap;
class CModel;

class ENGINE_DLL CPhysicsAttackOverlap final : public CComponent
{
private:
	using Super = CComponent;

public:
	constexpr static EComponentType _ID = EComponentType::PX_ATTACKOVERLAP;

private:
	CPhysicsAttackOverlap();
	CPhysicsAttackOverlap(const CPhysicsAttackOverlap& rhs);
	virtual ~CPhysicsAttackOverlap() = default;

	virtual HRESULT Initialize_Prototype(void* pArg);
	virtual HRESULT Initialize(void* pArg) override;

#ifdef _DEBUG
public:
	virtual void Render() override;
#endif

public:
	void Bind_Events();
	void Update(_float fTimeDelta);

	void Ready_Event();
	void Release_Event();
	void CallbackEvent(const AnimNotifyKey& key);

/// <summary>
/// Animation tool
/// </summary>
public:
	vector<DTO::ATTACKEVENT>& GetEvents() { return m_tDesc.attackEvents; }

	void Modify_AttackOverlap(_uint eventIdx, DTO::ATTACKEVENT event);
	void Modify_AttackOverlap(vector<DTO::ATTACKEVENT> events);

private:
	void GetAnimation();

	void Ready_OverlapInfo();

	void PoolClear();
private:
	DTO::ATTACKOVERLAP_DESC m_tDesc = {};

	const Matrix* m_pOwnerMatrix = { nullptr };
	CModel* m_pOwnerModel = { nullptr };

	class CPhysics_QueryFilterCallback* m_pFilterCallback = { nullptr };

	vector<CActiveAttackOverlap*> m_activeEvents;
	std::queue<CActiveAttackOverlap*> m_eventPool;

	// event
	DelegateHandle m_EventHandle;

public:
	static CPhysicsAttackOverlap* Create(void* pArg);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END