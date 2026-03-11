#include "pch.h"
#include "UIMinimap_Manager.h"
#include "FileUtils.h"
#include "Canvas.h"
#include "GenericUI.h"
#include "UITutorial_Manager.h"
#include "GameInstance.h"

NS_BEGIN(Client)

#define MINIMAP_RADIUS_LENGTH 75.f


IMPLEMENT_SINGLETON(CUIMinimap_Manager)
CUIMinimap_Manager::CUIMinimap_Manager()
	:m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CUIMinimap_Manager::Initialize_UIManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	return S_OK;
}

_float CUIMinimap_Manager::WorldLook_Convert_To_Radian(const Vec3& vLook)
{
	Vec2 vDir2D = Vec2{ vLook.x, vLook.z };
	if (vDir2D.Length() > 1e-6f)
		vDir2D.Normalize();
	return atan2f(vDir2D.x, vDir2D.y);
}

void CUIMinimap_Manager::Free()
{
	Safe_Release(m_pGameInstance);
}

NS_END