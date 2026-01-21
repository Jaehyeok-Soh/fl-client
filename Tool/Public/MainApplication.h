#pragma once
#include "Tool_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Tool)

class CImGui_ToolManager;

class CMainApplication : public CBase
{
	using Super = CBase;
public:
	
private:
	CMainApplication();
	virtual ~CMainApplication() = default;

	HRESULT Initialize();
public:
	void Update(const _float fTimeDelta);
	HRESULT Render();
private:
	HRESULT Start_Level(ELevelType eStartLevel);
	HRESULT Ready_Static_Prototype();
	HRESULT Ready_GuiManager(_uint iWidth, _uint iHeight, ELevelType eStartLevel);
private:
	CImGui_ToolManager* m_pImGuiManager = { nullptr };
	ID3D11Device* m_pDevice					= { nullptr };
	ID3D11DeviceContext* m_pDeviceContext	= { nullptr };
	Engine::CGameInstance* m_pGameInstance = { nullptr };
public:
	virtual void Free() override;
	static CMainApplication* Create();
};

NS_END
