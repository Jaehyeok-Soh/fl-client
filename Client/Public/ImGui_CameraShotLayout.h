#pragma once
#include "ImGui_Layout.h"

NS_BEGIN(Client)
class CCameraMan_Targeter;

typedef struct tagCameraShotEditDoc
{
	std::string strName{""};
	SCRIPTED_CAMERA_SHOT_DESC tShotDesc{};
	SCRIPTED_CAMERA_SHOT_BINDING_DESC tBinding{};
}CAMER_SHOT_EDIT_DOC;

class CImGui_CameraShotLayout final : public CImGui_Layout
{
	using Super = CImGui_Layout;
private:
	CImGui_CameraShotLayout();
	virtual ~CImGui_CameraShotLayout() = default;
	HRESULT Initialize();
public:
	virtual _bool Can_Render(CGameObject* pGo) override { return true; }
	virtual void Render(CGameObject* pGo) override;
private:
	// preset / doc
	void New_Document();
	void Reset_ToPlayerDefault();
	void Reset_ToBossDefault();
	void Save_Document();
	void Load_Document();

	// preview target apply
	void Apply_PlayerPreviewBinding();
	void Apply_BossPreviewBinding();
	void Sync_LookAtBindingFromPivot();

	// render section
	void Render_PresetSection();
	void Render_PlaybackSection();
	void Render_BindingSection();

	void Render_ShotStartSection();
	void Render_ShotStartActionSection();
	void Render_ShotPivotBasicSection();
	void Render_ShotChannelSection();
	void Render_ShotLookAtSection();
	void Render_ShotRecoverSection();
	void Render_ShotFinishActionSection();

	void Render_RuntimeSection();	
private:
	Client::CCameraMan_Targeter* Get_Targeter() const;
	Engine::CGameObject* Get_DebugBoss() const;
private:
	void Render_Channel1D(const _char* pLabel, OUT CAMERA_SHOT_CHANNEL_1D& outChannel);
	void Sort_Channel(OUT CAMERA_SHOT_CHANNEL_1D& outChannel);
	void Ensure_ChannelKeyUIDs(OUT CAMERA_SHOT_CHANNEL_1D& outChannel);
	void Clamp_ChannelToDuration(CAMERA_SHOT_CHANNEL_1D& refChannel, _float fDuration);

	CAMERA_SHOT_PRESET Build_PresetFromDoc() const;
	void Apply_DocFromPreset(const CAMERA_SHOT_PRESET& preset);
	void Refresh_DocAfterLoad();
	void Delete_CurrentPreset();
private:
	CAMER_SHOT_EDIT_DOC m_tDoc = {};

	string m_strSelectedPresetTag{ "" };
	_bool m_bPause = false;
	_bool m_bDirty = false;
	_int  m_iBossIndex = 0;
	_uint m_iNextChannelKeyUID = 1;
public:
	static CImGui_CameraShotLayout* Create();
	virtual void Free() override;
};

NS_END