#pragma once
#include "ImGui_Panel.h"

NS_BEGIN(Tool)

class CAttackPreset_DataManager;
class CBuilder_AttackPreset;

class CPanel_PresetList final : public CImGui_Panel
{
	using Super = CImGui_Panel;
private:
	CPanel_PresetList(CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CPanel_PresetList() = default;

private:
	HRESULT	Initialize();
public:
	virtual void Update(const _float fTimeDelta) override;
	virtual HRESULT Render(CToolObject* pGo) override;
private:
	const _char* To_CategoryFolder(DTO::EAttackPresetCategory eCategory);
	DTO::EAttackPresetCategory Mapping_Category(_int iValue);
	std::string Make_FileName(_ushort iID, const string& strTag);
	HRESULT All_Load();
	HRESULT Save_One(const DTO::TAttackPreset_Data& inData);
private:
	CAttackPreset_DataManager* m_pPresetManager{ nullptr };
	CBuilder_AttackPreset* m_pBuilder{ nullptr };
public:
	static			CPanel_PresetList* Create(CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void	Free() override;
};

NS_END