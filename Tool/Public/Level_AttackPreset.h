#pragma once
#include "Level.h"

NS_BEGIN(Tool)

class CImGui_Panel;

class CLevel_AttackPreset final : public CLevel
{
	using Super = CLevel;
public:
	enum class Elements
	{
		PresetList,
		PresetMain,
		END,
	};
private:
	CLevel_AttackPreset(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLevel_AttackPreset() = default;

	virtual HRESULT Initialize() override;
public:
	virtual HRESULT Awake(const _uint iLevelID) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual HRESULT Render() override;
private:
	HRESULT	Ready_GUI();
	void Clear();
private:
	class CImGui_ToolManager* m_pImGuiManager = { nullptr };
	array< CImGui_Panel*, ENUM_TO_SZET(Elements::END)> m_arrayImGuiPanel;
public:
	static CLevel_AttackPreset* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

NS_END