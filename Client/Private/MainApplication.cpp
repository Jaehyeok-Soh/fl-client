#include "pch.h"
#include "MainApplication.h"
#include "Collision_Manager.h"
#include "Material.h"
#include "MaterialInstance.h"
#include "VIBuffer_Rect_Color.h"
#include "VIBuffer_Cube_Tex.h"
#include "Shader.h"
#include "ComputeShader.h"
#include "Character.h"
#include "Texture.h"
#include "GameInstance.h"
#include "VIBuffer_Rect_Tex.h"
#include "Level_Loading.h"
#include "ImGui_ClientDebug.h"
#include "EngineConsole.h"
#include "PhysicsRigidBody.h"
#include "PhysicsCollider.h"
#include "PhysicsCCT.h"
#include "MyStat.h"
#include "SkillBase.h"
#include "ActionSkill.h"

// UI
#include "UI_Manager.h"
#include "Canvas.h"
#include "WorldUI_Component.h"
#include "UILoading_Text.h"
#include "UILoading_Progress.h"
#include "UILoading_Image.h"

USING(Client)

CMainApplication::CMainApplication()
	: m_pGameInstance (CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApplication::Initialize()
{
	ENGINE_DESC EngineDesc;
	::ZeroMemory(&EngineDesc, sizeof(EngineDesc));
	EngineDesc.eIsWindow = WINMODE::WIN;
	EngineDesc.iWinCX = g_iWinSizeX;
	EngineDesc.iWinCY = g_iWinSizeY;
	EngineDesc.iLevelCount = g_iLevelType_Count;
	EngineDesc.hWnd = g_hWnd;
	EngineDesc.hInst = g_hInstance;

	if (FAILED(m_pGameInstance->Initialize_Engine(EngineDesc, &m_pDevice, &m_pDeviceContext)))
		return E_FAIL;

	if (FAILED(Ready_Static_Prototype()))
		return E_FAIL;

	if (FAILED(Ready_Managers()))
		return E_FAIL;

	if (FAILED(Ready_Fonts()))
		return E_FAIL;

#ifdef _DEBUG
	CEngineConsole::Initialize();
	CEngineConsole::Set_Title(L"DebugConsole, 달려라 달려!");
	m_pDebugGui = CImGui_ClientDebug::GetInstance();
	if (m_pDebugGui == nullptr)
		return E_FAIL;
	if (FAILED(m_pDebugGui->Initialize(g_hWnd, m_pDevice, m_pDeviceContext)))
		return E_FAIL;
#endif

	if (FAILED(Start_Level(ELevelType::LOGO)))
		return E_FAIL;	

	CMonsterState_Factory::GetInstance()->Initialize();

	return S_OK;
}

HRESULT CMainApplication::Start_Level(ELevelType eStartLevel)
{
	m_pGameInstance->Request_ChangeLevel(ENUM_TO_UINT(ELevelType::LOADING), CLevel_Loading::Create(m_pDevice, m_pDeviceContext, eStartLevel));

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
	Vec4 ClearColor = { 0.f, 0.f, 1.f, 1.f };
	m_pGameInstance->Draw_Begin(&ClearColor);
	m_pGameInstance->Draw();

#ifdef _DEBUG
	m_pDebugGui->Render();
#endif

	m_pGameInstance->Draw_End();

	return S_OK;
}

HRESULT CMainApplication::Ready_Static_Prototype()
{
	//====================
	// Resource Component
	//====================
	// For. Prototype_Component_Shader_VtxPosTex
	{
		CShader::SHADER_ORIGIN_DESC shaderDesc = {};
		shaderDesc.pShaderFilePath = L"../../Shaders/Shader_VtxPosTex.hlsl";
		shaderDesc.eLayout = EVtxLayout::VTXPOSTEX;
		if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Shader_VtxPosTex",
			CShader::Create(m_pDevice, m_pDeviceContext, &shaderDesc))))
			return E_FAIL;
	}

	// For. Prototype_Component_Shader_VtxMesh_SkillEffect
	{
		CShader::SHADER_ORIGIN_DESC shaderDesc = {};
		shaderDesc.pShaderFilePath = L"../../Shaders/Shader_VtxMesh_SkillEffect.hlsl";
		shaderDesc.eLayout = EVtxLayout::VTXMESH;
		if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Shader_VtxMesh_SkillEffect",
			CShader::Create(m_pDevice, m_pDeviceContext, &shaderDesc))))
			return E_FAIL;
	}

	// For. Prototype_Component_Shader_VtxNorTex
	{
		CShader::SHADER_ORIGIN_DESC shaderDesc = {};
		shaderDesc.pShaderFilePath = L"../../Shaders/Shader_VtxNorTex.hlsl";
		shaderDesc.eLayout = EVtxLayout::VTXNORTEX;
		if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Shader_VtxNorTex",
			CShader::Create(m_pDevice, m_pDeviceContext, &shaderDesc))))
			return E_FAIL;
	}

	// For. Prototype_Component_Shader_VtxMesh
	{
		CShader::SHADER_ORIGIN_DESC shaderDesc = {};
		shaderDesc.pShaderFilePath = L"../../Shaders/Shader_VtxMesh.hlsl";
		shaderDesc.eLayout = EVtxLayout::VTXMESH;
		if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Shader_VtxMesh",
			CShader::Create(m_pDevice, m_pDeviceContext, &shaderDesc))))
			return E_FAIL;
	}

	// For. Prototype_Component_Shader_InstanceMesh
	{
		CShader::SHADER_ORIGIN_DESC shaderDesc = {};
		shaderDesc.pShaderFilePath = L"../../Shaders/Shader_VtxInstanceMesh.hlsl";
		shaderDesc.eLayout = EVtxLayout::VTX_INSTANCE_MESH;
		if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Shader_VtxInstanceMesh",
			CShader::Create(m_pDevice, m_pDeviceContext, &shaderDesc))))
			return E_FAIL;
	}

	// For. Prototype_Component_Shader_VtxCube
	{
		CShader::SHADER_ORIGIN_DESC shaderDesc = {};
		shaderDesc.pShaderFilePath = L"../../Shaders/Shader_VtxCube.hlsl";
		shaderDesc.eLayout = EVtxLayout::VTXCUBE;
		if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Shader_VtxCube",
			CShader::Create(m_pDevice, m_pDeviceContext, &shaderDesc))))
			return E_FAIL;
	}

	// For. Prototype_Component_Shader_AnimMesh
	{
		CShader::SHADER_ORIGIN_DESC shaderDesc = {};
		shaderDesc.pShaderFilePath = L"../../Shaders/Shader_VtxAnimMesh.hlsl";
		shaderDesc.eLayout = EVtxLayout::VTXANIMMESH;
		if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Shader_VtxAnimMesh",
			CShader::Create(m_pDevice, m_pDeviceContext, &shaderDesc))))
			return E_FAIL;
	}

