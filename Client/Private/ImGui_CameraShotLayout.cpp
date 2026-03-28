#include "pch.h"
#include "ImGui_CameraShotLayout.h"
#include "CameraMan_Targeter.h"
#include "CameraShotPrestBuilder.h"
#include "CameraPreset_Manager.h"
#include "shobjidl.h"
#include "GameInstance.h"

CImGui_CameraShotLayout::CImGui_CameraShotLayout()
	: Super("CameraShot")
{
}

HRESULT CImGui_CameraShotLayout::Initialize()
{
	New_Document();
	Refresh_PresetList();
	return S_OK;
}

CImGui_CameraShotLayout* CImGui_CameraShotLayout::Create()
{
	CImGui_CameraShotLayout* pInstance = new CImGui_CameraShotLayout();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("CImGui_CameraShotLayout::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CImGui_CameraShotLayout::Free()
{
	Super::Free();
}


// 채널 목록 중앙 관리
vector<std::pair<const char*, CAMERA_SHOT_CHANNEL_1D*>>
CImGui_CameraShotLayout::Get_AllChannels()
{
	auto& p = m_tDoc.tShotDesc.Pivot;
	return {
		{ "PivotOffsetX",  &p.PivotOffsetX  },
		{ "PivotOffsetY",  &p.PivotOffsetY  },
		{ "PivotOffsetZ",  &p.PivotOffsetZ  },
		{ "LookAtOffsetX", &p.LookAtOffsetX },
		{ "LookAtOffsetY", &p.LookAtOffsetY },
		{ "LookAtOffsetZ", &p.LookAtOffsetZ },
		{ "LocalX",        &p.LocalX        },
		{ "LocalY",        &p.LocalY        },
		{ "LocalZ",        &p.LocalZ        },
		{ "OrbitYawDeg",   &p.OrbitYawDeg   },
	};
}

void CImGui_CameraShotLayout::Render(CGameObject* pGo)
{
	Render_PresetSection();
	Render_PlaybackSection();
	Render_BindingSection();

	Render_ShotStartSection();
	Render_ShotStartActionSection();
	Render_ShotPivotBasicSection();
	Render_ShotChannelSection();
	Render_ShotLookAtSection();
	Render_ShotRecoverSection();
	Render_ShotFinishActionSection();

	Render_RuntimeSection();
}

// 문서 관리
void CImGui_CameraShotLayout::New_Document()
{
	m_tDoc = {};
	m_tDoc.strName = "NewPreset";
	m_tDoc.tShotDesc.strName = m_tDoc.strName;

	m_strSelectedPresetTag.clear();
	m_iSelectedPresetIndex = -1;
	m_szPresetPath[0] = '\0';

	m_bPause = false;
	m_bDirty = false;
}

void CImGui_CameraShotLayout::Reset_ToPlayerDefault()
{
	m_tDoc.tShotDesc = CCameraShotPrestBuilder::Make_DebugPlain5SecShot();
	m_tDoc.tBinding = CCameraShotPrestBuilder::Make_DebugPlayerBinding();

	m_tDoc.strName = "PlayerDefault";
	m_tDoc.tShotDesc.strName = m_tDoc.strName;
	m_strSelectedPresetTag = m_tDoc.strName;

	m_bPause = false;
	m_bDirty = true;

	Refresh_DocAfterLoad();
}

void CImGui_CameraShotLayout::Reset_ToBossDefault()
{
	m_tDoc.tShotDesc = CCameraShotPrestBuilder::Make_DebugBossPlain5SecShot();

	Engine::CGameObject* pBoss = Get_DebugBoss();
	if (pBoss)
		m_tDoc.tBinding = CCameraShotPrestBuilder::Make_DebugBossBinding(pBoss);

	m_tDoc.strName = "BossDefault";
	m_tDoc.tShotDesc.strName = m_tDoc.strName;
	m_strSelectedPresetTag = m_tDoc.strName;

	m_bPause = false;
	m_bDirty = true;

	Refresh_DocAfterLoad();
}

CAMERA_SHOT_PRESET CImGui_CameraShotLayout::Build_PresetFromDoc() const
{
	CAMERA_SHOT_PRESET preset = {};
	preset.strPresetTag = m_tDoc.strName;
	preset.tShotDesc = m_tDoc.tShotDesc;
	preset.tBinding = m_tDoc.tBinding;
	preset.tShotDesc.strName = preset.strPresetTag;
	return preset;
}

void CImGui_CameraShotLayout::Apply_DocFromPreset(const CAMERA_SHOT_PRESET& preset)
{
	m_tDoc = {};
	m_tDoc.strName = preset.strPresetTag;
	m_tDoc.tShotDesc = preset.tShotDesc;
	m_tDoc.tBinding = preset.tBinding;
	m_strSelectedPresetTag = m_tDoc.strName;

	if (m_tDoc.tShotDesc.strName.empty())
		m_tDoc.tShotDesc.strName = m_tDoc.strName;

	Refresh_DocAfterLoad();
	m_bDirty = false;
}

void CImGui_CameraShotLayout::Refresh_DocAfterLoad()
{
	// Get_AllChannels로 채널관리 중이라 이 늘어나도 이 함수는 수정 불필요
	const _float fDuration = m_tDoc.tShotDesc.Pivot.fDuration;

	for (auto& [label, pChannel] : Get_AllChannels())
	{
		Ensure_ChannelKeyUIDs(*pChannel);
		Clamp_ChannelToDuration(*pChannel, fDuration);
	}
}

// 파일 IO
void CImGui_CameraShotLayout::Save_Document()
{
	CCameraPreset_Manager* pCameraManager = CCameraPreset_Manager::GetInstance();
	if (pCameraManager == nullptr)
		return;

	CAMERA_SHOT_PRESET preset = Build_PresetFromDoc();
	if (preset.strPresetTag.empty())
		return;

	// 파일명은 PresetTag로 고정
	_char szDir[MAX_PATH] = {};
	BrowseFolderDialog(szDir);
	if (szDir[0] == '\0')
		return;

	std::filesystem::path filePath =
		std::filesystem::path(szDir) / (preset.strPresetTag + ".json");

	if (FAILED(pCameraManager->Register_Preset(preset)))
		return;

	if (FAILED(pCameraManager->Save_PresetFile(preset, filePath)))
		return;

	m_strSelectedPresetTag = preset.strPresetTag;
	strcpy_s(m_szPresetPath, filePath.string().c_str());

	Refresh_PresetList();
	m_bDirty = false;
}

void CImGui_CameraShotLayout::Load_Document()
{
	if (m_strSelectedPresetTag.empty())
		return;

	CCameraPreset_Manager* pCameraManager = CCameraPreset_Manager::GetInstance();
	if (pCameraManager == nullptr)
		return;

	const CAMERA_SHOT_PRESET* pPreset = pCameraManager->Find_Preset(m_strSelectedPresetTag);
	if (pPreset == nullptr)
		return;

	Apply_DocFromPreset(*pPreset);
}

void CImGui_CameraShotLayout::Load_DocumentFromFile()
{
	CCameraPreset_Manager* pCameraManager = CCameraPreset_Manager::GetInstance();
	if (pCameraManager == nullptr)
		return;

	_char szPath[MAX_PATH] = {};
	OpenFileDialog(szPath, "Json Files\0*.json\0All Files\0*.*\0");
	if (szPath[0] == '\0')
		return;

	std::filesystem::path filePath = szPath;

	if (FAILED(pCameraManager->Load_PresetFile(filePath)))
		return;

	// 파일명은 PresetTag로 고정
	const string strTag = filePath.stem().string();
	m_strSelectedPresetTag = strTag;
	strcpy_s(m_szPresetPath, filePath.string().c_str());

	Refresh_PresetList();

	const CAMERA_SHOT_PRESET* pPreset = pCameraManager->Find_Preset(strTag);
	if (pPreset == nullptr)
		return;

	Apply_DocFromPreset(*pPreset);
}

void CImGui_CameraShotLayout::Delete_CurrentPreset()
{
	if (m_strSelectedPresetTag.empty())
		return;

	CCameraPreset_Manager* pManager = CCameraPreset_Manager::GetInstance();
	if (pManager == nullptr)
		return;

	if (FAILED(pManager->Unregister_Preset(m_strSelectedPresetTag)))
		return;

	if (m_tDoc.strName == m_strSelectedPresetTag)
		New_Document();

	m_strSelectedPresetTag.clear();
	m_szPresetPath[0] = '\0';

	Refresh_PresetList();
}

void CImGui_CameraShotLayout::Refresh_PresetList()
{
	m_vecPresetTags.clear();
	m_iSelectedPresetIndex = -1;

	CCameraPreset_Manager* pManager = CCameraPreset_Manager::GetInstance();
	if (pManager == nullptr)
		return;

	m_vecPresetTags = pManager->Get_PresetTags();

	for (_uint i = 0; i < m_vecPresetTags.size(); ++i)
	{
		if (m_vecPresetTags[i] == m_strSelectedPresetTag)
		{
			m_iSelectedPresetIndex = static_cast<_int>(i);
			break;
		}
	}
}

// Dialog
void CImGui_CameraShotLayout::OpenFileDialog(_char* pBuffer, const _char* pFilter)
{
	OPENFILENAMEA ofn = {};
	char szFile[MAX_PATH] = {};

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = g_hWnd;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = pFilter;
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

	if (GetOpenFileNameA(&ofn) == TRUE)
		strcpy_s(pBuffer, MAX_PATH, ofn.lpstrFile);
}

void CImGui_CameraShotLayout::BrowseFolderDialog(_char* pBuffer)
{
	IFileDialog* pDialog = nullptr;
	if (FAILED(CoCreateInstance(CLSID_FileOpenDialog, nullptr,
		CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pDialog))))
		return;

	// 폴더 선택 모드
	DWORD dwOptions = 0;
	pDialog->GetOptions(&dwOptions);
	pDialog->SetOptions(dwOptions | FOS_PICKFOLDERS | FOS_FORCEFILESYSTEM);

	if (SUCCEEDED(pDialog->Show(g_hWnd)))
	{
		IShellItem* pItem = nullptr;
		if (SUCCEEDED(pDialog->GetResult(&pItem)))
		{
			PWSTR pszPath = nullptr;
			if (SUCCEEDED(pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszPath)))
			{
				// wchar -> char 변환
				WideCharToMultiByte(CP_ACP, 0, pszPath, -1,
					pBuffer, MAX_PATH, nullptr, nullptr);
				CoTaskMemFree(pszPath);
			}
			pItem->Release();
		}
	}

	pDialog->Release();
}

