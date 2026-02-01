#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CGameObject;

class CPicking final : public CBase
{
	using Super = CBase;
private:
	CPicking(HWND hWnd);
	virtual ~CPicking() = default;
public:
	void Update(const Vec4& _vNDC);
	void Update(const _float fWinCX, const _float fWinCY);
public:
	void TransformRayToLocalSpace(const Matrix& matInvWorld);
	_bool IntersectrayWithTriangle_World(const Vec3& vPointA, const Vec3& vPointB, const Vec3& vPointC, OUT Vec3& vOut);
	_bool IntersectrayWithTriangle_Local(const Vec3& vPointA, const Vec3& vPointB, const Vec3& vPointC, OUT Vec3& vOut);
	_bool IntersectrayWithAABB_World(BoundingBox* pDesc, OUT Vec3& vOut);
	_bool IntersectrayWithAABB_Local(BoundingBox* pOriginDesc, OUT Vec3& vOut);
	_bool IntersectrayWithOBB_World(BoundingOrientedBox* pDesc, OUT Vec3& vOut);
	_bool IntersectrayWithOBB_Local(BoundingOrientedBox* pOriginDesc, OUT Vec3& vOut);
	_bool IntersectrayWithSphere_World(BoundingSphere* pDesc, OUT Vec3& vOut);
	_bool IntersectrayWithSphere_Local(BoundingSphere* pOriginDesc, OUT Vec3& vOut);
public:
	const Vec3& Get_RayPos(bool isLocal = false) const;
	const Vec3& Get_RayDir(bool isLocal = false) const;
private:
	HWND m_hWnd = { NULL };
	class CGameInstance* m_pGameInstance = { nullptr };
	Vec3 m_vRayPos = {};
	Vec3 m_vRayDir = {};
	Vec3 m_vRayPos_Local = {};
	Vec3 m_vRayDir_Local = {};
public:
	static CPicking* Create(HWND hWnd);
	virtual void Free() override;
};

NS_END