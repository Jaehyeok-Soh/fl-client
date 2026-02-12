#pragma once
#include "ImGui_Panel.h"
#include "Animation_Defines.h"

NS_BEGIN(Tool)

class CPanel_AnimModelFile final : public CImGui_Panel
{
	using Super = CImGui_Panel;

private:
	explicit CPanel_AnimModelFile(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CPanel_AnimModelFile() = default;

private:
	HRESULT	Initialize();

public:
	virtual HRESULT Render(CToolObject* pGo)override;
	virtual void Update(const _float fTimeDelta)override;

	// window
private:
	void DirectoryWindow();
	void FileWindow();

private:
	DIR GetRootDir() { return m_tRootDirectory; }
	DIR RefreshModelDir();
	DIR SearchDir(fs::directory_entry directory);

	void SetDirectoryTree(DIR dir, fs::path parent);

	void CheckAnimModel(DIR dir, fs::path parent);
	_bool CheckResource(DIR dir, string dirName, string extents);

private:
	fs::path m_rootPath = { g_wszModelRelativePath };

	DIR m_tRootDirectory{};
	DIR m_tCurrentDirectory{};
	vector<fs::path> m_files;

	_int m_iResourceTreeID;

public:
	static			CPanel_AnimModelFile* Create(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void	Free() override;
};

NS_END