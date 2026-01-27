#pragma once
#include "ImGui_Base.h"


NS_BEGIN(Engine)
class CGameInstance;
class CDataDocumentBase;
NS_END

NS_BEGIN(Tool)

class CToolObject;

class CImGui_Dockspace_MenuBar final : public CImGui_Base
{
	using Super = CImGui_Base;
private:
	CImGui_Dockspace_MenuBar(const _char *pLabel, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CImGui_Dockspace_MenuBar() = default;
public:
	virtual HRESULT Render(CToolObject* pGo = nullptr);
private:
	void Open_FileDialog();
	void Save_FileDialog();
	void Save_Data(const wstring& wstrFilePath);
	void Load_Data(const wstring& wstrFilePath);
private:
	void Save_MapData(const wstring& wstrFilePath);
	void Save_AnimationData(const wstring& wstrFilePath);
	void Save_EffectData(const wstring& wstrFilePath);
	void Save_CameraData(const wstring& wstrFilePath);
	void Save_UIData(const wstring& wstrFilePath);

	void Request_ExportData(ELevelType eLevelID, DTO::ECategory eCategory, const wstring& wstrLayerTag, CDataDocumentBase* pDocument);
private:
	CGameInstance* m_pGameInstance{nullptr};
public:
	static CImGui_Dockspace_MenuBar* Create(const _char* pLabel, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

NS_END