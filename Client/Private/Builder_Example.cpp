#include "pch.h"
#include "Builder_Example.h"
#include "DataDocument_Example.h"
#include "GameInstance.h"

CBuilder_Example::CBuilder_Example(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iLevelID)
	: Super(pDevice, pDeviceContext, iLevelID)
{
}

HRESULT CBuilder_Example::Initialize()
{
	return S_OK;
}

HRESULT CBuilder_Example::Build(const CDataDocumentBase& document)
{
	if (document.Get_Category() != DTO::ECategory::MAP)
		return E_FAIL;


	const auto& doc = static_cast<const CDataDocument_Example&>(document);
	// For. 

	//{
	//	const vector<Engine::IObjectDataBase*> vecList = doc.Get_ListByType(ENUM_TO_UINT(DTO::EMapType::StaticObject));
	//	for (const auto& pObjectData : vecList)
	//	{
	//		const auto* pStaticObjectData = static_cast<const Engine::CExample_StaticObject*>(pObjectData);
	//		if (FAILED(Create_StaticObject(pStaticObjectData->Get_Data())))
	//			return E_FAIL;
	//	}
	//}
	//// For. Light
	//{
	//	const vector<Engine::IObjectDataBase*> vecList = doc.Get_ListByType(ENUM_TO_UINT(DTO::EMapType::LIGHT));
	//	for (const auto& pObjectData : vecList)
	//	{
	//		const auto* pLightData = static_cast<const Engine::CExample_LightData*>(pObjectData);
	//		if (FAILED(Create_Light(pLightData->Get_Data())))
	//			return E_FAIL;
	//	}
	//}

	return S_OK;
}

HRESULT CBuilder_Example::Create_Light(const DTO::TExample_LightData& data)
{
	if (data.eType != DTO::EMapType::LIGHT)
		return E_FAIL;

	return S_OK;
}

CBuilder_Example* CBuilder_Example::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iLevelID)
{
	CBuilder_Example* pInstance = new CBuilder_Example(pDevice, pDeviceContext, iLevelID);
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("CBuilder_Example::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBuilder_Example::Free()
{
	Super::Free();
}
