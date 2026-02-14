#pragma once
#include "ImGui_Panel.h"
#include "Animation_Defines.h"

NS_BEGIN(Tool)

class CPanel_AnimModelFile final : public CImGui_Panel
{
	struct LOAD_OPTIONS
	{
		bool bLoadAnimInfo = true;
		char strAnimPath[256] = "";

		bool bLoadHitbox = true;
		char strHitboxPath[256] = "";

		bool bLoadEffect = true;
		char strEffectPath[256] = "";

		bool bLoadSound = true;
		char strSoundPath[256] = "";

		// 모달을 열 때 초기화하는 함수
		void Reset() {
			memset(strAnimPath, 0, 256);
			memset(strHitboxPath, 0, 256);
			memset(strEffectPath, 0, 256);
			memset(strSoundPath, 0, 256);
			bLoadAnimInfo = true;
			bLoadHitbox = true;
			bLoadEffect = true;
			bLoadSound = true;
		}
	};

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
	void ButtonsWindow();

	// modal
private:
	LOAD_OPTIONS m_tLoadOptions; // 모달 상태 변수
	void OpenFileDialog(char* buffer, const char* filter);
	void OpenLoadModal();
	void RenderLoadModal(); // 매 프레임 호출 필요

	// Load data
private:
	void Load_HitboxData(fs::path path);

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

	_int m_iResourceTreeID = {};

	class CAnimTool_Manager* m_pAnimToolManager = { nullptr };

public:
	static			CPanel_AnimModelFile* Create(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void	Free() override;
};

NS_END