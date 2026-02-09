#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)

class CImGui_ClientDebug;

class CMainApplication final : public CBase
{
	using Super = CBase;
private:
	CMainApplication();
	virtual ~CMainApplication() = default;
	
	HRESULT Initialize();
private:
	HRESULT Start_Level(ELevelType eStartLevel);
public:
	void Update(const _float fTimeDelta);
	HRESULT Render();
private:
	HRESULT Ready_Static_Prototype();
	HRESULT Ready_Managers();
	HRESULT Ready_Fonts();

private:
	ID3D11Device*			m_pDevice			= { nullptr };
	ID3D11DeviceContext*	m_pDeviceContext	= { nullptr };
public:
	virtual void Free() override;
	static CMainApplication* Create();
private:
	CGameInstance*	m_pGameInstance		= { nullptr };
#ifdef _DEBUG
	CImGui_ClientDebug* m_pDebugGui = { nullptr };
#endif
};

NS_END