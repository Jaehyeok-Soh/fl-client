#pragma once
#include "Level.h"

NS_BEGIN(Tool)

class CLevel_Camera final : public CLevel
{
	using Super = CLevel;
public:
	using Super = CLevel;
	enum class Event
	{
		ChangeSelectedObject = 0,
		END,
	};
	enum class Elements
	{
		Camera,
		END
	};
private:
	explicit CLevel_Camera(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLevel_Camera() = default;

	virtual HRESULT Initialize() override;

private:
	HRESULT			Reday_Gui();
	HRESULT			Ready_Lights();
	HRESULT			Ready_DebugLine();
	HRESULT			Ready_Camera_Layer(const wstring& wstrLayerTag);


	HRESULT			Ready_Event();
	HRESULT			Release_Event();

	HRESULT			Ready_Camera_Setting(const _uint iLevelID);
public:
	virtual HRESULT Awake(const _uint iLevelID) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual void Update_Picking() override;
	virtual HRESULT Render() override;

	HRESULT Render_Elements();
private:
	HRESULT Ready_Player_Layer(const wstring& wstrLayerTag);
	HRESULT Ready_UI_Layer(const wstring& wstrLayerTag);
private:
	class CImGui_ToolManager* m_pImGuiManager = { nullptr };
	array<class CImGui_Panel*, ENUM_TO_SZET(Elements::END)>	m_arrayImGuiPanel{};
public:
	static CLevel_Camera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

NS_END
