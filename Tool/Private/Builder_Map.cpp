#include "pch.h"
#include "Builder_Map.h"
#include "MapToolManager.h"
#include "GameInstance.h"
#include "StaticModel.h"
#include "InstanceModel.h"

CBuilder_Map::CBuilder_Map(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iLevelID)
	: CBuilderBase(pDevice,pDeviceContext,iLevelID) , m_pGameInstance(CGameInstance::GetInstance()) , m_pMapToolManager(CMapToolManager::GetInstance())
{
}

HRESULT	CBuilder_Map::Initialize()
{



	return S_OK;
}


HRESULT CBuilder_Map::Build(const CDataDocumentBase& document)
{
	const auto& doc = static_cast<const CDataDocument_Map&>(document);
	// For. StaticModel
	{
		const vector<Engine::IObjectDataBase*> vecList = doc.Get_ListByType(ENUM_TO_UINT(DTO::EMapObject_Type::STATICMODEL));
		for (const auto& pObjectData : vecList)
		{
			pObjectData->Get_Type();
			const auto* pStaticModelData = static_cast<const Engine::CData_StaticModel*>(pObjectData);
			if (FAILED(Create_StaticModel(pStaticModelData->Get_Data())))
				return E_FAIL;
		}
	}
	// For. InstanceModel
	{
		const vector<Engine::IObjectDataBase*> vecList = doc.Get_ListByType(ENUM_TO_UINT(DTO::EMapObject_Type::INSTANCEMODEL));
		for (const auto& pObjectData : vecList)
		{
			const auto* pInstanceModelData = static_cast<const Engine::CData_InstanceModel*>(pObjectData);
			if (FAILED(Create_InstanceModel(pInstanceModelData->Get_Data())))
				return E_FAIL;
		}
	}
	return S_OK;
}

HRESULT CBuilder_Map::Create_StaticModel(const DTO::TMap_StaticModelData& tData)
{
	CStaticModel::STATICMODEL_DESC tStaticModelDesc{};
	tStaticModelDesc.tData.eMapObjectType = EMapObject_Type::STATICMODEL;
	tStaticModelDesc.wstrLayerTag = g_wszStaticModelLayer;
	tStaticModelDesc.isLoaded = true;
	tStaticModelDesc.iLevelIndex = ENUM_TO_UINT(ELevelType::MAP);
	tStaticModelDesc.eState = CMapObject::EState::Default;

	/* Origin SRT 복사 */
	memcpy(&tStaticModelDesc.tData.tOriginSRT,&tData.tSRTData,sizeof(tData.tSRTData));
	/* Transform 생성 */
	CTransform::TRANSFORM_DESC tTsDesc{};
	tTsDesc.ScaleMatrix = Matrix::CreateScale(tStaticModelDesc.tData.tOriginSRT.vScale);
	tTsDesc.RotationMatrix = Matrix::CreateFromQuaternion(tStaticModelDesc.tData.tOriginSRT.vQuat);
	tTsDesc.TranslationMatrix = Matrix::CreateTranslation(tStaticModelDesc.tData.tOriginSRT.vPosition);
	tStaticModelDesc.pTransform_Desc = &tTsDesc;

	/* Model Info */
	tStaticModelDesc.tData.tUsingModelInfo.wstrName = tData.tUsingModelInfo.wstrName;
	tStaticModelDesc.tData.tUsingModelInfo.wstrPath = tData.tUsingModelInfo.wstrPath;
	tStaticModelDesc.tData.tUsingModelInfo.wstrMtl_JsonFile_Path = tData.tUsingModelInfo.wstrMtl_JsonFile_Path;


	if(!tData.tUsingModelInfo.vecOverrideMaterial.empty())
	{
		_uint iCount = static_cast<_uint>(tData.tUsingModelInfo.vecOverrideMaterial.size());
		tStaticModelDesc.tData.tUsingModelInfo.vecOverrideMaterial.resize(iCount);

		for (_uint i = 0; i < iCount; ++i)
		{
			tStaticModelDesc.tData.tUsingModelInfo.vecOverrideMaterial[i].isNull = tData.tUsingModelInfo.vecOverrideMaterial[i].isNull;
			tStaticModelDesc.tData.tUsingModelInfo.vecOverrideMaterial[i].wstrMtl_JsonFile_Name = tData.tUsingModelInfo.vecOverrideMaterial[i].wstrMtl_JsonFile_Name;
			tStaticModelDesc.tData.tUsingModelInfo.vecOverrideMaterial[i].wstrMtl_JsonFile_Path = tData.tUsingModelInfo.vecOverrideMaterial[i].wstrMtl_JsonFile_Path;
			tStaticModelDesc.tData.tUsingModelInfo.vecOverrideMaterial[i].vecUsingTextureInfo = tData.tUsingModelInfo.vecOverrideMaterial[i].vecUsingTextureInfo;
		}
	}

	m_pMapToolManager->Make_MapObject(EMapObject_Type::STATICMODEL,&tStaticModelDesc,false);

	return S_OK;
}

