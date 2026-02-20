#include "pch.h"
#include "Builder_EffectEvent.h"
#include "DataDocument_EffectEvent.h"
#include "AnimEffectHandler.h"
#include "Engine_Utils.h"
#include "Animation_Defines.h"
#include "GameInstance.h"

CBuilder_EffectEvent::CBuilder_EffectEvent(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iLevelID)
    : Super(pDevice, pDeviceContext, iLevelID)
{
}

HRESULT CBuilder_EffectEvent::Initialize() { return S_OK; }

HRESULT CBuilder_EffectEvent::Build(const CDataDocumentBase& document)
{
    if (document.Get_Category() != DTO::ECategory::EFFECTEVENT)
        return E_FAIL;

    const auto& doc = static_cast<const CDataDocument_EffectEvent&>(document);
    const vector<Engine::IObjectDataBase*> vecDtoList = doc.Get_ListByType(ENUM_TO_UINT(DTO::ECategory::EFFECTEVENT));

    for (const auto& pDtoBase : vecDtoList)
    {
        const auto* pDto = static_cast<const Engine::CDataStruct_EffectEvent*>(pDtoBase);

        if (FAILED(Create_Effect_Prototype(pDto->Get_Data())))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CBuilder_EffectEvent::Create_Effect_Prototype(const DTO::EFFECT_EVENT_INFO_DESC& data)
{
    CAnimEffectHandler::ANIM_EFFECT_HANDLER_DESC tHandlerDesc = {};
    tHandlerDesc.strOwnerTag = data.strOwnerTag;

    for (auto& event : data.vecEffectEvents)
    {
        tHandlerDesc.mapEvents[event.iAnimIndex].push_back(event);
    }

    CAnimEffectHandler* pHandler = CAnimEffectHandler::Create(&tHandlerDesc);
    if (nullptr == pHandler) return E_FAIL;

    CGameInstance::GetInstance()->Broadcast<LoadEffectEvent>(pHandler);

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

void CBuilder_EffectEvent::Free() { Super::Free(); }