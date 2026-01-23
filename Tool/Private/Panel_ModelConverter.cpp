#include "Panel_ModelConverter.h"
#include "GameInstance.h"
#include "Converter.h"
 

USING(Tool)

CPanel_ModelConverter::CPanel_ModelConverter(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CImGui_Panel(pLabel, pOwner, pDevice, pDeviceContext) , m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
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

	XMMATRIX SRTMatirx = XMMatrixScalingFromVector(XMLoadFloat3(&m_vScale)) *
		XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_vRotation.x), XMConvertToRadians(m_vRotation.y), XMConvertToRadians(m_vRotation.z)) *
			XMMatrixTranslationFromVector(XMVectorSetW(XMLoadFloat3(&m_vTranslation), 1.f));

	ImGui::Text("  %f  %f  %f  %f " , SRTMatirx.r[0] );
	


	ImGui::Separator();


	if (ImGui::Button(" Convert Fbx File " ,  ImVec2(256,128)))
	{
		Open_FileDialog(true);
	}

	if (ImGui::Button(" Convert Fbx Files In Floder ", ImVec2(256, 128)))
	{
		Open_FileDialog(false);
	}
	

	ImGui::End();

	return S_OK;
}

void CPanel_ModelConverter::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CPanel_ModelConverter::Open_FileDialog(bool isFile)
{
	if (isFile == true)
	{
		OPENFILENAMEW ofn{};
		_tchar szFile[MAX_PATH] = { 0 };

		ofn.lStructSize = sizeof(OPENFILENAMEW);
		ofn.hwndOwner = g_hWnd;
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrFilter = L"FBX Files (*.fbx)\0*.fbx\0All Files (*.*)\0*.*\0\0";
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		/* 파일을 눌렀을떄  */
		if (::GetOpenFileNameW(&ofn) == TRUE)
		{
			wstring result = szFile;
			// ParsingData
			if (result.ends_with(L".fbx"))
			{
				Convert_FbxFile(szFile);
			}
			else
			{
				MSG_BOX("FBX만 추출 가능합니다");
			}
		}
	}
	else
	{

	}
}

void CPanel_ModelConverter::Convert_FbxFile(const wchar_t* wszFilePath)
{
	/* Pre Matrix */
	std::filesystem::path pathFile{ wszFilePath };

	//CConverter* pConverter = CConverter::Create(m_pDevice, m_pDeviceContext , wszFilePath ,  );
	//pConverter->ReadAndExportFile();
	//Safe_Release(pConverter);
}

CPanel_ModelConverter* CPanel_ModelConverter::Create(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	return  new CPanel_ModelConverter(pLabel, pOwner, pDevice, pDeviceContext);
}

void CPanel_ModelConverter::Free()
{
	Super::Free();

	Safe_Release(m_pGameInstance);

}
