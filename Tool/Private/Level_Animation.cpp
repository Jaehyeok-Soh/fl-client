#include "pch.h"
#include "Level_Animation.h"
#include "Level_Loading.h"

// ready obj
#include "CameraMan_Free.h"
#include "AnimObj.h"
#include "Tool_ContainerObject.h"
#include "Tool_Weapon.h"

// ImGui
#include "ImGui_Base.h"

// Manager
#include "Picking_ToolManager.h"
#include "ImGui_ToolManager.h"
#include "GameInstance.h"
#include "AnimTool_Manager.h"

// Component
#include "Model.h"
#include "Bone.h"

// Panel
#include "Panel_AnimModelFile.h"
#include "Panel_AnimationController.h"
#include "Panel_AnimDescription.h"
#include "Panel_ModelInfo.h"
#include "Panel_Parts.h"
#include "Panel_State.h"
#include "Panel_AnimationMix.h"

#include "DebugDraw.h"

CLevel_Animation::CLevel_Animation(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
	, m_pImGuiManager(CImGui_ToolManager::GetInstance())
	, m_pPickingManager(CPicking_ToolManager::GetInstance())
{
	Safe_AddRef(m_pImGuiManager);
	Safe_AddRef(m_pPickingManager);
}

HRESULT CLevel_Animation::Initialize()
{
	if (FAILED(Super::Initialize()))
		return E_FAIL;

	Ready_Camera(g_wszCameraLayer);
	Ready_Lights();
	Ready_Sounds();
#ifdef _DEBUG
	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pDeviceContext);
	m_pEffect = new BasicEffect(m_pDevice);
	m_pEffect->SetVertexColorEnabled(true);

	const void* pShaderInput = {};
	size_t iShaderInputLength = {};

	m_pEffect->GetVertexShaderBytecode(&pShaderInput, &iShaderInputLength);

	if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements,
		VertexPositionColor::InputElementCount,
		pShaderInput,
		iShaderInputLength,
		&m_pInputLayout)))
	{
		return E_FAIL;
	}

	D3D11_DEPTH_STENCIL_DESC dssDesc{};
	ZeroMemory(&dssDesc, sizeof(dssDesc));
	dssDesc.DepthEnable = TRUE;
	dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dssDesc.DepthFunc = D3D11_COMPARISON_LESS;

	m_pDevice->CreateDepthStencilState(&dssDesc, &m_pDSS);
#endif // _DEBUG

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

	m_pAnimToolManager = CAnimTool_Manager::GetInstance();
	m_pAnimToolManager->Initialize_AnimTool(m_pDevice, m_pDeviceContext);

	return S_OK;
}

void CLevel_Animation::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

	Update_Elements(fTimeDelta);

	m_pAnimToolManager->Update(fTimeDelta);
}

void CLevel_Animation::Update_Picking()
{
	Super::Update_Picking();

	if (m_pGameInstance->Mouse_Pressing(MOUSEKEYSTATE::LB))
		m_pPickingManager->Picking();

}

HRESULT CLevel_Animation::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;
	
#if _DEBUG
	Render_Grid();
#endif

	m_pAnimToolManager->Render(); // ????여기 맞는지 확인필요 소재혁 26.02.14

	m_pImGuiManager->Render_Begin();
	m_pImGuiManager->ImGuizmo_Render_Begin();
	m_pImGuiManager->Render_Dockspace();
	//////////////////////////
	// Element Render

	Render_Elements();

	//////////////////////////
	m_pImGuiManager->Render_Viewport(m_pSelectedObject);
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
	m_GuiElements[Elements::ANIMATION] = CPanel_AnimationController::Create("Panel_Animation", this, m_pDevice, m_pDeviceContext);
	//m_GuiElements[Elements::PARTS];
	m_GuiElements[Elements::DESCRIPTION] = CPanel_AnimDescription::Create("Panel_AnimDescription", this, m_pDevice, m_pDeviceContext);

	m_GuiElements[Elements::MODELINFO] = CPanel_ModelInfo::Create("Panel_ModelInfo", this, m_pDevice, m_pDeviceContext);

	m_GuiElements[Elements::PARTSINFO] = CPanel_Parts::Create("Panel_PartsInfo", this, m_pDevice, m_pDeviceContext);

	m_GuiElements[Elements::STATEEDITOR] = CPanel_State::Create("Panel_State", this, m_pDevice, m_pDeviceContext);

	m_GuiElements[Elements::MIXER] = CPanel_AnimationMix::Create("Panel_AnimMixer", this, m_pDevice, m_pDeviceContext);

	return S_OK;
}

