#include "pch.h"
#include "Level_Animation.h"
#include "Level_Loading.h"

// ready obj
#include "CameraMan_Free.h"

// ImGui
#include "ImGui_Base.h"

// Manager
#include "Picking_ToolManager.h"
#include "ImGui_ToolManager.h"
#include "GameInstance.h"

CLevel_Animation::CLevel_Animation(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
	, m_pImGuiManager(CImGui_ToolManager::GetInstance())
{
	Safe_AddRef(m_pImGuiManager);
}

HRESULT CLevel_Animation::Initialize()
{
	if (FAILED(Super::Initialize()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Animation::Awake(const _uint iLevelID)
{
	if (FAILED(Super::Awake(iLevelID)))
		return E_FAIL;

	MSG_BOX("Animation");

	return S_OK;
}

void CLevel_Animation::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

	//Update_Elements(fTimeDelta);
}

HRESULT CLevel_Animation::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	m_pImGuiManager->Render_Begin();
	m_pImGuiManager->ImGuizmo_Render_Begin();
	m_pImGuiManager->Render_Dockspace();
	//////////////////////////
	// Element Render

	//Render_Elements();

	ImGui::ShowDemoWindow();

	//////////////////////////
	m_pImGuiManager->Render_Viewport(nullptr);
	m_pImGuiManager->Render_End();
	return S_OK;
}

HRESULT CLevel_Animation::Ready_Camera(const _wstring wstrLayerTag)
{
	{
		CGameObject* pResult = { nullptr };
		CCameraMan::GAMEOBJECT_DESC goDesc = {};
		CTransform::TRANSFORM_DESC TransformDesc = {};
		TransformDesc.vPosition = { 0.f, 0.f, -1.f };
		TransformDesc.fMovePerSec = { 6.f };
		TransformDesc.fRotatePerSec = { 1.f };
		CCamera::CAMERA_DESC CameraDesc = {};

		CameraDesc.eProjectionType = EProjectionType::PERSPECTIVE;
		CameraDesc.fFov = ::XMConvertToRadians(60.f);
		CameraDesc.fViewWidth = (_float)g_iWinSizeX;
		CameraDesc.fViewHeight = (_float)g_iWinSizeY;
		CameraDesc.fNear = 0.1f;
		CameraDesc.fFar = 100.f;

		goDesc.pTransform_Desc = &TransformDesc;
		goDesc.pCamera_Desc = &CameraDesc;
		if (!(pResult = m_pGameInstance->Add_GameObject(ENUM_TO_UINT(ELevelType::STATIC),
			L"Prototype_GameObject_CameraManFree",
			ENUM_TO_UINT(ELevelType::ANIMATION),
			wstrLayerTag, &goDesc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Animation::Ready_Lights(const _wstring wstrLayerTag)
{
	{
		LIGHT_DESC desc = {};
		desc.eType		= LIGHT_TYPE::DIRECTIONAL;
		desc.vDirection = Vec3(1.f, -1.f, 1.f);
		desc.vDiffuse	= Vec4(1.f, 1.f, 1.f, 1.f);
		desc.vAmbient	= Vec4(0.3f, 0.3f, 0.35f, 1.f);
		desc.vSpecular	= Vec4(1.f, 1.f, 1.f, 1.f);

		if (FAILED(m_pGameInstance->Add_Light(desc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Animation::Ready_Panels()
{
	return S_OK;
}

void CLevel_Animation::Update_Elements(const _float fTimeDelta)
{
	for (CImGui_Base* pElement : m_GuiElements)
	{
		if (pElement)
			pElement->Update(fTimeDelta);
	}
}

void CLevel_Animation::Render_Elements()
{
	for (CImGui_Base* pElement : m_GuiElements)
	{
		if (pElement)
			pElement->Render(m_pSelectedObject);
	}
}

CLevel_Animation* CLevel_Animation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CLevel_Animation* pInstance = new CLevel_Animation(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("CLevel_Animation::Create, Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Animation::Free()
{
	//for (CImGui_Base* pElement : m_GuiElements)
	//{
	//	Safe_Release(pElement);
	//}
	//m_GuiElements.fill(nullptr);

	Safe_Release(m_pImGuiManager);
	Safe_Release(m_pPickingManager);
	Super::Free();
}
