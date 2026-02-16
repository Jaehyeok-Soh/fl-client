#include "pch.h"
#include "UEMapDataLoader.h"
#include "Transform.h"
#include <algorithm>
#include "Engine_Utils.h"
#include "Engine_GlobalValue.h"
#include "ToolObject.h"
#include "Model.h"
#include "UEMapdataParser.h"
#include "GameInstance.h"

CUEMapDataLoader::CUEMapDataLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
	, m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CUEMapDataLoader::Make_Prototype(_uint Leveltype, const wstring& wstrMapModelFolderPath)
{
	std::filesystem::path mapFolderPath = wstrMapModelFolderPath;
	if (std::filesystem::exists(mapFolderPath) == false)
	{
		MSG_BOX("CUEMapDataLoader::Make_Prototype, cant open folder!");
		return E_FAIL;
	}

	const std::filesystem::path basePath = L"../../Resources/Models/";
	const wstring wstrModelTag = L"Prototype_Component_Model_";

	for (const auto& entry : std::filesystem::recursive_directory_iterator(mapFolderPath))
	{
		if (entry.is_regular_file())
		{
			std::filesystem::path fileFullPath = entry.path();
			if (fileFullPath.extension() != L".mesh") continue;
			wstring wstrFileName = fileFullPath.stem();
			{
				CBase* pFinded = { nullptr };
				if (pFinded = m_pGameInstance->Find_Prototype(Leveltype, wstrModelTag + wstrFileName))
					continue;
			}

			CModel::MODEL_ORIGIN_DESC desc = {};
			desc.eType = EModelType::STATIC;
			desc.iPrototypeLevelIndex = Leveltype;
			desc.wstrModelFolderName = fileFullPath.lexically_relative(basePath);
			m_pGameInstance->Add_Prototype(Leveltype, wstrModelTag + wstrFileName, CModel::Create(m_pDevice, m_pDeviceContext, &desc));
		}
	}

	return S_OK;
}

HRESULT CUEMapDataLoader::Make_StaticModel(const wstring& wstrRawDataFilePath, const wstring& wstrStaticModelLayerTag, const wstring& wstrColmeshLayerTag)
{
	return S_OK;
}

CUEMapDataLoader* CUEMapDataLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	return new CUEMapDataLoader(pDevice, pDeviceContext);
}

void CUEMapDataLoader::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pGameInstance);
	Super::Free();
}
