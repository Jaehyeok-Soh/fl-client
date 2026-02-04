#pragma once
#include "ImGui_Panel.h"

NS_BEGIN(Engine)

class CCameraMan;
class CCamera;

NS_END


NS_BEGIN(Tool)

class CMapToolManager;

class CPanel_MapTool :
	public CImGui_Panel
{
	using Super = CImGui_Panel;
protected:
	explicit CPanel_MapTool(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CPanel_MapTool() = default;
private:
	HRESULT	Initialize();
public:
	virtual HRESULT Render(CToolObject* pGo)override;
	virtual void Update(const _float fTimeDelta)override;
	HRESULT	Update_MapObjectList();
private:
	HRESULT Render_RaySetting();
	HRESULT Render_CameraSetting();
	HRESULT Render_PreViewInfo();
private:

	CGameInstance* m_pGameInstance{ nullptr };
	CMapToolManager* m_pMapToolManager{ nullptr };

private:
	ID3D11Device*			m_pDevice{ nullptr };
	ID3D11DeviceContext*	m_pContext{ nullptr };

	CCamera*				m_pCamera{ nullptr };
	CCameraMan*				m_pCameraMan{ nullptr };
private:
	float					m_fFixRayRange{};
	float					m_fFixMouseWheelSpeed{};
public:
	static  CPanel_MapTool* Create(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

NS_END