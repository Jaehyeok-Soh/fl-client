#pragma once
#include "ImGui_Panel.h"


NS_BEGIN(Tool)

class CFolder;
class CFile;
class CMapToolManager;

class CPanel_FileExplore : public CImGui_Panel
{
	using Super = CImGui_Panel;
protected:
	explicit CPanel_FileExplore(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CPanel_FileExplore() = default;
private:
	HRESULT	Initialize(const wchar_t* pRootFloaderPath, vector<string> vecShowExtName);
public:
	virtual HRESULT Render(CToolObject* pGo)override;
	virtual void Update(const _float fTimeDelta)override;
	HRESULT	Update_MapObjectList();
private:
	HRESULT	Render_MapObjectList();
	HRESULT	Render_SelectInfo();
	HRESULT Render_MakeModelFilePath(const wstring& wstrPath);
private:
	void						FloderWindow();
	void						FileWindow();
	void						Draw_TreeFiles(CFolder* pTreeFloder);
	void						FileFindWindow();

private:
	vector<wstring>				m_vecFindFilePathList{};
	CFolder*					m_pRootFolder{ nullptr };
	vector<CFile*>				m_vecFiles;
	string						m_strCurPath;
	char						m_szFileName[MAX_PATH];
	char						m_szKey[MAX_PATH];

private:
	CMapToolManager*			m_pMapToolManager{nullptr};

	char						m_szFindFileName[MAX_PATH];

	class ImGui::FileBrowser*  m_pImFileBrowser{};
	CGameInstance*			   m_pGameInstance{ nullptr };
public:
	static			CPanel_FileExplore* Create(const wchar_t* pRootFloaderPath, vector<string> vecShowExtName,const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void	Free() override;
};

NS_END