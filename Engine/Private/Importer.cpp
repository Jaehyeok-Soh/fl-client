#include "Engine_pch.h"
#include "Importer.h"

CImporter::CImporter()
{
}

HRESULT CImporter::Initialize()
{
	m_pImporter = new Assimp::Importer();
	m_pImporter->SetPropertyBool(AI_CONFIG_IMPORT_FBX_READ_TEXTURES, true);
	m_pImporter->SetPropertyBool(AI_CONFIG_IMPORT_FBX_READ_MATERIALS, true);
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
