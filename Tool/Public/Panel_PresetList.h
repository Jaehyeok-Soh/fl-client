#pragma once
#include "ImGui_Panel.h"

NS_BEGIN(Tool)

class CAttackPreset_DataManager;

class CPanel_PresetList final : public CImGui_Panel
{
	using Super = CImGui_Panel;
private:
	CPanel_PresetList(CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CPanel_PresetList() = default;

private:
	HRESULT	Initialize();
private:
	DTO::EAttackPresetCategory Mapping_Category(_int iValue);
public:
	virtual void Update(const _float fTimeDelta) override;
	virtual HRESULT Render(CToolObject* pGo) override;
private:
	CAttackPreset_DataManager* m_pPresetManager{ nullptr };
public:
	static			CPanel_PresetList* Create(CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void	Free() override;
};

NS_END