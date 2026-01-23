#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL CRay final : public CBase
{
	using Super = CBase;
private:
	CRay(const Vec3& vLocalOrigin, const Vec3& vLocalDir);
	virtual ~CRay() = default;

	HRESULT Initialize();
public:
	void Setup_Ray(const Matrix *pMatParent);
	void Setup_Ray(const Vec3& vOrigin, const Vec3& vDir) { m_vOrigin = vOrigin; m_vDir = vDir; }
	const Vec3& Get_Origin() const { return m_vOrigin; }
	const Vec3& Get_Dir() const { return m_vDir; }
	const Vec3& Get_LocalOrigin() const { return m_vLocalOrigin; }
	const Vec3& Get_LocalDir() const { return m_vLocalDir; }
	_bool IntersectrayWithAABB_World(BoundingBox* pDesc, OUT Vec3& vOut);
	_bool IntersectrayWithAABB_Local(BoundingBox* pOriginDesc, OUT Vec3& vOut);
	_bool IntersectrayWithOBB_World(BoundingOrientedBox* pDesc, OUT Vec3& vOut);
	_bool IntersectrayWithOBB_Local(BoundingOrientedBox* pOriginDesc, OUT Vec3& vOut);
	_bool IntersectrayWithSphere_World(BoundingSphere* pDesc, OUT Vec3& vOut);
	_bool IntersectrayWithSphere_Local(BoundingSphere* pOriginDesc, OUT Vec3& vOut);
	_bool IntersectrayWithTriangle_World(const Vec3& vPointA, const Vec3& vPointB, const Vec3& vPointC, OUT Vec3& vOut);
	_bool IntersectrayWithTriangle_Local(const Vec3& vPointA, const Vec3& vPointB, const Vec3& vPointC, OUT Vec3& vOut);
private:
	Vec3 m_vOrigin = { 0.f, 0.f, 0.f };
	Vec3 m_vDir = { 0.f, 0.f, 0.f };
	Vec3 m_vLocalOrigin = { 0.f, 0.f, 0.f };
	Vec3 m_vLocalDir = { 0.f, 0.f, 0.f };
public:
	static CRay* Create(const Vec3& vLocalOrigin, const Vec3& vLocalDir);
	virtual void Free() override;
};

NS_END