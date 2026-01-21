#include "GameInstance.h"
#include "GameObject.h"
#include "Picking.h"

CPicking::CPicking(HWND hWnd)
	: m_pGameInstance(CGameInstance::GetInstance())
	, m_hWnd(hWnd)
{
	Safe_AddRef(m_pGameInstance);
}

void CPicking::Update(const _float4& _vNDC)
{
	_vector vNDC = ::XMLoadFloat4(&_vNDC);

	// 뷰 스페이스 좌표
	_matrix		matInvProj = ::XMLoadFloat4x4(&m_pGameInstance->Get_ProjMatrix());
	matInvProj = ::XMMatrixInverse(nullptr, matInvProj);
	vNDC = ::XMVector3TransformCoord(vNDC, matInvProj);

	// 뷰스페이스 상의 마우스 레이와 레이 위치
	_vector vRayPos = ::XMVectorSet(0.f, 0.f, 0.f, 1.f);
	_vector vRayDir = ::XMVector3Normalize(vNDC - vRayPos);

	// 월드 스페이스 상의 마우스 레이와 레이 위치
	_matrix		matInvView = ::XMLoadFloat4x4(&m_pGameInstance->Get_ViewMatrix());
	matInvView = ::XMMatrixInverse(nullptr, matInvView);
	vRayPos = ::XMVector3TransformCoord(vRayPos, matInvView);
	vRayDir = ::XMVector3TransformNormal(vRayDir, matInvView);
	vRayDir = ::XMVector3Normalize(vRayDir);

	::XMStoreFloat4(&m_vRayPos, vRayPos);
	::XMStoreFloat3(&m_vRayDir, vRayDir);
}

void CPicking::Update(const _float fWinCX, const _float fWinCY)
{
	POINT			ptMouse;

	::GetCursorPos(&ptMouse);

	::ScreenToClient(m_hWnd, &ptMouse);

	// 투영 마우스 좌표 
	_vector			vMousePos = { 0.f, 0.f, 0.f, 1.f };
	::XMVectorSet(
		_float(ptMouse.x / (fWinCX * 0.5f) - 1),
		_float(ptMouse.y / (fWinCY * -0.5f) + 1),
		0.f,
		1.f);

	// 뷰 스페이스 좌표
	_matrix		matInvProj = ::XMLoadFloat4x4(&m_pGameInstance->Get_ProjMatrix());
	matInvProj = ::XMMatrixInverse(nullptr, matInvProj);
	vMousePos = ::XMVector3TransformCoord(vMousePos, matInvProj);

	// 뷰스페이스 상의 마우스 레이와 레이 위치
	_vector vRayPos = ::XMVectorSet(0.f, 0.f, 0.f, 1.f);
	_vector vRayDir = ::XMVector3Normalize(vMousePos - vRayPos);

	// 월드 스페이스 상의 마우스 레이와 레이 위치
	_matrix		matInvView = ::XMLoadFloat4x4(&m_pGameInstance->Get_ViewMatrix());
	matInvView = ::XMMatrixInverse(nullptr, matInvView);
	vRayPos = ::XMVector3TransformCoord(vRayPos, matInvView);
	vRayDir = ::XMVector3TransformNormal(vRayDir, matInvView);
	vRayDir = ::XMVector3Normalize(vRayDir);

	::XMStoreFloat4(&m_vRayPos, vRayPos);
	::XMStoreFloat3(&m_vRayDir, vRayDir);
}

//bool    XM_CALLCONV     Intersects(_In_ FXMVECTOR Origin, _In_ FXMVECTOR Direction, _In_ FXMVECTOR V0, _In_ GXMVECTOR V1, _In_ HXMVECTOR V2, _Out_ float& Dist) noexcept;

void CPicking::TransformRayToLocalSpace(const _float4x4& matInvWorld)
{
	::XMStoreFloat4(&m_vRayPos_Local, ::XMVector3TransformCoord(::XMLoadFloat4(&m_vRayPos), ::XMLoadFloat4x4(&matInvWorld)));
	::XMStoreFloat3(&m_vRayDir_Local, ::XMVector3TransformNormal(::XMLoadFloat3(&m_vRayDir), ::XMLoadFloat4x4(&matInvWorld)));
	::XMStoreFloat3(&m_vRayDir_Local, ::XMVector3Normalize(::XMLoadFloat3(&m_vRayDir_Local)));
}

