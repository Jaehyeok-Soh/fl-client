#pragma once
#include "ImGui_Panel.h"

namespace fs = std::filesystem;

NS_BEGIN(Tool)

class CPanel_AnimModelFile :
    public CImGui_Panel
{
public:
	typedef struct tagDirectory
	{
		fs::path directory;

		vector<fs::path> files;

		vector<tagDirectory> directories;

		vector<fs::path> GetFiles() { return files; }
		vector<tagDirectory> GetDirectories() { return directories; }
	}DIR;

private:
	using Super = CImGui_Panel;

protected:
	explicit CPanel_AnimModelFile(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CPanel_AnimModelFile() = default;

private:
	HRESULT	Initialize();

public:
	virtual HRESULT Render(CToolObject* pGo)override;
	virtual void Update(const _float fTimeDelta)override;

public:

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