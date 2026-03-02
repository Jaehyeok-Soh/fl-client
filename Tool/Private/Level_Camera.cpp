#include "pch.h"
#include "Level_Camera.h"
#include "Level_Loading.h"
#include "ImGui_ToolManager.h"
#include "DebugLine.h"
#include "CameraMan.h"
#include "CPanel_Camera.h"
#include "GameInstance.h"

CLevel_Camera::CLevel_Camera(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
	, m_pImGuiManager(CImGui_ToolManager::GetInstance())
{
	Safe_AddRef(m_pImGuiManager);
}

HRESULT CLevel_Camera::Initialize()
{
	if (FAILED(Super::Initialize()))
		return E_FAIL;

	/* 蝴 技泼 */
	if (FAILED(Ready_Lights()))
		return E_FAIL;
	
	/* 墨皋扼 积己 */
	if (FAILED(Ready_Camera_Layer(g_wszCameraLayer)))
		return E_FAIL;

	/* Debug Line 积己 */
	if (FAILED(Ready_DebugLine()))
		return E_FAIL;


	return S_OK;
}


HRESULT CLevel_Camera::Ready_Lights()
{
	LIGHT_DESC desc = {};
	desc.eType = LIGHT_TYPE::DIRECTIONAL;
	desc.vDirection = Vec3(1.f, -1.f, 1.f);
	desc.vDiffuse = Vec4(1.f, 1.f, 1.f, 1.f);
	desc.vAmbient = Vec4(1.f, 1.f, 1.f, 1.f);
	desc.vSpecular = Vec4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->Add_Light(desc)))
		return E_FAIL;

	return S_OK;
}


HRESULT CLevel_Camera::Ready_Camera_Layer(const wstring& wstrLayerTag)
{

	CGameObject* pResult = { nullptr };
	CCameraMan::GAMEOBJECT_DESC goDesc = {};
	CTransform::TRANSFORM_DESC TransformDesc = {};
	TransformDesc.RotationMatrix = Matrix::CreateRotationX(XMConvertToRadians(45.f));
	TransformDesc.TranslationMatrix = Matrix::CreateTranslation(Vec3(0.f, 5.f, -5.f));
	TransformDesc.fMovePerSec = { 50.f };
	TransformDesc.fRotatePerSec = { 2.f };
	CCamera::CAMERA_DESC CameraDesc = {};

	CameraDesc.eProjectionType = EProjectionType::PERSPECTIVE;
	CameraDesc.fFov = ::XMConvertToRadians(60.f);
	CameraDesc.fViewWidth = (_float)g_iWinSizeX;
	CameraDesc.fViewHeight = (_float)g_iWinSizeY;
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 1000.f;

	goDesc.pTransform_Desc = &TransformDesc;
	goDesc.pCamera_Desc = &CameraDesc;
	if (!(pResult = m_pGameInstance->Add_GameObject(ENUM_TO_UINT(ELevelType::STATIC),
		L"Prototype_GameObject_CameraManFree",
		ENUM_TO_UINT(ELevelType::CAMERA),
		wstrLayerTag, &goDesc)))
		return E_FAIL;


	return S_OK;
}



HRESULT CLevel_Camera::Reday_Gui()
{
	m_arrayImGuiPanel[static_cast<UINT32>(CLevel_Camera::Elements::Camera)] = CPanel_Camera::Create(" Camera ImGui ", this, m_pDevice, m_pDeviceContext);


	return S_OK;
}


HRESULT CLevel_Camera::Ready_Event()
{
	//m_EventHandles[ENUM_TO_UINT(Event::ChangeSelectedObject)] =
	//	m_pGameInstance->Subscribe<ChangeSelectedObject>(this, &CLevel_Camera::On_ChangeSelectedObject);
	return S_OK;
}

HRESULT CLevel_Camera::Release_Event()
{
	//m_pGameInstance->Unsubscribe<ChangeSelectedObject>(m_EventHandles[ENUM_TO_UINT(Event::ChangeSelectedObject)]);
	return S_OK;
}



HRESULT CLevel_Camera::Ready_DebugLine()
{
	CDebugLine::DEBUGLINE_DESC tDesc{};
	tDesc.vColor_X = { 0.f,1.f,0.f,1.f };
	tDesc.vColor_Z = { 1.f,0.f,0.f,1.f };

	CTransform::TRANSFORM_DESC tTsDesc{};
	tDesc.pTransform_Desc = &tTsDesc;

	m_pGameInstance->Add_GameObject(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_GameObject_DebugLine", ENUM_TO_UINT(ELevelType::CAMERA), L"Layer_DebugLine",
		&tDesc);

	return S_OK;
}


HRESULT CLevel_Camera::Ready_Camera_Setting(const _uint iLevelID)
{
	CCameraMan* pFreeCamera = static_cast<CCameraMan*>(m_pGameInstance->Get_GameObject_Back(iLevelID, L"Camera_Layer"));
	m_pGameInstance->Add_Camera(CameraType::STATIC, g_FreeCameraName, pFreeCamera);
	m_pGameInstance->Change_MainCamera(CameraType::STATIC, g_FreeCameraName);
	m_pGameInstance->Ready_Frustrum();
	return S_OK;
}

HRESULT CLevel_Camera::Awake(const _uint iLevelID)
{
	if (FAILED(Super::Awake(iLevelID)))
		return E_FAIL;

	if (FAILED(Ready_Camera_Setting(iLevelID)))
		return E_FAIL;

	if (FAILED(Reday_Gui()))
		return E_FAIL;

	if (FAILED(Ready_Event()))
		return E_FAIL;

	if (FAILED(m_pImGuiManager->Ready_Events()))
		return E_FAIL;

	MSG_BOX("Camera");

	return S_OK;
}


void CLevel_Camera::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CLevel_Camera::Update_Picking()
{
}


HRESULT CLevel_Camera::Render_Elements()
{
	for (CImGui_Base* pElement : m_arrayImGuiPanel)
	{
		if (pElement)
			pElement->Render(nullptr);
	}

	return S_OK;
}


HRESULT CLevel_Camera::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	m_pImGuiManager->Render_Begin();
	m_pImGuiManager->ImGuizmo_Render_Begin();
	m_pImGuiManager->Render_Dockspace();
	//////////////////////////
	// Element Render

	if (FAILED(Render_Elements()))
		return E_FAIL;
	//////////////////////////
	m_pImGuiManager->Render_Viewport(nullptr);
	m_pImGuiManager->Render_End();

	return S_OK;
}

HRESULT CLevel_Camera::Ready_Player_Layer(const wstring& wstrLayerTag)
{
	return S_OK;
}

HRESULT CLevel_Camera::Ready_UI_Layer(const wstring& wstrLayerTag)
{
	return S_OK;
}

CLevel_Camera* CLevel_Camera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CLevel_Camera* pInstance = new CLevel_Camera(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("CLevel_Camera::Create, Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Camera::Free()
{
	Safe_Release(m_pImGuiManager);

	for (auto& Panel : m_arrayImGuiPanel)
		Safe_Release(Panel);

	Super::Free();
}