HRESULT CLevel_Animation::Ready_Event()
{
	m_EventHandles[Event::ChangeSelectedObject] =
		m_pGameInstance->Subscribe<ChangeSelectedObject>(this, &CLevel_Animation::On_ChangeSelectedObject);

	m_EventHandles[Event::LOAD] =
		m_pGameInstance->Subscribe<LoadAnimModel>(this, &CLevel_Animation::Load_AnimModel);

	m_EventHandles[Event::LOAD_PART] =
		m_pGameInstance->Subscribe<LoadAnimModelPart>(this, &CLevel_Animation::Load_PartObject);

	return S_OK;
}

void CLevel_Animation::On_ChangeSelectedObject(CGameObject* pGo)
{
	if (pGo) // 무언가 피킹되었다면
	{
		if (CToolObject* pToolGo = dynamic_cast<CToolObject*>(pGo))
		{
			m_pSelectedObject = pToolGo;
			return;
		}
	}
	else // 바닥이나 빈 공간을 눌러서 nullptr이 들어왔다면 선택 해제
	{
		return;
	}
}

HRESULT CLevel_Animation::Ready_PlayerSound()
{
	_uint iLevelID = ENUM_TO_UINT(ELevelType::ANIMATION);

	////Resources\Sounds\SFX\Monster\Common
	//if (FAILED(m_pGameInstance->Load_Sounds(iLevelID, ESoundCategory::SFX, L"../../Resources/Sounds/SFX/Monster/Common")))
	//	return E_FAIL;
	////Resources\Sounds\SFX\Monster\Boomer
	//if (FAILED(m_pGameInstance->Load_Sounds(iLevelID, ESoundCategory::SFX, L"../../Resources/Sounds/SFX/Monster/Boomer")))
	//	return E_FAIL;
	////Resources\Sounds\SFX\Monster\Dog
	//if (FAILED(m_pGameInstance->Load_Sounds(iLevelID, ESoundCategory::SFX, L"../../Resources/Sounds/SFX/Monster/Dog")))
	//	return E_FAIL;
	////Resources\Sounds\SFX\Monster\Fly
	//if (FAILED(m_pGameInstance->Load_Sounds(iLevelID, ESoundCategory::SFX, L"../../Resources/Sounds/SFX/Monster/Fly")))
	//	return E_FAIL;
	////Resources\Sounds\SFX\Monster\Veteran
	//if (FAILED(m_pGameInstance->Load_Sounds(iLevelID, ESoundCategory::SFX, L"../../Resources/Sounds/SFX/Monster/Veteran")))
	//	return E_FAIL;

	//C:\Users\admin\Eunbi\04.Final\Resources\Sounds\SFX\Player\Static\Combat\Dual
	if (FAILED(m_pGameInstance->Load_Sounds(iLevelID, ESoundCategory::SFX, L"../../Resources/Sounds/SFX/Player/Static/Combat/Dual")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Load_Sounds(iLevelID, ESoundCategory::SFX, L"../../Resources/Sounds/SFX/Player/Static/Combat/Sword")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Load_Sounds(iLevelID, ESoundCategory::SFX, L"../../Resources/Sounds/SFX/Player/Static/Combat/Gun")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Load_Sounds(iLevelID, ESoundCategory::SFX, L"../../Resources/Sounds/SFX/Player/Static/Combat/Condemn")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Load_Sounds(iLevelID, ESoundCategory::SFX, L"../../Resources/Sounds/SFX/Player/Static/Combat/Common")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Load_Sounds(iLevelID, ESoundCategory::SFX, L"../../Resources/Sounds/SFX/Player/Static/Combat/Skill")))
		return E_FAIL;
	//C:\Users\admin\Eunbi\04.Final\Resources\Sounds\SFX\Player\Static\Basic
	if (FAILED(m_pGameInstance->Load_Sounds(iLevelID, ESoundCategory::SFX, L"../../Resources/Sounds/SFX/Player/Static/Basic/Cloth")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Load_Sounds(iLevelID, ESoundCategory::SFX, L"../../Resources/Sounds/SFX/Player/Static/Basic/Dodge")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Load_Sounds(iLevelID, ESoundCategory::SFX, L"../../Resources/Sounds/SFX/Player/Static/Basic/Jump")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Load_Sounds(iLevelID, ESoundCategory::SFX, L"../../Resources/Sounds/SFX/Player/Static/Basic/Movement")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Load_Sounds(iLevelID, ESoundCategory::SFX, L"../../Resources/Sounds/SFX/Player/Static/Basic/Slide")))
		return E_FAIL;
	//C:\Users\admin\Eunbi\04.Final\Resources\Sounds\SFX\Player\Static\Hit
	if (FAILED(m_pGameInstance->Load_Sounds(iLevelID, ESoundCategory::SFX, L"../../Resources/Sounds/SFX/Player/Static/Hit")))
		return E_FAIL;
	//C:\Users\admin\Eunbi\04.Final\Resources\Sounds\SFX\Player\FootSound\TutorialVillage
	if (FAILED(m_pGameInstance->Load_Sounds(iLevelID, ESoundCategory::SFX, L"../../Resources/Sounds/SFX/Player/FootSound/TutorialVillage")))
		return E_FAIL;
	//"C:\Users\admin\Eunbi\04.Final\Resources\Sounds\SFX\Player\Voice"
	if (FAILED(m_pGameInstance->Load_Sounds(iLevelID, ESoundCategory::SFX, L"../../Resources/Sounds/SFX/Player/Voice")))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Animation::Ready_XibiSound()
{
	_uint iLevelID = ENUM_TO_UINT(ELevelType::ANIMATION);

	if (FAILED(m_pGameInstance->Load_Sounds(iLevelID, ESoundCategory::SFX, L"../../Resources/Sounds/SFX/Player/Static/Combat/Common")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Load_Sounds(iLevelID, ESoundCategory::SFX, L"../../Resources/Sounds/SFX/Player/Static/Basic/Cloth")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Load_Sounds(iLevelID, ESoundCategory::SFX, L"../../Resources/Sounds/SFX/Player/Static/Basic/Slide")))
		return E_FAIL;

	//C:\Users\admin\Eunbi\04.Final\Resources\Sounds\SFX\Boss\Xibi
	if (FAILED(m_pGameInstance->Load_Sounds(iLevelID, ESoundCategory::SFX, L"../../Resources/Sounds/SFX/Boss/Xibi")))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Animation::Ready_LianSound()
{
	_uint iLevelID = ENUM_TO_UINT(ELevelType::ANIMATION);

	if (FAILED(m_pGameInstance->Load_Sounds(iLevelID, ESoundCategory::SFX, L"../../Resources/Sounds/SFX/Player/Static/Combat/Common")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Load_Sounds(iLevelID, ESoundCategory::SFX, L"../../Resources/Sounds/SFX/Player/Static/Basic/Cloth")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Load_Sounds(iLevelID, ESoundCategory::SFX, L"../../Resources/Sounds/SFX/Player/Static/Basic/Slide")))
		return E_FAIL;

	//C:\Users\admin\Eunbi\04.Final\Resources\Sounds\SFX\Boss\Xibi
	if (FAILED(m_pGameInstance->Load_Sounds(iLevelID, ESoundCategory::SFX, L"../../Resources/Sounds/SFX/Boss/Lian")))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Animation::Release_Event()
{
	m_pGameInstance->Unsubscribe<LoadAnimModel>(m_EventHandles[Event::LOAD]);
	m_pGameInstance->Unsubscribe<LoadAnimModelPart>(m_EventHandles[Event::LOAD_PART]);
	m_pGameInstance->Unsubscribe<ChangeSelectedObject>(m_EventHandles[Event::ChangeSelectedObject]);

	return S_OK;
}

HRESULT CLevel_Animation::Ready_Sounds()
{
	_uint iLevelID = ENUM_TO_UINT(ELevelType::ANIMATION);
	if (FAILED(m_pGameInstance->Load_Sounds(iLevelID, ESoundCategory::SFX, L"../../Resources/Sounds/SFX/Test/")))
		return E_FAIL;

	//C:\Users\admin\Eunbi\04.Final\Resources\Sounds\SFX\Die
	if (FAILED(m_pGameInstance->Load_Sounds(iLevelID, ESoundCategory::SFX, L"../../Resources/Sounds/SFX/Die")))
		return E_FAIL;

	//if (FAILED(Ready_PlayerSound()))
	//	return E_FAIL;

	if (FAILED(Ready_XibiSound()))
		return E_FAIL;

	if (FAILED(Ready_LianSound()))
		return E_FAIL;

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

#ifdef _DEBUG
void CLevel_Animation::Render_Grid()
{
	m_pDeviceContext->RSSetState(nullptr);

	float blendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
	m_pDeviceContext->OMSetBlendState(nullptr, blendFactor, 0xffffffff);

	m_pDeviceContext->OMSetDepthStencilState(m_pDSS, 0);

	m_pEffect->SetWorld(Matrix::Identity);
	m_pEffect->SetView(m_pGameInstance->Get_ViewMatrix());
	m_pEffect->SetProjection(m_pGameInstance->Get_ProjMatrix());

	m_pEffect->SetLightingEnabled(false);
	m_pEffect->SetTextureEnabled(false);

	m_pEffect->SetVertexColorEnabled(true);

	m_pEffect->Apply(m_pDeviceContext);

	m_pDeviceContext->IASetInputLayout(m_pInputLayout);
	m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	m_pBatch->Begin();

	_float fGridSize = 100.f;
	size_t numDiv = 100;

	Vec4 xAxis = Vec4(1.f, 0.f, 0.f, 0.f);
	Vec4 zAxis = Vec4(0.f, 0.f, 1.f, 0.f);
	Vec4 origin = Vec4(0.f, 0.f, 0.f, 1.f);

	Vec4 gridAxis1 = xAxis * fGridSize;
	Vec4 gridAxis2 = zAxis * fGridSize;

	DX::DrawGrid(m_pBatch, XMLoadFloat4(&gridAxis1), XMLoadFloat4(&gridAxis2), origin, numDiv, numDiv, DirectX::Colors::DarkGray);

	m_pBatch->End();
}
#endif // _DEBUG

void CLevel_Animation::Load_AnimModel(fs::path animModelPath, ANIM_SRT pretransform)
{
	m_pGameInstance->Clear_Layer(ENUM_TO_UINT(ELevelType::ANIMATION), m_wstrLayer);
	//Safe_Release(m_pSelectedObject);
	//if (m_pSelectedObject)
	//	m_pGameInstance->Request_DeleteGameObject(ENUM_TO_UINT(ELevelType::ANIMATION), m_wstrLayer, m_pSelectedObject);
	//if (m_pSelectedObject)
	//	m_pGameInstance->Immediately_DeleteGameObject(ENUM_TO_UINT(ELevelType::ANIMATION), m_wstrLayer, m_pSelectedObject);
	Create_AnimModel(animModelPath, pretransform);
	SetAnimationInfo(animModelPath);
}

void CLevel_Animation::Load_PartObject(fs::path animPartModelPath, ANIM_SRT pretransform, _int iSocketBondIdx, _bool bCombine, _bool bStatic, _int iRootBoneIdx)
{
	Matrix matScale = Matrix::CreateScale(pretransform.vScale);
	Matrix matRotation = Matrix::CreateFromYawPitchRoll(pretransform.vRot);
	Matrix matTranslation = Matrix::CreateTranslation(pretransform.vTranslation);
	Matrix matPreTransform = matScale * matRotation * matTranslation;

	//Matrix EunbiMat = Matrix::CreateScale(0.0001f);

	if (!m_pSelectedObject)
	{
		MSG_BOX("먼저 베이스 모델을 로드하세요");
		return;
	}

	CAnimObj* pAnimObj = static_cast<CAnimObj*>(m_pSelectedObject);
	CModel* pParentModel = const_cast<CModel*>(pAnimObj->Get_ModelComPtr());

	// 모델 프로토타입 추가.
	wstring modelProtoTag = L"Prototype_Component_Model_" + animPartModelPath.stem().wstring();
	CModel::MODEL_ORIGIN_DESC desc = {};
	desc.eType = bStatic ? EModelType::STATIC : EModelType::ANIM; // 무기는 보통 고정 모델, 필요시 분기
	desc.iPrototypeLevelIndex = ENUM_TO_UINT(ELevelType::ANIMATION);
	desc.wstrModelFolderName = animPartModelPath.stem().wstring();
	desc.pMatPreTransform = &matPreTransform;
	desc.FStageBone = CModel::STAGEING_BONE::SB_ZEROBONE;

	if (!bStatic && iRootBoneIdx >= 0)
	{
		CModel::DATA_ANIMCHANNEL tAniChannelData = {};
		tAniChannelData.iRootBoneIndex = iRootBoneIdx;
		desc.pAniChannelData = &tAniChannelData;
	}

	CModel* pModelProto = CModel::Create(m_pDevice, m_pDeviceContext, &desc);
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::ANIMATION), modelProtoTag, pModelProto)))
		Safe_Release(pModelProto);

	// 컨테이너 파츠 추가
	CTool_Weapon::WEAPON_DESC weaponDesc;
	weaponDesc.pMatParent = &pAnimObj->Get_Component<CTransform>()->Get_WorldMatrix(); // Tool_PartObject가 기대하는 부모
	weaponDesc.wstrModelPrototypeName = modelProtoTag;

	if (iSocketBondIdx < 0)
		weaponDesc.pMatSocket = nullptr;
	if(bCombine)
		weaponDesc.pMatSocket = &pParentModel->Get_Bone(iSocketBondIdx)->Get_CombinedTransformMatrix();
	else
		weaponDesc.pMatSocket = &pParentModel->Get_Bone(iSocketBondIdx)->Get_BindPoseTransformMatrix();

	weaponDesc.eModel = bStatic ? CTool_Weapon::Weapon_ModelType::STATIC : CTool_Weapon::Weapon_ModelType::ANIM;
	weaponDesc.iSocketIdx = iSocketBondIdx;

	_uint iPartID = (_uint)pAnimObj->Get_PartList().size();
	pAnimObj->Add_Part(iPartID, ENUM_TO_UINT(ELevelType::ANIMATION), L"Prototype_GameObject_Tool_Weapon", &weaponDesc);
}

