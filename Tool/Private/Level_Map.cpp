#include "pch.h"
#include "Level_Map.h"
///////////////
// Component //
///////////////
#include "VIBuffer_Terrain.h"
#include "Shader.h"
#include "Material.h"

////////////////
// GameObject //
////////////////
#include "ToolObject.h"
#include "CameraMan_Free.h"
#include "UEMapDataLoader.h"

///////////
// ImGui //
///////////
#include "ImGui_Base.h"
#include "Panel_MapObjectList.h"
#include "Panel_MapDataController.h"
#include "Panel_FileExplore.h"
#include "Panel_MapTool.h"
/////////////
// Manager //
/////////////
#include "Picking_ToolManager.h"
#include "ImGui_ToolManager.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Engine_Utils.h"
#include "Tool_Defines.h"

#include "DebugDraw.h"
#include "DebugLine.h"

#include "UEMapdataParser.h"
#include "MapToolManager.h"

CLevel_Map::CLevel_Map(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
	, m_pImGuiManager(CImGui_ToolManager::GetInstance())
	, m_pPickingManager(CPicking_ToolManager::GetInstance())
	, m_pUEMapDataParser(CUEMapdataParser::GetInstance())
	, m_pMapToolManager(CMapToolManager::GetInstance())
{
	Safe_AddRef(m_pImGuiManager);
	Safe_AddRef(m_pPickingManager);
	m_arrayImGuiPanel.fill(nullptr);
}

HRESULT CLevel_Map::Initialize()
{
	if (FAILED(Ready_MapObject_Layer()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Camera_Layer(g_wszCameraLayer)))
		return E_FAIL;

	if (FAILED(Ready_DebugLine()))
		return E_FAIL;

	m_pUEMapDataParser->Initialize(m_pDevice,m_pDeviceContext);
	m_pMapToolManager->Initialize(m_pDevice, m_pDeviceContext);

	return S_OK;
}

HRESULT CLevel_Map::Awake(const _uint iLevelID)
{
	if (FAILED(Super::Awake(iLevelID)))
		return E_FAIL;

	MSG_BOX("Map");

	if (FAILED(Ready_Camera_Setting(iLevelID)))
		return E_FAIL;

	if (FAILED(Reday_Gui()))
		return E_FAIL;

	Ready_Event();
	m_pImGuiManager->Ready_Events();

	/* Cam Setting */
	m_pGameInstance->Get_MainCamera()->Get_Component<CCamera>()->Set_Fov(XMConvertToRadians(60.f));

	/* Batch */


	return S_OK;
}

void CLevel_Map::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
	if(m_pMapToolManager->Get_Preview() == nullptr)
		m_pPickingManager->Picking();

	m_pMapToolManager->Update(fTimeDelta);

	for (CImGui_Panel* pElement : m_arrayImGuiPanel)
	{
		if (pElement)
			pElement->Update(fTimeDelta);
	}
}

HRESULT CLevel_Map::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	m_pImGuiManager->Render_Begin();
	m_pImGuiManager->ImGuizmo_Render_Begin();
	m_pImGuiManager->Render_Dockspace();
	//////////////////////////
	// Element Render


	Render_Elements();

	m_pImGuiManager->Render_Viewport(m_pSelectedObject);
	m_pImGuiManager->Render_End();


	return S_OK;
}

void CLevel_Map::Render_Elements()
{
	for (CImGui_Panel* pElement : m_arrayImGuiPanel)
	{
		if (pElement)
			pElement->Render(m_pSelectedObject);
	}
}

HRESULT CLevel_Map::Reday_Gui()
{
	m_arrayImGuiPanel[static_cast<UINT32>(CLevel_Map::Elements::ObjectList)] = CPanel_MapObjectList::Create(" Map Object List ", this  ,m_pDevice , m_pDeviceContext);
	m_arrayImGuiPanel[static_cast<UINT32>(CLevel_Map::Elements::MapData)] = CPanel_MapDataController::Create(" Map Data Controller ", this, m_pDevice, m_pDeviceContext);
	m_arrayImGuiPanel[static_cast<UINT32>(CLevel_Map::Elements::FileExplore)] = CPanel_FileExplore::Create(
		L"../../Resources", {".fbx" ,".Mesh",".png",".dds" ,"png" , ".json"},
		" Panel_FileExplore ", this, m_pDevice, m_pDeviceContext);

	m_arrayImGuiPanel[static_cast<UINT32>(CLevel_Map::Elements::MapTool)] = CPanel_MapTool::Create(" Map Tool " , this , m_pDevice, m_pDeviceContext);

	return S_OK;
}

