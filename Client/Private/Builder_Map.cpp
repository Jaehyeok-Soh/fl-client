#include "pch.h"
#include "Builder_Map.h"
#include "GameInstance.h"
#include "StaticModel.h"
#include "InstanceModel.h"
#include "Bounds.h"
#include "DataDocument_Map.h"

CBuilder_Map::CBuilder_Map(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iLevelID)
	: CBuilderBase(pDevice, pDeviceContext, iLevelID), m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
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
			const auto* pStaticModelData = static_cast<const Engine::CData_StaticModel*>(pObjectData);
			if (FAILED(Create_StaticModel(pStaticModelData->Get_Data())))
				return E_FAIL;
		}
	}

	// For. StaticModel
	{
		const vector<Engine::IObjectDataBase*> vecList = doc.Get_ListByType(ENUM_TO_UINT(DTO::EMapObject_Type::INSTANCEMODEL));
		for (const auto& pObjectData : vecList)
		{
			const auto* pInstanceModel = static_cast<const Engine::CData_InstanceModel*>(pObjectData);
			if (FAILED(Create_InstanceModel(pInstanceModel->Get_Data())))
				return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CBuilder_Map::Create_StaticModel(const DTO::TMap_StaticModelData& tData)
{
	Client::CStaticModel::STATICMODEL_DESC tStaticModelDesc{};
	tStaticModelDesc.tUsingModelInfo = tData.tUsingModelInfo;
	tStaticModelDesc.iLevelIndex = ENUM_TO_UINT(ELevelType::LOGO);
	tStaticModelDesc.vScale_Isolated = tData.tSRTData.vScale_Isolated;

	CTransform::TRANSFORM_DESC tTsDesc{};
	tTsDesc.ScaleMatrix = Matrix::CreateScale(tData.tSRTData.vScale);
	tTsDesc.RotationMatrix = Matrix::CreateFromQuaternion(tData.tSRTData.vQuat);
	tTsDesc.TranslationMatrix = Matrix::CreateTranslation(tData.tSRTData.vPosition);
	tStaticModelDesc.pTransform_Desc = &tTsDesc;


	CGameObject* pResult{nullptr};
	if (!(pResult = m_pGameInstance->Add_GameObject(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_GameObject_StaticModel", tStaticModelDesc.iLevelIndex
		, g_wszStaticModelLayer, &tStaticModelDesc)))
	{
		Safe_Release(pResult);
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBuilder_Map::Create_InstanceModel(const DTO::TMap_InstanceModelData& tData)
{
	CInstanceModel::INSTANCEMODEL_DESC tInstanceModelDesc{};
	tInstanceModelDesc.iLevelIndex = ENUM_TO_UINT(ELevelType::LOGO);
	tInstanceModelDesc.eType = EMapObject_Type::INSTANCEMODEL;
	tInstanceModelDesc.tData = tData;


	CGameObject* pResult{ nullptr };
	if (!(pResult = m_pGameInstance->Add_GameObject(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_GameObject_InstanceModel", tInstanceModelDesc.iLevelIndex
		, g_wszInstanceModelLayer, &tInstanceModelDesc)))
	{
		Safe_Release(pResult);
		return E_FAIL;
	}

	return S_OK;
}


CBuilder_Map* CBuilder_Map::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iLevelID)
{
	CBuilder_Map* pBuilder = new CBuilder_Map(pDevice, pDeviceContext, iLevelID);

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
	Safe_Release(m_pGameInstance);


	Super::Free();
}