#pragma region Compute_Shader
	// For. Prototype_Component_Shader_CPT_Effect_Particle
	{
		CComputeShader::ComShaderOriginDesc shaderDesc = {};
		shaderDesc.pShaderFilePath = L"../../Shaders/Shader_CPT_Effect_Particle.hlsl";
		if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Shader_CPT_Effect_Particle",
			CComputeShader::Create(m_pDevice, m_pDeviceContext, &shaderDesc))))
			return E_FAIL;
	}

	// For. Prototype_Component_Shader_BondCombine
	{
		CComputeShader::ComShaderOriginDesc shaderDesc = {};
		shaderDesc.pShaderFilePath = L"../../Shaders/ComShader_BoneCombine.hlsl";
		if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Shader_BondCombine",
			CComputeShader::Create(m_pDevice, m_pDeviceContext, &shaderDesc))))
			return E_FAIL;
	}

	// For. Prototype_Component_Shader_AnimEv
	{
		CComputeShader::ComShaderOriginDesc shaderDesc = {};
		shaderDesc.pShaderFilePath = L"../../Shaders/ComShader_AnimEvaluate.hlsl";
		if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Shader_AnimEv",
			CComputeShader::Create(m_pDevice, m_pDeviceContext, &shaderDesc))))
			return E_FAIL;
	}

	// For. Prototype_Component_Shader_AnimB
	{
		CComputeShader::ComShaderOriginDesc shaderDesc = {};
		shaderDesc.pShaderFilePath = L"../../Shaders/ComShader_AnimBlend.hlsl";
		if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Shader_AnimB",
			CComputeShader::Create(m_pDevice, m_pDeviceContext, &shaderDesc))))
			return E_FAIL;
	}

	// For. Prototype_Component_Shader_BoneMesh
	{
		CComputeShader::ComShaderOriginDesc shaderDesc = {};
		shaderDesc.pShaderFilePath = L"../../Shaders/ComShader_BoneMesh.hlsl";
		if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Shader_BoneMesh",
			CComputeShader::Create(m_pDevice, m_pDeviceContext, &shaderDesc))))
			return E_FAIL;
	}

	//// For. Prototype_Component_Shader_GetBone
	//{
	//	CComputeShader::ComShaderOriginDesc shaderDesc = {};
	//	shaderDesc.pShaderFilePath = L"../../Shaders/ComShader_GetBoneCombine.hlsl";
	//	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Shader_GetBone",
	//		CComputeShader::Create(m_pDevice, m_pDeviceContext, &shaderDesc))))
	//		return E_FAIL;
	//}

	// For. Prototype_Component_Shader_AnimMix
	{
		//ComShader_AnimMix
		CComputeShader::ComShaderOriginDesc shaderDesc = {};
		shaderDesc.pShaderFilePath = L"../../Shaders/ComShader_AnimMix.hlsl";
		if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Shader_AnimMix",
			CComputeShader::Create(m_pDevice, m_pDeviceContext, &shaderDesc))))
			return E_FAIL;
	}

	// For. Prototype_Component_Shader_AnimAdditiveMix
	{
		//ComShader_AnimMix
		CComputeShader::ComShaderOriginDesc shaderDesc = {};
		shaderDesc.pShaderFilePath = L"../../Shaders/ComShader_AnimAdditiveMix.hlsl";
		if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Shader_AnimAdditiveMix",
			CComputeShader::Create(m_pDevice, m_pDeviceContext, &shaderDesc))))
			return E_FAIL;
	}
