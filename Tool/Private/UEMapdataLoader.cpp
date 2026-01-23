#include "Tool_Defines.h"
#include "Transform.h"
#include <algorithm>
#include "Engine_Utils.h"
#include "Engine_GlobalValue.h"
#include "ToolObject.h"
#include "GameInstance.h"
#include "Model.h"
#include "UEMapdataParser.h"
#include "UEMapDataLoader.h"

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
	CUEMapdataParser::MAPPARSER_DESC desc = {};
	desc.wstrPath = wstrRawDataFilePath;
	CUEMapdataParser* pParser = CUEMapdataParser::Create(desc);
	if (!pParser)
		return E_FAIL;

	if (FAILED(pParser->Read_Mapdata()))
		return E_FAIL;

	const _char* pFilterName = "StaticMeshComponent0";

	_uint iCount = { 0 };
	for (size_t i = 0; i < pParser->m_vecData.size(); ++i)
	{
		const PARSED_MAPDATA_OUTER& mapdataOuter = pParser->m_vecData[i];
		if (::strcmp(pFilterName, mapdataOuter.strName.c_str()) != 0)
			continue; 

		string strFilteredName = mapdataOuter.Properties.StaticMesh.strObjectName;
		// StaticMesh'.....' 형태로 이루어져있음
		// staticmesh' 지우기
		strFilteredName.erase(0, 11);
		// 마지막 ' 지우기
		strFilteredName.erase(strFilteredName.size() - 1, 1);
		CGameObject* pResult = { nullptr };

		// ColMesh
		if (strFilteredName.ends_with("_COL") || strFilteredName.starts_with("COL_"))
		{
			//CColMesh::COLMESH_DESC desc = {};
			//desc.wstrLayerTag = wstrColmeshLayerTag;
			//desc.iLevelIndex = ENUM_TO_UINT(LEVELID::MAP);
			//desc.wstrFileName = Engine::Engine_Utils::ToWString(strFilteredName);
			//if (!(pResult = m_pGameInstance->Add_GameObject(ENUM_TO_UINT(LEVELID::MAP),
			//	L"Prototype_GameObject_ColMesh",
			//	ENUM_TO_UINT(LEVELID::MAP),
			//	wstrColmeshLayerTag, &desc)))
			//{
			//	Safe_Release(pParser);
			//	return E_FAIL;
			//}
		}
		// StaticModel
		else
		{			
			//CStaticModel::STATICMODEL_DESC desc = {};
			//desc.wstrLayerTag = wstrStaticModelLayerTag;
			//desc.iLevelIndex = ENUM_TO_UINT(LEVELID::MAP);
			//desc.wstrFileName = Engine::Engine_Utils::ToWString(strFilteredName);
			//if (!(pResult = m_pGameInstance->Add_GameObject(ENUM_TO_UINT(LEVELID::MAP),
			//	L"Prototype_GameObject_StaticModel",
			//	ENUM_TO_UINT(LEVELID::MAP),
			//	wstrStaticModelLayerTag, &desc)))
			//{
			//	Safe_Release(pParser);
			//	return E_FAIL;
			//}
		}

		CTransform* pTransform = pResult->Get_Component<CTransform>();
		// Unreal엔진에서는 Degree로 표현됨, 이를 radian으로 바꿔줘야함
		_matrix matRotation = ::XMMatrixRotationRollPitchYaw(
			::XMConvertToRadians(mapdataOuter.Properties.vPitchYawRoll.x),
			::XMConvertToRadians(mapdataOuter.Properties.vPitchYawRoll.y),
			::XMConvertToRadians(mapdataOuter.Properties.vPitchYawRoll.z));
		pTransform->Set_Info(TRANSFORM_INFO_STATE::RIGHT, matRotation.r[0]);
		pTransform->Set_Info(TRANSFORM_INFO_STATE::UP, matRotation.r[1]);
		pTransform->Set_Info(TRANSFORM_INFO_STATE::LOOK, matRotation.r[2]);
		// scale은 양수이니 그냥 z, y 만 스왑
		if (mapdataOuter.Properties.vScale.x > 0.f && mapdataOuter.Properties.vScale.z > 0.f && mapdataOuter.Properties.vScale.y > 0.f)
			pTransform->Set_Scale(mapdataOuter.Properties.vScale.x, mapdataOuter.Properties.vScale.z, mapdataOuter.Properties.vScale.y);

		// position은 z, y 스왑후 z값쪽에 음수를 입혀야함
		pTransform->Set_Info(TRANSFORM_INFO_STATE::POS, ::XMVectorSet(mapdataOuter.Properties.vPosition.x * 0.01f, mapdataOuter.Properties.vPosition.z * 0.01f, mapdataOuter.Properties.vPosition.y * -0.01f, 1.f));

		// TODO - CreateObject 이벤트로 뺴고나서 이거 지워야함
		if (m_pGameInstance->Is_Awaked())
			pResult->Awake(ENUM_TO_UINT(ELevelType::MAP));
	}

	Safe_Release(pParser);
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
