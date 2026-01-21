#include "Tool_Defines.h"
#include "ImGui_Dockspace_MenuBar.h"
#include "ImGui_Dockspace_TabBar.h"
#include "ToolObject.h"
#include "Level_Loading.h"
#include "ImGui_ToolManager.h"

IMPLEMENT_SINGLETON(CImGui_ToolManager)

CImGui_ToolManager::CImGui_ToolManager()
{
}

HRESULT CImGui_ToolManager::Initialize_ToolManager(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, ELevelType eStartLevel)
{
	m_pDevice = pDevice;
	m_hWnd = hWnd;
	m_pDeviceContext = pDeviceContext;
	m_pGameInstance = CGameInstance::GetInstance();

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
	Safe_AddRef(m_pGameInstance);

	IMGUI_CHECKVERSION();
	ImGuiContext* pImGuiContext = ImGui::CreateContext();
	if (!pImGuiContext)
		return E_FAIL;

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui::StyleColorsDark();

	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	if (!ImGui_ImplWin32_Init(m_hWnd))
		return E_FAIL;

	if (!ImGui_ImplDX11_Init(m_pDevice, m_pDeviceContext))
		return E_FAIL;

	if (FAILED(Ready_DockSpace_Elements(eStartLevel)))
		return E_FAIL;

	return S_OK;
}

void CImGui_ToolManager::RayUpdate()
{
	_float4 vNDC = { 0.f, 0.f, 0.f, 1.f };
	// 마우스가 Viewport안에 없을시에 실패 연산 안함
	if (Calculate_ViewportUV(vNDC.x, vNDC.y) == false)
		return;

	m_pGameInstance->PickingUpdate_ForTool(vNDC);
}

void CImGui_ToolManager::Render_Begin()
{
	// 모든 Render 이후 Gui 렌더
	// 이때 BackBuffer 복사해오기
	m_pGameInstance->Copy_BackBufferTexture(&m_pViewportFrameTexture);

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void CImGui_ToolManager::ImGuizmo_Render_Begin()
{
	ImGuizmo::BeginFrame();
}

void CImGui_ToolManager::ImGuizmo_Render(CToolObject* pSelectedObject)
{
	if (pSelectedObject)
	{
		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist();
		ImGuizmo::SetGizmoSizeClipSpace(0.07f);
		ImGuizmo::SetRect(m_vViewportBounds[0].x, m_vViewportBounds[0].y
			, m_vViewportBounds[1].x - m_vViewportBounds[0].x, m_vViewportBounds[1].y - m_vViewportBounds[0].y);

		const _float4x4& matView = m_pGameInstance->Get_ViewMatrix();
		const _float4x4& matProj = m_pGameInstance->Get_ProjMatrix();

		// Object Transform
		CTransform* pTransform = pSelectedObject->Get_Component<CTransform>();
		_float4x4 matWorld = pTransform->Get_WorldMatrix();

		// snapping
		_bool bSnap = KEY_BUTTON_HOLD(DIK_LSHIFT);
		_float fSnapValue = 0.5f;

		ImGuizmo::OPERATION operation = {};

		switch (m_eGuizmoState)
		{
		case EGuizmoState::TRANSLATION:
			operation = ImGuizmo::TRANSLATE;
			break;
		case EGuizmoState::ROTATION:
			operation = ImGuizmo::ROTATE;
			break;
		case EGuizmoState::SCALE:
			operation = ImGuizmo::SCALE;
			break;
		}

		// snap to 45 degrees for rotation
		if (operation == ImGuizmo::OPERATION::ROTATE)
			fSnapValue = 45.0f;

		float snapValues[3] = { fSnapValue, fSnapValue, fSnapValue };

		ImGuizmo::Manipulate(*matView.m, *matProj.m, operation
			, ImGuizmo::WORLD, *matWorld.m, nullptr, bSnap ? snapValues : nullptr);

		if (ImGuizmo::IsUsing())
		{
			// float translation[3];
			// float rotation[3];
			// float scale[3];
			// ImGuizmo::DecomposeMatrixToComponents(*matWorld.m, translation, rotation, scale);

			pTransform->Set_Info(TRANSFORM_INFO_STATE::RIGHT, *reinterpret_cast<_float4*>(&matWorld.m[0]));
			pTransform->Set_Info(TRANSFORM_INFO_STATE::UP, *reinterpret_cast<_float4*>(&matWorld.m[1]));
			pTransform->Set_Info(TRANSFORM_INFO_STATE::LOOK, *reinterpret_cast<_float4*>(&matWorld.m[2]));
			pTransform->Set_Info(TRANSFORM_INFO_STATE::POS, *reinterpret_cast<_float4*>(&matWorld.m[3]));
		}
	}
	else
		m_eGuizmoState = EGuizmoState::TRANSLATION;
}

bool CImGui_ToolManager::Calculate_ViewportUV(OUT _float &fU, OUT _float& fV)
{
	POINT			ptMouse;

	::GetCursorPos(&ptMouse);

	::ScreenToClient(m_hWnd, &ptMouse);

	// m_vViewportBounds는 실제 정렬된 Offset 검정색 영역을 제외한 실제 이미지 영역
	const _float2& vMin = m_vViewportBounds[0];
	const _float2& vMax = m_vViewportBounds[1];

	// Imgui viewport 밖이면?
	if (ptMouse.x < vMin.x || ptMouse.y < vMin.y ||
		ptMouse.x > vMax.x || ptMouse.y > vMax.y)
	{
		m_bOutofViewport = true;
		return false;
	}

	const _float fWidth = vMax.x - vMin.x;
	const _float fHeight = vMax.y - vMin.y;

	fU = (ptMouse.x - vMin.x) / (fWidth * 0.5f) - 1;
	fV = (ptMouse.y - vMin.y) / (fHeight * -0.5f) + 1;

	m_bOutofViewport = false;
	return true;
}

void CImGui_ToolManager::Update_Dockspace()
{
	m_Flag = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	m_Flag |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	m_Flag |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	/*else
		m_DockspaceFlags &= ~ImGuiDockNodeFlags_PassthruCentralNode;*/

	if (m_DockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode)
		m_Flag |= ImGuiWindowFlags_NoBackground;
	
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
}

void CImGui_ToolManager::Render_Dockspace()
{
	if (m_eState == State::Disable)
		return;

	Update_Dockspace();

	_bool bActive = static_cast<bool>(m_eState);
	ImGui::Begin("ToolApplication", &bActive, m_Flag);
	ImGui::PopStyleVar();
	ImGui::PopStyleVar(2);

	// Submit the DockSpace
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("DockingSpace");
		Show_TabBar(bActive);
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), m_DockspaceFlags);
	}
	else
	{
		ImGuiIO& io = ImGui::GetIO();
		ImGui::Text("ERROR: Docking is not enabled! See Demo > Configuration.");
		ImGui::Text("Set io.ConfigFlags |= ImGuiConfigFlags_DockingEnable in your code, or ");
		ImGui::SameLine(0.0f, 0.0f);
		if (ImGui::SmallButton("click here"))
			io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	}

	Show_Menubar(bActive);
}

