#pragma once
#include "Component.h"

#include "Physics_QueryFilterCallback.h"

NS_BEGIN(Engine)

class CActiveAttackOverlap;
class CModel;

class ENGINE_DLL CPhysicsAttackOverlap final : public CComponent
{
public:
	enum Enum
	{
		BOX,
		SPHERE
	};

	typedef struct tagHitboxDesc
	{
		_wstring wstrName = {};
		
		PxGeometryHolder geometry;
		PxQueryFilterData filterData;
		Matrix matOffset;
		CPhysics_QueryFilterCallback* filterCallback = { nullptr };

		CPhysicsAttackOverlap::Enum eType = CPhysicsAttackOverlap::Enum::SPHERE;
		Vec3 vExtents = {};
		_float fRadius = {};
		Vec3 vOffset = {};
		
		_float fDuration = {};
		_float fTickTime = { -1.f };
		
		_float fDamage = {};

		_uint iMaxHit = { 32 };

		PHYSICSFILTERGROUP::Enum eFilterLayer = PHYSICSFILTERGROUP::Enum::NONE;
		_uint iFilterMask = {};
	}HITBOX_DESC;

	typedef struct tagAttackEvent
	{
		_uint fAnimIndex = {};
		_float fStartTrackPosition = {};
		HITBOX_DESC tHitboxDesc;
	}ATTACKEVENT;

	typedef struct tagAttackOverlapDesc
	{
		_uint iNumPool = {};
		vector<ATTACKEVENT> attackEvents;
	}ATTACKOVERLAP_DESC;

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
	void Awake();
	void Update(_float fTimeDelta);

private:
	void GetAnimation();
	void CheckAnim();

	void Ready_OverlapInfo();

	void PoolClear();

private:
	ATTACKOVERLAP_DESC m_tDesc = {};

	const Matrix* m_pOwnerMatrix = { nullptr };
	CModel* m_pOwnerModel = { nullptr };

	class CPhysics_QueryFilterCallback* m_pFilterCallback = { nullptr };

	vector<CActiveAttackOverlap*> m_activeEvents;
	std::queue<CActiveAttackOverlap*> m_eventPool;

	_uint m_iPrevAnimIndex = {};
	_float m_fPrevTrackPosition = {};

public:
	static CPhysicsAttackOverlap* Create(void* pArg);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END