#include "pch.h"
#include "File.h"
#include "GameInstance.h"
#include "Folder.h"
#include "Engine_Utils.h"


CFile::CFile()
	: m_tInfo{}, m_eFileType(FILE_TYPE::NONE)
{
}

CFile::~CFile()
{
}

HRESULT CFile::Initialize(const wstring& wstrPath, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    m_tInfo = FILE_INFO(wstrPath);
	return S_OK;
}

CFile* CFile::Create(const wstring& wstrPath, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFile* pFile = new CFile;

	if (FAILED(pFile->Initialize(wstrPath,pDevice,pContext)))
	{
		Safe_Release(pFile);
		MSG_BOX("File is Failed To Create");
		return nullptr;
	}

	return pFile;

}

void CFile::Free()
{
	__super::Free();


}