void CImGui_ToolManager::Render_Viewport(CToolObject* pSelectedObject)
{
	// 창 스타일 설정 및 이름
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
	ImGui::Begin("Scene");

#pragma region Legacy
	//auto viewportMinRegion = ImGui::GetWindowContentRegionMin(); // 씬뷰의 최소 좌표
	//auto viewportMaxRegion = ImGui::GetWindowContentRegionMax(); // 씬뷰의 최대 좌표
	//auto viewportOffset = ImGui::GetWindowPos(); // 씬뷰의 위치

	////lefttop, rightbottom 좌표 저장
	//const _uint& LeftTop = 0;
	//m_vViewportBounds[LeftTop] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
	//const _uint& RightBottom = 1;
	//m_vViewportBounds[RightBottom] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

	//// 키보드 활성화 및 마우스 활성화 여부 체크
	//m_bViewprotFocused = ImGui::IsWindowFocused();
	//m_bViewprotHovered = ImGui::IsWindowHovered();

	//// to do : mouse, keyboard event
	//// 

	//ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
	//m_vViewportSize = { viewportPanelSize.x, viewportPanelSize.y };
	//// BackBuffer의 이미지를 들고와 ImGuiImagme로 "복사"
	//ImGui::Image((ImTextureID)m_pViewportFrameSRV, ImVec2{ m_vViewportSize.x, m_vViewportSize.y }
	//, ImVec2{ 0, 0 }, ImVec2{ 1, 1 });
#pragma endregion

	// 패널의 영역
	const ImVec2 vAvail = ImGui::GetContentRegionAvail();
	const float fPanelWidth = vAvail.x;
	const float fPanelHeight = vAvail.y;

	// 종횡비 고정!
	const float fAspect = g_fAspectio;
	float fViewWidth = fPanelWidth;
	float fViewHeight = fViewWidth / fAspect;
	
	// 세로가 더 커지면 가로를 줄임
	if (fViewHeight > fPanelHeight)
	{
		fViewHeight = fPanelHeight;
		fViewWidth = fPanelHeight * fAspect;
	}

	// viewort크기에 따라 aspectio를 유지하며 여백 공간을 검정색으로 표현해내기 위함
	// 아래 Offset은 Bar를 제외한 실제 이미지가 출력된 LeftTop Offset
	const float fOffset_X = (fPanelWidth - fViewWidth) * 0.5f;
	const float fOffset_Y = (fPanelHeight - fViewHeight) * 0.5f;

	// 오프셋만큼 이미지 이동
	const ImVec2 srcCursor = ImGui::GetCursorPos();
	ImGui::SetCursorPos(ImVec2{ srcCursor.x + fOffset_X, srcCursor.y + fOffset_Y });

	// 실제 스크린 좌표 사각형 ( 절대 렌더 전에 연산할것, 렌더 후에 연산하니까 이상한값 들어감 )
	const ImVec2 vImageScreenMin = ImGui::GetCursorScreenPos();
	const ImVec2 vImageScreenMax = ImVec2{ vImageScreenMin.x + fViewWidth, vImageScreenMin.y + fViewHeight };

	// 뷰포트 렌더
	ImGui::Image((ImTextureID)m_pViewportFrameSRV,			// 그릴 SRV, 현재는 BackBuffer 복사
		ImVec2{ fViewWidth, fViewHeight }					// Viewport 크기
		, ImVec2{ 0, 0 }, ImVec2{ 1, 1 });					// uv 좌표

	// 키보드 활성화 및 마우스 활성화 여부 체크
	m_bViewprotFocused = ImGui::IsWindowFocused();
	m_bViewprotHovered = ImGui::IsWindowHovered();

	// 뷰포트 경계 ( 실제 이미지 그려진 영역, 일전에 바까지 포함한걸 다 했어서 난리남 )
	const _uint& LeftTop = 0;
	m_vViewportBounds[LeftTop] = { vImageScreenMin.x, vImageScreenMin.y };
	const _uint& RightBottom = 1;
	m_vViewportBounds[RightBottom] = { vImageScreenMax.x, vImageScreenMax.y };

	m_vViewportSize = { fViewWidth, fViewHeight };
	m_vViewportOffset = { fOffset_X, fOffset_Y };
	
	ImGuizmo_Render(pSelectedObject);

	ImGui::End();
	ImGui::PopStyleVar();

	RayUpdate();
}