// 바인딩 유틸
void CImGui_CameraShotLayout::Apply_PlayerPreviewBinding()
{
	m_tDoc.tBinding.Pivot.eSource = Engine::ECameraAnchorSource::ACTOR;
	m_tDoc.tBinding.Pivot.pObject = nullptr;

	if (m_tDoc.tBinding.bUseSeparateLookAt)
	{
		m_tDoc.tBinding.LookAt.eSource = Engine::ECameraAnchorSource::ACTOR;
		m_tDoc.tBinding.LookAt.pObject = nullptr;
	}
}

void CImGui_CameraShotLayout::Apply_BossPreviewBinding()
{
	Engine::CGameObject* pBoss = Get_DebugBoss();
	if (pBoss == nullptr)
		return;

	m_tDoc.tBinding.Pivot.eSource = Engine::ECameraAnchorSource::OBJECT;
	m_tDoc.tBinding.Pivot.pObject = pBoss;

	if (m_tDoc.tBinding.bUseSeparateLookAt)
	{
		m_tDoc.tBinding.LookAt.eSource = Engine::ECameraAnchorSource::OBJECT;
		m_tDoc.tBinding.LookAt.pObject = pBoss;
	}
}

void CImGui_CameraShotLayout::Sync_LookAtBindingFromPivot()
{
	m_tDoc.tBinding.LookAt.eSource = m_tDoc.tBinding.Pivot.eSource;
	m_tDoc.tBinding.LookAt.eResolve = m_tDoc.tBinding.Pivot.eResolve;
	m_tDoc.tBinding.LookAt.iPartIndex = m_tDoc.tBinding.Pivot.iPartIndex;
	m_tDoc.tBinding.LookAt.pObject = m_tDoc.tBinding.Pivot.pObject;
	m_tDoc.tBinding.LookAt.vLocalOffset = m_tDoc.tBinding.Pivot.vLocalOffset;
	m_tDoc.tBinding.LookAt.strAnchorTag = m_tDoc.tBinding.Pivot.strAnchorTag;
}

