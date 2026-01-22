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
	void Update(const _float4& _vNDC);
	void Update(const _float fWinCX, const _float fWinCY);
public:
	void TransformRayToLocalSpace(const _float4x4& matInvWorld);
	_bool IntersectrayWithTriangle_World(const _float3& vPointA, const _float3& vPointB, const _float3& vPointC, OUT _float4& vOut);
	_bool IntersectrayWithTriangle_Local(const _float3& vPointA, const _float3& vPointB, const _float3& vPointC, OUT _float4& vOut);
	_bool IntersectrayWithAABB_World(BoundingBox* pDesc, OUT _float4& vOut);
	_bool IntersectrayWithAABB_Local(BoundingBox* pOriginDesc, OUT _float4& vOut);
	_bool IntersectrayWithOBB_World(BoundingOrientedBox* pDesc, OUT _float4& vOut);
	_bool IntersectrayWithOBB_Local(BoundingOrientedBox* pOriginDesc, OUT _float4& vOut);
	_bool IntersectrayWithSphere_World(BoundingSphere* pDesc, OUT _float4& vOut);
	_bool IntersectrayWithSphere_Local(BoundingSphere* pOriginDesc, OUT _float4& vOut);
private:
	HWND m_hWnd = { NULL };
	class CGameInstance* m_pGameInstance = { nullptr };
	_float4 m_vRayPos = {};
	_float3 m_vRayDir = {};
	_float4 m_vRayPos_Local = {};
	_float3 m_vRayDir_Local = {};
public:
	static CPicking* Create(HWND hWnd);
	virtual void Free() override;
};

NS_END