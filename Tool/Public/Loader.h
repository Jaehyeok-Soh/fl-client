#pragma once
#include <thread>
#include <mutex>
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Tool)

class CLoader final : public CBase
{
	using Super = CBase;
private:
	explicit CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, ELevelType eLoadingELevelType);
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
	HRESULT Loading_For_Map();
	HRESULT Loading_For_Animation();
	HRESULT Loading_For_Effect();
	HRESULT Loading_For_Camera();
	HRESULT Loading_For_UI();
	HRESULT Loading_For_AssetConverter();
private:
	HRESULT Loading_Texturessss(const wstring& wstrFile);
	HRESULT Loading_Textures(const wstring& wstrFolder);
	HRESULT Loading_Texture(const wstring& wstrFile);
private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pDeviceContext = { nullptr };
	CGameInstance* m_pGameInstance = { nullptr };

	ELevelType m_eLoadingELevelType = { ELevelType::END };
	std::thread m_LoadingThread = {};
	std::mutex m_mutex_1 = {};
	std::atomic_bool m_isFinished = { false };
	_tchar m_szFPS[MAX_PATH] = L"";
public:
	static CLoader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, ELevelType eLoadingELevelType);
	virtual void Free() override;
};

NS_END