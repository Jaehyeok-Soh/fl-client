#include "pch.h"
#include "Event_Effect_Module.h"
#include "GameInstance.h"
#include "AnimObj.h"

CEvent_Effect_Module::CEvent_Effect_Module(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : Super(), m_pDevice(pDevice), m_pDeviceContext(pDeviceContext), m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pDeviceContext);
    Safe_AddRef(m_pGameInstance);
}

HRESULT CEvent_Effect_Module::Initialize()
{
    return S_OK;
}

void CEvent_Effect_Module::Update(const _float fTimeDelta)
{
    if (m_pEffectEvent)
        m_pEffectEvent->Update(fTimeDelta);
}

void CEvent_Effect_Module::Render()
{
}

void CEvent_Effect_Module::SetEFfectEvent(CAnimEffectHandler* pEffectHandler, CAnimObj* pOwner)
{
    Safe_Release(m_pEffectEvent);
    m_pEffectEvent = pEffectHandler;

    if (m_pEffectEvent)
    {
        m_pEffectEvent->Set_Owner(pOwner);
        m_pEffectEvent->Awake();
    }
}

void CEvent_Effect_Module::SetOwner(CAnimObj* pOwner)
{
    if (m_pEffectEvent)
    {
        m_pEffectEvent->Set_Owner(pOwner);
        m_pEffectEvent->Awake();
    }
}

// 특정 애니메이션 인덱스에 해당하는 이벤트 리스트 참조 반환
unordered_map<_uint, vector<DTO::EFFECTEVENT>>& CEvent_Effect_Module::GetEvents()
{
    return m_pEffectEvent->GetEvents();
}

// 툴 패널에서 수정된 '전체' 리스트를 받아 핸들러의 mapEvents를 동기화
void CEvent_Effect_Module::Modify_EFfectEvent(vector<DTO::EFFECTEVENT> events)
{
    if (nullptr == m_pEffectEvent) return;

    // 핸들러의 m_tDesc(ANIM_EFFECT_HANDLER_DESC)에 직접 접근하여 map을 재구성
    auto& mapEvents = m_pEffectEvent->Get_Desc().mapEvents;

    mapEvents.clear();
    for (auto& evt : events)
    {
        // 툴에서 편집된 애니메이션 인덱스 기준으로 다시 분류
        mapEvents[evt.iAnimIndex].push_back(evt);
    }
}

void CEvent_Effect_Module::Awake()
{
    if (m_pEffectEvent)
        m_pEffectEvent->Awake();
}

CEvent_Effect_Module* CEvent_Effect_Module::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
    CEvent_Effect_Module* pInstance = new CEvent_Effect_Module(pDevice, pDeviceContext);
    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CEvent_Effect_Module");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CEvent_Effect_Module::Free()
{
    Safe_Release(m_pEffectEvent);
    Safe_Release(m_pDevice);
    Safe_Release(m_pDeviceContext);
    Safe_Release(m_pGameInstance);

    Super::Free();
}