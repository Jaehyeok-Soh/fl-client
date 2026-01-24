#pragma once
#include "Level.h"

NS_BEGIN(Engine)

NS_END

NS_BEGIN(Tool)

class CLevel_UI final : public CLevel
{
	using Super = CLevel;
	enum class Elements
	{
		INSPECTOR,
		HIERACHY,
		END
	};

private:
	explicit CLevel_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLevel_UI() = default;

	virtual HRESULT Initialize() override;
public:
	virtual HRESULT Awake(const _uint iLevelID) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual HRESULT Render() override;
private:
	HRESULT Ready_UI_Inspector();
	HRESULT Ready_UI_Object(const _wstring& wstrLayerTag);

	array<class CImGui_Base*, ENUM_TO_SZET(Elements::END)> m_GuiElements;

private:
	class CImGui_ToolManager* m_pImGuiManager = { nullptr };

public:
	static CLevel_UI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

NS_END