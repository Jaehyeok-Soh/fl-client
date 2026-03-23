#include "Engine_pch.h"
#include "CameraEvent_Handler.h"
#include "Model.h"
#include "CameraTypeConverters.h"
#include "GameInstance.h"

CCameraEvent_Handler::CCameraEvent_Handler()
    : Super()
{
}

CCameraEvent_Handler::CCameraEvent_Handler(const CCameraEvent_Handler& rhs)
    : Super(rhs)
{
}

HRESULT CCameraEvent_Handler::Initialize_Prototype()
{
    if (FAILED(Super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CCameraEvent_Handler::Initialize(void* pArg)
{
    if (FAILED(Super::Initialize(pArg)))
        return E_FAIL;

    if (pArg == nullptr)
        return E_FAIL;

    if (FAILED(Ready_Desc(pArg)))
        return E_FAIL;

    return S_OK;
}

HRESULT CCameraEvent_Handler::Ready_Desc(void* pArg)
{
    if (pArg == nullptr)
        return E_FAIL;

    m_tDesc = *static_cast<CAMERA_EVENT_HANDLER_DESC*>(pArg);
    return S_OK;
}

void CCameraEvent_Handler::Set_Desc(const CAMERA_EVENT_HANDLER_DESC& desc)
{
    m_tDesc = desc;

    if (m_pOwnerModel)
    {
        Clear_CameraNotifies();
        Ready_CameraState();
    }
}

void CCameraEvent_Handler::Setup_ForOwner(CModel* pModel)
{
    Safe_Release(m_pOwnerModel);
    m_pOwnerModel = pModel;
    Safe_AddRef(m_pOwnerModel);

    Clear_CameraNotifies();
    Ready_CameraState();
}

void CCameraEvent_Handler::Clear_CameraNotifies()
{
    if (m_pOwnerModel == nullptr)
        return;

    auto& animations = m_pOwnerModel->Get_Animations();
    for (auto& pAnimation : animations)
    {
        if (pAnimation)
            pAnimation->Clear_Notifies(EAnimNotifyId::CameraControl);
    }
}

AnimNotifyKey CCameraEvent_Handler::Build_CameraNotifyKey(const DTO::CAMERACOTRNOL_EVENT& event, _uint iEventIndex) const
{
    AnimNotifyKey key{};
    key.eID = Engine::EAnimNotifyId::CameraControl;
    key.fTrackPosition = event.fStartTrackPosition;

    // Camera´Â payload¸¦ key¿¡ ´Ù ¾È ½Æ°í, ÀÎµ¦½º¸¸ ½Æ´Â´Ù
    key.iParam0 = iEventIndex;

    return key;
}

HRESULT CCameraEvent_Handler::Ready_CameraState()
{
    if (m_pOwnerModel == nullptr)
        return E_FAIL;

    Release_Event();

    auto& animations = m_pOwnerModel->Get_Animations();

    for (_uint i = 0; i < (_uint)m_tDesc.vecCameraEvents.size(); ++i)
    {
        auto& evt = m_tDesc.vecCameraEvents[i];

        if (evt.strAnimTag.empty())
            continue;

        const _int iResolvedAnimIndex =
            m_pOwnerModel->Get_AnimationIndex(Engine_Utils::ToWString(evt.strAnimTag));

        evt.iAnimIndex = iResolvedAnimIndex;

        if (iResolvedAnimIndex < 0 || iResolvedAnimIndex >= static_cast<_int>(animations.size()))
            continue;

        CModelAnimation* pAnimation = animations[iResolvedAnimIndex];
        if (pAnimation == nullptr)
            continue;

        AnimNotifyKey key = Build_CameraNotifyKey(evt, i);
        pAnimation->Pushback_Notifies(evt.ePhase, key);
    }

    for (auto& pAnimation : animations)
    {
        if (pAnimation)
            pAnimation->Sort_Notifies();
    }

    m_EventHandle = m_pOwnerModel->OnNotify.Subscribe(
        [this](const AnimNotifyKey& key)
        {
            this->CallbackEvent(key);
        });

    return S_OK;
}

void CCameraEvent_Handler::CallbackEvent(const AnimNotifyKey& key)
{
    if (key.eID != EAnimNotifyId::CameraControl)
        return;

    const _int iEventIndex = static_cast<_int>(key.iParam0);
    if (iEventIndex < 0 || iEventIndex >= (_int)m_tDesc.vecCameraEvents.size())
        return;

    const DTO::CAMERACOTRNOL_EVENT& evt = m_tDesc.vecCameraEvents[iEventIndex];

    switch (evt.eCommand)
    {
    case DTO::EAnimCameraControlCommand::Shake:
        m_pGameInstance->Request_MainCameraShake(Engine::ToRuntimeDesc(evt.shake));
        break;

    case DTO::EAnimCameraControlCommand::FOV:
        m_pGameInstance->Request_MainCameraFOV(Engine::ToRuntimeDesc(evt.fov));
        break;

    case DTO::EAnimCameraControlCommand::RotationOffset:
        m_pGameInstance->Request_MainCameraRotationOffset(Engine::ToRuntimeDesc(evt.rotationOffset));
        break;

    case DTO::EAnimCameraControlCommand::PositionOffset:
        m_pGameInstance->Request_MainCameraPositionOffset(Engine::ToRuntimeDesc(evt.positionOffset));
        break;
    }
}

void CCameraEvent_Handler::Release_Event()
{
    if (m_pOwnerModel)
        m_pOwnerModel->OnNotify.Unsubscribe(m_EventHandle);
}

CCameraEvent_Handler* CCameraEvent_Handler::Create()
{
    CCameraEvent_Handler* pInstance = new CCameraEvent_Handler();

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CCameraEvent_Handler");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CCameraEvent_Handler::Clone(void* pArg)
{
    CCameraEvent_Handler* pInstance = new CCameraEvent_Handler(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CCameraEvent_Handler");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCameraEvent_Handler::Free()
{
    Release_Event();
    Clear_CameraNotifies();

    Safe_Release(m_pOwnerModel);

    Super::Free();
}