#pragma endregion

#pragma region EFFECT_Shader
	// For. Prototype_Component_Shader_VtxEffectMesh
	{
		CShader::SHADER_ORIGIN_DESC shaderDesc = {};
		shaderDesc.pShaderFilePath = L"../../Shaders/Shader_VtxEffectMesh.hlsl";
		shaderDesc.eLayout = EVtxLayout::VTXPOS_PARTICLEMESH;
		if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Shader_VtxEffectMesh",
			CShader::Create(m_pDevice, m_pDeviceContext, &shaderDesc))))
			return E_FAIL;
	}

	// For. Prototype_Component_Shader_VtxEffectParticle
	{
		CShader::SHADER_ORIGIN_DESC shaderDesc = {};
		shaderDesc.pShaderFilePath = L"../../Shaders/Shader_VtxEffectParticle.hlsl";
		shaderDesc.eLayout = EVtxLayout::VTXPOS_PARTICLE;
		if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Shader_VtxEffectParticle",
			CShader::Create(m_pDevice, m_pDeviceContext, &shaderDesc))))
			return E_FAIL;
	}

	// For. Prototype_Component_Shader_VtxEffectTexture
	{
		CShader::SHADER_ORIGIN_DESC shaderDesc = {};
		shaderDesc.pShaderFilePath = L"../../Shaders/Shader_VtxEffectTexture.hlsl";
		shaderDesc.eLayout = EVtxLayout::VTXPOS_PARTICLE;
		if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Shader_VtxEffectTexture",
			CShader::Create(m_pDevice, m_pDeviceContext, &shaderDesc))))
			return E_FAIL;
	}
