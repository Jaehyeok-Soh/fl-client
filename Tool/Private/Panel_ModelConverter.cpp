#include "Panel_ModelConverter.h"
#include "GameInstance.h"
#include "Converter.h" 

USING(Tool)

CPanel_ModelConverter::CPanel_ModelConverter(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CImGui_Panel(pLabel, pOwner, pDevice, pDeviceContext) , m_pGameInstance(CGameInstance::GetInstance()) 
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CPanel_ModelConverter::Initialize()
{
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
		IID_IFileOpenDialog, reinterpret_cast<void**>(&m_pOpenDialog));

	DWORD dwOptions{};
	m_pOpenDialog->SetOptions(dwOptions | FOS_PICKFOLDERS | FOS_FORCEFILESYSTEM);


	return hr;
}


HRESULT CPanel_ModelConverter::Render(CToolObject* pGo)
{
	ImGui::Begin(m_strLabel.c_str());


	ImGui::SeparatorText(" Pre Matrix Setting ");

	ImGui::Text("---S R T---");

	ImGui::InputFloat3("S" , &m_vScale.x);
	ImGui::InputFloat3("R" , &m_vRotation.x);
	ImGui::InputFloat3("T" , &m_vTranslation.x);


	ImGui::Text(" Matrix ");


	m_SRTMatirx = Matrix::CreateScale(m_vScale) * Matrix::CreateFromYawPitchRoll(m_vRotation * (XM_PI / 180.f)) * Matrix::CreateTranslation(m_vTranslation);

	ImGui::Text("  %.2f  %.2f  %.2f  %.2f " , m_SRTMatirx._11,	m_SRTMatirx._12,  m_SRTMatirx._13,  m_SRTMatirx._14);
	ImGui::Text("  %.2f  %.2f  %.2f  %.2f ",  m_SRTMatirx._21,  m_SRTMatirx._22,  m_SRTMatirx._23,  m_SRTMatirx._24);
	ImGui::Text("  %.2f  %.2f  %.2f  %.2f ",  m_SRTMatirx._31,  m_SRTMatirx._32,  m_SRTMatirx._33,  m_SRTMatirx._34);
	ImGui::Text("  %.2f  %.2f  %.2f  %.2f ",  m_SRTMatirx._41,  m_SRTMatirx._42,  m_SRTMatirx._43,  m_SRTMatirx._44);


	ImGui::Separator();


	if (ImGui::Button(" Convert Fbx Floder (Chose Folder) " ,  ImVec2(256,128)))
		Open_FolderDialog();

	ImGui::End();

	return S_OK;
}

void CPanel_ModelConverter::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CPanel_ModelConverter::Open_FolderDialog()
{
	OPENFILENAMEW ofn{};
	_tchar szFile[MAX_PATH] = { 0 };

	m_pOpenDialog->Show(g_hWnd);

	IShellItem* pItem{nullptr};
	if (SUCCEEDED(m_pOpenDialog->GetResult(&pItem)))
	{
		LPWSTR pPath{nullptr};
		pItem->GetDisplayName(SIGDN_FILESYSPATH , &pPath);
		wcscpy_s(szFile,pPath);
	}

	return;
}

void CPanel_ModelConverter::Convert_FbxFolder(const wchar_t* wszFloderPath)
{
	/* Pre Matrix */
	wstring ResultMsg{};
	std::filesystem::path pathFile{ wszFloderPath };
	CConverter* pConverter = CConverter::Create(m_pDevice, m_pDeviceContext , wszFloderPath, m_SRTMatirx );
	if (FAILED(pConverter->ReadAndExportFile()))
		ResultMsg = pathFile.filename().wstring() + L" Convert is Failed", L"Converter";
	else
		ResultMsg = pathFile.filename().wstring() + L" Convert is Complete", L"Converter";
	Safe_Release(pConverter);

	MessageBox(nullptr,ResultMsg.c_str(),L"Converter",MB_OK);
}

CPanel_ModelConverter* CPanel_ModelConverter::Create(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CPanel_ModelConverter* pPanel = new CPanel_ModelConverter(pLabel, pOwner, pDevice, pDeviceContext);

	if (FAILED(pPanel->Initialize()))
	{
		Safe_Release(pPanel);
		MSG_BOX(" Panel Model Convertor Is Failed To Create ");
		return nullptr;
	}
	return pPanel;
}

void CPanel_ModelConverter::Free()
{
	Super::Free();

	Safe_Release(m_pGameInstance);

}