_bool CPicking::IntersectrayWithTriangle_World(const _float3& vPointA, const _float3& vPointB, const _float3& vPointC, OUT _float4& vOut)
{
	_float fDist = {};
	if (true == TriangleTests::Intersects(::XMLoadFloat4(&m_vRayPos), ::XMLoadFloat3(&m_vRayDir),
		::XMLoadFloat3(&vPointA), ::XMLoadFloat3(&vPointB), ::XMLoadFloat3(&vPointC),
		fDist))
	{
		::XMStoreFloat4(&vOut, ::XMLoadFloat4(&m_vRayPos) + ::XMLoadFloat3(&m_vRayDir) * fDist);
		return true;
	}
	return false;
}

_bool CPicking::IntersectrayWithTriangle_Local(const _float3& vPointA, const _float3& vPointB, const _float3& vPointC, OUT _float4& vOut)
{
	_float fDist = {};
	if (true == TriangleTests::Intersects(::XMLoadFloat4(&m_vRayPos_Local), ::XMLoadFloat3(&m_vRayDir_Local),
		::XMLoadFloat3(&vPointA), ::XMLoadFloat3(&vPointB), ::XMLoadFloat3(&vPointC),
		fDist))
	{
		::XMStoreFloat4(&vOut, ::XMLoadFloat4(&m_vRayPos_Local) + ::XMLoadFloat3(&m_vRayDir_Local) * fDist);
		return true;
	}
	return false;
}

_bool CPicking::IntersectrayWithAABB_World(BoundingBox* pDesc, OUT _float4& vOut)
{
	_float fDist = {};
	if (pDesc->Intersects(::XMLoadFloat4(&m_vRayPos), ::XMLoadFloat3(&m_vRayDir), fDist))
	{
		::XMStoreFloat4(&vOut, ::XMLoadFloat4(&m_vRayPos) + ::XMLoadFloat3(&m_vRayDir) * fDist);
		return true;
	}
	return false;
}

_bool CPicking::IntersectrayWithAABB_Local(BoundingBox* pOriginDesc, OUT _float4& vOut)
{
	_float fDist = {};
	if (pOriginDesc->Intersects(::XMLoadFloat4(&m_vRayPos_Local), ::XMLoadFloat3(&m_vRayDir_Local), fDist))
	{
		::XMStoreFloat4(&vOut, ::XMLoadFloat4(&m_vRayPos_Local) + ::XMLoadFloat3(&m_vRayDir_Local) * fDist);
		return true;
	}
	return false;
}

_bool CPicking::IntersectrayWithOBB_World(BoundingOrientedBox* pDesc, OUT _float4& vOut)
{
	_float fDist = {};
	if (pDesc->Intersects(::XMLoadFloat4(&m_vRayPos), ::XMLoadFloat3(&m_vRayDir), fDist))
	{
		::XMStoreFloat4(&vOut, ::XMLoadFloat4(&m_vRayPos) + ::XMLoadFloat3(&m_vRayDir) * fDist);
		return true;
	}
	return false;
}

_bool CPicking::IntersectrayWithOBB_Local(BoundingOrientedBox* pOriginDesc, OUT _float4& vOut)
{
	_float fDist = {};
	if (pOriginDesc->Intersects(::XMLoadFloat4(&m_vRayPos_Local), ::XMLoadFloat3(&m_vRayDir_Local), fDist))
	{
		::XMStoreFloat4(&vOut, ::XMLoadFloat4(&m_vRayPos_Local) + ::XMLoadFloat3(&m_vRayDir_Local) * fDist);
		return true;
	}
	return false;
}

_bool CPicking::IntersectrayWithSphere_World(BoundingSphere* pDesc, OUT _float4& vOut)
{
	_float fDist = {};
	if (pDesc->Intersects(::XMLoadFloat4(&m_vRayPos), ::XMLoadFloat3(&m_vRayDir), fDist))
	{
		::XMStoreFloat4(&vOut, ::XMLoadFloat4(&m_vRayPos) + ::XMLoadFloat3(&m_vRayDir) * fDist);
		return true;
	}
	return false;
}

_bool CPicking::IntersectrayWithSphere_Local(BoundingSphere* pOriginDesc, OUT _float4& vOut)
{
	_float fDist = {};
	if (pOriginDesc->Intersects(::XMLoadFloat4(&m_vRayPos_Local), ::XMLoadFloat3(&m_vRayDir_Local), fDist))
	{
		::XMStoreFloat4(&vOut, ::XMLoadFloat4(&m_vRayPos_Local) + ::XMLoadFloat3(&m_vRayDir_Local) * fDist);
		return true;
	}
	return false;
}

CPicking* CPicking::Create(HWND hWnd)
{
	return new CPicking(hWnd);
}

void CPicking::Free()
{
	Safe_Release(m_pGameInstance);
	Super::Free();
}
