#include "MainApplication.h"
#include "Picking_ToolManager.h"
#include "Tool_Defines.h"
#include "VertexData.h"
#include "Engine_Utils.h"
#include "ImGui_ToolManager.h"
#include "Level_Loading.h"
#include "GameInstance.h"
//=================
// GameObject
//=================
#include "CameraMan_Free.h"

//=================
// Component
//=================
#include "VIBuffer_Line_Color.h"
#include "MonoBehaviour.h"
#include "Texture.h"
#include "Shader.h"
#include "Camera.h"
#include "Transform.h"

//=================
// MaterialInstance
//=================
#include "MaterialInstance.h"

USING(Tool)

CMainApplication::CMainApplication()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApplication::Initialize()
{
	ENGINE_DESC EngineDesc = {};
	::ZeroMemory(&EngineDesc, sizeof(EngineDesc));
	EngineDesc.eIsWindow = WINMODE::WIN;
	EngineDesc.iWinCX = g_iWinSizeX;
	EngineDesc.iWinCY = g_iWinSizeY;
	EngineDesc.iLevelCount = static_cast<_uint>(g_iLevelType_Count);
	EngineDesc.hWnd = g_hWnd;
	EngineDesc.hInst = g_hInstance;

	if (FAILED(m_pGameInstance->Initialize_Engine(EngineDesc, &m_pDevice, &m_pDeviceContext)))
		return E_FAIL;

	if (FAILED(Ready_Static_Prototype()))
		return E_FAIL;

	// Gui 세팅과 동시에 Level 스타트
	if (FAILED(Ready_GuiManager(EngineDesc.iWinCX, EngineDesc.iWinCY, /* StartLevel */ ELevelType::MAP)))
		return E_FAIL;

	CPicking_ToolManager* pPickingManager = { nullptr };
	if (!(pPickingManager = CPicking_ToolManager::GetInstance()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApplication::Start_Level(ELevelType eStartLevel)
{
	m_pGameInstance->Request_ChangeLevel(ENUM_TO_UINT(ELevelType::LOADING), CLevel_Loading::Create(m_pDevice, m_pDeviceContext, eStartLevel));

	return S_OK;
}

HRESULT CMainApplication::Ready_Static_Prototype()
{
	//=================
	// Component
	//=================
	{
		CTexture::TEXTURE_COMPONENT_ORIGIN_DESC desc = {};
		if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Texture_Empty", CTexture::Create(&desc))))
			return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Transform", CTransform::Create())))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Camera", CCamera::Create())))
		return E_FAIL;
	// For. Prototype_Component_VIBuffer_Line_Color
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_VIBuffer_Line_Color", CVIBuffer_Line_Color::Create(m_pDevice, m_pDeviceContext, nullptr))))
		return E_FAIL;

	// For. Prototype_Component_Texture_Default
	{
		CTexture::TEXTURE_COMPONENT_ORIGIN_DESC textureDesc = {};
		textureDesc.iTextureCount = 1;
		textureDesc.wstrTexturePath = L"../../Resources/Textures/Default.png";
		if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Texture_Default",
			CTexture::Create(&textureDesc))))
			return E_FAIL;
	}

	//=================
	// Resource Component
	//=================
	// For. Prototype_Component_Shader_VtxCol
	{
		CShader::SHADER_ORIGIN_DESC shaderDesc = {};
		shaderDesc.pShaderFilePath = L"../../Shaders/Shader_VtxCol.hlsl";
		shaderDesc.iNumElements = Engine::VTXPOSCOL::iNumElements;
		shaderDesc.pElements = Engine::VTXPOSCOL::Elements;
		if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Shader_VtxCol",
			CShader::Create(m_pDevice, m_pDeviceContext, &shaderDesc))))
			return E_FAIL;
	}

	// For. Prototype_Component_Shader_VtxPosTex
	{
		CShader::SHADER_ORIGIN_DESC shaderDesc = {};
		shaderDesc.pShaderFilePath = L"../../Shaders/Shader_VtxPosTex.hlsl";
		shaderDesc.iNumElements = Engine::VTXPOSTEX::iNumElements;
		shaderDesc.pElements = Engine::VTXPOSTEX::Elements;
		if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Shader_VtxPosTex",
			CShader::Create(m_pDevice, m_pDeviceContext, &shaderDesc))))
			return E_FAIL;
	}

	// For. Prototype_Component_Shader_VtxNorTex
	{
		CShader::SHADER_ORIGIN_DESC shaderDesc = {};
		shaderDesc.pShaderFilePath = L"../../Shaders/Shader_VtxNorTex.hlsl";
		shaderDesc.iNumElements = Engine::VTXNORTEX::iNumElements;
		shaderDesc.pElements = Engine::VTXNORTEX::Elements;
		if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Shader_VtxNorTex",
			CShader::Create(m_pDevice, m_pDeviceContext, &shaderDesc))))
			return E_FAIL;
	}

	// For. Prototype_Component_Shader_VtxMesh
	{
		CShader::SHADER_ORIGIN_DESC shaderDesc = {};
		shaderDesc.pShaderFilePath = L"../../Shaders/Shader_VtxMesh.hlsl";
		shaderDesc.iNumElements = Engine::VTXMESH::iNumElements;
		shaderDesc.pElements = Engine::VTXMESH::Elements;
		if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Shader_VtxMesh",
			CShader::Create(m_pDevice, m_pDeviceContext, &shaderDesc))))
			return E_FAIL;
	}

	// For. Prototype_Component_Shader_AnimMesh
	{
		CShader::SHADER_ORIGIN_DESC shaderDesc = {};
		shaderDesc.pShaderFilePath = L"../../Shaders/Shader_VtxAnimMesh.hlsl";
		shaderDesc.iNumElements = Engine::VTXANIMMESH::iNumElements;
		shaderDesc.pElements = Engine::VTXANIMMESH::Elements;
		if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Shader_AnimMesh",
			CShader::Create(m_pDevice, m_pDeviceContext, &shaderDesc))))
			return E_FAIL;
	}

	// For. Prototype_Component_Shader_VtxMesh_SkillEffect
	{
		CShader::SHADER_ORIGIN_DESC shaderDesc = {};
		shaderDesc.pShaderFilePath = L"../../Shaders/Shader_VtxMesh_SkillEffect.hlsl";
		shaderDesc.iNumElements = Engine::VTXMESH::iNumElements;
		shaderDesc.pElements = Engine::VTXMESH::Elements;
		if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Shader_VtxMesh_SkillEffect",
			CShader::Create(m_pDevice, m_pDeviceContext, &shaderDesc))))
			return E_FAIL;
	}

	//=================
	// GameObject
	//=================
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_GameObject_CameraManFree", CCameraMan_Free::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	//=================
	// MaterialInstance
	//=================
	auto makeMaterialInstance = [&](const _float4 &vColor, EMaterialInstanceType eType)->HRESULT
	{
		CMaterialInstance::tagMaterialInstanceOrignDesc desc = {};
		desc.vTintColor = vColor;
		desc.eType = eType;
		desc.wstrName = Engine_Utils::MI_ToWString(eType);
		desc.fEmissivePower = 1.f;
		if (FAILED(m_pGameInstance->Add_Resource(desc.wstrName, CMaterialInstance::Create(m_pDevice, m_pDeviceContext, &desc))))
			return E_FAIL;
		
		return S_OK;
	};
	// For. MaterialInstance_Default
	if (FAILED(makeMaterialInstance(_float4{ 1.f, 1.f, 1.f, 1.f }, EMaterialInstanceType::Default)))
		return E_FAIL;

	// For. MaterialInstance_Concrete
	if (FAILED(makeMaterialInstance(_float4{ 0.3f, 0.3f, 0.3f, 1.f }, EMaterialInstanceType::Concrete)))
		return E_FAIL;

	// For. MaterialInstance_Mirror
	if (FAILED(makeMaterialInstance(_float4{ 0.375f, 0.25f, 0.5f, 1.f }, EMaterialInstanceType::Mirror)))
		return E_FAIL;

	// For. MaterialInstance_Water
	if (FAILED(makeMaterialInstance(_float4{ 0.235f, 0.3675f, 0.5f, 1.f }, EMaterialInstanceType::Water)))
		return E_FAIL;

	// For. MaterialInstance_Dirt
	if (FAILED(makeMaterialInstance(_float4{ 0.415f, 0.251047f, 0.077674f, 1.f }, EMaterialInstanceType::Dirt)))
		return E_FAIL;

	// For. MaterialInstance_Red
	if (FAILED(makeMaterialInstance(_float4{ 0.7f, 0.1f, 0.1f, 1.f }, EMaterialInstanceType::Red)))
		return E_FAIL;

	// For. MaterialInstance_Blue
	if (FAILED(makeMaterialInstance(_float4{ 0.1f, 0.1f, 0.5f, 1.f }, EMaterialInstanceType::Blue)))
		return E_FAIL;

	// For. MaterialInstance_Green
	if (FAILED(makeMaterialInstance(_float4{ 0.2f, 0.6f, 0.1f, 1.f }, EMaterialInstanceType::Green)))
		return E_FAIL;

	// For. MaterialInstance_Grass
	if (FAILED(makeMaterialInstance(_float4{ 0.3f, 0.5f, 0.1f, 1.f }, EMaterialInstanceType::Grass)))
		return E_FAIL;

	// For. MaterialInstance_Orange
	if (FAILED(makeMaterialInstance(_float4{ 0.86f, 0.3f, 0.f, 1.f }, EMaterialInstanceType::Orange)))
		return E_FAIL;

	// For. MaterialInstance_Brown
	if (FAILED(makeMaterialInstance(_float4{ 0.567f, 0.304864f, 0.195615f, 1.f }, EMaterialInstanceType::Brown)))
		return E_FAIL;

	// For. MaterialInstance_Pupple
	if (FAILED(makeMaterialInstance(_float4{ 0.27f, 0.15f, 0.48f, 1.f }, EMaterialInstanceType::Pupple)))
		return E_FAIL;

	// For. MaterialInstance_WinterGreen
	if (FAILED(makeMaterialInstance(_float4{ 0.3f, 0.5f, 0.5f, 1.f }, EMaterialInstanceType::WinterGreen)))
		return E_FAIL;

	// For. MaterialInstance_BurnishedBrown
	if (FAILED(makeMaterialInstance(_float4{ 0.48f, 0.35f, 0.34f, 1.f }, EMaterialInstanceType::BurnishedBrown)))
		return E_FAIL;

	// For. MaterialInstance_ConcreteLight
	if (FAILED(makeMaterialInstance(_float4{ 0.72f, 0.72f, 0.69f, 1.f }, EMaterialInstanceType::ConcreteLight)))
		return E_FAIL;

	// For. MaterialInstance_ConcreteMid
	if (FAILED(makeMaterialInstance(_float4{ 0.55f, 0.56f, 0.54f, 1.f }, EMaterialInstanceType::ConcreteMid)))
		return E_FAIL;

	// For. MaterialInstance_ConcreteWarm
	if (FAILED(makeMaterialInstance(_float4{ 0.63f, 0.60f, 0.55f, 1.f }, EMaterialInstanceType::ConcreteWarm)))
		return E_FAIL;

	// For. MaterialInstance_MetalCool
	if (FAILED(makeMaterialInstance(_float4{ 0.62f, 0.67f, 0.72f, 1.f }, EMaterialInstanceType::MetalCool)))
		return E_FAIL;

	// For. MaterialInstance_MetalDark
	if (FAILED(makeMaterialInstance(_float4{ 0.28f, 0.31f, 0.34f, 1.f }, EMaterialInstanceType::MetalDark)))
		return E_FAIL;

	// For. MaterialInstance_CourtBlue
	if (FAILED(makeMaterialInstance(_float4{ 0.16f, 0.22f, 0.52f, 1.f }, EMaterialInstanceType::CourtBlue)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApplication::Ready_GuiManager(_uint iWidth, _uint iHeight, ELevelType eStartLevel)
{
	if (!(m_pImGuiManager = CImGui_ToolManager::GetInstance()))
		return E_FAIL;

	if (FAILED(m_pImGuiManager->Initialize_ToolManager(g_hWnd, m_pDevice, m_pDeviceContext, eStartLevel)))
		return E_FAIL;

	if (FAILED(m_pImGuiManager->CreateOrResizeViewportFrameTargets(iWidth, iHeight)))
		return E_FAIL;

	if (FAILED(Start_Level(eStartLevel)))
		return E_FAIL;

	return S_OK;
}

void CMainApplication::Update(const _float fTimeDelta)
{
	// 프레임 시작 직전 이벤트 Flush
	m_pGameInstance->Flush_All();

	m_pGameInstance->Update_Engine(fTimeDelta);
}

HRESULT CMainApplication::Render()
{
	_float4 ClearColor = { 0.f, 0.f, 1.f, 1.f };
	m_pGameInstance->Draw_Begin(&ClearColor);
	m_pGameInstance->Draw();
	m_pGameInstance->Draw_End();
	return S_OK;
}

void CMainApplication::Free()
{
	Safe_Release(m_pImGuiManager);
	CPicking_ToolManager::GetInstance()->DestroyInstance();
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);

	m_pImGuiManager->DestroyInstance();
	m_pGameInstance->Destroy_Engine();
	Super::Free();
}

CMainApplication* CMainApplication::Create()
{
	CMainApplication* pMainApplication = new CMainApplication;

	if (FAILED(pMainApplication->Initialize()))
	{
		MSG_BOX("CMainApplication::Create, Failed");
		Safe_Release(pMainApplication);
	}

	return pMainApplication;
}