#pragma endregion
	// ======================		BUFFER		======================

	// For. Prototype_Component_VIBuffer_Rect_Tex
	{
		CVIBuffer_Rect_Tex::VIBUFFER_ORIGIN_DESC viBufferDesc = {};
		if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_VIBuffer_Rect_Tex",
			CVIBuffer_Rect_Tex::Create(m_pDevice, m_pDeviceContext, &viBufferDesc))))
			return E_FAIL;
	}

	// For. Prototype_Component_VIBuffer_Cube_Tex
	{
		CVIBuffer_Cube_Tex::VIBUFFER_ORIGIN_DESC viBufferDesc = {};
		if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_VIBuffer_Cube_Tex",
			CVIBuffer_Cube_Tex::Create(m_pDevice, m_pDeviceContext, &viBufferDesc))))
			return E_FAIL;
	}

	// For. Prototype_Component_Texture_Empty
	{
		CTexture::TEXTURE_COMPONENT_ORIGIN_DESC desc = {};
		if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Texture_Empty",
			CTexture::Create(&desc))))
			return E_FAIL;
	}

	// For. Prototype_Component_Texture_Default
	{
		CTexture::TEXTURE_COMPONENT_ORIGIN_DESC textureDesc = {};
		textureDesc.iTextureCount = 1;
		textureDesc.wstrTexturePath = L"../../Resources/Textures/Default.png";
		if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Texture_Default",
			CTexture::Create(&textureDesc))))
			return E_FAIL;
	}

	// For. Prototype_ScriptComponent_WorldUI
	{
		if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_ScriptComponent_WorldUI",
			CWorldUI_Component::Create())))
			return E_FAIL;
	}
	

	// For. UI Texture
	if (FAILED(Loading_Textures(L"../../Resources/Textures/UI/UI_Client/Loading/")))
		return E_FAIL;

	// For. UI Texture
	if (FAILED(Loading_Textures(L"../../Resources/Textures/UI/UI_Client/")))
		return E_FAIL;

	// For. Prototype_Component_Transform
	{
		if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC),
			L"Prototype_Component_Transform",
			CTransform::Create())))
			return E_FAIL;
	}
	
	//====================
	// Resource Material
	//====================
	// For. Material_Default
	{
		CMaterial::tagMaterialDesc desc = {};
		desc.wstrName = L"Material_Default";
		if (FAILED(m_pGameInstance->Add_Resource(desc.wstrName, CMaterial::Create(m_pDevice, m_pDeviceContext, &desc))))
			return E_FAIL;
	}
	//=================
	// MaterialInstance
	//=================
	auto makeMaterialInstance = [&](const Vec4& vColor, EMaterialInstanceType eType)->HRESULT
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
	if (FAILED(makeMaterialInstance(Vec4{ 1.f, 1.f, 1.f, 1.f }, EMaterialInstanceType::Default)))
		return E_FAIL;

	// For. MaterialInstance_Concrete
	if (FAILED(makeMaterialInstance(Vec4{ 0.3f, 0.3f, 0.3f, 1.f }, EMaterialInstanceType::Concrete)))
		return E_FAIL;

	// For. MaterialInstance_Mirror
	if (FAILED(makeMaterialInstance(Vec4{ 0.375f, 0.25f, 0.5f, 1.f }, EMaterialInstanceType::Mirror)))
		return E_FAIL;

	// For. MaterialInstance_Water
	if (FAILED(makeMaterialInstance(Vec4{ 0.235f, 0.3675f, 0.5f, 1.f }, EMaterialInstanceType::Water)))
		return E_FAIL;

	// For. MaterialInstance_Dirt
	if (FAILED(makeMaterialInstance(Vec4{ 0.415f, 0.251047f, 0.077674f, 1.f }, EMaterialInstanceType::Dirt)))
		return E_FAIL;

	// For. MaterialInstance_Red
	if (FAILED(makeMaterialInstance(Vec4{ 0.7f, 0.1f, 0.1f, 1.f }, EMaterialInstanceType::Red)))
		return E_FAIL;

	// For. MaterialInstance_Blue
	if (FAILED(makeMaterialInstance(Vec4{ 0.1f, 0.1f, 0.5f, 1.f }, EMaterialInstanceType::Blue)))
		return E_FAIL;

	// For. MaterialInstance_Green
	if (FAILED(makeMaterialInstance(Vec4{ 0.2f, 0.6f, 0.1f, 1.f }, EMaterialInstanceType::Green)))
		return E_FAIL;

	// For. MaterialInstance_Grass
	if (FAILED(makeMaterialInstance(Vec4{ 0.3f, 0.5f, 0.1f, 1.f }, EMaterialInstanceType::Grass)))
		return E_FAIL;

	// For. MaterialInstance_Orange
	if (FAILED(makeMaterialInstance(Vec4{ 0.86f, 0.3f, 0.f, 1.f }, EMaterialInstanceType::Orange)))
		return E_FAIL;

	// For. MaterialInstance_Brown
	if (FAILED(makeMaterialInstance(Vec4{ 0.567f, 0.304864f, 0.195615f, 1.f }, EMaterialInstanceType::Brown)))
		return E_FAIL;

	// For. MaterialInstance_Pupple
	if (FAILED(makeMaterialInstance(Vec4{ 0.27f, 0.15f, 0.48f, 1.f }, EMaterialInstanceType::Pupple)))
		return E_FAIL;

	// For. MaterialInstance_WinterGreen
	if (FAILED(makeMaterialInstance(Vec4{ 0.3f, 0.5f, 0.5f, 1.f }, EMaterialInstanceType::WinterGreen)))
		return E_FAIL;

	// For. MaterialInstance_BurnishedBrown
	if (FAILED(makeMaterialInstance(Vec4{ 0.48f, 0.35f, 0.34f, 1.f }, EMaterialInstanceType::BurnishedBrown)))
		return E_FAIL;

	// For. MaterialInstance_ConcreteLight
	if (FAILED(makeMaterialInstance(Vec4{ 0.72f, 0.72f, 0.69f, 1.f }, EMaterialInstanceType::ConcreteLight)))
		return E_FAIL;

	// For. MaterialInstance_ConcreteMid
	if (FAILED(makeMaterialInstance(Vec4{ 0.55f, 0.56f, 0.54f, 1.f }, EMaterialInstanceType::ConcreteMid)))
		return E_FAIL;

	// For. MaterialInstance_ConcreteWarm
	if (FAILED(makeMaterialInstance(Vec4{ 0.63f, 0.60f, 0.55f, 1.f }, EMaterialInstanceType::ConcreteWarm)))
		return E_FAIL;

	// For. MaterialInstance_MetalCool
	if (FAILED(makeMaterialInstance(Vec4{ 0.62f, 0.67f, 0.72f, 1.f }, EMaterialInstanceType::MetalCool)))
		return E_FAIL;

	// For. MaterialInstance_MetalDark
	if (FAILED(makeMaterialInstance(Vec4{ 0.28f, 0.31f, 0.34f, 1.f }, EMaterialInstanceType::MetalDark)))
		return E_FAIL;

	// For. MaterialInstance_CourtBlue
	if (FAILED(makeMaterialInstance(Vec4{ 0.16f, 0.22f, 0.52f, 1.f }, EMaterialInstanceType::CourtBlue)))
		return E_FAIL;

	// For. MaterialInstance_CourtBlue
	if (FAILED(makeMaterialInstance(Vec4{1.f,1.f,1.f,1.f}, EMaterialInstanceType::Free)))
		return E_FAIL;

	//=================
	// Physics Component
	//=================
	// For. Prototype_Component_Physics_CCT
	{
		if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Physics_CCT",
			CPhysicsCCT::Create(m_pDevice, m_pDeviceContext))))
			return E_FAIL;
	}

	// For. Prototype_Component_Physics_RigidBody
	{
		if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Physics_RigidBody",
			CPhysicsRigidBody::Create(m_pDevice, m_pDeviceContext))))
			return E_FAIL;
	}

	// For. Prototype_Component_Physics_Collider
	{
		if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Physics_Collider",
			CPhysicsCollider::Create(m_pDevice, m_pDeviceContext, nullptr))))
			return E_FAIL;
	}

	//=================
	// Skill & Stat
	//=================
	// For. Prototype_Component_Stat
	{
		if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Stat",
			CMyStat::Create())))
			return E_FAIL;
	}
	// For. Prototype_Component_ActionSkill
	{
		if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_ActionSkill",
			CActionSkill::Create())))
			return E_FAIL;
	}

	// For. Prototype_UI_Canvas
	{
		if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_UI_Canvas",
			CCanvas::Create(m_pDevice, m_pDeviceContext))))
			return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_UI_LoadingImage", CUILoading_Image::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_UI_LoadingProgress", CUILoading_Progress::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_UI_LoadingText", CUILoading_Text::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApplication::Ready_Managers()
{
	CUI_Manager::GetInstance()->Initialize_UIManager(m_pDevice, m_pDeviceContext);
	return S_OK;
}

