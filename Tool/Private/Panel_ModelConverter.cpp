#include "pch.h"
#include "Panel_ModelConverter.h"
#include "Converter.h" 
#include "GameInstance.h"
#include "Engine_Utils.h"

USING(Tool)

CPanel_ModelConverter::CPanel_ModelConverter(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CImGui_Panel(pLabel, pOwner, pDevice, pDeviceContext) , m_pGameInstance(CGameInstance::GetInstance()) 
	, m_wstrSelectPreMatrix{L"Identity"}
{
	Safe_AddRef(m_pGameInstance);
	m_mapPreMatrix.clear();
}

HRESULT CPanel_ModelConverter::Initialize()
{
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
		IID_IFileOpenDialog, reinterpret_cast<void**>(&m_pOpenDialog));

	DWORD dwOptions{};
	m_pOpenDialog->SetOptions(dwOptions | FOS_FORCEFILESYSTEM);


	if (FAILED(Ready_PreMatirxPreset()))
		return E_FAIL;

	return hr;
}

HRESULT CPanel_ModelConverter::Ready_PreMatirxPreset()
{
	m_mapPreMatrix.emplace( L"Identity", Matrix::Identity );

	Matrix matUECoord = ::XMMatrixSet(
		1.f, 0.f, 0.f, 0.f,		// x' = x
		0.f, 0.f, 1.f, 0.f,		// y' = z
		0.f, -1.f, 0.f, 0.f,	// z' = -y
		0.f, 0.f, 0.f, 1.f
	);
	m_mapPreMatrix.emplace(L"Unreal_To_DirectX", matUECoord);


	return S_OK;
}


HRESULT CPanel_ModelConverter::Render(CToolObject* pGo)
{
	ImGui::Begin(m_strLabel.c_str());


	ImGui::SeparatorText(" Pre Matrix Setting ");

	ImGui::SeparatorText(" Preset Matrix List ");

	
	string strSelectPreMatrix = Engine_Utils::ToString(m_wstrSelectPreMatrix);

	INT32 iIndex{0};


	if (ImGui::BeginCombo("Preset Matrix List##Preset_Matrix_Combo", strSelectPreMatrix.c_str()))
	{
		for (auto& Pair : m_mapPreMatrix)
		{
			string strPreMatrixName = Engine_Utils::ToString(Pair.first);
			bool isSelected = (strSelectPreMatrix == strPreMatrixName);
			if (ImGui::Selectable(strPreMatrixName.c_str(), isSelected))
			{
				m_wstrSelectPreMatrix = Engine_Utils::ToWString(strPreMatrixName);
				m_SRTMatirx  =  m_mapPreMatrix[m_wstrSelectPreMatrix];
			}
			if (isSelected == true)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}

	ImGui::Separator();


	ImGui::Text("---S R T---");

	ImGui::InputFloat3("S" , &m_vScale.x);
	ImGui::InputFloat3("R" , &m_vRotation.x);
	ImGui::InputFloat3("T" , &m_vTranslation.x);

	if(ImGui::Button("Create Matrix By SRT" , ImVec2(256,32)))
		m_SRTMatirx = Matrix::CreateScale(m_vScale) * Matrix::CreateFromYawPitchRoll(m_vRotation * (XM_PI / 180.f)) * Matrix::CreateTranslation(m_vTranslation);

	ImGui::Text(" Matrix ");

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

		path FilePath = pPath;
		FilePath.remove_filename();
		Convert_FbxFolder(FilePath.c_str());
	}

	return;
}

void CPanel_ModelConverter::Convert_FbxFolder(const wchar_t* wszFloderPath)
{
	/* Pre Matrix */
	wstring ResultMsg{};
	std::filesystem::path pathFile{ wszFloderPath };
	CConverter* pConverter = CConverter::Create(m_pDevice, m_pDeviceContext, SOLUTION_DIR, wszFloderPath, m_SRTMatirx);
	if (FAILED(pConverter->ReadAndExport()))
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


	m_mapPreMatrix.clear();

	Safe_Release(m_pGameInstance);

}