void CLevel_Animation::Create_AnimModel(fs::path animModelPath, ANIM_SRT pretransform)
{
	//m_pSelectedObject;
	wstring prototypeTag = Create_AnimModelPrototype(animModelPath, pretransform);

	CAnimObj::ANIMOBJ_DESC animObjDesc{};
	CTransform::TRANSFORM_DESC transformDesc = {};
	animObjDesc.iLevelIndex = ENUM_TO_UINT(ELevelType::LOGO);
	animObjDesc.wstrModelProtoTag = prototypeTag;
	transformDesc.TranslationMatrix = Matrix::CreateTranslation(Vec3(0.f, 0.f, 0.f));
	animObjDesc.pTransform_Desc = &transformDesc;
	animObjDesc.wstrLayerTag = m_wstrLayer;

	if (!(m_pSelectedObject = static_cast<CToolObject*>(m_pGameInstance->Add_GameObject(ENUM_TO_UINT(ELevelType::ANIMATION), L"Prototype_GameObject_AnimObject", ENUM_TO_UINT(ELevelType::ANIMATION), m_wstrLayer, &animObjDesc))))
		m_pGameInstance->Immediately_DeleteGameObject(ENUM_TO_UINT(ELevelType::ANIMATION), m_pSelectedObject);

	m_pSelectedObject->Get_Component<CTransform>()->Set_WorldMatrix(Matrix::Identity);
}

