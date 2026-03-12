#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CPhysicsSpringArm final : public CComponent
{
private:
	using Super = CComponent;

public:
	constexpr static EComponentType _ID = EComponentType::PX_SPRINGARM;

private:
	CPhysicsSpringArm();
	CPhysicsSpringArm(const CPhysicsSpringArm& rhs);
	virtual ~CPhysicsSpringArm() = default;

	virtual HRESULT Initialize_Prototype(void* pArg);
	virtual HRESULT Initialize(void* pArg) override;

public:
	Vec3 CheckResolveCollision(Vec3 vCameraPos, Vec3 vTargetPos);

private:
	PxScene* m_pScene = { nullptr };
	class CPhysics_QueryFilterCallback_SpringArm* m_pFilterCallback = { nullptr };
	PxQueryFilterData m_filterData = { PxQueryFilterData() };

	PxSphereGeometry m_pxCameraSphere = {};
	PxTransform m_pxTransform = { PxTransform(PxIdentity) };

	PxSweepBuffer m_SweepBuffer = {};

public:
	static CPhysicsSpringArm* Create(void* pArg);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END