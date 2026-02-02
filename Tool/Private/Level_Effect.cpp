#include "pch.h"
#include "Level_Effect.h"
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
#include "CEffectObject.h"
#include "Effect.h"

///////////
// ImGui //
///////////
#include "ImGui_Base.h"
#include "CParticle_System_Panel.h"
#include "EffectType_Selection_Panel.h"

/////////////
// Manager //
/////////////
#include "Picking_ToolManager.h"
#include "ImGui_ToolManager.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Engine_Utils.h"
#include "Tool_Defines.h"

CLevel_Effect::CLevel_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
	, m_pImGuiManager(CImGui_ToolManager::GetInstance())
	, m_pPickingManager(CPicking_ToolManager::GetInstance())
{
	Safe_AddRef(m_pImGuiManager);
	Safe_AddRef(m_pPickingManager);
	m_GuiElements.fill(nullptr);
}

HRESULT CLevel_Effect::Initialize()
{
	if (FAILED(Super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Camera(g_wszCameraLayer)))
		return E_FAIL;

	if (FAILED(Ready_EffectObjectSetting()))
		return E_FAIL;

	m_vClearColor = { 0.3f, 0.3f, 0.3f, 1.f };

	// For. Prototype_GameObject_MainPlayer
	return S_OK;
}

HRESULT CLevel_Effect::Awake(const _uint iLevelID)
{
	if (FAILED(Super::Awake(iLevelID)))
		return E_FAIL;

	MSG_BOX("Effect");

	if (FAILED(Ready_CameraSetting(iLevelID)))
		return E_FAIL;

	if (FAILED(Ready_Gui()))
		return E_FAIL;

	Ready_Event();
	m_pImGuiManager->Ready_Events();
	return S_OK;
}

void CLevel_Effect::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
	m_pPickingManager->Picking();

	for (CImGui_Base* pElement : m_GuiElements)
	{
		if (pElement)
			pElement->Update(fTimeDelta);
	}
}

HRESULT CLevel_Effect::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	m_pImGuiManager->Render_Begin();
	m_pImGuiManager->ImGuizmo_Render_Begin();
	m_pImGuiManager->Render_Dockspace();
	//////////////////////////
	Render_Elements();


	//////////////////////////
	m_pImGuiManager->Render_Viewport(m_pSelectedObject);
	m_pImGuiManager->Render_End();

	return S_OK;
}

HRESULT CLevel_Effect::Ready_Camera(const wstring& wstrLayerTag)
{
	{
		CGameObject* pResult = { nullptr };
		CCameraMan::GAMEOBJECT_DESC goDesc = {};
		CTransform::TRANSFORM_DESC TransformDesc = {};
		TransformDesc.TranslationMatrix = Matrix::CreateTranslation(0.f,0.f,-1.f);
		TransformDesc.fMovePerSec = { 6.f };
		TransformDesc.fRotatePerSec = { 1.f };
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
			ENUM_TO_UINT(ELevelType::EFFECT),
			wstrLayerTag, &goDesc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Effect::Ready_Terrain(const wstring& wstrLayerTag)
{
	return S_OK;
}

HRESULT CLevel_Effect::Ready_Lights()
{
	{
		LIGHT_DESC desc = {};
		desc.eType = LIGHT_TYPE::DIRECTIONAL;
		desc.vDirection = Vec3(1.f, -1.f, 1.f);
		desc.vDiffuse = Vec4(1.f, 1.f, 1.f, 1.f);
		desc.vAmbient = Vec4(1.f, 1.f, 1.f, 1.f);
		desc.vSpecular = Vec4(1.f, 1.f, 1.f, 1.f);

		if (FAILED(m_pGameInstance->Add_Light(desc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Effect::Ready_Gui()
{
	m_GuiElements[ENUM_TO_UINT(Elements::EffectSystem)] = CEffectType_Selection_Panel::Create("Effect_Selection_Panel", this, m_pDevice, m_pDeviceContext, &m_pSelectedObject);
	m_GuiElements[ENUM_TO_UINT(Elements::ParticleSystem)] = CParticle_System_Panel::Create("CParticle_System_Panel", this, m_pDevice, m_pDeviceContext);

	return S_OK;
}

HRESULT CLevel_Effect::Ready_EffectObjectSetting()
{
	m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::EFFECT), L"Prototype_GameObject_Effect", Effect::Create(EToolObjectType::MESHEFFECT, m_pDevice, m_pDeviceContext));
	m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::EFFECT), L"Prototype_GameObject_Effect_Parts", CEffectObject::Create(EToolObjectType::MESHEFFECT, m_pDevice, m_pDeviceContext));
	
	return S_OK;
}

HRESULT CLevel_Effect::Ready_CameraSetting(const _uint iLevelID)
{
	CGameObject* pFreeCamera = m_pGameInstance->Get_GameObject_Back(iLevelID, g_wszCameraLayer);
	m_pGameInstance->Add_Camera(CameraType::STATIC, g_FreeCameraName, static_cast<CCameraMan*>(pFreeCamera));
	m_pGameInstance->Change_MainCamera(CameraType::STATIC, g_FreeCameraName);

	return S_OK;
}



void CLevel_Effect::Render_Elements()
{
	for (CImGui_Base* pElement : m_GuiElements)
	{
		if (pElement)
			pElement->Render(m_pSelectedObject);
	}
}

void CLevel_Effect::Ready_Event()
{
	
}

void CLevel_Effect::Release_Event()
{
	
}

CLevel_Effect* CLevel_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CLevel_Effect* pInstance = new CLevel_Effect(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("CLevel_Effect::Create, Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Effect::Free()
{
	Release_Event();
	for (CImGui_Base* pElement : m_GuiElements)
	{
		Safe_Release(pElement);
	}
	m_GuiElements.fill(nullptr);
	Safe_Release(m_pImGuiManager);
	Safe_Release(m_pPickingManager);
	Super::Free();
}
