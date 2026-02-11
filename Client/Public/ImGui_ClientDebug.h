#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CGameObject;
class CGameInstance;
NS_END


NS_BEGIN(Client)

class CImGui_Inspector;
class CImGui_Layout;

class CImGui_ClientDebug : public CBase
{
	DECLARE_SINGLETON(CImGui_ClientDebug)
	using Super = CBase;
private:
	enum EInspectorType : unsigned int
	{
		Player = 0,
		Global,
		Monster,
		Light,
		END
	};
private:
	CImGui_ClientDebug();
	virtual ~CImGui_ClientDebug() = default;
	HRESULT Ready_Inspector();
public:
	HRESULT Initialize(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);	
	void Render();
	// TODO - Event로 옮기기
	void Set_Player(CGameObject* pGo) { m_pPlayer = pGo; };
private:

private:
	HWND m_hWnd = { NULL };
	_bool m_bInitialized = { false };
	_bool m_bLockSelection = { true };
	CGameObject* m_pPlayer = { nullptr };
	std::array<CImGui_Inspector*, EInspectorType::END> m_arrInspectors;
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pDeviceContext{ nullptr };
	CGameInstance* m_pGameInstance = { nullptr };
public:
	virtual void Free() override;
};

NS_END