#include "pch.h"
#include "Builder_AttackPreset.h"
#include "DataDocument_AttackPreset.h"
#include "GameInstance.h"

CBuilder_AttackPreset::CBuilder_AttackPreset(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iLevelID)
	:Super(pDevice, pDeviceContext, iLevelID)
{
}

HRESULT CBuilder_AttackPreset::Initialize()
{
	return S_OK;
}

HRESULT CBuilder_AttackPreset::Build(const CDataDocumentBase& document)
{
	if (document.Get_Category() != DTO::ECategory::ATTACK_PRESET)
		return E_FAIL;

	const auto& doc = static_cast<const CDataDocument_AttackPreset&>(document);
	{
		const vector<Engine::IObjectDataBase*> vecDtoList = doc.Get_ListByType(ENUM_TO_UINT(DTO::EAttackPresetType::NORMAL));
		for (const auto& pDtoBase : vecDtoList)
		{
			const auto* pDto = static_cast<const Engine::CDataStruct_AttackPreset*>(pDtoBase);
			if (FAILED(Set_AttackPreset(pDto->Get_Data())))
				return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CBuilder_AttackPreset::Set_AttackPreset(const DTO::TAttackPreset_Data& data)
{
	if (FAILED(m_pGameInstance->Upsert_AttackPresetData(data)))
		return E_FAIL;
	return S_OK;
}

CBuilder_AttackPreset* CBuilder_AttackPreset::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iLevelID)
{
	CBuilder_AttackPreset* pInstance = new CBuilder_AttackPreset(pDevice, pDeviceContext, iLevelID);
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("CBuilder_AttackPreset::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBuilder_AttackPreset::Free()
{
	Super::Free();
}