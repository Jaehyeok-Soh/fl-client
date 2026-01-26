#include "pch.h"
#include "UEMapDataLoader.h"
#include "Transform.h"
#include <algorithm>
#include "Engine_Utils.h"
#include "Engine_GlobalValue.h"
#include "ToolObject.h"
#include "Model.h"
#include "UEMapdataParser.h"
#include "StaticModel.h"
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

HRESULT CUEMapDataLoader::Make_Prototype(const wstring& wstrMapModelFolderPath)
{
	std::filesystem::path mapFolderPath = wstrMapModelFolderPath;
	if (std::filesystem::exists(mapFolderPath) == false)
	{
		MSG_BOX("CUEMapDataLoader::Make_Prototype, cant open folder!");
		return E_FAIL;
	}

	const std::filesystem::path basePath = L"../../Resources/Models/";
	const wstring wstrModelTag = L"Prototype_Component_Model_";

	for (const auto& entry : std::filesystem::directory_iterator(mapFolderPath))
	{
		if (entry.is_regular_file())
		{
			std::filesystem::path fileFullPath = entry.path();
			wstring wstrFileName = fileFullPath.stem();
			{
				CBase* pFinded = { nullptr };
				if (pFinded = m_pGameInstance->Find_Prototype(ENUM_TO_UINT(ELevelType::MAP), wstrModelTag + wstrFileName))
					continue;
			}

			CModel::MODEL_ORIGIN_DESC desc = {};
			desc.eType = EModelType::STATIC;
			desc.iPrototypeLevelIndex = ENUM_TO_UINT(ELevelType::MAP);
			desc.wstrModelFolderName = fileFullPath.lexically_relative(basePath);
			m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::MAP), wstrModelTag + wstrFileName, CModel::Create(m_pDevice, m_pDeviceContext, &desc));
		}
	}

	return S_OK;
}

HRESULT CUEMapDataLoader::Make_StaticModel(const wstring& wstrRawDataFilePath, const wstring& wstrStaticModelLayerTag, const wstring& wstrColmeshLayerTag)
{
	//CUEMapdataParser::MAPPARSER_DESC desc = {};
	//desc.wstrPath = wstrRawDataFilePath;
	//CUEMapdataParser* pParser = CUEMapdataParser::Create(desc);
	//if (!pParser)
	//	return E_FAIL;

	//if (FAILED(pParser->Read_Mapdata(false)))
	//	return E_FAIL;

	//const _char* pFilterName = "StaticMeshComponent0";
	//const UINT32 iLevelID = ENUM_TO_UINT(ELevelType::MAP);

	//wstring wstrModelName{};
	//wstring wstrModelPath{};

	//_uint iCount = { 0 };
	//for (size_t i = 0; i < pParser->m_vecData.size(); ++i)
	//{
	//	const PARSED_MAPDATA_OUTER& mapdataOuter = pParser->m_vecData[i];
	//	wstrModelName = Engine_Utils::ToWString(mapdataOuter.Properties.StaticMesh.strObjectName);
	//	wstrModelPath = Engine_Utils::ToWString(mapdataOuter.Properties.StaticMesh.strObjectPath);

	//	CGameObject* pResult{ nullptr };

	//	CModel::MODEL_ORIGIN_DESC tModelDesc{};
	//	tModelDesc.eType = EModelType::STATIC;
	//	tModelDesc.wstrModelFolderName = wstrModelPath;
	//	tModelDesc.iPrototypeLevelIndex = iLevelID;
	//	CModel* pModel =  CModel::Create(m_pDevice, m_pDeviceContext, &tModelDesc);
	//	if (pModel)
	//	{
	//		if (FAILED(m_pGameInstance->Add_Prototype(iLevelID, L"Prototype_Component_Model_" + wstrModelName, pModel)))
	//			Safe_Release(pModel);
	//	}


	//	CStaticModel::STATICMODEL_DESC desc = {};
	//	desc.wstrLayerTag = wstrStaticModelLayerTag;
	//	desc.iLevelIndex = ENUM_TO_UINT(ELevelType::MAP);
	//	desc.wstrModelPath = wstrModelPath;
	//	desc.wstrModelName = wstrModelName;
	//	if (!(pResult = m_pGameInstance->Add_GameObject(desc.iLevelIndex,
	//		L"Prototype_GameObject_StaticModel",
	//		desc.iLevelIndex,
	//		wstrStaticModelLayerTag, &desc)))
	//	{
	//		Safe_Release(pParser);
	//		return E_FAIL;
	//	}

	//	CTransform* pTransform = pResult->Get_Component<CTransform>();
	//	// Unreal엔진에서는 Degree로 표현됨, 이를 radian으로 바꿔줘야함

	//	Vec3 vSwapRotation = Vec3(
	//		XMConvertToRadians(mapdataOuter.Properties.vPitchYawRoll.x), XMConvertToRadians(mapdataOuter.Properties.vPitchYawRoll.y), XMConvertToRadians(mapdataOuter.Properties.vPitchYawRoll.z));
	//	Matrix matRotation = Matrix::CreateFromYawPitchRoll(vSwapRotation);

	//	pTransform->Set_Info(TRANSFORM_INFO_STATE::RIGHT, matRotation.Right());
	//	pTransform->Set_Info(TRANSFORM_INFO_STATE::UP, matRotation.Up());
	//	pTransform->Set_Info(TRANSFORM_INFO_STATE::LOOK, matRotation.Backward());

	//	Vec3 vSwapScale = Vec3(mapdataOuter.Properties.vScale.x, mapdataOuter.Properties.vScale.z , mapdataOuter.Properties.vScale.y);
	//	pTransform->Set_Scale(vSwapScale);

	//	float vMulSize = 0.01f;
	//	Vec3  vSwapPosition = Vec3(mapdataOuter.Properties.vPosition.x * vMulSize , mapdataOuter.Properties.vPosition.z * vMulSize, mapdataOuter.Properties.vPosition.y * -vMulSize);
	//	pTransform->Set_Info(TRANSFORM_INFO_STATE::POS, vSwapPosition);
	//}

	//Safe_Release(pParser);
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