// 채널 유틸
void CImGui_CameraShotLayout::Sort_Channel(OUT CAMERA_SHOT_CHANNEL_1D& outChannel)
{
	std::sort(outChannel.vecKeys.begin(), outChannel.vecKeys.end(),
		[](const Engine::CAMERA_SHOT_KEY_1D& a, const Engine::CAMERA_SHOT_KEY_1D& b)
		{
			return a.fTime < b.fTime;
		});
}

void CImGui_CameraShotLayout::Ensure_ChannelKeyUIDs(OUT CAMERA_SHOT_CHANNEL_1D& outChannel)
{
	for (auto& key : outChannel.vecKeys)
	{
		if (key.iEditorUID == 0)
			key.iEditorUID = m_iNextChannelKeyUID++;
	}
}

void CImGui_CameraShotLayout::Clamp_ChannelToDuration(
	CAMERA_SHOT_CHANNEL_1D& refChannel, _float fDuration)
{
	for (auto& key : refChannel.vecKeys)
		key.fTime = std::clamp(key.fTime, 0.f, fDuration);
}

CCameraMan_Targeter* CImGui_CameraShotLayout::Get_Targeter() const
{
	CGameObject* pCameraMan = m_pGameInstance->Get_MainCamera();
	if (pCameraMan == nullptr)
		return nullptr;

	return dynamic_cast<CCameraMan_Targeter*>(pCameraMan);
}