HRESULT CMainApplication::Ready_Fonts()
{
	// font
	for (const auto& e : std::filesystem::directory_iterator(L"..\\..\\Resources\\Fonts"))
	{
		const std::wstring key = e.path().stem().wstring();   // 파일명(확장자 제외)
		const std::wstring path = e.path().wstring();          // 실제 경로
		if (FAILED(m_pGameInstance->Add_Font(key.c_str(), path.c_str())))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CMainApplication::Loading_Textures(const wstring& wstrFolder)
{
	if (std::filesystem::exists(wstrFolder) == false)
		return E_FAIL;

	size_t iFileCount = { 0 };
	for (const auto& entry : std::filesystem::directory_iterator(wstrFolder))
	{
		if (entry.is_regular_file())
		{
			++iFileCount;
		}
	}

	for (const auto& entry : std::filesystem::directory_iterator(wstrFolder))
	{
		wstring wstrFileName = { L"" };
		_wstring ext = { L"" };
		if (entry.is_regular_file())
		{
			ext = entry.path().extension().wstring();
			if (ext == L".ini")
				continue;
			wstrFileName = entry.path().filename().lexically_normal().stem();
			CTextureBase::RESOURCE_BASE_DESC desc = {};
			desc.wstrName = wstrFileName;
			desc.wstrPath = entry.path();
			if (FAILED(m_pGameInstance->Add_Resource(L"Texture_" + wstrFileName, CTextureBase::Create(m_pDevice, m_pDeviceContext, &desc))))
				return E_FAIL;
		}
	}
	return S_OK;
}

void CMainApplication::Free()
{	
	CMonsterState_Factory::DestroyInstance();

	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
	CUI_Manager::GetInstance()->DestroyInstance();	// 오브젝트 삭제 이후 삭제해야되는데 / 오브젝트에서 Addref 하고 있어서 안터짐
	Safe_Release(m_pGameInstance);
	m_pGameInstance->Destroy_Engine();

#ifdef _DEBUG
	CEngineConsole::Shutdown();
	m_pDebugGui->DestroyInstance();
#endif
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
