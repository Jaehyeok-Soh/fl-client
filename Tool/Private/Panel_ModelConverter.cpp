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
	m_mapPreMatrix.emplace(L"Unreal_To_DirectX", matUECoord   );
	//*Matrix::CreateRotationY(-180.f)

	matUECoord = ::XMMatrixSet(
		1.f, 0.f, 0.f, 0.f,   // 그대로 유지

		0.f, 0.f, -1.f, 0.f,

		0.f, 1.f, 0.f, 0.f,

		// [Row 4]
		0.f, 0.f, 0.f, 1.f
	);
	m_mapPreMatrix.emplace(L"Fmodel_To_DirectX", matUECoord);

	return S_OK;
}


HRESULT CPanel_ModelConverter::Render(CToolObject* pGo)
{
	if (FAILED(Render_ConvertWindow()))
		return E_FAIL;


	if (FAILED(Render_FunctionWindow()))
		return E_FAIL;


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

	DWORD dwOptions{};


	if (!m_isRecursiveDirectory)
		m_pOpenDialog->SetOptions(dwOptions | FOS_FORCEFILESYSTEM);
	else
		m_pOpenDialog->SetOptions(dwOptions | FOS_FORCEFILESYSTEM | FOS_PICKFOLDERS);


	m_pOpenDialog->Show(g_hWnd);

	IShellItem* pItem{nullptr};
	if (SUCCEEDED(m_pOpenDialog->GetResult(&pItem)))
	{


		LPWSTR pPath{nullptr};
		pItem->GetDisplayName(SIGDN_FILESYSPATH , &pPath);

		path FilePath = pPath;
		if(!m_isRecursiveDirectory)
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

	vector<wstring> vecCheckFolderPath{};

	if (m_isRecursiveDirectory)
	{
		/* 폴더 순회 O */
		/* 이 폴더에 Fbx 파일이 있는지 없는지 Check를 한다 */
		for (auto& Path : std::filesystem::recursive_directory_iterator(wszFloderPath))
		{
			path	pathFile{Path};
			if (std::filesystem::is_directory(pathFile))
				continue;
			/* Fbx 파일 검사 */
			if (pathFile.extension().wstring() != g_wszModelExtension)
				continue;

			path    pathParent = path(Path);
			pathParent._Remove_filename_and_separator();

			bool isCheck{false};
			for (auto& wstrCheckFloderPath : vecCheckFolderPath)
				if (wstrCheckFloderPath == pathParent)
				{
					isCheck = true;
					break;
				}

			if (isCheck)
			{
				/* 폴더가 이미 진행된 폴더라면? pass*/
				continue;
			}
			else
				vecCheckFolderPath.push_back(pathParent);

			CConverter* pConverter = CConverter::Create(m_pDevice, m_pDeviceContext, SOLUTION_DIR, pathParent.c_str() , m_SRTMatirx);
			if (FAILED(pConverter->ReadAndExport()))
				ResultMsg = pathFile.filename().wstring() + L" Convert is Failed", L"Converter";
			else
			{
				ResultMsg += pathFile.filename().wstring() + L" Convert is Complete", L"Converter";
			}
			Safe_Release(pConverter);
		}
	}
	else
	{
		/* 폴더 순회 X */
		
		CConverter* pConverter = CConverter::Create(m_pDevice, m_pDeviceContext, SOLUTION_DIR, wszFloderPath, m_SRTMatirx);
		if (FAILED(pConverter->ReadAndExport()))
			ResultMsg = pathFile.filename().wstring() + L" Convert is Failed", L"Converter";
		else
			ResultMsg = pathFile.filename().wstring() + L" Convert is Complete", L"Converter";
		Safe_Release(pConverter);

	}


	MessageBox(nullptr,ResultMsg.c_str(),L"Converter",MB_OK);
}

HRESULT CPanel_ModelConverter::Render_ConvertWindow()
{
	ImGui::Begin(m_strLabel.c_str());


	ImGui::SeparatorText(" Pre Matrix Setting ");

	ImGui::SeparatorText(" Preset Matrix List ");


	string strSelectPreMatrix = Engine_Utils::ToString(m_wstrSelectPreMatrix);

	INT32 iIndex{ 0 };


	if (ImGui::BeginCombo("Preset Matrix List##Preset_Matrix_Combo", strSelectPreMatrix.c_str()))
	{
		for (auto& Pair : m_mapPreMatrix)
		{
			string strPreMatrixName = Engine_Utils::ToString(Pair.first);
			bool isSelected = (strSelectPreMatrix == strPreMatrixName);
			if (ImGui::Selectable(strPreMatrixName.c_str(), isSelected))
			{
				m_wstrSelectPreMatrix = Engine_Utils::ToWString(strPreMatrixName);
				m_SRTMatirx = m_mapPreMatrix[m_wstrSelectPreMatrix];
			}
			if (isSelected == true)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}

	ImGui::Separator();


	ImGui::Text("---S R T---");

	ImGui::InputFloat3("S", &m_vScale.x);
	ImGui::InputFloat3("R", &m_vRotation.x);
	ImGui::InputFloat3("T", &m_vTranslation.x);

	if (ImGui::Button("Create Matrix By SRT", ImVec2(256, 32)))
		m_SRTMatirx = Matrix::CreateScale(m_vScale) * Matrix::CreateFromYawPitchRoll(m_vRotation * (XM_PI / 180.f)) * Matrix::CreateTranslation(m_vTranslation);

	ImGui::Text(" Matrix ");

	ImGui::Text("  %.2f  %.2f  %.2f  %.2f ", m_SRTMatirx._11, m_SRTMatirx._12, m_SRTMatirx._13, m_SRTMatirx._14);
	ImGui::Text("  %.2f  %.2f  %.2f  %.2f ", m_SRTMatirx._21, m_SRTMatirx._22, m_SRTMatirx._23, m_SRTMatirx._24);
	ImGui::Text("  %.2f  %.2f  %.2f  %.2f ", m_SRTMatirx._31, m_SRTMatirx._32, m_SRTMatirx._33, m_SRTMatirx._34);
	ImGui::Text("  %.2f  %.2f  %.2f  %.2f ", m_SRTMatirx._41, m_SRTMatirx._42, m_SRTMatirx._43, m_SRTMatirx._44);


	ImGui::Separator();


	if (ImGui::Button(" Convert Fbx Floder (Chose Folder) ", ImVec2(256, 128)))
		Open_FolderDialog();

	ImGui::Checkbox(" Recursive Directory##Recursive_Directory_CheckBox ",&m_isRecursiveDirectory);

	ImGui::End();

	return S_OK;



	return S_OK;
}

HRESULT CPanel_ModelConverter::Render_FunctionWindow()
{
	ImGui::Begin(" Function Window ");

	ImGui::Separator();

	if (ImGui::Button(" Check None Export Fbx Model  ", ImVec2(256, 32)))
	{
		OPENFILENAMEW ofn{};
		_tchar szFile[MAX_PATH] = { 0 };

		DWORD dwOptions{};
		m_pOpenDialog->SetOptions(dwOptions | FOS_PICKFOLDERS | FOS_FORCEFILESYSTEM);


		m_pOpenDialog->Show(g_hWnd);

		IShellItem* pItem{ nullptr };
		if (SUCCEEDED(m_pOpenDialog->GetResult(&pItem)))
		{
			LPWSTR pPath{ nullptr };
			pItem->GetDisplayName(SIGDN_FILESYSPATH, &pPath);

			path FilePath = pPath;
			FilePath.remove_filename();
			Check_NoneExport_FbxModel(FilePath.c_str());
			m_wstrCheckNoneExportFbxModelFloderPath = FilePath;
		}
	}


	if (!m_vecNoneExportFbxModelPath.empty())
	{
		ImGui::Begin( " None Export Fbx Model Info ");

		UINT32 iTotalNum = ENUM_TO_UINT(m_vecNoneExportFbxModelPath.size());

		wstring& wstrPath = m_vecNoneExportFbxModelPath[m_iCurWorkCheckNoneExportFbxModelIndex];
		wstring  wwstrFileName = path(wstrPath).filename();
		string   strName = path(wstrPath).filename().stem().string();

		ImGui::Text("File [%d / %d]", m_iCurWorkCheckNoneExportFbxModelIndex + 1 , iTotalNum );

		if (ImGui::BeginCombo(" File List#Combo_ExportFile", strName.c_str() ))
		{
			string strCurFileName{""};
			for (UINT32 i = 0; i < iTotalNum; ++i)
			{
				strCurFileName = Engine_Utils::GetFileNameWithoutExtension(Engine_Utils::ToString(m_vecNoneExportFbxModelPath[i]));

				bool isSelect = (m_iCurWorkCheckNoneExportFbxModelIndex == i);

				if (ImGui::Selectable(strCurFileName.c_str(), isSelect))
					m_iCurWorkCheckNoneExportFbxModelIndex = i;
				if (isSelect == true)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		ImGui::TextWrapped("Path : %ls", wstrPath.c_str());

		ImGui::TextWrapped("Name : %s", strName.c_str());


		if (ImGui::Button("Open File"))
			ShellExecuteW(NULL, L"open", L"explorer.exe", (L"/select," + wstrPath).c_str(), NULL, SW_SHOWNORMAL);

		if (ImGui::Button(" Pre ")) {
			if (m_iCurWorkCheckNoneExportFbxModelIndex == 0)
				m_iCurWorkCheckNoneExportFbxModelIndex = iTotalNum - 1 ;
			else
				m_iCurWorkCheckNoneExportFbxModelIndex--;
		}

		ImGui::SameLine();

		if (ImGui::Button(" Next ")) {
			m_iCurWorkCheckNoneExportFbxModelIndex++ ;
			if (m_iCurWorkCheckNoneExportFbxModelIndex >= iTotalNum)
				m_iCurWorkCheckNoneExportFbxModelIndex = 0;
		}


		if (ImGui::Button(" Check Export File "))
		{
			path	pathFile{wstrPath};
			pathFile._Remove_filename_and_separator();

			bool isExport{false};

			for (auto& FindExportPath : std::filesystem::directory_iterator(pathFile))
			{
				path Path{ FindExportPath };
				wstring wstrExt = Path.extension();
				if (wstrExt != L".fbx")
					continue;
				string wstrExportFbxName = path(Path.filename()).stem().string();

				if (isExport = (wstrExportFbxName == strName))
					break;
			}

			if (isExport)
				MSG_BOX("This Model is Complete Export Fbx File , Work Next File");
			else
				MSG_BOX("This Model is None Export");
		}
		
		if (ImGui::Button(" End Work "))
		{
			m_vecNoneExportFbxModelPath.clear();
			m_iCurWorkCheckNoneExportFbxModelIndex = 0;
		}


		ImGui::End();
	}
	else
		m_wstrCheckNoneExportFbxModelFloderPath.clear();

	ImGui::Separator();

	ImGui::End();
	return S_OK;
}

void CPanel_ModelConverter::Check_NoneExport_FbxModel(const wchar_t* wszFloderPath)
{
	if (wszFloderPath == nullptr) return;

	path pathCheckFloder{wszFloderPath};

	if (pathCheckFloder.has_extension()) return;

	m_vecNoneExportFbxModelPath.clear();
	m_iCurWorkCheckNoneExportFbxModelIndex = 0;

	for (auto& CheckPath : std::filesystem::recursive_directory_iterator(pathCheckFloder))
	{
		path Path{CheckPath};
		wstring wstrPath = Path.wstring();
		wstring wstrExt  = Path.extension();
		wstring wstrFileName = path(Path.filename()).stem();
		

		if (wstrExt != L".uemodel")
			continue;

		wstring wstrTarget = L"\\";
		size_t Pos_Target = wstrPath.rfind(wstrTarget);
		if (Pos_Target == std::wstring::npos)
			continue;

		wstring wstrParentFolderPath = wstrPath.erase( Pos_Target , wstrPath.length()) ;

		bool isExport{false};


		for(auto& FindExportPath : std::filesystem::directory_iterator(wstrParentFolderPath) )
		{ 
			path Path{ FindExportPath };
			wstring wstrExt = Path.extension();
			if (wstrExt != L".fbx")
				continue;
			wstring wstrExportFbxName = path(Path.filename()).stem();
			
			if ( isExport  = (wstrExportFbxName == wstrFileName ))
				break;
		}
		if (isExport)
			continue;
		else
			m_vecNoneExportFbxModelPath.push_back(Path);
	}
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