CGameObject* CImGui_CameraShotLayout::Get_DebugBoss() const
{
	return m_pGameInstance->Get_GameObject(
		m_pGameInstance->Get_CurrentLevelIndex(),
		g_wszBossLayer,
		m_iBossIndex);
}

// Render Preset
void CImGui_CameraShotLayout::Render_PresetSection()
{
	if (!ImGui::CollapsingHeader("Preset##Preset", ImGuiTreeNodeFlags_DefaultOpen))
		return;

	// 이름 편집
	_char szName[MAX_NAME] = {};
	strncpy_s(szName, m_tDoc.strName.c_str(), _TRUNCATE);
	if (ImGui::InputText("Preset Name##Preset", szName, IM_ARRAYSIZE(szName)))
	{
		m_tDoc.strName = szName;
		m_tDoc.tShotDesc.strName = m_tDoc.strName;
		m_bDirty = true;
	}

	// Default 로드
	if (ImGui::Button("New##Preset"))                     New_Document();
	ImGui::SameLine();
	if (ImGui::Button("Load Player Default##Preset"))     Reset_ToPlayerDefault();
	ImGui::SameLine();
	if (ImGui::Button("Load Boss Default##Preset"))       Reset_ToBossDefault();

	ImGui::Separator();

	// 프리셋 목록
	if (ImGui::Button("Refresh##Preset"))
		Refresh_PresetList();

	string strPreview = "<None>";
	if (m_iSelectedPresetIndex >= 0 &&
		m_iSelectedPresetIndex < static_cast<_int>(m_vecPresetTags.size()))
		strPreview = m_vecPresetTags[m_iSelectedPresetIndex];

	if (ImGui::BeginCombo("Preset List##Preset", strPreview.c_str()))
	{
		for (_uint i = 0; i < m_vecPresetTags.size(); ++i)
		{
			const bool bSelected = (m_iSelectedPresetIndex == static_cast<_int>(i));
			if (ImGui::Selectable(m_vecPresetTags[i].c_str(), bSelected))
			{
				m_iSelectedPresetIndex = static_cast<_int>(i);
				m_strSelectedPresetTag = m_vecPresetTags[i];
			}
			if (bSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	ImGui::Separator();

	// 파일 IO
	if (ImGui::Button("Save As...##Preset"))        Save_Document();
	ImGui::SameLine();
	if (ImGui::Button("Load From File...##Preset")) Load_DocumentFromFile();
	ImGui::SameLine();
	if (ImGui::Button("Load Selected##Preset"))     Load_Document();
	ImGui::SameLine();
	if (ImGui::Button("Delete Selected##Preset"))   Delete_CurrentPreset();

	ImGui::Spacing();
	ImGui::Text("Selected Tag : %s", m_strSelectedPresetTag.empty() ? "<None>" : m_strSelectedPresetTag.c_str());
	ImGui::Text("Last Path    : %s", m_szPresetPath[0] == '\0' ? "<None>" : m_szPresetPath);
	ImGui::Text("Dirty        : %s", m_bDirty ? "YES" : "NO");
}

// Render Playback
void CImGui_CameraShotLayout::Render_PlaybackSection()
{
	if (!ImGui::CollapsingHeader("Playback##Playback", ImGuiTreeNodeFlags_DefaultOpen))
		return;

	ImGui::InputInt("Boss Index##Playback", &m_iBossIndex);
	if (m_iBossIndex < 0)
		m_iBossIndex = 0;

#ifdef _DEBUG
	Client::CCameraMan_Targeter* pTargeter = Get_Targeter();

	if (ImGui::Button("Play Player Preview##Playback"))
	{
		Apply_PlayerPreviewBinding();
		if (pTargeter)
			pTargeter->Debug_PlayScriptedShot(m_tDoc.tShotDesc, m_tDoc.tBinding);
	}
	ImGui::SameLine();
	if (ImGui::Button("Play Boss Preview##Playback"))
	{
		Apply_BossPreviewBinding();
		if (pTargeter)
			pTargeter->Debug_PlayScriptedShot(m_tDoc.tShotDesc, m_tDoc.tBinding);
	}

	ImGui::Separator();

	if (ImGui::Button("Stop##Playback"))
	{
		if (pTargeter)
			pTargeter->Debug_StopScriptedShot();
	}
	ImGui::SameLine();
	if (ImGui::Button(m_bPause ? "Resume##Playback" : "Pause##Playback"))
	{
		m_bPause = !m_bPause;
		if (pTargeter)
			pTargeter->Debug_SetScriptedShotPause(m_bPause);
	}
#endif
}

// Render Binding
void CImGui_CameraShotLayout::Render_BindingSection()
{
	if (!ImGui::CollapsingHeader("Binding##Binding", ImGuiTreeNodeFlags_DefaultOpen))
		return;

	//Pivot
	{
		std::string strPivotTag;
		if (!m_tDoc.tBinding.Pivot.strAnchorTag.empty())
			strPivotTag.assign(
				m_tDoc.tBinding.Pivot.strAnchorTag.begin(),
				m_tDoc.tBinding.Pivot.strAnchorTag.end());

		char szPivotTag[MAX_NAME] = {};
		strncpy_s(szPivotTag, strPivotTag.c_str(), _TRUNCATE);
		if (ImGui::InputText("Pivot Bone Tag##Binding", szPivotTag, IM_ARRAYSIZE(szPivotTag)))
		{
			std::string strNew = szPivotTag;
			m_tDoc.tBinding.Pivot.strAnchorTag.assign(strNew.begin(), strNew.end());
			m_bDirty = true;
		}
	}

	if (ImGui::DragFloat3("Pivot Local Offset##Binding",
		&m_tDoc.tBinding.Pivot.vLocalOffset.x, 0.01f, -10.f, 10.f))
		m_bDirty = true;

	ImGui::Separator();

	//Separate LookAt 토글
	_bool bUseSeparate = m_tDoc.tBinding.bUseSeparateLookAt;
	if (ImGui::Checkbox("Use Separate LookAt##Binding", &bUseSeparate))
	{
		m_tDoc.tBinding.bUseSeparateLookAt = bUseSeparate;
		if (m_tDoc.tBinding.bUseSeparateLookAt)
			Sync_LookAtBindingFromPivot();
		m_bDirty = true;
	}

	// LookAt 비활성화 가능
	ImGui::BeginDisabled(!m_tDoc.tBinding.bUseSeparateLookAt);
	{
		std::string strLookAtTag;
		if (!m_tDoc.tBinding.LookAt.strAnchorTag.empty())
			strLookAtTag.assign(
				m_tDoc.tBinding.LookAt.strAnchorTag.begin(),
				m_tDoc.tBinding.LookAt.strAnchorTag.end());

		char szLookAtTag[MAX_NAME] = {};
		strncpy_s(szLookAtTag, strLookAtTag.c_str(), _TRUNCATE);
		if (ImGui::InputText("LookAt Bone Tag##Binding", szLookAtTag, IM_ARRAYSIZE(szLookAtTag)))
		{
			std::string strNew = szLookAtTag;
			m_tDoc.tBinding.LookAt.strAnchorTag.assign(strNew.begin(), strNew.end());
			m_bDirty = true;
		}
	}

	if (ImGui::DragFloat3("LookAt Local Offset##Binding",
		&m_tDoc.tBinding.LookAt.vLocalOffset.x, 0.01f, -10.f, 10.f))
		m_bDirty = true;

	ImGui::EndDisabled();
}

// Render Shot, Start
void CImGui_CameraShotLayout::Render_ShotStartSection()
{
	if (!ImGui::CollapsingHeader("Start##ShotStart", ImGuiTreeNodeFlags_DefaultOpen))
		return;

	const _char* szStartModes[] = { "INHERIT_CURRENT", "FIXED_FROM_PIVOT" };
	_int iStartMode = static_cast<_int>(m_tDoc.tShotDesc.Start.eMode);
	if (ImGui::Combo("Start Mode##ShotStart", &iStartMode, szStartModes, IM_ARRAYSIZE(szStartModes)))
	{
		m_tDoc.tShotDesc.Start.eMode = static_cast<Engine::ECameraShotStartMode>(iStartMode);
		m_bDirty = true;
	}

	if (ImGui::Checkbox("Apply Start Pose Immediately##ShotStart",
		&m_tDoc.tShotDesc.Start.bApplyStartPoseImmediately))
		m_bDirty = true;

	ImGui::BeginDisabled(m_tDoc.tShotDesc.Start.eMode != Engine::ECameraShotStartMode::FixedFromPivot);
	if (ImGui::DragFloat3("Start Local Offset##ShotStart",
		&m_tDoc.tShotDesc.Start.vLocaloffset.x, 0.01f, -20.f, 20.f))
		m_bDirty = true;
	ImGui::EndDisabled();
}

// Render Shot,Start Action
void CImGui_CameraShotLayout::Render_ShotStartActionSection()
{
	if (!ImGui::CollapsingHeader("Start Action##StartAction", ImGuiTreeNodeFlags_DefaultOpen))
		return;

	const _char* szActions[] =
	{
		"NONE",
		"DISABLE_UI",
		"DISABLE_PLAYER_INPUT",
		"DISABLE_UI_AND_INPUT",
		"BOSS_INTRO_START_EVENT",
		"DISABLE_UI_AND_BOSS_INTRO_START"
	};

	_int iAction = static_cast<_int>(m_tDoc.tShotDesc.iClientStartAction);
	if (ImGui::Combo("Client Start Action##StartAction", &iAction, szActions, IM_ARRAYSIZE(szActions)))
	{
		m_tDoc.tShotDesc.iClientStartAction = static_cast<_uint>(iAction);
		m_bDirty = true;
	}

	ImGui::TextDisabled("Executed when shot actually begins.");
}

// Render Shot,PivotBasic
void CImGui_CameraShotLayout::Render_ShotPivotBasicSection()
{
	if (!ImGui::CollapsingHeader("Pivot Basic##ShotPivotBasic", ImGuiTreeNodeFlags_DefaultOpen))
		return;

	if (ImGui::DragFloat("Duration##ShotPivotBasic",
		&m_tDoc.tShotDesc.Pivot.fDuration, 0.05f, 0.05f, 30.f))
	{
		const _float fDuration = m_tDoc.tShotDesc.Pivot.fDuration;
		for (auto& [label, pChannel] : Get_AllChannels())
			Clamp_ChannelToDuration(*pChannel, fDuration);
		m_bDirty = true;
	}

	if (ImGui::Checkbox("Follow Live Pivot##ShotPivotBasic",
		&m_tDoc.tShotDesc.Pivot.bFollowLivePivot))  m_bDirty = true;

	if (ImGui::Checkbox("Follow Live LookAt##ShotPivotBasic",
		&m_tDoc.tShotDesc.Pivot.bFollowLiveLookAt)) m_bDirty = true;

	if (ImGui::Checkbox("Look At Target##ShotPivotBasic",
		&m_tDoc.tShotDesc.Pivot.bLookAtTarget))     m_bDirty = true;

	const _char* szBasisModes[] = { "TARGET_TRANSFORM_YAW", "WORLD" };
	_int iBasis = static_cast<_int>(m_tDoc.tShotDesc.Pivot.eBasisMode);
	if (ImGui::Combo("Basis Mode##ShotPivotBasic", &iBasis, szBasisModes, IM_ARRAYSIZE(szBasisModes)))
	{
		m_tDoc.tShotDesc.Pivot.eBasisMode = static_cast<ECameraBasisMode>(iBasis);
		m_bDirty = true;
	}
}

// Render Shot,Channels
void CImGui_CameraShotLayout::Render_ShotChannelSection()
{
	if (!ImGui::CollapsingHeader("Channels##ShotChannels", ImGuiTreeNodeFlags_DefaultOpen))
		return;

	ImGui::SeparatorText("Pivot Offset");
	Render_Channel1D("PivotOffsetX", m_tDoc.tShotDesc.Pivot.PivotOffsetX);
	Render_Channel1D("PivotOffsetY", m_tDoc.tShotDesc.Pivot.PivotOffsetY);
	Render_Channel1D("PivotOffsetZ", m_tDoc.tShotDesc.Pivot.PivotOffsetZ);

	ImGui::SeparatorText("Local Move");
	Render_Channel1D("LocalX", m_tDoc.tShotDesc.Pivot.LocalX);
	Render_Channel1D("LocalY", m_tDoc.tShotDesc.Pivot.LocalY);
	Render_Channel1D("LocalZ", m_tDoc.tShotDesc.Pivot.LocalZ);

	ImGui::SeparatorText("Orbit");
	Render_Channel1D("OrbitYawDeg", m_tDoc.tShotDesc.Pivot.OrbitYawDeg);
}

// Render Shot,Look At
void CImGui_CameraShotLayout::Render_ShotLookAtSection()
{
	if (!ImGui::CollapsingHeader("LookAt##ShotLookAt", ImGuiTreeNodeFlags_DefaultOpen))
		return;

	ImGui::TextDisabled("LookAt offsets are evaluated in the same basis as shot motion.");

	Render_Channel1D("LookAtOffsetX", m_tDoc.tShotDesc.Pivot.LookAtOffsetX);
	Render_Channel1D("LookAtOffsetY", m_tDoc.tShotDesc.Pivot.LookAtOffsetY);
	Render_Channel1D("LookAtOffsetZ", m_tDoc.tShotDesc.Pivot.LookAtOffsetZ);
}

// Render Shot,Recover
void CImGui_CameraShotLayout::Render_ShotRecoverSection()
{
	if (!ImGui::CollapsingHeader("Recover##ShotRecover", ImGuiTreeNodeFlags_DefaultOpen))
		return;

	// Target
	const _char* szRecoverTargets[] = { "PRE_SHOT_SNAPSHOT", "GAMEPLAY_SOLVED" };
	_int iRecoverTarget = static_cast<_int>(m_tDoc.tShotDesc.Recover.eTarget);
	if (ImGui::Combo("Recover Target##ShotRecover",
		&iRecoverTarget, szRecoverTargets, IM_ARRAYSIZE(szRecoverTargets)))
	{
		m_tDoc.tShotDesc.Recover.eTarget = static_cast<ECameraShotRecoverTarget>(iRecoverTarget);
		m_bDirty = true;
	}

	// Method
	const _char* szRecoverMethods[] = { "SNAP", "BLEND" };
	_int iRecoverMethod = static_cast<_int>(m_tDoc.tShotDesc.Recover.eMethod);
	if (ImGui::Combo("Recover Method##ShotRecover",
		&iRecoverMethod, szRecoverMethods, IM_ARRAYSIZE(szRecoverMethods)))
	{
		m_tDoc.tShotDesc.Recover.eMethod = static_cast<ECameraShotRecoverMethod>(iRecoverMethod);
		m_bDirty = true;
	}

	// BlendTime, Blend일때만
	ImGui::BeginDisabled(m_tDoc.tShotDesc.Recover.eMethod != Engine::ECameraShotRecoverMethod::Blend);
	if (ImGui::DragFloat("Recover Blend Time##ShotRecover",
		&m_tDoc.tShotDesc.Recover.fBlendTime, 0.01f, 0.01f, 10.f))
		m_bDirty = true;
	ImGui::EndDisabled();

	// Ease
	const _char* szEaseNames[] =
	{
		"LINEAR", "SMOOTHSTEP", "EASE_OUT_QUAD", "EASE_IN_OUT_QUAD", "EASE_OUT_BACK"
	};
	_int iEase = static_cast<_int>(m_tDoc.tShotDesc.Recover.eEase);
	if (ImGui::Combo("Recover Ease##ShotRecover", &iEase, szEaseNames, IM_ARRAYSIZE(szEaseNames)))
	{
		m_tDoc.tShotDesc.Recover.eEase = static_cast<ECameraShotEase>(iEase);
		m_bDirty = true;
	}
}

// Render Shot,Finish Action
void CImGui_CameraShotLayout::Render_ShotFinishActionSection()
{
	if (!ImGui::CollapsingHeader("Finish Action##FinishAction", ImGuiTreeNodeFlags_DefaultOpen))
		return;

	const _char* szActions[] =
	{
		"NONE",
		"ENABLE_UI",
		"ENABLE_PLAYER_INPUT",
		"ENABLE_UI_AND_INPUT",
		"BOSS_INTRO_END_EVENT",
		"ENABLE_UI_AND_BOSS_INTRO_END"
	};

	int iAction = static_cast<_int>(m_tDoc.tShotDesc.iClientFinishAction);
	if (ImGui::Combo("Client Finish Action##FinishAction", &iAction, szActions, IM_ARRAYSIZE(szActions)))
	{
		m_tDoc.tShotDesc.iClientFinishAction = static_cast<_uint>(iAction);
		m_bDirty = true;
	}

	ImGui::TextDisabled("Executed after full shot finish.");
	ImGui::TextDisabled("BLEND recover: after Recover end.");
	ImGui::TextDisabled("SNAP recover: immediately on finish.");
}

void CImGui_CameraShotLayout::Render_RuntimeSection()
{
	if (!ImGui::CollapsingHeader("Runtime##Runtime", ImGuiTreeNodeFlags_DefaultOpen))
		return;

#ifdef _DEBUG
	CCameraMan_Targeter* pTargeter = Get_Targeter();
	if (pTargeter == nullptr)
	{
		ImGui::TextDisabled("Targeter not found");
		return;
	}

	ImGui::Text("Playing         : %s", pTargeter->Debug_IsScriptedShotPlaying() ? "YES" : "NO");
	ImGui::Text("Elapsed         : %.3f", pTargeter->Debug_GetScriptedShotTime());
	ImGui::Text("Pause           : %s", m_bPause ? "YES" : "NO");
	ImGui::Text("Dirty           : %s", m_bDirty ? "YES" : "NO");
	ImGui::Text("BossIdx         : %d", m_iBossIndex);
	ImGui::Text("Separate LookAt : %s", m_tDoc.tBinding.bUseSeparateLookAt ? "YES" : "NO");
#endif
}

// + 버튼은 TreeNode 헤더 인라인 배치
// Ensure_ChannelKeyUIDs는 로드, 키 추가 시점에만 호출
void CImGui_CameraShotLayout::Render_Channel1D(
	const _char* pLabel, OUT CAMERA_SHOT_CHANNEL_1D& outChannel)
{
	const bool bOpen = ImGui::TreeNode(pLabel);

	// 헤더 옆 인라인 + 버튼
	ImGui::SameLine();
	const std::string strAddBtn = std::string("+##Add_") + pLabel;
	if (ImGui::SmallButton(strAddBtn.c_str()))
	{
		Engine::CAMERA_SHOT_KEY_1D tKey = {};
		tKey.fTime = 0.f;
		tKey.fValue = 0.f;
		tKey.eEase = Engine::ECameraShotEase::SmoothStep;
		tKey.iEditorUID = m_iNextChannelKeyUID++; //    추가 시점에 UID 즉시 부여
		outChannel.vecKeys.push_back(tKey);
		Sort_Channel(outChannel);
		m_bDirty = true;
	}

	if (!bOpen)
		return;

	_bool bSortRequested = false;

	ImGui::Separator();

	for (size_t i = 0; i < outChannel.vecKeys.size(); ++i)
	{
		Engine::CAMERA_SHOT_KEY_1D& tKey = outChannel.vecKeys[i];

		ImGui::PushID(static_cast<_int>(tKey.iEditorUID));

		ImGui::Text("Key %d", static_cast<_int>(i));

		// Time
		const _float fMaxTime = (std::max)(0.01f, m_tDoc.tShotDesc.Pivot.fDuration);
		ImGui::DragFloat("Time", &tKey.fTime, 0.01f, 0.f, fMaxTime);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			bSortRequested = true;
			m_bDirty = true;
		}

		// Value
		if (ImGui::DragFloat("Value", &tKey.fValue, 0.01f, -50.f, 50.f))
			m_bDirty = true;

		// Ease
		const char* szEaseNames[] =
		{
			"LINEAR", "SMOOTHSTEP", "EASE_OUT_QUAD", "EASE_IN_OUT_QUAD", "EASE_OUT_BACK"
		};
		int iEase = static_cast<_int>(tKey.eEase);
		if (ImGui::Combo("Ease", &iEase, szEaseNames, IM_ARRAYSIZE(szEaseNames)))
		{
			tKey.eEase = static_cast<Engine::ECameraShotEase>(iEase);
			m_bDirty = true;
		}

		// Delete - erase 후 break로 벡터 무효화 안전 탈출
		if (ImGui::Button("Delete"))
		{
			outChannel.vecKeys.erase(outChannel.vecKeys.begin() + i);
			m_bDirty = true;
			ImGui::PopID();
			break;
		}

		ImGui::Separator();
		ImGui::PopID();
	}

	if (bSortRequested)
		Sort_Channel(outChannel);

	ImGui::TreePop();
}