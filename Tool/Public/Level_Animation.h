#pragma once
#include "Level.h"

NS_BEGIN(Tool)

class CLevel_Animation final : public CLevel
{
	using Super = CLevel;

	enum class Elements
	{
		LOAD
		,MODEL
		,ANIMATION
		,PARTS
		,END
	};

private:
	explicit CLevel_Animation(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLevel_Animation() = default;

	virtual HRESULT Initialize() override;
public:
	virtual HRESULT Awake(const _uint iLevelID) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual HRESULT Render() override;

	/* Ready Funcs*/
private:
	HRESULT Ready_Camera(const _wstring wstrLayerTag);
	HRESULT Ready_Lights(const _wstring wstrLayerTag);
	HRESULT Ready_Panels();

private:
	void	Update_Elements(const _float fTimeDelta);
	void	Render_Elements();

private:
	class CImGui_ToolManager*	m_pImGuiManager		= { nullptr };
	class CPicking_ToolManager* m_pPickingManager	= { nullptr };
	class CToolObject*			m_pSelectedObject	= { nullptr };

private:
	array<class CImGui_Base*, ENUM_TO_SZET(Elements::END)> m_GuiElements;

public:
	static CLevel_Animation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

NS_END
