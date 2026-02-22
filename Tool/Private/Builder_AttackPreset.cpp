#include "pch.h"
#include "Builder_AttackPreset.h"
#include "DataDocument_AttackPreset.h"
#include "AttackPreset_DataManager.h"
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

	// For. DataOverlap
	{
		/* 문서에 저장된 IObjectDataBase -> 데이터를 가진 클래스의 부모 */
		const vector<Engine::IObjectDataBase*> vecDtoList = doc.Get_ListByType(ENUM_TO_UINT(DTO::EAttackPresetType::NORMAL));
		for (const auto& pDtoBase : vecDtoList)
		{
			/* 데이터를 보유한 클래스 다운캐스팅 */
			const auto* pDto = static_cast<const Engine::CDataStruct_AttackPreset*>(pDtoBase);

			/* 프로토타입 태그는 캐릭터(플레이어, 몬스터)와 1:1 연결될 수 있게 */
			if (FAILED(Set_AttackPreset(pDto->Get_Data())))
				return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CBuilder_AttackPreset::Set_AttackPreset(const DTO::TAttackPreset_Data& data)
{
	CAttackPreset_DataManager* pDataManager = CAttackPreset_DataManager::GetInstance();
	if (FAILED(pDataManager->Upsert_Data(data)))
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