HRESULT CBuilder_Map::Create_InstanceModel(const DTO::TMap_InstanceModelData& tData)
{
	CInstanceModel::INSTANCEMODEL_DESC tInstanceModelDesc{};
	tInstanceModelDesc.iLevelIndex = ENUM_TO_UINT(ELevelType::MAP);
	tInstanceModelDesc.isLoaded = true;
	tInstanceModelDesc.eState = CMapObject::EState::Default;
	tInstanceModelDesc.eType = EMapObject_Type::INSTANCEMODEL;
	tInstanceModelDesc.wstrLayerTag = g_wszInstanceModelLayer;

	tInstanceModelDesc.tData.eInstance_Usage = tData.eInstance_Usage;

	tInstanceModelDesc.tData.vecSRT.resize(tData.vecSRTData.size());
	memcpy(tInstanceModelDesc.tData.vecSRT.data() , tData.vecSRTData.data(), sizeof(SRT_DATA) * tData.vecSRTData.size() );

	/* Using Model Info  */
	tInstanceModelDesc.tData.tUsingModelInfo.wstrName = tData.tUsingModelInfo.wstrName;
	tInstanceModelDesc.tData.tUsingModelInfo.wstrPath = tData.tUsingModelInfo.wstrPath;
	tInstanceModelDesc.tData.tUsingModelInfo.wstrMtl_JsonFile_Path = tData.tUsingModelInfo.wstrMtl_JsonFile_Path;

	if (!tData.tUsingModelInfo.vecOverrideMaterial.empty())
	{
		_uint iCount = static_cast<_uint>(tData.tUsingModelInfo.vecOverrideMaterial.size());
		tInstanceModelDesc.tData.tUsingModelInfo.vecOverrideMaterial.resize(iCount);
		for (_uint i = 0; i < iCount; ++i)
		{
			tInstanceModelDesc.tData.tUsingModelInfo.vecOverrideMaterial[i].isNull = tData.tUsingModelInfo.vecOverrideMaterial[i].isNull;
			tInstanceModelDesc.tData.tUsingModelInfo.vecOverrideMaterial[i].wstrMtl_JsonFile_Name = tData.tUsingModelInfo.vecOverrideMaterial[i].wstrMtl_JsonFile_Name;
			tInstanceModelDesc.tData.tUsingModelInfo.vecOverrideMaterial[i].wstrMtl_JsonFile_Path = tData.tUsingModelInfo.vecOverrideMaterial[i].wstrMtl_JsonFile_Path;
			tInstanceModelDesc.tData.tUsingModelInfo.vecOverrideMaterial[i].vecUsingTextureInfo = tData.tUsingModelInfo.vecOverrideMaterial[i].vecUsingTextureInfo;
		}
	}
	m_pMapToolManager->Make_MapObject(EMapObject_Type::INSTANCEMODEL,&tInstanceModelDesc, false);
	return S_OK;
}


CBuilder_Map* CBuilder_Map::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iLevelID)
{
	CBuilder_Map* pBuilder = new CBuilder_Map(pDevice,pDeviceContext,iLevelID);

	if (FAILED(pBuilder->Initialize()))
	{
		Safe_Release(pBuilder);
		MSG_BOX(" Builder Map is Failed To Create ");
		return nullptr;
	}

	return pBuilder;
}

void CBuilder_Map::Free()
{
	Super::Free();
}
