#include "pch.h"
#include "Builder_EffectEvent.h"
#include "DataDocument_EffectEvent.h"
#include "AnimEffectHandler.h"
#include "Engine_Utils.h"
#include "GameInstance.h"

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
	if (document.Get_Category() != DTO::ECategory::EFFECTEVENT)
		return E_FAIL;

	const auto& doc = static_cast<const CDataDocument_EffectEvent&>(document);

	const vector<Engine::IObjectDataBase*> vecDtoList = doc.Get_ListByType(ENUM_TO_UINT(DTO::ECategory::EFFECTEVENT));

	for (const auto& pDtoBase : vecDtoList)
	{
		const auto* pDto = static_cast<const Engine::CDataStruct_EffectEvent*>(pDtoBase);

		if (FAILED(Create_Effect(pDto->Get_Data())))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CBuilder_EffectEvent::Create_Effect(const DTO::ANIM_EVENT_INFO_DESC& data)
{
	CAnimEffectHandler::ANIM_EFFECT_HANDLER_DESC tHandlerDesc = {};
	tHandlerDesc.strOwnerTag = data.strOwnerTag;

	for (_uint i = 0; i < ENUM_TO_UINT(EAnimNotifyId::END); ++i)
	{
		for (auto& eventBase : data.vecAnimEvents[i])
		{
			// 해당 애니메이션 인덱스의 리스트에 이벤트 추가
			// key: iAnimIndex, value: vector<ANIM_EVENT_BASE>
			tHandlerDesc.mapEvents[eventBase.iAnimIndex].push_back(eventBase);
		}
	}

	// 프로토타입 태그 생성 (Prototype_Component_AnimEffectHandler_PlayerMoon)
	wstring wstrOwnerTag = Engine_Utils::ToWString(data.strOwnerTag);
	wstring prototypeTag = L"Prototype_Component_AnimEffectHandler_" + wstrOwnerTag;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC),
		prototypeTag, CAnimEffectHandler::Create(&tHandlerDesc))))
	{
		return E_FAIL;
	}

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