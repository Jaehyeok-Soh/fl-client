#pragma once
#include "Base.h"
#include "Client_Defines.h"
#include <thread>
#include <mutex>
#include "json_forward.h"

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
	HRESULT Loading_For_Tutorial_Village();
	HRESULT Loading_For_Tutorial_Boss();
	HRESULT Loading_For_Square();
	HRESULT Loading_For_Tavern();
	HRESULT Loading_For_Kuangkeng();
	HRESULT Loading_For_Lianhuo();
	HRESULT Loading_For_LoadLevel();
	HRESULT Loading_For_Test();
private:
	HRESULT Loading_Files(_uint iLevelID, DTO::ECategory eCategory, const wstring& wstrFolderPath);
	HRESULT Loading_File(_uint iLevelID, DTO::ECategory eCategory,const wstring& wstrFilePath);
	HRESULT Loading_Textures(const wstring &wstrFolder);
	HRESULT Loading_Texture(const wstring& wstrFile);
	// 복수, 단일 mesh가 들어있는 폴더, 파일 넣으면 가능
	HRESULT Make_StaticObject_Prototype(ELevelType eType, const wstring& wstrFolderPath);

	HRESULT Build_Prototype();
	HRESULT Build_Files();
	HRESULT Ready_AttackOverlap();
	HRESULT Ready_AttackPresets();
	HRESULT Ready_EffectEvent();
	HRESULT Ready_Spawner();

	HRESULT Ready_Sounds();
	HRESULT Ready_Sounds_Player();

	HRESULT Ready_EffectEvent_AnimationData();
	HRESULT Ready_AttackOverlap_PlayerMoon();
	HRESULT Ready_AttackOverlap_Monster_Dog();
	HRESULT Ready_AttackOverlap_Monster_Boomer();
	HRESULT Ready_AttackOverlap_Monster_Veteran();
	HRESULT Ready_AttackOverlap_Xibi();
	HRESULT Ready_AttackOverlap_Lianhuo();

	HRESULT	Ready_CCS();
public:
	const _float* Get_LoadingRatio() const { return &m_fLoadingRatio; }

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pDeviceContext = { nullptr };
	class CGameInstance* m_pGameInstance = { nullptr };

	CBuilderSystem* m_pBuilderSystem = { nullptr };

	ELevelType m_eLoadingLevelID = { ELevelType::END };
	std::thread m_LoadingThread = {};
	std::mutex m_mutex_1 = {};
	std::atomic_bool m_isFinished = { false };
	_tchar m_szFPS[MAX_PATH] = L"";

	_float m_fLoadingRatio = {};


public:
	static CLoader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, ELevelType eLoadingLevelID);
	virtual void Free() override;
};

NS_END