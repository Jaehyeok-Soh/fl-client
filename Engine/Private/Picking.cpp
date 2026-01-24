#include "Engine_pch.h"
#include "Picking.h"
#include "GameObject.h"
#include "GameInstance.h"

CPicking::CPicking(HWND hWnd)
	: m_pGameInstance(CGameInstance::GetInstance())
	, m_hWnd(hWnd)
{
	Safe_AddRef(m_pGameInstance);
}

void CPicking::Update(const Vec4& _vNDC)
{
	Vec4 vNDC = _vNDC;

	// 뷰 스페이스 좌표
	Matrix matInvProj = m_pGameInstance->Get_ProjMatrix().Invert();
	vNDC = Vec4::Transform(vNDC, matInvProj);

	// 뷰스페이스 상의 마우스 레이와 레이 위치
	Vec3 vRayPos = Vec3::Zero;
	Vec3 vRayDir = vNDC - vRayPos;

	// 월드 스페이스 상의 마우스 레이와 레이 위치
	Matrix matInvView = m_pGameInstance->Get_ViewMatrix().Invert();
	vRayPos = Vec3::Transform(vRayPos, matInvView);
	vRayDir = Vec3::TransformNormal(vRayDir, matInvView);
	vRayDir.Normalize();

	m_vRayPos = vRayPos;
	m_vRayDir = vRayDir;
}

void CPicking::Update(const _float fWinCX, const _float fWinCY)
{
	POINT			ptMouse;

	::GetCursorPos(&ptMouse);

	::ScreenToClient(m_hWnd, &ptMouse);

	// TODO - 피킹 이상하면 여기
	// 투영 마우스 좌표
	//Vec4			vMousePos = { 0.f, 0.f, 0.f, 1.f };
	Vec4			vMousePos =
		::XMVectorSet(
		_float(ptMouse.x / (fWinCX * 0.5f) - 1),
		_float(ptMouse.y / (fWinCY * -0.5f) + 1),
		0.f,
		1.f);
	

	// 뷰 스페이스 좌표
	Matrix		matInvProj = m_pGameInstance->Get_ProjMatrix().Invert();
	vMousePos = Vec4::Transform(vMousePos, matInvProj);

	// 뷰스페이스 상의 마우스 레이와 레이 위치
	Vec3 vRayPos = Vec3::Zero;
	Vec3 vRayDir = vMousePos - vRayPos;
	vRayDir.Normalize();

	// 월드 스페이스 상의 마우스 레이와 레이 위치
	Matrix matInvView = m_pGameInstance->Get_ViewMatrix().Invert();
	vRayPos = Vec3::Transform(vRayPos, matInvView);
	vRayDir = Vec3::TransformNormal(vRayDir, matInvView);
	vRayDir.Normalize();

	m_vRayPos = vRayPos;
	m_vRayDir = vRayDir;
}

//bool    XM_CALLCONV     Intersects(_In_ FXMVECTOR Origin, _In_ FXMVECTOR Direction, _In_ FXMVECTOR V0, _In_ GXMVECTOR V1, _In_ HXMVECTOR V2, _Out_ float& Dist) noexcept;

void CPicking::TransformRayToLocalSpace(const Matrix& matInvWorld)
{
	m_vRayPos_Local = Vec3::Transform(m_vRayPos, matInvWorld);
	m_vRayDir_Local = Vec3::TransformNormal(m_vRayDir, matInvWorld);
	m_vRayDir_Local.Normalize();
}

_bool CPicking::IntersectrayWithTriangle_World(const Vec3& vPointA, const Vec3& vPointB, const Vec3& vPointC, OUT Vec3& vOut)
{
	_float fDist = {};
	if (true == TriangleTests::Intersects(m_vRayPos, m_vRayDir, vPointA, vPointB, vPointC, fDist))
	{
		vOut = m_vRayPos + m_vRayDir * fDist;
		return true;
	}
	return false;
}

_bool CPicking::IntersectrayWithTriangle_Local(const Vec3& vPointA, const Vec3& vPointB, const Vec3& vPointC, OUT Vec3& vOut)
{
	_float fDist = {};
	if (true == TriangleTests::Intersects(m_vRayPos_Local, m_vRayDir_Local, vPointA, vPointB, vPointC, fDist))
	{
		vOut = m_vRayPos_Local + m_vRayDir_Local * fDist;
		return true;
	}
	return false;
}

_bool CPicking::IntersectrayWithAABB_World(BoundingBox* pDesc, OUT Vec3& vOut)
{
	_float fDist = {};
	if (pDesc->Intersects(m_vRayPos, m_vRayDir, fDist))
	{
		vOut = m_vRayPos + m_vRayDir * fDist;
		return true;
	}
	return false;
}

_bool CPicking::IntersectrayWithAABB_Local(BoundingBox* pOriginDesc, OUT Vec3& vOut)
{
	_float fDist = {};
	if (pOriginDesc->Intersects(m_vRayPos_Local, m_vRayDir_Local, fDist))
	{
		vOut = m_vRayPos_Local + m_vRayDir_Local * fDist;
		return true;
	}
	return false;
}

_bool CPicking::IntersectrayWithOBB_World(BoundingOrientedBox* pDesc, OUT Vec3& vOut)
{
	_float fDist = {};
	if (pDesc->Intersects(m_vRayPos, m_vRayDir, fDist))
	{
		vOut = m_vRayPos + m_vRayDir * fDist;
		return true;
	}
	return false;
}

_bool CPicking::IntersectrayWithOBB_Local(BoundingOrientedBox* pOriginDesc, OUT Vec3& vOut)
{
	_float fDist = {};
	if (pOriginDesc->Intersects(m_vRayPos_Local, m_vRayDir_Local, fDist))
	{
		vOut = m_vRayPos_Local + m_vRayDir_Local * fDist;
		return true;
	}
	return false;
}

_bool CPicking::IntersectrayWithSphere_World(BoundingSphere* pDesc, OUT Vec3& vOut)
{
	_float fDist = {};
	if (pDesc->Intersects(m_vRayPos, m_vRayDir, fDist))
	{
		vOut = m_vRayPos + m_vRayDir * fDist;
		return true;
	}
	return false;
}

_bool CPicking::IntersectrayWithSphere_Local(BoundingSphere* pOriginDesc, OUT Vec3& vOut)
{
	_float fDist = {};
	if (pOriginDesc->Intersects(m_vRayPos_Local, m_vRayDir_Local, fDist))
	{
		vOut = m_vRayPos_Local + m_vRayDir_Local * fDist;
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
