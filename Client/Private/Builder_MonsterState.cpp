#include "pch.h"
#include "Builder_MonsterState.h"
#include "GameInstance.h"
#include "DataDocument_MonsterState.h"
#include "DataStruct_MonsterState.h"

CBuilder_MonsterState::CBuilder_MonsterState(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iLevelID)
	:Super(pDevice, pDeviceContext, iLevelID)
{
}

HRESULT CBuilder_MonsterState::Initialize()
{
	return S_OK;
}

HRESULT CBuilder_MonsterState::Build(const CDataDocumentBase& document)
{
	if (document.Get_Category() != DTO::ECategory::MONSTER_STATE)
		return E_FAIL;

	const auto& doc = static_cast<const CDataDocument_MonsterState&>(document);

	// For. DataOverlap
	{
		/* 문서에 저장된 IObjectDataBase -> 데이터를 가진 클래스의 부모 */
		const vector<Engine::IObjectDataBase*> vecDtoList = doc.Get_ListByType(DTO::ATTACKOVERLAP_TYPE::NORMAL);
		for (const auto& pDtoBase : vecDtoList)
		{
			/* 데이터를 보유한 클래스 다운캐스팅 */
			const auto* pDto = static_cast<const Engine::CDataStruct_MonsterState*>(pDtoBase);

			/* 프로토타입 태그는 캐릭터(플레이어, 몬스터)와 1:1 연결될 수 있게 */
			if (FAILED(Create_MonsterState_Prototype(pDto->Get_Data())))
				return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CBuilder_MonsterState::Create_MonsterState_Prototype(const DTO::MONSTER_STATEBASE_DESC& data)
{
	return S_OK;
}

CBuilder_MonsterState* CBuilder_MonsterState::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iLevelID)
{
	CBuilder_MonsterState* pInstance = new CBuilder_MonsterState(pDevice, pDeviceContext, iLevelID);
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("CBuilder_MonsterState::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBuilder_MonsterState::Free()
{
	Super::Free();
}