HRESULT CImGui_ToolManager::Show_TabBar(_bool bActive)
{
	return m_pTabBar->Render(nullptr);
}

HRESULT CImGui_ToolManager::Show_Menubar(_bool bActive)
{
	return m_pMenuBar->Render(nullptr);
}

HRESULT CImGui_ToolManager::Ready_DockSpace_Elements(ELevelType eStartLevel)
{
	if (!(m_pMenuBar = CImGui_Dockspace_MenuBar::Create("MenuBar##Dockspace", m_pDevice, m_pDeviceContext)))
		return E_FAIL;

	if (!(m_pTabBar = CImGui_Dockspace_TabBar::Create("TabBar##Dockspace", m_pDevice, m_pDeviceContext, eStartLevel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CImGui_ToolManager::Ready_Events()
{
	return S_OK;
}

void CImGui_ToolManager::Render_End()
{
	ImGuiIO& io = ImGui::GetIO();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}

HRESULT CImGui_ToolManager::CreateOrResizeViewportFrameTargets(_uint iWidth, _uint iHeight)
{
	Safe_Release(m_pViewportFrameTexture);
	Safe_Release(m_pViewportFrameSRV);

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = iWidth;
	textureDesc.Height = iHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;          // 백버퍼와 동일
	textureDesc.SampleDesc.Count = 1;                          // 백버퍼와 동일(여기선 1)
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	if (FAILED(m_pDevice->CreateTexture2D(&textureDesc, nullptr, &m_pViewportFrameTexture)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateShaderResourceView(m_pViewportFrameTexture, nullptr, &m_pViewportFrameSRV)))
		return E_FAIL;

	return S_OK;
}

CImGui_ToolManager* CImGui_ToolManager::Create(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, ELevelType eStartLevel)
{
    CImGui_ToolManager* pInstance = new CImGui_ToolManager();
    if (FAILED(pInstance->Initialize_ToolManager(hWnd, pDevice, pDeviceContext, eStartLevel)))
    {
        MSG_BOX("CImGui_ToolManager::Create");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CImGui_ToolManager::Free()
{
	::ImGui_ImplDX11_Shutdown();
	::ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	Safe_Release(m_pMenuBar);
	Safe_Release(m_pTabBar);
	Safe_Release(m_pViewportFrameTexture);
	Safe_Release(m_pViewportFrameSRV);
	Safe_Release(m_pGameInstance);
    Safe_Release(m_pDeviceContext);
    Safe_Release(m_pDevice);
    Super::Free();
}
