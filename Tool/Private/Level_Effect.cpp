///////////////
// Component //
///////////////
#include "VIBuffer_Terrain.h"
#include "PresetReceiver.h"
#include "Shader.h"
#include "Material.h"

////////////////
// GameObject //
////////////////
#include "CameraMan_Free.h"
#include "Terrain.h"

///////////
// ImGui //
///////////
#include "ImGui_Inspector_Effect.h"
#include "ImGui_PresetBrowser_Effect.h"
#include "ImGui_Base.h"

/////////////
// Manager //
/////////////
#include "Picking_ToolManager.h"
#include "ImGui_ToolManager.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Engine_Utils.h"
#include "Tool_Defines.h"
#include "Level_Effect.h"

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

	if (FAILED(Ready_Camera(L"Camera_Layer")))
		return E_FAIL;

	m_vClearColor = { 0.3f, 0.3f, 0.3f, 1.f };
	return S_OK;
}

HRESULT CLevel_Effect::Awake(const _uint iLevelID)
{
	if (FAILED(Super::Awake(iLevelID)))
		return E_FAIL;

	MSG_BOX("Effect");

	if (FAILED(m_pGameInstance->Awake_GameObjects(iLevelID, L"Camera_Layer")))
		return E_FAIL;

	if (FAILED(Ready_CameraSetting(iLevelID)))
		return E_FAIL;

	if (FAILED(Ready_Gui()))
		return E_FAIL;

	if (FAILED(Ready_Preset()))
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

_uint CLevel_Effect::Get_PresetId()
{
	return m_iSelectedPresetID;
}

const EFFECT_PRESET_SNAPSHOT* CLevel_Effect::Find_PresetSnapshot(_uint iPresetID) const
{
	auto itr = m_umapPresets.find(iPresetID);
	if (itr == m_umapPresets.end())
		return nullptr;
	return &itr->second.snapShot;
}

_bool CLevel_Effect::Rename_Preset(_uint iPresetID, const string& strName)
{
	if (strName.empty() == true)
		return false;

	auto itr = m_umapPresets.find(iPresetID);
	if (itr == m_umapPresets.end())
		return false;

	itr->second.strName = strName;
	return true;
}

const string* CLevel_Effect::Get_PresetName(_uint iPresetID)
{
	EFFECT_PRESET* pPreset = Get_Preset(iPresetID);
	if (pPreset == nullptr)
		return nullptr;
	
	return &pPreset->strName;
}

HRESULT CLevel_Effect::Load_EffectMeshPreviews(const MAPOBJECT_SAVEDATA& data, CGameObject** ppResult)
{
	if (data.meshEffect.has_value() == false)
		return E_FAIL;

	auto &loadedData = (*data.meshEffect);

	_uint iPresetID = loadedData.iPresetID;
	if (Get_Preset(iPresetID) == nullptr)
		iPresetID = 0;

	CSkillPreviewMesh::SM_DESC desc = {};
	desc.eRenderCategory = RENDER_CATEGORY::NONELIGHT;
	desc.wstrLayerTag = g_wszMeshPreviewLayer;
	desc.wstrModelPrototypeTag = Engine_Utils::ToWString(loadedData.strModelTag);
	desc.wstrShaderPrototypeTag = L"Prototype_Component_Shader_VtxMesh_SkillEffect";
	desc.wstrMaterialTag = L"Material_Default";
	desc.iPass = loadedData.iShaderPass;
	CToolObject* pNewObj = Spawn_PreviewMesh(&desc);
	if (!pNewObj)
		return E_FAIL;

	CSkillPreviewMesh* pPreview = static_cast<CSkillPreviewMesh*>(pNewObj);
	pPreview->Set_PresetID(iPresetID);
	const EFFECT_PRESET* pPreset = Get_Preset(iPresetID);
	if (pPreset)
	{
		if(CMonoBehaviour* pScript = pPreview->Get_Script_Component(L"PresetReceiver"))
			static_cast<CPresetReceiver*>(pScript)->Queue(pPreset->snapShot);
	}

	pPreview->Set_Name(data.name);
	m_vecPreviewMeshes.push_back(pPreview);

	if (ppResult)
		*ppResult = pPreview;	

	m_pGameInstance->Broadcast<ChangeSelectedObject>(pPreview);
	return S_OK;
}

HRESULT CLevel_Effect::Ready_Preset()
{
	MAPFILE_DATA loadpresetData = {};
	m_pGameInstance->Load_MapData(g_wszMeshEffectPresetPath, loadpresetData);
	size_t iSize = loadpresetData.meshEffectPresets.size();
	if (iSize <= 0)
		return E_FAIL;
	
	_int iMaxID = -1;
	m_umapPresets.reserve(iSize);
	for (size_t i = 0; i < iSize; ++i)
	{
		EFFECT_PRESET preset = {};
		preset.iPressetID = loadpresetData.meshEffectPresets[i].iPresetID;
		preset.strName = loadpresetData.meshEffectPresets[i].strName;
		preset.snapShot = loadpresetData.meshEffectPresets[i].snapShot;
		m_umapPresets[preset.iPressetID] = preset;
		if (iMaxID < (_int)preset.iPressetID)
			iMaxID = (_int)preset.iPressetID;
	}

	m_iNextPresetID = iMaxID + 1;
	return S_OK;
}

HRESULT CLevel_Effect::Ready_Camera(const wstring& wstrLayerTag)
{
	{
		CGameObject* pResult = { nullptr };
		CCameraMan::GAMEOBJECT_DESC goDesc = {};
		CTransform::TRANSFORM_DESC TransformDesc = {};
		TransformDesc.vPosition = { 1.f, 1.f, -1.f };
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
		if (!(pResult = m_pGameInstance->Add_GameObject(ENUM_TO_UINT(LEVELID::STATIC),
			L"Prototype_GameObject_CameraManFree",
			ENUM_TO_UINT(LEVELID::EFFECT),
			wstrLayerTag, &goDesc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Effect::Ready_Terrain(const wstring& wstrLayerTag)
{
	{
		CToolObject::TOOLOBJECT_DESC goDesc = {};
		CTransform::TRANSFORM_DESC TransformDesc = {};
		TransformDesc.vPosition = { -128.f, -10.f, -128.f };

		goDesc.iLevelIndex = ENUM_TO_UINT(LEVELID::EFFECT);
		goDesc.wstrLayerTag = wstrLayerTag;
		goDesc.pTransform_Desc = &TransformDesc;
		if (!(m_pGameInstance->Add_GameObject(ENUM_TO_UINT(LEVELID::EFFECT),
			L"Prototype_GameObject_Terrain",
			ENUM_TO_UINT(LEVELID::EFFECT),
			wstrLayerTag, &goDesc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Effect::Ready_Lights()
{
	{
		LIGHT_DESC desc = {};
		desc.eType = LIGHT_TYPE::DIRECTIONAL;
		desc.vDirection = _float3(1.f, -1.f, 1.f);
		desc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
		desc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
		desc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

		if (FAILED(m_pGameInstance->Add_Light(desc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Effect::Ready_Gui()
{
	m_GuiElements[ENUM_TO_UINT(Elements::Inspector)] = CImGui_Inspector_Effect::Create(m_pDevice, this, m_pDeviceContext);
	m_GuiElements[ENUM_TO_UINT(Elements::PresetBrowser)] = CImGui_PresetBrowser_Effect::Create(this, m_pDevice, m_pDeviceContext);
	return S_OK;
}

HRESULT CLevel_Effect::Ready_CameraSetting(const _uint iLevelID)
{
	CGameObject* pFreeCamera = m_pGameInstance->Get_GameObject_Back(iLevelID, L"Camera_Layer");
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
	m_EventHandles[ENUM_TO_UINT(Event::ChangeSelectedObject)] =
		m_pGameInstance->Subscribe<ChangeSelectedObject>(this, &CLevel_Effect::On_ChangeSelectedObject);

	m_EventHandles[ENUM_TO_UINT(Event::PresetSelected)] =
		m_pGameInstance->Subscribe<PresetSelected>(this, &CLevel_Effect::On_PresetSelected);
	m_EventHandles[ENUM_TO_UINT(Event::PresetDeleteRequested)] =
		m_pGameInstance->Subscribe<PresetDeleteRequested>(this, &CLevel_Effect::On_PresetDeleteRequested);
	m_EventHandles[ENUM_TO_UINT(Event::PresetCreateRequested)] =
		m_pGameInstance->Subscribe<PresetCreateRequested>(this, &CLevel_Effect::On_PresetCreateRequested);
	m_EventHandles[ENUM_TO_UINT(Event::PresetApplyRequested)] =
		m_pGameInstance->Subscribe<PresetApplyRequested>(this, &CLevel_Effect::On_PresetApplyRequested);
	m_EventHandles[ENUM_TO_UINT(Event::PresetApplyAndCreateRequested)] =
		m_pGameInstance->Subscribe<PresetApplyAndCreateRequested>(this, &CLevel_Effect::On_PresetApplyAndCreateRequested);
	m_EventHandles[ENUM_TO_UINT(Event::PresetDuplicateRequested)] =
		m_pGameInstance->Subscribe<PresetDuplicateRequested>(this, &CLevel_Effect::On_PresetDuplicateRequested);

	m_EventHandles[ENUM_TO_UINT(Event::PreviewAddRequested)] =
		m_pGameInstance->Subscribe<PreviewAddRequested>(this, &CLevel_Effect::On_PreviewAddRequested);
	m_EventHandles[ENUM_TO_UINT(Event::PreviewDuplicateRequested)] =
		m_pGameInstance->Subscribe<PreviewDuplicateRequested>(this, &CLevel_Effect::On_PreviewDuplicateRequested);
	m_EventHandles[ENUM_TO_UINT(Event::PreviewDeleteRequested)] =
		m_pGameInstance->Subscribe<PreviewDeleteRequested>(this, &CLevel_Effect::On_PreviewDeleteRequested);
	m_EventHandles[ENUM_TO_UINT(Event::PreviewModelChangeRequested)] =
		m_pGameInstance->Subscribe<PreviewModelChangeRequested>(this, &CLevel_Effect::On_PreviewModelChangeRequested);
}

void CLevel_Effect::Release_Event()
{
	m_pGameInstance->Unsubscribe<ChangeSelectedObject>(m_EventHandles[ENUM_TO_UINT(Event::ChangeSelectedObject)]);

	m_pGameInstance->Unsubscribe<PresetSelected>(m_EventHandles[ENUM_TO_UINT(Event::PresetSelected)]);
	m_pGameInstance->Unsubscribe<PresetDeleteRequested>(m_EventHandles[ENUM_TO_UINT(Event::PresetDeleteRequested)]);
	m_pGameInstance->Unsubscribe<PresetCreateRequested>(m_EventHandles[ENUM_TO_UINT(Event::PresetCreateRequested)]);
	m_pGameInstance->Unsubscribe<PresetApplyRequested>(m_EventHandles[ENUM_TO_UINT(Event::PresetApplyRequested)]);
	m_pGameInstance->Unsubscribe<PresetApplyAndCreateRequested>(m_EventHandles[ENUM_TO_UINT(Event::PresetApplyAndCreateRequested)]);
	m_pGameInstance->Unsubscribe<PresetDuplicateRequested>(m_EventHandles[ENUM_TO_UINT(Event::PresetDuplicateRequested)]);

	m_pGameInstance->Unsubscribe<PreviewAddRequested>(m_EventHandles[ENUM_TO_UINT(Event::PreviewAddRequested)]);
	m_pGameInstance->Unsubscribe<PreviewDuplicateRequested>(m_EventHandles[ENUM_TO_UINT(Event::PreviewDuplicateRequested)]);
	m_pGameInstance->Unsubscribe<PreviewDeleteRequested>(m_EventHandles[ENUM_TO_UINT(Event::PreviewDeleteRequested)]);
	m_pGameInstance->Unsubscribe<PreviewModelChangeRequested>(m_EventHandles[ENUM_TO_UINT(Event::PreviewModelChangeRequested)]);
}

void CLevel_Effect::On_ChangeSelectedObject(CGameObject* pGo)
{
	if (m_pSelectedObject == pGo)
		return;

	if (pGo)
	{
		if (CToolObject* pToolGo = dynamic_cast<CToolObject*>(pGo))
		{
			m_pSelectedObject = pToolGo;
			if (CSkillPreviewMesh * pPreview = dynamic_cast<CSkillPreviewMesh*>(pToolGo))
			{
				m_iSelectedPresetID = pPreview->Get_PresetID();
			}
			return;
		}
	}
	if (!ImGuizmo::IsOver() && !ImGuizmo::IsUsing())
		m_pSelectedObject = nullptr;
}

void CLevel_Effect::On_PresetSelected(_uint iPresetID)
{
	if (Get_Preset(iPresetID) == nullptr)
		return;

	m_iSelectedPresetID = iPresetID;
}

void CLevel_Effect::On_PresetDeleteRequested(_uint iPresetID)
{
	if (iPresetID == 0)
		return;

	if (Get_Preset(iPresetID) == nullptr)
		return;

	auto itr = std::find_if(m_vecPreviewMeshes.begin(), m_vecPreviewMeshes.end(),
		[iPresetID](CToolObject* pToolObject)->bool
		{
			if (CSkillPreviewMesh* pPreview = dynamic_cast<CSkillPreviewMesh*>(pToolObject))
			{
				return pPreview->Get_PresetID() == iPresetID;
			}
			return false;
		});

	if (itr != m_vecPreviewMeshes.end())
		return;

	m_umapPresets.erase(iPresetID);
	m_bFileDirty = true;
}

void CLevel_Effect::On_PresetCreateRequested()
{
	_uint iNewID = Create_NewPreset();
	m_iSelectedPresetID = iNewID;
	m_bFileDirty = true;
}

// UI에서 작성된 Preset 수정 및 적용
// 이 후 해당 Peset을 참조하고있던 Mesh들 모두 수정
void CLevel_Effect::On_PresetApplyRequested(CGameObject* pTarget, _uint iPresetID, EFFECT_PRESET_SNAPSHOT snapShot)
{
	if (EFFECT_PRESET* pPreset = Get_Preset(iPresetID))
		pPreset->snapShot = snapShot;
	else
		return;

	m_iSelectedPresetID = iPresetID;

	if (pTarget)
	{
		if (CSkillPreviewMesh* pPreview = dynamic_cast<CSkillPreviewMesh*>(pTarget))
		{
			if (pPreview->Get_PresetID() != iPresetID)
				pPreview->Set_PresetID(iPresetID);
		}
	}

	for (auto& pPreview : m_vecPreviewMeshes)
	{
		if (pPreview == nullptr)
			continue;

		if (static_cast<CSkillPreviewMesh*>(pPreview)->Get_PresetID() == iPresetID)
		{
			if (CMonoBehaviour* pOtherRetunred = pPreview->Get_Script_Component(L"PresetReceiver"))
			{
				static_cast<CPresetReceiver*>(pOtherRetunred)->Queue(snapShot);
			}
		}
	}

	m_bFileDirty = true;
}

// UI에서 작성된 Preset을 기존 Preset에 덮어쓰지 않고 새로 생성
void CLevel_Effect::On_PresetApplyAndCreateRequested(CGameObject* pTarget, EFFECT_PRESET_SNAPSHOT snapShot)
{
	CSkillPreviewMesh* pPreview = { nullptr };

	_uint iNewID = Create_NewPreset();
	EFFECT_PRESET* pCurrentPreset = Get_Preset(iNewID);
	pCurrentPreset->snapShot = snapShot;

	if (pTarget)
	{
		if (!(pPreview = dynamic_cast<CSkillPreviewMesh*>(pTarget)))
			return;
	}
	else
		return;

	pPreview->Set_PresetID(iNewID);

	CMonoBehaviour* pReturned = { nullptr };
	if (!(pReturned = pPreview->Get_Script_Component(L"PresetReceiver")))
		return;

	CPresetReceiver* pReceiver = static_cast<CPresetReceiver*>(pReturned);
	pReceiver->Queue(m_umapPresets[iNewID].snapShot);

	m_iSelectedPresetID = iNewID;
	m_bFileDirty = true;
}

// Preview에 영향없이 그냥 새 Preset 복제 생성
void CLevel_Effect::On_PresetDuplicateRequested(_uint iSrcPresetID)
{
	EFFECT_PRESET* pSrcPreset = Get_Preset(iSrcPresetID);
	_uint iNewID = Create_NewPreset();
	EFFECT_PRESET* pCurrentPreset = Get_Preset(iNewID);
	if (pSrcPreset)
		pCurrentPreset->snapShot = pSrcPreset->snapShot;
}

void CLevel_Effect::On_PreviewAddRequested()
{
	CSkillPreviewMesh::SM_DESC desc = {};
	desc.eRenderCategory = RENDER_CATEGORY::NONELIGHT;
	desc.wstrLayerTag = g_wszMeshPreviewLayer;
	desc.wstrModelPrototypeTag = L"Prototype_Component_Model_Sphere";
	desc.wstrShaderPrototypeTag = L"Prototype_Component_Shader_VtxMesh_SkillEffect";
	desc.wstrMaterialTag = L"Material_Default";
	desc.iPass = 1;

	if (CToolObject* pNew = Spawn_PreviewMesh(&desc))
	{
		if (CSkillPreviewMesh* pMesh = dynamic_cast<CSkillPreviewMesh*>(pNew))
		{
			m_vecPreviewMeshes.push_back(pNew);
			m_pSelectedObject = pNew;
			m_pGameInstance->Broadcast<ChangeSelectedObject>(pNew);
		}
	}

	m_bFileDirty = true;
}

void CLevel_Effect::On_PreviewDuplicateRequested(CGameObject* pTarget)
{
	if (pTarget == nullptr)
		return;

	CSkillPreviewMesh* pPreview = dynamic_cast<CSkillPreviewMesh*>(pTarget);
	if (pPreview == nullptr)
		return;

	const CSkillPreviewMesh::SM_DESC& gettedDesc = pPreview->Get_Desc();
	CSkillPreviewMesh::SM_DESC desc = {};
	desc.wstrLayerTag = g_wszMeshPreviewLayer;
	desc.eRenderCategory = gettedDesc.eRenderCategory;
	desc.iPass = gettedDesc.iPass;
	desc.wstrModelPrototypeTag = gettedDesc.wstrModelPrototypeTag;
	desc.wstrMaterialTag = gettedDesc.wstrMaterialTag;
	desc.wstrShaderPrototypeTag = gettedDesc.wstrShaderPrototypeTag;
	desc.wstrPatternTextureTag = gettedDesc.wstrPatternTextureTag;
	desc.wstrDissolveNoiseTextureTag = gettedDesc.wstrDissolveNoiseTextureTag;
	desc.wstrDistortionNoiseTextureTag = gettedDesc.wstrDistortionNoiseTextureTag;
	desc.wstrGradationTextureTag = gettedDesc.wstrGradationTextureTag;
	desc.wstrGradationMapTextureTag = gettedDesc.wstrGradationMapTextureTag;
	desc.wstrPatternTextureTag = gettedDesc.wstrPatternTextureTag;

	if (CToolObject* pNew = Spawn_PreviewMesh(&desc))
	{
		if (CSkillPreviewMesh* pMesh = dynamic_cast<CSkillPreviewMesh*>(pNew))
		{
			_uint iDestPresetID = pPreview->Get_PresetID();
			pMesh->Set_PresetID(iDestPresetID);

			m_vecPreviewMeshes.push_back(pMesh);
			m_pSelectedObject = pMesh;
			m_pGameInstance->Broadcast<ChangeSelectedObject>(pMesh);
		}
	}
}

void CLevel_Effect::On_PreviewDeleteRequested(CGameObject* pTarget)
{
	if (pTarget == nullptr)
		return;

	CToolObject* pToolObject = dynamic_cast<CToolObject*>(pTarget);
	if (pToolObject == nullptr)
		return;

	auto itr = std::find(m_vecPreviewMeshes.begin(), m_vecPreviewMeshes.end(), pToolObject);
	if (itr != m_vecPreviewMeshes.end())
		m_vecPreviewMeshes.erase(itr);

	if (m_pSelectedObject == pToolObject)
		m_pSelectedObject = nullptr;

	pToolObject->Set_Dead(g_wszMeshPreviewLayer);
	m_pGameInstance->Broadcast<ChangeSelectedObject>(m_pSelectedObject);
}

void CLevel_Effect::On_PreviewModelChangeRequested(CGameObject* pTarget, string strModelTag)
{
	CSkillPreviewMesh* pOldPreview = dynamic_cast<CSkillPreviewMesh*>(pTarget);
	if (pOldPreview == nullptr)
		return;
	CSkillPreviewMesh::SM_DESC desc = {};
	desc = pOldPreview->Get_Desc();
	desc.wstrLayerTag = g_wszMeshPreviewLayer;
	desc.eRenderCategory = RENDER_CATEGORY::NONELIGHT;
	desc.wstrModelPrototypeTag = Engine_Utils::ToWString(strModelTag);
	CSkillPreviewMesh* pNewPreview = static_cast<CSkillPreviewMesh*>(Spawn_PreviewMesh(&desc));
	if (pNewPreview == nullptr)
		return;

	pNewPreview->Set_PresetID(pOldPreview->Get_PresetID());

	CTransform* pNewPreviewTransform = pNewPreview->Get_Component<CTransform>();
	CTransform* pOldPreviewTransform = pOldPreview->Get_Component<CTransform>();

	pNewPreviewTransform->Set_Scale(pOldPreviewTransform->Get_Scaled());
	pNewPreviewTransform->Set_Info(TRANSFORM_INFO_STATE::RIGHT, ::XMVector3Normalize(pOldPreviewTransform->Get_Info(TRANSFORM_INFO_STATE::RIGHT)));
	pNewPreviewTransform->Set_Info(TRANSFORM_INFO_STATE::UP, ::XMVector3Normalize(pOldPreviewTransform->Get_Info(TRANSFORM_INFO_STATE::UP)));
	pNewPreviewTransform->Set_Info(TRANSFORM_INFO_STATE::LOOK, ::XMVector3Normalize(pOldPreviewTransform->Get_Info(TRANSFORM_INFO_STATE::LOOK)));
	pNewPreviewTransform->Set_Info(TRANSFORM_INFO_STATE::POS, pOldPreviewTransform->Get_Info(TRANSFORM_INFO_STATE::POS));

	_uint iDestPresetID = pOldPreview->Get_PresetID();

	CMonoBehaviour* pReturned = { nullptr };
	if (!(pReturned = pNewPreview->Get_Script_Component(L"PresetReceiver")))
	{
		pNewPreview->Set_Dead(g_wszMeshPreviewLayer);
		return;
	}
		
	CPresetReceiver* pReceiver = static_cast<CPresetReceiver*>(pReturned);
	pReceiver->Queue(m_umapPresets[iDestPresetID].snapShot);

	pNewPreview->Set_PresetID(iDestPresetID);

	auto itr = std::find(m_vecPreviewMeshes.begin(), m_vecPreviewMeshes.end(), pOldPreview);
	if (itr != m_vecPreviewMeshes.end())
		m_vecPreviewMeshes.erase(itr);
	pOldPreview->Set_Dead(g_wszMeshPreviewLayer);
	m_vecPreviewMeshes.push_back(pNewPreview);
	m_pGameInstance->Broadcast<ChangeSelectedObject>(pNewPreview);
}

CToolObject* CLevel_Effect::Spawn_PreviewMesh(void* pArg)
{
	CGameObject* pReturn = m_pGameInstance->Add_GameObject(
		ENUM_TO_UINT(LEVELID::EFFECT),
		L"Prototype_GameObject_SkillPreviewMesh",
		ENUM_TO_UINT(LEVELID::EFFECT),
		g_wszMeshPreviewLayer,
		pArg);

	if (pReturn == nullptr)
		return nullptr;

	CToolObject* pToolObject = dynamic_cast<CToolObject*>(pReturn);
	if (pToolObject == nullptr)
	{
		Safe_Release(pReturn);
		return nullptr;
	}

	pToolObject->Set_Visible();
	return pToolObject;
}

EFFECT_PRESET* CLevel_Effect::Get_Preset(_uint iPresetID)
{
	auto itr = m_umapPresets.find(iPresetID);
	if (itr == m_umapPresets.end())
		return nullptr;
	return &itr->second;
}

_uint CLevel_Effect::Create_NewPreset(const wstring& wstrTag)
{
	EFFECT_PRESET preset;
	preset.iPressetID = Allocate_PresetID();
	preset.strName = wstrTag.empty() ? ("Preset_" + std::to_string(preset.iPressetID)) : Engine_Utils::ToString(wstrTag);

	m_umapPresets.emplace(preset.iPressetID, preset);
	Select_Preset(preset.iPressetID);
	return preset.iPressetID;
}

_uint CLevel_Effect::Duplicate_Preset(_uint iSrcID)
{
	auto itr = m_umapPresets.find(iSrcID);
	if( itr == m_umapPresets.end())
		return 0;

	EFFECT_PRESET tCopy = itr->second;
	tCopy.iPressetID = Allocate_PresetID();
	tCopy.strName += "_Copy";

	m_umapPresets.emplace(tCopy.iPressetID, tCopy);
	Select_Preset(tCopy.iPressetID);
	return tCopy.iPressetID;
}

void CLevel_Effect::Save_PresetData(MAPFILE_DATA& data)
{
	size_t iSize = m_umapPresets.size();
	data.meshEffectPresets.reserve(iSize);
	for (const auto& Element : m_umapPresets)
	{
		MESHEEFFECT_PRESET_SAVEDATA presetData = {};
		presetData.iPresetID = Element.second.iPressetID;
		presetData.snapShot = Element.second.snapShot;
		presetData.strName = Element.second.strName;
		data.meshEffectPresets.push_back(presetData);
	}
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
