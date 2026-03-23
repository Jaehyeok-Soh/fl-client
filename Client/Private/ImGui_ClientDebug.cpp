#include "pch.h"
#include "ImGui_ClientDebug.h"
#include "MainPlayer.h"
#include "ImGui_PlayerInspector.h"
#include "ImGui_GlobalInspector.h"
#include "ImGui_PoolInspector.h"
#include "ImGui_TextureDebugInspector.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CImGui_ClientDebug)

CImGui_ClientDebug::CImGui_ClientDebug()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
	m_arrInspectors.fill(nullptr);
}

HRESULT CImGui_ClientDebug::Initialize(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	if (m_bInitialized == true)
		return S_OK;

	m_hWnd = hWnd;
	m_pDevice = pDevice;
	m_pDeviceContext = pDeviceContext;
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);	

	::IMGUI_CHECKVERSION();
	ImGuiContext* pImGuiContext = ImGui::CreateContext();
	if (pImGuiContext == nullptr)
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

	if (ImGui_ImplWin32_Init(m_hWnd) == false)
		return E_FAIL;

	if (ImGui_ImplDX11_Init(m_pDevice, m_pDeviceContext) == false)
		return E_FAIL;

	if (FAILED(Ready_Inspector()))
		return E_FAIL;

	m_bInitialized = true;
	return S_OK;
}

HRESULT CImGui_ClientDebug::Ready_Inspector()
{
	m_arrInspectors[EInspectorType::Player] = CImGui_PlayerInspector::Create();
	m_arrInspectors[EInspectorType::Global] = CImGui_GlobalInspector::Create();
	m_arrInspectors[EInspectorType::TextureDebug] = CImGui_TextureDebugInspector::Create();
	m_arrInspectors[EInspectorType::PoolDebug] = CImGui_PoolInspector::Create();
	//m_arrInspectors[EInspectorType::Light] = CImGui
	return S_OK;
}

void CImGui_ClientDebug::Render()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	m_arrInspectors[EInspectorType::Player]->Render(m_pPlayer);
	m_arrInspectors[EInspectorType::Global]->Render(nullptr);
	m_arrInspectors[EInspectorType::TextureDebug]->Render(nullptr);
	m_arrInspectors[EInspectorType::PoolDebug]->Render(nullptr);

	ImGuiIO& io = ImGui::GetIO();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}

void CImGui_ClientDebug::Free()
{
	for (CImGui_Inspector* pElement : m_arrInspectors)
		Safe_Release(pElement);
	m_arrInspectors.fill(nullptr);
	
	::ImGui_ImplDX11_Shutdown();
	::ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pGameInstance);
	Super::Free();
}