wstring CLevel_Animation::Create_AnimModelPrototype(fs::path animModelPath, ANIM_SRT pretransform)
{
	Matrix matScale = Matrix::CreateScale(pretransform.vScale);
	Matrix matRotation = Matrix::CreateFromYawPitchRoll(pretransform.vRot * TO_RAD);
	Matrix matTranslation = Matrix::CreateTranslation(pretransform.vTranslation);
	Matrix matPreTransform = matScale * matRotation * matTranslation;

	wstring prototypeTag(L"Prototype_Component_Model_");

	// For. Prototype_Component_Model_Master
	{
		CModel::MODEL_ORIGIN_DESC desc = {};
		desc.eType = EModelType::ANIM;
		desc.iPrototypeLevelIndex = ENUM_TO_UINT(ELevelType::ANIMATION);
		desc.pMatPreTransform = &(matPreTransform);
		desc.wstrModelFolderName = animModelPath.stem().wstring();

		desc.FStageBone = CModel::STAGEING_BONE::SB_ALLBONE;

		//CModel::DATA_ANIMCHANNEL tAniChannelData = {};
		//tAniChannelData.iRootBoneIndex = 2;
		//desc.pAniChannelData = &tAniChannelData;

		prototypeTag += desc.wstrModelFolderName;
		CModel* pInstance = CModel::Create(m_pDevice, m_pDeviceContext, &desc);
		if(FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::ANIMATION), prototypeTag, pInstance)))
			Safe_Release(pInstance);
	}

	return prototypeTag;
}

void CLevel_Animation::SetAnimationInfo(fs::path animModelPath)
{
	m_pAnimToolManager->SetAnimationObject(static_cast<CAnimObj*>(m_pSelectedObject), animModelPath);
	static_cast<CPanel_AnimationController*>(m_GuiElements[Elements::ANIMATION])->SetAnimationObject();
	static_cast<CPanel_AnimDescription*>(m_GuiElements[Elements::DESCRIPTION])->SetAnimationObject();
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

#ifdef _DEBUG
	Safe_Delete(m_pBatch);
	Safe_Delete(m_pEffect);

	Safe_Release(m_pInputLayout);
	Safe_Release(m_pDSS);
#endif

	for (CImGui_Base* pElement : m_GuiElements)
	{
		Safe_Release(pElement);
	}
	m_GuiElements.fill(nullptr);

	m_pAnimToolManager->DestroyInstance();

	m_pImGuiManager->Clear_GuizmoTarget();
	Safe_Release(m_pImGuiManager);
	Safe_Release(m_pPickingManager);
	Super::Free();
}
