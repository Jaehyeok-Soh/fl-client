#pragma once
#include "Tool_Defines.h"
#include "Base.h"
#include "GameInstance.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Tool)

class CToolObject;

class CImGui_ToolManager final : public CBase
{
	DECLARE_SINGLETON(CImGui_ToolManager)
	using Super = CBase;
public:
	enum class State : _uint
	{
		Disable = 0,
		Active,
		Destroy,
	};
	enum EGuizmoState : _uint
	{
		TRANSLATION,
		ROTATION,
		SCALE
	};
private:
	CImGui_ToolManager();
	virtual ~CImGui_ToolManager() = default;
public:
	HRESULT Initialize_ToolManager(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, ELevelType eStartLevel);
	HRESULT CreateOrResizeViewportFrameTargets(_uint iWidth, _uint iHeight);

	void RayUpdate();
	void Render_Begin();
	void ImGuizmo_Render_Begin();
	void ImGuizmo_Render(CToolObject* pSelectedObject);
	void Render_Dockspace();
	void Render_Viewport(CToolObject* pSelectedObject);
	void Render_End();

	_bool IsOutofViewport() const {	return m_bOutofViewport; }
	HRESULT Ready_Events();
	EGuizmoState Get_GuizmoState() const { return m_eGuizmoState; }
	void Set_GuizmoState(EGuizmoState eState) { m_eGuizmoState = eState; }

	_bool		Get_MousePosInViewPort() const;
	const Vec2& Get_CurViewportSize() const { return m_vViewportSize; }
	const Vec2& Get_ViewportMousePos() const { return m_vViewportMousePos; }
	POINT Get_ViewportMousePos_Point() const { return POINT{ (LONG)m_vViewportMousePos.x, (LONG)m_vViewportMousePos.y }; }
	POINT Get_CalculatedMousePos_Point() const { return POINT{ (LONG)m_vCalcMousePos.x, (LONG)m_vCalcMousePos.y }; }
private:	
	bool Calculate_ViewportUV(OUT _float& fU, OUT _float& fV);
	void Update_Dockspace();
	HRESULT Show_TabBar(_bool bActive);
	HRESULT Show_Menubar(_bool bActive);
	HRESULT Ready_DockSpace_Elements(ELevelType eStartLevel);
	void Calc_ViewportMousePos();

private:
	_bool m_bViewprotFocused = { false };
	_bool m_bViewprotHovered = { false };
	ImGuiWindowFlags m_Flag = { ImGuiWindowFlags_None };
	ImGuiDockNodeFlags m_DockspaceFlags = { ImGuiDockNodeFlags_None };
	State m_eState = { State::Active };
private:
	HWND		m_hWnd = { NULL };
	_bool		m_bOutofViewport = { false };
	EGuizmoState m_eGuizmoState = { EGuizmoState::TRANSLATION };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pDeviceContext{ nullptr };
	CGameInstance* m_pGameInstance = { nullptr };
	class CImGui_Dockspace_MenuBar* m_pMenuBar = { nullptr };
	class CImGui_Dockspace_TabBar* m_pTabBar = { nullptr };
private:
	ID3D11Texture2D* m_pViewportFrameTexture = { nullptr };
	ID3D11ShaderResourceView* m_pViewportFrameSRV = { nullptr };
	Vec2 m_vViewportOffset = {};
	Vec2 m_vViewportSize = {};
	Vec2 m_vViewportBounds[2] = {};
	Vec2 m_vViewportMousePos = {};
	_float m_fXScale = {};
	_float m_fYScale = {};
	Vec2 m_vCalcMousePos = {};
public:
	static CImGui_ToolManager* Create(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, ELevelType eStartLevel);
	virtual void Free() override;
};

NS_END