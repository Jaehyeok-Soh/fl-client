#include "pch.h"
#include "Level_Animation.h"
#include "Level_Loading.h"
#include "Animation_Defines.h"

// ready obj
#include "CameraMan_Free.h"
#include "AnimObj.h"

// ImGui
#include "ImGui_Base.h"

// Manager
#include "Picking_ToolManager.h"
#include "ImGui_ToolManager.h"
#include "GameInstance.h"

// Component
#include "Model.h"

// Panel
#include "Panel_AnimModelFile.h"

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

	Ready_Camera(g_wszCameraLayer);
	Ready_Lights();

	return S_OK;
}

HRESULT CLevel_Animation::Awake(const _uint iLevelID)
{
	if (FAILED(Super::Awake(iLevelID)))
		return E_FAIL;

	MSG_BOX("Animation");

	if (FAILED(Ready_Panels()))
		return E_FAIL;

	Ready_Event();
	m_pImGuiManager->Ready_Events();

	Ready_Camera_Setting(ENUM_TO_UINT(ELevelType::ANIMATION));

	return S_OK;
}

void CLevel_Animation::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

	Update_Elements(fTimeDelta);
}

void CLevel_Animation::Update_Picking()
{
	Super::Update_Picking();
	//m_pPickingManager->Picking();
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

	Render_Elements();

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
		TransformDesc.RotationMatrix = Matrix::CreateRotationX(XMConvertToRadians(45.f));
		TransformDesc.TranslationMatrix = Matrix::CreateTranslation(Vec3(0.f, -5.f, 5.f));
		TransformDesc.fMovePerSec = { 15.f };
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
			ENUM_TO_UINT(ELevelType::ANIMATION),
			wstrLayerTag, &goDesc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Animation::Ready_Camera_Setting(const _uint iLevelID)
{
	CCameraMan* pFreeCamera = static_cast<CCameraMan*>(m_pGameInstance->Get_GameObject_Back(iLevelID, g_wszCameraLayer));
	m_pGameInstance->Add_Camera(CameraType::STATIC, g_FreeCameraName, pFreeCamera);
	m_pGameInstance->Change_MainCamera(CameraType::STATIC, g_FreeCameraName);
	return S_OK;
}

HRESULT CLevel_Animation::Ready_Lights()
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
	m_GuiElements[Elements::FILE] = CPanel_AnimModelFile::Create("Panel_Explore", this, m_pDevice, m_pDeviceContext);
	//m_GuiElements[Elements::LOAD];
	//m_GuiElements[Elements::MODEL];
	//m_GuiElements[Elements::ANIMATION];
	//m_GuiElements[Elements::PARTS];

	return S_OK;
}

HRESULT CLevel_Animation::Ready_Event()
{
	m_EventHandles[Event::LOAD] =
		m_pGameInstance->Subscribe<LoadAnimModel>(this, &CLevel_Animation::Load_AnimModel);

	return S_OK;
}

HRESULT CLevel_Animation::Release_Event()
{
	m_pGameInstance->Unsubscribe<LoadAnimModel>(m_EventHandles[Event::LOAD]);

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

void CLevel_Animation::Load_AnimModel(fs::path animModelPath)
{
	if (m_pSelectedObject)
		m_pGameInstance->Immediately_DeleteGameObject(ENUM_TO_UINT(ELevelType::ANIMATION), m_wstrLayer, m_pSelectedObject);

	Create_AnimModel(animModelPath);

	m_pGameInstance->Add_GameObject(ENUM_TO_UINT(ELevelType::ANIMATION),
		L"anim model prototype",
		ENUM_TO_UINT(ELevelType::ANIMATION),
		m_wstrLayer, m_pSelectedObject);
}

void CLevel_Animation::Create_AnimModel(fs::path animModelPath)
{
	m_pSelectedObject;
	wstring prototypeTag = Create_AnimModelPrototype(animModelPath);

	CAnimObj::ANIMOBJ_DESC animObjDesc{};
	CTransform::TRANSFORM_DESC transformDesc = {};
	animObjDesc.iLevelIndex = ENUM_TO_UINT(ELevelType::LOGO);
	animObjDesc.wstrModelProtoTag = prototypeTag;
	transformDesc.TranslationMatrix = Matrix::CreateTranslation(Vec3(0.f, 0.f, 0.f));
	animObjDesc.pTransform_Desc = &transformDesc;
	animObjDesc.wstrLayerTag = m_wstrLayer;

	if (!(m_pSelectedObject = static_cast<CToolObject*>(m_pGameInstance->Add_GameObject(ENUM_TO_UINT(ELevelType::ANIMATION), L"Prototype_GameObject_AnimObject", ENUM_TO_UINT(ELevelType::ANIMATION), m_wstrLayer, &animObjDesc))))
		m_pGameInstance->Immediately_DeleteGameObject(ENUM_TO_UINT(ELevelType::ANIMATION), m_wstrLayer, m_pSelectedObject);
}

wstring CLevel_Animation::Create_AnimModelPrototype(fs::path animModelPath)
{
	//Matrix matPreTransformScale = Matrix::CreateScale(0.01f, 0.01f, 0.01f);
	Matrix matPreTransformScale = Matrix::Identity;
	Matrix matPreTransformIdentity = Matrix::Identity;
	Matrix matPreTransformTurn90 = matPreTransformScale * Matrix::CreateFromYawPitchRoll(XMConvertToRadians(90.f), 0.f, 0.f);

	wstring prototypeTag(L"Prototype_Component_Model_");

	// For. Prototype_Component_Model_Master
	{
		CModel::MODEL_ORIGIN_DESC desc = {};
		desc.eType = EModelType::ANIM;
		desc.iPrototypeLevelIndex = ENUM_TO_UINT(ELevelType::ANIMATION);
		desc.pMatPreTransform = &(matPreTransformScale);	// matPreTransformScale // matPreTransformTurn90
		desc.wstrModelFolderName = animModelPath.stem().wstring();

		CModel::DATA_ANIMCHANNEL tAniChannelData = {};
		tAniChannelData.iRootBoneIndex = 2;
		desc.pAniChannelData = &tAniChannelData;

		prototypeTag += desc.wstrModelFolderName;
		m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::ANIMATION), prototypeTag, CModel::Create(m_pDevice, m_pDeviceContext, &desc));
	}

	return prototypeTag;
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
