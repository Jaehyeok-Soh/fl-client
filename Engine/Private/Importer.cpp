#include "Importer.h"

CImporter::CImporter()
{
}

HRESULT CImporter::Initialize()
{
	m_pImporter = new Assimp::Importer();

	return S_OK;
}

CImporter* CImporter::Create()
{
	CImporter* pInstance = new CImporter();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("CImporter::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CImporter::Free()
{
	Safe_Delete(m_pImporter);
	Super::Free();
}
