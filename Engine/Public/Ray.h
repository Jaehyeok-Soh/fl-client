#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL CRay final : public CBase
{
	using Super = CBase;
private:
	CRay(const _float4& vLocalOrigin, const _float3& vLocalDir);
	virtual ~CRay() = default;

	HRESULT Initialize();
public:
	void Setup_Ray(const _float4x4 *pMatParent);
	void Setup_Ray(const _float4& vOrigin, const _float3& vDir) { m_vOrigin = vOrigin; m_vDir = vDir; }
	void Setup_Ray(_fvector vOrigin, _fvector vDir);
	const _float4& Get_Origin() const { return m_vOrigin; }
	const _float3& Get_Dir() const { return m_vDir; }
	const _float4& Get_LocalOrigin() const { return m_vLocalOrigin; }
	const _float3& Get_LocalDir() const { return m_vLocalDir; }
	_bool IntersectrayWithAABB_World(BoundingBox* pDesc, OUT _float4& vOut);
	_bool IntersectrayWithAABB_Local(BoundingBox* pOriginDesc, OUT _float4& vOut);
	_bool IntersectrayWithOBB_World(BoundingOrientedBox* pDesc, OUT _float4& vOut);
	_bool IntersectrayWithOBB_Local(BoundingOrientedBox* pOriginDesc, OUT _float4& vOut);
	_bool IntersectrayWithSphere_World(BoundingSphere* pDesc, OUT _float4& vOut);
	_bool IntersectrayWithSphere_Local(BoundingSphere* pOriginDesc, OUT _float4& vOut);
	_bool IntersectrayWithTriangle_World(const _float3& vPointA, const _float3& vPointB, const _float3& vPointC, OUT _float4& vOut);
	_bool IntersectrayWithTriangle_Local(const _float3& vPointA, const _float3& vPointB, const _float3& vPointC, OUT _float4& vOut);
private:
	_float4 m_vOrigin = {0.f, 0.f, 0.f, 1.f};
	_float3 m_vDir = { 0.f, 0.f, 0.f };
	_float4 m_vLocalOrigin = { 0.f, 0.f, 0.f, 1.f };
	_float3 m_vLocalDir = { 0.f, 0.f, 0.f };
public:
	static CRay* Create(const _float4& vLocalOrigin, const _float3& vLocalDir);
	virtual void Free() override;
};

NS_END