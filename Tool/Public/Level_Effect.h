#pragma once
#include "Level.h"

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
		EffectSystem,

		ParticleSystem,
		END
	};
private:
	explicit CLevel_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLevel_Effect() = default;

	virtual HRESULT Initialize() override;
public:
	virtual HRESULT Awake(const _uint iLevelID) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual void Update_Picking() override;
	virtual HRESULT Render() override;
private:
	HRESULT Build_Prototype();
	HRESULT Build_Files();
	HRESULT Ready_Camera(const wstring& wstrLayerTag);
	HRESULT Ready_Terrain(const wstring& wstrLayerTag);
	HRESULT Ready_Lights();
	HRESULT Ready_Gui();
	HRESULT Ready_CameraSetting(const _uint iLevelID);
	HRESULT Ready_EffectObjectSetting();
	HRESULT Ready_DebugLine();
	HRESULT Ready_DevMap();
	HRESULT Ready_Component();

	void Render_Elements();
	void Ready_Event();
	void Release_Event();

private:
	class CImGui_ToolManager* m_pImGuiManager = { nullptr };
	class CPicking_ToolManager* m_pPickingManager = { nullptr };
	class CToolObject* m_pSelectedObject = { nullptr };

	array<DelegateHandle, ENUM_TO_SZET(Event::END)> m_EventHandles;
	array<class CImGui_Base*, ENUM_TO_SZET(Elements::END)> m_GuiElements;
public:
	static CLevel_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

inline constexpr wchar_t g_wszStaticObjectLayer[]{ L"StaticObject_Layer" };

NS_END
