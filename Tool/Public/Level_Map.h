#pragma once
#include "Level.h"

NS_BEGIN(Tool)

class CToolObject;
class CImGui_Panel;


class CLevel_Map final : public CLevel
{
	using Super = CLevel;
	enum class Event
	{
		ChangeSelectedObject = 0,
		CreateMode,
		END
	};
	enum class Elements
	{
		Inspector = 0,
		ObjectList,
		CreateMode,
		END
	};
private:
	CLevel_Map(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLevel_Map() = default;

	virtual HRESULT Initialize() override;
public:
	virtual HRESULT Awake(const _uint iLevelID) override;
	virtual void	Update(const _float fTimeDelta) override;
	virtual HRESULT Render() override;
private:
	void	Render_Elements();
	HRESULT Reday_Gui();
	HRESULT Ready_Lights();
	HRESULT Ready_MapObject_Layer();
	HRESULT Ready_Camera_Layer(const wstring& wstrLayerTag);
	HRESULT Ready_Camera_Setting(const _uint iLevelID);
	void Ready_Event();
	void Release_Event();
	void On_ChangeSelectedObject(CGameObject* pGo);
	void On_CreateMode(_bool bValue);
private:
	_bool m_bCreateMode = { false };
	class CImGui_ToolManager* m_pImGuiManager = { nullptr };
	class CPicking_ToolManager* m_pPickingManager = { nullptr };
	CToolObject* m_pSelectedObject = { nullptr };
	std::array<DelegateHandle, ENUM_TO_SZET(Event::END)>	m_EventHandles;
	array<class CImGui_Panel* , ENUM_TO_SZET(Event::END)>	m_arrayImGuiPanel{};
public:
	static CLevel_Map* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

NS_END
