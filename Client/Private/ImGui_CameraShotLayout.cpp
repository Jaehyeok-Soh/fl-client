#include "pch.h"
#include "ImGui_CameraShotLayout.h"
#include "CameraMan_Targeter.h"
#include "CameraShotPrestBuilder.h"
#include "CameraPreset_Manager.h"
#include "GameInstance.h"

CImGui_CameraShotLayout::CImGui_CameraShotLayout()
	: Super("CameraShot")
{
}

HRESULT CImGui_CameraShotLayout::Initialize()
{
	New_Document();
	return S_OK;
}

void CImGui_CameraShotLayout::Render(CGameObject* pGo)
{
	CCameraMan_Targeter* pTargeter = Get_Targeter();

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

void CImGui_CameraShotLayout::New_Document()
{
	m_tDoc = {};

	Reset_ToPlayerDefault();
	m_tDoc.strName = "NewCameraShot";
	m_tDoc.tShotDesc.strName = m_tDoc.strName;

	m_bPause = false;
	m_bDirty = false;
}

void CImGui_CameraShotLayout::Reset_ToPlayerDefault()
{
	m_tDoc.tShotDesc = CCameraShotPrestBuilder::Make_DebugPlain5SecShot();
	m_tDoc.tBinding = CCameraShotPrestBuilder::Make_DebugPlayerBinding();

	if (m_tDoc.strName.empty())
		m_tDoc.strName = "PlayerDefault";

	if (m_tDoc.tShotDesc.strName.empty())
		m_tDoc.tShotDesc.strName = m_tDoc.strName;

	m_bPause = false;
	m_bDirty = false;
}

void CImGui_CameraShotLayout::Reset_ToBossDefault()
{
	m_tDoc.tShotDesc = CCameraShotPrestBuilder::Make_DebugBossPlain5SecShot();

	Engine::CGameObject* pBoss = Get_DebugBoss();
	if (pBoss)
	{
		m_tDoc.tBinding = CCameraShotPrestBuilder::Make_DebugBossBinding(pBoss);
	}
	else
	{
		m_tDoc.tBinding = {};
		m_tDoc.tBinding.Pivot.eSource = Engine::ECameraAnchorSource::OBJECT;
		m_tDoc.tBinding.Pivot.pObject = nullptr;
	}

	if (m_tDoc.strName.empty())
		m_tDoc.strName = "BossDefault";

	if (m_tDoc.tShotDesc.strName.empty())
		m_tDoc.tShotDesc.strName = m_tDoc.strName;

	m_bPause = false;
	m_bDirty = false;
}

void CImGui_CameraShotLayout::Save_Document()
{
	if (m_tDoc.strName.empty())
		return;

	CCameraPreset_Manager* pManager = CCameraPreset_Manager::GetInstance();
	if (pManager == nullptr)
		return;

	CAMERA_SHOT_PRESET preset = Build_PresetFromDoc();

	if (FAILED(pManager->Register_Preset(preset)))
		return;

	if (FAILED(pManager->Save_PresetFile(preset)))
		return;

	m_strSelectedPresetTag = preset.strPresetTag;
	m_bDirty = false;
}

void CImGui_CameraShotLayout::Load_Document()
{
}

void CImGui_CameraShotLayout::Render_ShotStartSection()
{
	if (ImGui::CollapsingHeader("Start##ShotStart", ImGuiTreeNodeFlags_DefaultOpen))
	{
		_int iStartMode = static_cast<_int>(m_tDoc.tShotDesc.Start.eMode);

		const _char* szStartModes[] =
		{
			"INHERIT_CURRENT",
			"FIXED_FROM_PIVOT"
		};

		if (ImGui::Combo("Start Mode##ShotStart", &iStartMode, szStartModes, IM_ARRAYSIZE(szStartModes)))
		{
			m_tDoc.tShotDesc.Start.eMode = static_cast<Engine::ECameraShotStartMode>(iStartMode);
			m_bDirty = true;
		}

		if (ImGui::Checkbox("Apply Start Pose Immediately##ShotStart", &m_tDoc.tShotDesc.Start.bApplyStartPoseImmediately))
			m_bDirty = true;

		ImGui::BeginDisabled(m_tDoc.tShotDesc.Start.eMode != Engine::ECameraShotStartMode::FixedFromPivot);
		if (ImGui::DragFloat3("Start Local Offset##ShotStart", &m_tDoc.tShotDesc.Start.vLocaloffset.x, 0.01f, -20.f, 20.f))
			m_bDirty = true;
		ImGui::EndDisabled();
	}
}

void CImGui_CameraShotLayout::Render_ShotStartActionSection()
{
	if (ImGui::CollapsingHeader("Start Action##StartAction", ImGuiTreeNodeFlags_DefaultOpen))
	{
		_int iAction = static_cast<_int>(m_tDoc.tShotDesc.iClientStartAction);

		const _char* szActions[] =
		{
			"NONE",
			"DISABLE_UI",
			"DISABLE_PLAYER_INPUT",
			"DISABLE_UI_AND_INPUT",
			"BOSS_INTRO_START_EVENT",
			"DISABLE_UI_AND_BOSS_INTRO_START"
		};

		if (ImGui::Combo("Client Start Action##StartAction", &iAction, szActions, IM_ARRAYSIZE(szActions)))
			m_tDoc.tShotDesc.iClientStartAction = static_cast<_uint>(iAction);

		ImGui::TextDisabled("Executed when shot actually begins.");
	}
}

void CImGui_CameraShotLayout::Render_ShotPivotBasicSection()
{
	if (ImGui::CollapsingHeader("Pivot Basic##ShotPivotBasic", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::DragFloat("Duration##ShotPivotBasic", &m_tDoc.tShotDesc.Pivot.fDuration, 0.05f, 0.05f, 30.f))
		{
			const _float fDuration = m_tDoc.tShotDesc.Pivot.fDuration;

			Clamp_ChannelToDuration(m_tDoc.tShotDesc.Pivot.PivotOffsetX, fDuration);
			Clamp_ChannelToDuration(m_tDoc.tShotDesc.Pivot.PivotOffsetY, fDuration);
			Clamp_ChannelToDuration(m_tDoc.tShotDesc.Pivot.PivotOffsetZ, fDuration);

			Clamp_ChannelToDuration(m_tDoc.tShotDesc.Pivot.LookAtOffsetX, fDuration);
			Clamp_ChannelToDuration(m_tDoc.tShotDesc.Pivot.LookAtOffsetY, fDuration);
			Clamp_ChannelToDuration(m_tDoc.tShotDesc.Pivot.LookAtOffsetZ, fDuration);

			Clamp_ChannelToDuration(m_tDoc.tShotDesc.Pivot.LocalX, fDuration);
			Clamp_ChannelToDuration(m_tDoc.tShotDesc.Pivot.LocalY, fDuration);
			Clamp_ChannelToDuration(m_tDoc.tShotDesc.Pivot.LocalZ, fDuration);

			Clamp_ChannelToDuration(m_tDoc.tShotDesc.Pivot.OrbitYawDeg, fDuration);

			m_bDirty = true;
		}

		if (ImGui::Checkbox("Follow Live Pivot##ShotPivotBasic", &m_tDoc.tShotDesc.Pivot.bFollowLivePivot))
			m_bDirty = true;

		if (ImGui::Checkbox("Follow Live LookAt##ShotPivotBasic", &m_tDoc.tShotDesc.Pivot.bFollowLiveLookAt))
			m_bDirty = true;

		if (ImGui::Checkbox("Look At Target##ShotPivotBasic", &m_tDoc.tShotDesc.Pivot.bLookAtTarget))
			m_bDirty = true;

		_int iBasis = static_cast<_int>(m_tDoc.tShotDesc.Pivot.eBasisMode);
		const _char* szBasisModes[] =
		{
			"TARGET_TRANSFORM_YAW",
			"WORLD"
		};

		if (ImGui::Combo("Basis Mode##ShotPivotBasic", &iBasis, szBasisModes, IM_ARRAYSIZE(szBasisModes)))
		{
			m_tDoc.tShotDesc.Pivot.eBasisMode = static_cast<ECameraBasisMode>(iBasis);
			m_bDirty = true;
		}
	}
}

void CImGui_CameraShotLayout::Render_ShotChannelSection()
{
	if (ImGui::CollapsingHeader("Channels##ShotChannels", ImGuiTreeNodeFlags_DefaultOpen))
	{
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
}

void CImGui_CameraShotLayout::Render_ShotLookAtSection()
{
	if (ImGui::CollapsingHeader("LookAt##ShotLookAt", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::TextDisabled("LookAt offsets are evaluated in the same basis as shot motion.");

		Render_Channel1D("LookAtOffsetX", m_tDoc.tShotDesc.Pivot.LookAtOffsetX);
		Render_Channel1D("LookAtOffsetY", m_tDoc.tShotDesc.Pivot.LookAtOffsetY);
		Render_Channel1D("LookAtOffsetZ", m_tDoc.tShotDesc.Pivot.LookAtOffsetZ);
	}
}

void CImGui_CameraShotLayout::Render_ShotRecoverSection()
{
	if (ImGui::CollapsingHeader("Recover##ShotRecover", ImGuiTreeNodeFlags_DefaultOpen))
	{
		_int iRecoverTarget = static_cast<_int>(m_tDoc.tShotDesc.Recover.eTarget);
		const _char* szRecoverTargets[] =
		{
			"PRE_SHOT_SNAPSHOT",
			"GAMEPLAY_SOLVED"
		};

		if (ImGui::Combo("Recover Target##ShotRecover", &iRecoverTarget, szRecoverTargets, IM_ARRAYSIZE(szRecoverTargets)))
		{
			m_tDoc.tShotDesc.Recover.eTarget = static_cast<ECameraShotRecoverTarget>(iRecoverTarget);
			m_bDirty = true;
		}

		_int iRecoverMethod = static_cast<_int>(m_tDoc.tShotDesc.Recover.eMethod);
		const _char* szRecoverMethods[] =
		{
			"SNAP",
			"BLEND"
		};

		if (ImGui::Combo("Recover Method##ShotRecover", &iRecoverMethod, szRecoverMethods, IM_ARRAYSIZE(szRecoverMethods)))
		{
			m_tDoc.tShotDesc.Recover.eMethod = static_cast<ECameraShotRecoverMethod>(iRecoverMethod);
			m_bDirty = true;
		}

		ImGui::BeginDisabled(m_tDoc.tShotDesc.Recover.eMethod != Engine::ECameraShotRecoverMethod::Blend);
		if (ImGui::DragFloat("Recover Blend Time##ShotRecover", &m_tDoc.tShotDesc.Recover.fBlendTime, 0.01f, 0.01f, 10.f))
			m_bDirty = true;
		ImGui::EndDisabled();

		_int iEase = static_cast<_int>(m_tDoc.tShotDesc.Recover.eEase);
		const _char* szEaseNames[] =
		{
			"LINEAR",
			"SMOOTHSTEP",
			"EASE_OUT_QUAD",
			"EASE_IN_OUT_QUAD",
			"EASE_OUT_BACK"
		};

		if (ImGui::Combo("Recover Ease##ShotRecover", &iEase, szEaseNames, IM_ARRAYSIZE(szEaseNames)))
		{
			m_tDoc.tShotDesc.Recover.eEase = static_cast<ECameraShotEase>(iEase);
			m_bDirty = true;
		}
	}
}

void CImGui_CameraShotLayout::Render_ShotFinishActionSection()
{
	if (ImGui::CollapsingHeader("Finish Action##FinishAction", ImGuiTreeNodeFlags_DefaultOpen))
	{
		int iAction = static_cast<_int>(m_tDoc.tShotDesc.iClientFinishAction);

		const _char* szActions[] =
		{
			"NONE",
			"ENABLE_UI",
			"ENABLE_PLAYER_INPUT",
			"ENABLE_UI_AND_INPUT",
			"BOSS_INTRO_END_EVENT",
			"ENABLE_UI_AND_BOSS_INTRO_END"
		};

		if (ImGui::Combo("Client Finish Action##FinishAction", &iAction, szActions, IM_ARRAYSIZE(szActions)))
			m_tDoc.tShotDesc.iClientFinishAction = static_cast<_uint>(iAction);

		ImGui::TextDisabled("Executed after full shot finish.");
		ImGui::TextDisabled("BLEND recover: after Recover end.");
		ImGui::TextDisabled("SNAP recover: immediately on finish.");
	}
}

void CImGui_CameraShotLayout::Render_RuntimeSection()
{
	if (ImGui::CollapsingHeader("Runtime##Runtime", ImGuiTreeNodeFlags_DefaultOpen))
	{
#ifdef _DEBUG
		CCameraMan_Targeter* pTargeter = Get_Targeter();
		if (pTargeter == nullptr)
		{
			ImGui::TextDisabled("Targeter not found");
			return;
		}

		ImGui::Text("Playing : %s", pTargeter->Debug_IsScriptedShotPlaying() ? "YES" : "NO");
		ImGui::Text("Elapsed : %.3f", pTargeter->Debug_GetScriptedShotTime());
		ImGui::Text("Pause   : %s", m_bPause ? "YES" : "NO");
		ImGui::Text("Dirty   : %s", m_bDirty ? "YES" : "NO");
		ImGui::Text("BossIdx : %d", m_iBossIndex);
		ImGui::Text("Separate LookAt : %s", m_tDoc.tBinding.bUseSeparateLookAt ? "YES" : "NO");
#endif
	}
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

void CImGui_CameraShotLayout::Clamp_ChannelToDuration(CAMERA_SHOT_CHANNEL_1D& refChannel, _float fDuration)
{
	for (auto& key : refChannel.vecKeys)
		key.fTime = std::clamp(key.fTime, 0.f, fDuration);
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

	if (m_tDoc.tShotDesc.strName.empty())
		m_tDoc.tShotDesc.strName = m_tDoc.strName;

	m_strSelectedPresetTag = m_tDoc.strName;

	Refresh_DocAfterLoad();
	m_bDirty = false;
}

void CImGui_CameraShotLayout::Refresh_DocAfterLoad()
{
	Ensure_ChannelKeyUIDs(m_tDoc.tShotDesc.Pivot.PivotOffsetX);
	Ensure_ChannelKeyUIDs(m_tDoc.tShotDesc.Pivot.PivotOffsetY);
	Ensure_ChannelKeyUIDs(m_tDoc.tShotDesc.Pivot.PivotOffsetZ);

	Ensure_ChannelKeyUIDs(m_tDoc.tShotDesc.Pivot.LookAtOffsetX);
	Ensure_ChannelKeyUIDs(m_tDoc.tShotDesc.Pivot.LookAtOffsetY);
	Ensure_ChannelKeyUIDs(m_tDoc.tShotDesc.Pivot.LookAtOffsetZ);

	Ensure_ChannelKeyUIDs(m_tDoc.tShotDesc.Pivot.LocalX);
	Ensure_ChannelKeyUIDs(m_tDoc.tShotDesc.Pivot.LocalY);
	Ensure_ChannelKeyUIDs(m_tDoc.tShotDesc.Pivot.LocalZ);

	Ensure_ChannelKeyUIDs(m_tDoc.tShotDesc.Pivot.OrbitYawDeg);

	const _float fDuration = m_tDoc.tShotDesc.Pivot.fDuration;

	Clamp_ChannelToDuration(m_tDoc.tShotDesc.Pivot.PivotOffsetX, fDuration);
	Clamp_ChannelToDuration(m_tDoc.tShotDesc.Pivot.PivotOffsetY, fDuration);
	Clamp_ChannelToDuration(m_tDoc.tShotDesc.Pivot.PivotOffsetZ, fDuration);

	Clamp_ChannelToDuration(m_tDoc.tShotDesc.Pivot.LookAtOffsetX, fDuration);
	Clamp_ChannelToDuration(m_tDoc.tShotDesc.Pivot.LookAtOffsetY, fDuration);
	Clamp_ChannelToDuration(m_tDoc.tShotDesc.Pivot.LookAtOffsetZ, fDuration);

	Clamp_ChannelToDuration(m_tDoc.tShotDesc.Pivot.LocalX, fDuration);
	Clamp_ChannelToDuration(m_tDoc.tShotDesc.Pivot.LocalY, fDuration);
	Clamp_ChannelToDuration(m_tDoc.tShotDesc.Pivot.LocalZ, fDuration);

	Clamp_ChannelToDuration(m_tDoc.tShotDesc.Pivot.OrbitYawDeg, fDuration);
}

void CImGui_CameraShotLayout::Delete_CurrentPreset()
{
}

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

void CImGui_CameraShotLayout::Render_PresetSection()
{
	if (ImGui::CollapsingHeader("Preset##Preset", ImGuiTreeNodeFlags_DefaultOpen))
	{
		_char szName[MAX_NAME] = {};
		strncpy_s(szName, m_tDoc.strName.c_str(), _TRUNCATE);

		if (ImGui::InputText("Preset Name##Preset", szName, IM_ARRAYSIZE(szName)))
		{
			m_tDoc.strName = szName;
			m_tDoc.tShotDesc.strName = m_tDoc.strName;
			m_bDirty = true;
		}

		if (ImGui::Button("New##Preset"))
			New_Document();

		ImGui::SameLine();

		if (ImGui::Button("Load Player Default##Preset"))
			Reset_ToPlayerDefault();

		ImGui::SameLine();

		if (ImGui::Button("Load Boss Default##Preset"))
			Reset_ToBossDefault();

		ImGui::Separator();

		if (ImGui::Button("Save##Preset"))
			Save_Document();

		ImGui::SameLine();

		if (ImGui::Button("Load##PresetFile"))
			Load_Document();

		ImGui::Text("Dirty : %s", m_bDirty ? "YES" : "NO");
	}
}

void CImGui_CameraShotLayout::Render_PlaybackSection()
{
	if (ImGui::CollapsingHeader("Playback##Playback", ImGuiTreeNodeFlags_DefaultOpen))
	{
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
}

void CImGui_CameraShotLayout::Render_BindingSection()
{
	if (ImGui::CollapsingHeader("Binding##Binding", ImGuiTreeNodeFlags_DefaultOpen))
	{
		{
			std::string strPivotTag;
			if (!m_tDoc.tBinding.Pivot.strAnchorTag.empty())
				strPivotTag.assign(m_tDoc.tBinding.Pivot.strAnchorTag.begin(), m_tDoc.tBinding.Pivot.strAnchorTag.end());

			char szPivotTag[128] = {};
			strncpy_s(szPivotTag, strPivotTag.c_str(), _TRUNCATE);

			if (ImGui::InputText("Pivot Bone Tag##Binding", szPivotTag, IM_ARRAYSIZE(szPivotTag)))
			{
				std::string strNew = szPivotTag;
				m_tDoc.tBinding.Pivot.strAnchorTag.assign(strNew.begin(), strNew.end());
				m_bDirty = true;
			}
		}

		ImGui::DragFloat3("Pivot Local Offset##Binding", &m_tDoc.tBinding.Pivot.vLocalOffset.x, 0.01f, -10.f, 10.f);

		bool bUseSeparate = m_tDoc.tBinding.bUseSeparateLookAt;
		if (ImGui::Checkbox("Use Separate LookAt##Binding", &bUseSeparate))
		{
			m_tDoc.tBinding.bUseSeparateLookAt = bUseSeparate;
			if (m_tDoc.tBinding.bUseSeparateLookAt)
				Sync_LookAtBindingFromPivot();
			m_bDirty = true;
		}

		ImGui::BeginDisabled(!m_tDoc.tBinding.bUseSeparateLookAt);

		{
			std::string strLookAtTag;
			if (!m_tDoc.tBinding.LookAt.strAnchorTag.empty())
				strLookAtTag.assign(m_tDoc.tBinding.LookAt.strAnchorTag.begin(), m_tDoc.tBinding.LookAt.strAnchorTag.end());

			char szLookAtTag[128] = {};
			strncpy_s(szLookAtTag, strLookAtTag.c_str(), _TRUNCATE);

			if (ImGui::InputText("LookAt Bone Tag##Binding", szLookAtTag, IM_ARRAYSIZE(szLookAtTag)))
			{
				std::string strNew = szLookAtTag;
				m_tDoc.tBinding.LookAt.strAnchorTag.assign(strNew.begin(), strNew.end());
				m_bDirty = true;
			}
		}

		ImGui::DragFloat3("LookAt Local Offset##Binding", &m_tDoc.tBinding.LookAt.vLocalOffset.x, 0.01f, -10.f, 10.f);

		ImGui::EndDisabled();
	}
}

void CImGui_CameraShotLayout::Render_Channel1D(const _char* pLabel, OUT CAMERA_SHOT_CHANNEL_1D& outChannel)
{
	if (ImGui::TreeNode(pLabel) == false)
		return;

	Ensure_ChannelKeyUIDs(outChannel);
	_bool bSortRequested = false;

	std::string strAddBtn = std::string("Add Key##") + pLabel;
	if (ImGui::Button(strAddBtn.c_str()))
	{
		Engine::CAMERA_SHOT_KEY_1D tKey = {};
		tKey.fTime = 0.f;
		tKey.fValue = 0.f;
		tKey.eEase = Engine::ECameraShotEase::SmoothStep;
		tKey.iEditorUID = m_iNextChannelKeyUID++;
		outChannel.vecKeys.push_back(tKey);
		Sort_Channel(outChannel);
	}

	ImGui::Separator();

	for (size_t i = 0; i < outChannel.vecKeys.size(); ++i)
	{
		Engine::CAMERA_SHOT_KEY_1D& tKey = outChannel.vecKeys[i];

		ImGui::PushID(static_cast<_int>(tKey.iEditorUID));

		ImGui::Text("Key %d", static_cast<_int>(i));

		const _float fMaxTime = (std::max)(0.01f, m_tDoc.tShotDesc.Pivot.fDuration);
		ImGui::DragFloat("Time", &tKey.fTime, 0.01f, 0.f, fMaxTime);
		if (ImGui::IsItemDeactivatedAfterEdit())
			bSortRequested = true;

		ImGui::DragFloat("Value", &tKey.fValue, 0.01f, -50.f, 50.f);

		int iEase = static_cast<_int>(tKey.eEase);
		const char* szEaseNames[] =
		{
			"LINEAR",
			"SMOOTHSTEP",
			"EASE_OUT_QUAD",
			"EASE_IN_OUT_QUAD",
			"EASE_OUT_BACK"
		};

		if (ImGui::Combo("Ease##Ease", &iEase, szEaseNames, IM_ARRAYSIZE(szEaseNames)))
			tKey.eEase = static_cast<Engine::ECameraShotEase>(iEase);

		if (ImGui::Button("Delete##Delete"))
		{
			outChannel.vecKeys.erase(outChannel.vecKeys.begin() + i);
			ImGui::PopID();
			break;
		}

		ImGui::Separator();
		ImGui::PopID();
	}

	if(bSortRequested)
		Sort_Channel(outChannel);

	ImGui::TreePop();
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
