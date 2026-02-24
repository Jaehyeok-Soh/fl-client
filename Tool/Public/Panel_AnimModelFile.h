#pragma once
#include "ImGui_Panel.h"
#include "Animation_Defines.h"

NS_BEGIN(Tool)

class CPanel_AnimModelFile final : public CImGui_Panel
{
	struct LOAD_OPTIONS
	{
		_char strAnimTag[256] = "";
		_int iPoolingCount = {};

		_bool bLoadAnimInfo = true;
		_char strAnimPath[256] = "";

		_bool bLoadHitbox = true;
		_char strHitboxPath[256] = "";

		_bool bLoadEffect = true;
		_char strEffectPath[256] = "";

		_bool bLoadSound = true;
		_char strSoundPath[256] = "";

		// 모달을 열 때 초기화하는 함수
		void Reset() {
			memset(strAnimTag, 0, 256);
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

	void DrawPreTransformMatrix(string id, ANIM_SRT& pretransform);

	// modal
private:
	LOAD_OPTIONS m_tLoadOptions; // 모달 상태 변수
	void OpenFileDialog(char* buffer, const char* filter);
	void SaveFileDialog(char* buffer, const char* filter);
	void OpenLoadModal();
	void OpenSaveModal();
	void RenderLoadModal();
	void RenderSaveModal();

	// Load data
private:
	void Load_HitboxData(fs::path path);
	void Save_HitboxData(fs::path path);

	void Load_EffectData(fs::path path);
	void Save_EffectData(fs::path path);

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

private:
	_int m_iSocketBoneIdx	= { -1 };
	_bool m_bCombine		= { true };
	ANIM_SRT				m_tAnimSrt{};
	ANIM_SRT				m_tPartSrt{};

public:
	static			CPanel_AnimModelFile* Create(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void	Free() override;
};

NS_END