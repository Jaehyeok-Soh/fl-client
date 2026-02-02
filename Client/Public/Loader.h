#pragma once
#include "Client_Defines.h"
#include <thread>
#include <mutex>
#include "Base.h"

NS_BEGIN(Client)

class CLoader final : public CBase
{
	using Super = CBase;
private:
	explicit CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, ELevelType eLoadingLevelID);
	virtual ~CLoader() = default;

	HRESULT Initailize();
public:
	_bool IsFinished() const { return m_isFinished; }
	void Output()
	{
		_tchar szDest[MAX_PATH] = {};

		{
			std::lock_guard<std::mutex> lockguard(m_mutex_1);
			::lstrcpyn(szDest, m_szFPS, MAX_PATH);

		}

		::SetWindowText(g_hWnd, szDest);
	}
private:
	HRESULT Loading();
	HRESULT Loading_For_Logo();
private:
	HRESULT Loading_Files(_uint iLevelID, DTO::ECategory eCategory, const wstring& wstrFolderPath);
	HRESULT Loading_File(_uint iLevelID, DTO::ECategory eCategory,const wstring& wstrFilePath);
	HRESULT Loading_Textures(const wstring &wstrFolder);
	HRESULT Loading_Texture(const wstring& wstrFile);
	// 복수, 단일 mesh가 들어있는 폴더, 파일 넣으면 가능
	HRESULT Make_StaticModel_Prototype(ELevelType eType, const wstring& wstrFolderPath);
private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pDeviceContext = { nullptr };
	class CGameInstance* m_pGameInstance = { nullptr };

	ELevelType m_eLoadingLevelID = { ELevelType::END };
	std::thread m_LoadingThread = {};
	std::mutex m_mutex_1 = {};
	std::atomic_bool m_isFinished = { false };
	_tchar m_szFPS[MAX_PATH] = L"";
public:
	static CLoader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, ELevelType eLoadingLevelID);
	virtual void Free() override;
};

NS_END