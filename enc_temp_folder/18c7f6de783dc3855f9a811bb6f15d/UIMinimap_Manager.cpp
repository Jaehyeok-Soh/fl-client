#include "pch.h"
#include "UIMinimap_Manager.h"
#include "FileUtils.h"
#include "Canvas.h"
#include "GenericUI.h"
#include "MainPlayer.h"
#include "UI_Manager.h"
#include "UIIcon_Component.h"
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
	m_vecRangedObjects.reserve(10);
	return S_OK;
}

void CUIMinimap_Manager::Update(const _float fTimeDelta)
{
}

_float CUIMinimap_Manager::WorldLook_Convert_To_Radian(const Vec3& vLook)
{
	Vec2 vDir2D = Vec2{ vLook.x, vLook.z };
	if (vDir2D.Length() > 1e-6f)
		vDir2D.Normalize();
	return atan2f(vDir2D.x, vDir2D.y);
}

HRESULT CUIMinimap_Manager::Add_Ranged_Object(CGameObject* pObj, EUIMinimapIconTypeID eID)
{
	if (nullptr == pObj)
		return E_FAIL;

	switch (eID)
	{
	case Client::EUIMinimapIconTypeID::MONSTER:
	{
		UI_PREFAB_DATA tPrefabData = {};
		UI_MINIMAP_MONSTER_ICON_PREFAB_DATA Desc = {};
		Desc.pTarget = pObj;
		tPrefabData.Data = Desc;
		dynamic_cast<CUIIcon_Component*>(pObj->Get_Script_Component(L"UIIconComp"))->Set_isRanged(true);
		CUI_Manager::GetInstance()->Request_Add_Prefab(m_pGameInstance->Get_CurrentLevelIndex(), EUIPrefabType::MINIMAP_MONSTER_ICON, m_pGameInstance->Get_CurrentLevelIndex(), &tPrefabData);
	}
	break;

	default:
		return E_FAIL;
	}

	//m_vecRangedObjects.push_back(pObj);
	return S_OK;
}

HRESULT CUIMinimap_Manager::Delete_Ranged_Object(CGameObject* pObj)
{
	if (nullptr == pObj)
		return E_FAIL;

	//for (size_t i = 0; i < m_vecRangedObjects.size(); ++i)
	//{
	//	if (m_vecRangedObjects[i] == pObj)
	//	{
	//		m_vecRangedObjects[i] = m_vecRangedObjects.back();
	//		m_vecRangedObjects.pop_back();
	//		dynamic_cast<CUIIcon_Component*>(pObj->Get_Script_Component(L"UIIconComp"))->Set_isRanged(false);
	//		return S_OK;
	//	}
	//}

	dynamic_cast<CUIIcon_Component*>(pObj->Get_Script_Component(L"UIIconComp"))->Set_isRanged(false);
	return E_FAIL;
}

Vec2 CUIMinimap_Manager::WorldPos_Convert_MinimapPosOffset(const Vec3& vPos, const Vec3& vPlayerPos)
{

	_float fScaleOffet = 125.f / 10.f; // 미니맵 반지름 / 그에 대응하는 유닛 // 유닛을 픽셀로 변환 //
	Vec3 vDeltaPos = vPos - vPlayerPos;
	

	_float PixelX = vDeltaPos.x * fScaleOffet;
	_float PixelZ = -vDeltaPos.z * fScaleOffet;

	return Vec2(PixelX, PixelZ);
}

void CUIMinimap_Manager::Free()
{
	Safe_Release(m_pGameInstance);
}

NS_END