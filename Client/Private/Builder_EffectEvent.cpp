#include "pch.h"
#include "Builder_EffectEvent.h"
#include "DataDocument_EffectEvent.h"
#include "GameInstance.h"
#include "Engine_Utils.h"

CBuilder_EffectEvent::CBuilder_EffectEvent(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iLevelID)
	:Super(pDevice, pDeviceContext, iLevelID)
{
}

HRESULT CBuilder_EffectEvent::Initialize()
{
	return S_OK;
}

HRESULT CBuilder_EffectEvent::Build(const CDataDocumentBase& document)
{
	// 1. 카테고리 체크 (정우님이 Document에 설정한 EFFECTEVENT 카테고리)
	if (document.Get_Category() != DTO::ECategory::EFFECTEVENT)
		return E_FAIL;

	const auto& doc = static_cast<const CDataDocument_EffectEvent&>(document);

	// 2. 문서 내의 모든 데이터(캐릭터별 이벤트 묶음) 리스트 확보
	// Get_Type()이 EFFECTEVENT를 반환하므로 해당 리스트를 가져옵니다.
	const vector<Engine::IObjectDataBase*> vecDtoList = doc.Get_ListByType(ENUM_TO_UINT(DTO::ECategory::EFFECTEVENT));

	for (const auto& pDtoBase : vecDtoList)
	{
		/* 데이터를 보유한 클래스 다운캐스팅 */
		const auto* pDto = static_cast<const Engine::CDataStruct_EffectEvent*>(pDtoBase);

		/* 실제 엔진에서 쓸 수 있는 애니메이션 통지 리스트로 변환 및 프로토타입 등록 */
		if (FAILED(Create_Effect(pDto->Get_Data())))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CBuilder_EffectEvent::Create_Effect(const DTO::ANIM_EVENT_INFO_DESC& data)
{
	// 캐릭터(Owner)별로 관리하기 위해 태그 생성 (예: EFFECT_EVENT_PlayerMoon)
	wstring wstrOwnerTag = Engine_Utils::ToWString(data.strOwnerTag);
	wstring prototypeTag = L"Prototype_Component_EffectEvent_" + wstrOwnerTag;

	// 핵심: DTO 데이터를 엔진의 AnimNotifyKey 구조로 변환하여 보관할 컨테이너가 필요할 수 있습니다.
	// 일단 여기서는 캐릭터별로 이벤트를 모아서 관리할 수 있도록 '이펙트 이벤트 데이터 컴포넌트' 
	// 혹은 'Manager'에 등록하는 과정을 수행합니다.

	// 예시: NotifyId 별로 루프를 돌며 엔진용 Key 구조체 생성
	for (_uint i = 0; i < ENUM_TO_UINT(EAnimNotifyId::END); ++i)
	{
		for (auto& eventBase : data.vecAnimEvents[i])
		{
			// 여기서 eventBase.vecScript 내의 EFFECT_EVENT_SCRIPT 데이터들을 
			// 캐릭터의 해당 애니메이션(iAnimIndex)에 AnimNotifyKey로 등록하는 로직이 들어갑니다.

			/*
			AnimNotifyKey tKey;
			tKey.eID = eventBase.eNotifyId;
			tKey.fTrackPosition = eventBase.fTrackPosition;
			// 이펙트 태그나 추가 정보는 string 혹은 Param에 할당
			tKey.strParam = eventBase.vecScript[0].strEffectTag;
			...
			*/
		}
	}

	// 정우님, 이 부분에서 생성된 NotifyKey 리스트를 어디에 저장할지가 관건입니다.
	// 보통은 캐릭터 프로토타입의 Model 컴포넌트에 직접 꽂아주거나, 
	// 전역 EffectManager에 캐릭터 태그별로 보관해둡니다.

	return S_OK;
}

CBuilder_EffectEvent* CBuilder_EffectEvent::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iLevelID)
{
	CBuilder_EffectEvent* pInstance = new CBuilder_EffectEvent(pDevice, pDeviceContext, iLevelID);
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("CBuilder_EffectEvent::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBuilder_EffectEvent::Free()
{
	Super::Free();
}