void CLevel_Map::On_ChangeSelectedObject(CGameObject* pGo)
{
	if (pGo)
	{
		if (CToolObject* pToolGo = dynamic_cast<CToolObject*>(pGo))
		{
			m_pSelectedObject = pToolGo;
			return;
		}
	}
	if (!ImGuizmo::IsOver() && !ImGuizmo::IsUsing())
		m_pSelectedObject = nullptr;
}

void CLevel_Map::On_CreateMode(_bool bValue)
{
	m_bCreateMode = bValue;
}

HRESULT CLevel_Map::Ready_MapObject_Layer()
{
	return S_OK;
}

HRESULT CLevel_Map::Ready_Camera_Layer(const wstring& wstrLayerTag)
{
	{
		CGameObject* pResult = { nullptr };
		CCameraMan::GAMEOBJECT_DESC goDesc = {};
		CTransform::TRANSFORM_DESC TransformDesc = {};
		TransformDesc.vPosition = {1.f, 1.f, -1.f};
		TransformDesc.fMovePerSec = { 15.f };
		TransformDesc.fRotatePerSec = {2.f};
		CCamera::CAMERA_DESC CameraDesc = {};

		CameraDesc.eProjectionType = EProjectionType::PERSPECTIVE;
		CameraDesc.fFov = ::XMConvertToRadians(90.f);
		CameraDesc.fViewWidth = (_float)g_iWinSizeX;
		CameraDesc.fViewHeight = (_float)g_iWinSizeY;
		CameraDesc.fNear = 0.1f;
		CameraDesc.fFar = 1000.f;

		goDesc.pTransform_Desc = &TransformDesc;
		goDesc.pCamera_Desc = &CameraDesc;
		if (!(pResult = m_pGameInstance->Add_GameObject(ENUM_TO_UINT(ELevelType::STATIC),
			L"Prototype_GameObject_CameraManFree",
			ENUM_TO_UINT(ELevelType::MAP),
			wstrLayerTag, &goDesc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Map::Ready_Lights()
{
	{
		LIGHT_DESC desc = {};
		desc.eType = LIGHT_TYPE::DIRECTIONAL;
		desc.vDirection = Vec3(1.f, -1.f, 1.f);
		desc.vDiffuse = Vec4(0.7f, 0.7f, 0.7f, 1.f);
		desc.vAmbient = Vec4(0.3f, 0.3f, 0.35f, 1.f);
		desc.vSpecular = Vec4(1.f, 1.f, 1.f, 1.f);

		if (FAILED(m_pGameInstance->Add_Light(desc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Map::Ready_Camera_Setting(const _uint iLevelID)
{
	CGameObject* pFreeCamera = m_pGameInstance->Get_GameObject_Back(iLevelID, L"Camera_Layer");
	m_pGameInstance->Add_Camera(CameraType::STATIC, g_FreeCameraName, static_cast<CCameraMan*>(pFreeCamera));
	m_pGameInstance->Change_MainCamera(CameraType::STATIC, g_FreeCameraName);
	return S_OK;
}

HRESULT CLevel_Map::Ready_DebugLine()
{
	m_pDebugLine = nullptr;

	CDebugLine::DEBUGLINE_DESC tDesc{};
	tDesc.vColor_X = {0.f,1.f,0.f,1.f};
	tDesc.vColor_Z = {1.f,0.f,0.f,1.f};

	CTransform::TRANSFORM_DESC tTsDesc{};
	tDesc.pTransform_Desc = &tTsDesc;


	m_pDebugLine =
		static_cast<CDebugLine*>(m_pGameInstance->Add_GameObject(ENUM_TO_UINT(ELevelType::STATIC),L"Prototype_GameObject_DebugLine",ENUM_TO_UINT(ELevelType::MAP),L"Layer_DebugLine",
			&tDesc));
	if (m_pDebugLine == nullptr) return E_FAIL;



	return S_OK;
}

void CLevel_Map::Ready_Event()
{
	m_EventHandles[ENUM_TO_UINT(Event::ChangeSelectedObject)] =
		m_pGameInstance->Subscribe<ChangeSelectedObject>(this, &CLevel_Map::On_ChangeSelectedObject);
}

void CLevel_Map::Release_Event()
{
	m_pGameInstance->Unsubscribe<ChangeSelectedObject>(m_EventHandles[ENUM_TO_UINT(Event::ChangeSelectedObject)]);
}

CLevel_Map* CLevel_Map::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CLevel_Map* pInstance = new CLevel_Map(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("CLevel_Map::Create, Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Map::Free()
{
	Release_Event();
	for (CImGui_Panel* pElement : m_arrayImGuiPanel)
	{
		Safe_Release(pElement);
	}
	m_arrayImGuiPanel.fill(nullptr);


	Safe_Release(m_pImGuiManager);
	Safe_Release(m_pPickingManager);

	m_pUEMapDataParser->DestroyInstance();
	m_pMapToolManager->DestroyInstance();


	Super::Free();
}
