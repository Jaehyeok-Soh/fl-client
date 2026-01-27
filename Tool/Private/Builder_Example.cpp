#include "pch.h"
#include "Builder_Example.h"
#include "GameInstance.h"

CBuilder_Example::CBuilder_Example(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
{
}

HRESULT CBuilder_Example::Build(const CDataDocumentBase &document)
{
	if (document.Get_Category() != DTO::ECategory::MAP)
		return E_FAIL;

	
	const auto& doc = static_cast<const CDataDocument_Example&>(document);
	// For. StaticModel
	{
		const vector<Engine::IObjectDataBase*> vecList = doc.Get_ListByType(ENUM_TO_UINT(DTO::EMapType::STATICMODEL));
		for (const auto& pObjectData : vecList)
		{
			const auto* pStaticModelData = static_cast<const Engine::CExample_StaticModel*>(pObjectData);
			if (FAILED(Create_StaticModel(pStaticModelData->Get_Data())))
				return E_FAIL;
		}
	}
	// For. Light
	{
		const vector<Engine::IObjectDataBase*> vecList = doc.Get_ListByType(ENUM_TO_UINT(DTO::EMapType::LIGHT));
		for (const auto& pObjectData : vecList)
		{
			const auto* pLightData = static_cast<const Engine::CExample_LightData*>(pObjectData);
			if (FAILED(Create_Light(pLightData->Get_Data())))
				return E_FAIL;
		}
	}

	

	return S_OK;
}

HRESULT CBuilder_Example::Create_StaticModel(const DTO::TExample_StaticModelData& data)
{
	if (data.eType != DTO::EMapType::STATICMODEL)
		return E_FAIL;

	// CStaticModel::STATICMODEL_DESC desc = {};
	//
	// desc Ã¤¿ì±â = data....
	//
	// CGameObject* pResult = m_pGameInstance->Add_GameObject(
	//	iLevelIndex,
	//	L"Prototype_GameObject_ColMesh",
	//	iLevelIndex,
	//	g_wszColMeshLayer, &desc);

	// if (pResult == nullptr)
	//	return E_FAIL;

	return S_OK;
}

HRESULT CBuilder_Example::Create_Light(const DTO::TExample_LightData& data)
{
	if (data.eType != DTO::EMapType::LIGHT)
		return E_FAIL;

	return S_OK;
}

void CBuilder_Example::Free()
{
	Super::Free();
}
