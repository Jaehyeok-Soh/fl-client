#pragma once
#include "Level.h"
#include "SkillPreviewMesh.h"

NS_BEGIN(Tool)

class CLevel_Effect final : public CLevel
{
	using Super = CLevel;
	enum class Event
	{
		ChangeSelectedObject = 0,

		PresetSelected,
		PresetDeleteRequested,
		PresetCreateRequested,
		PresetApplyRequested,
		PresetApplyAndCreateRequested,
		PresetDuplicateRequested,

		PreviewAddRequested,
		PreviewDuplicateRequested,
		PreviewDeleteRequested,
		PreviewModelChangeRequested,
		END
	};
	enum class Elements
	{
		Inspector = 0,
		PresetBrowser,
		END
	};
private:
	explicit CLevel_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLevel_Effect() = default;

	virtual HRESULT Initialize() override;
public:
	virtual HRESULT Awake(const _uint iLevelID) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual HRESULT Render() override;
	_uint Get_PresetId();
	const EFFECT_PRESET_SNAPSHOT* Find_PresetSnapshot(_uint iPresetID) const;
	const vector<CToolObject*>& Get_Previews() const { return m_vecPreviewMeshes; }
	const unordered_map<_uint, EFFECT_PRESET>& Get_Presets() const { return m_umapPresets; }
	_bool Rename_Preset(_uint iPresetID, const string& strName);
	const string* Get_PresetName(_uint iPresetID);
	_bool Is_FileDirty() const { return m_bFileDirty; }
	void Clear_PresetDirty() { m_bFileDirty = false; }
	HRESULT Load_EffectMeshPreviews(const MAPOBJECT_SAVEDATA& data, CGameObject** ppResult);
private:
	HRESULT Ready_Preset();
	HRESULT Ready_Camera(const wstring& wstrLayerTag);
	HRESULT Ready_Terrain(const wstring& wstrLayerTag);
	HRESULT Ready_Lights();
	HRESULT Ready_Gui();
	HRESULT Ready_CameraSetting(const _uint iLevelID);
	
	void Render_Elements();
	void Ready_Event();
	void Release_Event();
	void On_ChangeSelectedObject(CGameObject* pGo);

	void On_PresetSelected(_uint iPresetID);
	void On_PresetDeleteRequested(_uint iPresetID);
	void On_PresetCreateRequested();
	void On_PresetApplyRequested(CGameObject* pTarget, _uint iPresetID, EFFECT_PRESET_SNAPSHOT snapShot);
	void On_PresetApplyAndCreateRequested(CGameObject* pTarget, EFFECT_PRESET_SNAPSHOT snapShot);
	void On_PresetDuplicateRequested(_uint iSrcPresetID);

	void On_PreviewAddRequested();
	void On_PreviewDuplicateRequested(CGameObject* pTarget);
	void On_PreviewDeleteRequested(CGameObject* pTarget);
	void On_PreviewModelChangeRequested(CGameObject* pTarget, string strModelTag);
	CToolObject* Spawn_PreviewMesh(void* pArg);
	EFFECT_PRESET* Get_Preset(_uint iPresetID);
	_uint Create_NewPreset(const wstring& wstrTag = L"");
	_uint Duplicate_Preset(_uint iSrcID);
	_uint Allocate_PresetID() { return m_iNextPresetID++; }
	void Select_Preset(_uint ID) { m_iSelectedPresetID = ID; }
private:
	class CImGui_ToolManager* m_pImGuiManager = { nullptr };
	class CPicking_ToolManager* m_pPickingManager = { nullptr };
	CToolObject* m_pSelectedObject = { nullptr };

	_bool m_bFileDirty = { false };
	_uint m_iNextPresetID = { 0 };
	_uint m_iSelectedPresetID = { 0 };
	vector<CToolObject*> m_vecPreviewMeshes;
	unordered_map<_uint, EFFECT_PRESET> m_umapPresets;

	array<DelegateHandle, ENUM_TO_SZET(Event::END)> m_EventHandles;
	array<class CImGui_Base*, ENUM_TO_SZET(Elements::END)> m_GuiElements;
public:
	void Save_PresetData(MAPFILE_DATA& data);
	static CLevel_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

NS_END
