#include "Engine_pch.h"
#include "Sound_Handler.h"
#include "Model.h"
#include "GameInstance.h"

CSound_Handler::CSound_Handler()
    : Super()
{
}

CSound_Handler::CSound_Handler(const CSound_Handler& rhs)
    : Super(rhs)
{
}

HRESULT CSound_Handler::Initialize_Prototype()
{
    if (FAILED(Super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CSound_Handler::Initialize(void* pArg)
{
    if (FAILED(Super::Initialize(pArg)))
        return E_FAIL;

    if (pArg == nullptr)
        return E_FAIL;

    if (FAILED(Ready_Desc(pArg)))
        return E_FAIL;

    return S_OK;
}

HRESULT CSound_Handler::Ready_Desc(void* pArg)
{
    if (pArg == nullptr)
        return E_FAIL;

    m_tDesc = *static_cast<SOUND_HANDLER_DESC*>(pArg);
    return S_OK;
}

void CSound_Handler::Set_Desc(const SOUND_HANDLER_DESC& desc)
{
    m_tDesc = desc;

    if (m_pOwnerModel)
    {
        Clear_SoundNotifies();
        Ready_SoundState();
    }
}

void CSound_Handler::Setup_ForOwner(CModel* pModel)
{
    Safe_Release(m_pOwnerModel);
    m_pOwnerModel = pModel;
    Safe_AddRef(m_pOwnerModel);

    Clear_SoundNotifies();
    Ready_SoundState();
}

void CSound_Handler::Clear_SoundNotifies()
{
    if (m_pOwnerModel == nullptr)
        return;

    auto& animations = m_pOwnerModel->Get_Animations();
    for (auto& pAnimation : animations)
    {
        if (pAnimation)
            pAnimation->Clear_Notifies(EAnimNotifyId::Sound);
    }
}

AnimNotifyKey CSound_Handler::Build_SoundNotifyKey(const DTO::SOUNDEVENT& event) const
{
    AnimNotifyKey key{};
    key.eID = Engine::EAnimNotifyId::Sound;
    key.fTrackPosition = event.fStartTrackPosition;

    key.iParam0 = ENUM_TO_UINT(event.eCommand);
    key.iParam1 = event.strSoundTag.empty()
        ? 0u
        : Engine_Utils::ToHash(event.strSoundTag.c_str());
    key.iParam2 = static_cast<_uint>(event.iControlledId < 0 ? -1 : event.iControlledId);
    key.iParam3 = static_cast<_uint>((std::max)(0.f, event.fDelay) * 1000.f);

    key.fParam0 = event.fVolume;
    key.fParam1 = event.fPitch;

    key.bParam0 = event.bSteal;
    key.bParam1 = event.bLoop;

    return key;
}

HRESULT CSound_Handler::Ready_SoundState()
{
    if (m_pOwnerModel == nullptr)
        return E_FAIL;

    Release_Event();

    auto& animations = m_pOwnerModel->Get_Animations();

    for (const auto& evt : m_tDesc.vecSoundEvents)
    {
        if (evt.strAnimTag.empty())
            continue;

        const _int iResolvedAnimIndex =
            m_pOwnerModel->Get_AnimationIndex(Engine_Utils::ToWString(evt.strAnimTag));

        if (iResolvedAnimIndex < 0 || iResolvedAnimIndex >= static_cast<_int>(animations.size()))
            continue;

        CModelAnimation* pAnimation = animations[iResolvedAnimIndex];
        if (pAnimation == nullptr)
            continue;

        AnimNotifyKey key = Build_SoundNotifyKey(evt);

        const auto eCmd = static_cast<DTO::EAnimSoundCommand>(key.iParam0);
        if ((eCmd == DTO::EAnimSoundCommand::OneShot || eCmd == DTO::EAnimSoundCommand::ControlledPlay) &&
            key.iParam1 == 0)
        {
            continue;
        }

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

void CSound_Handler::CallbackEvent(const AnimNotifyKey& key)
{
    if (key.eID != EAnimNotifyId::Sound)
        return;

    const DTO::EAnimSoundCommand eCommand = static_cast<DTO::EAnimSoundCommand>(key.iParam0);
    const _uint iSoundHash = key.iParam1;
    const _uint iControlledId = key.iParam2;
    const _float fDelay = static_cast<_float>(key.iParam3) / 1000.f;
    const _float fVolume = key.fParam0;
    const _float fPitch = key.fParam1;
    const _bool bSteal = key.bParam0;
    const _bool bLoop = key.bParam1;


    _bool bPlayerFootHash = { false };

    if (iSoundHash == m_iPlayerFootSoundHash || iSoundHash == m_iPlayerLandSoundHash)
        bPlayerFootHash = true;

    switch (eCommand)
    {
    case DTO::EAnimSoundCommand::OneShot:
    {
        if (iSoundHash == 0)
            return;

        if (bPlayerFootHash)
        {
            if (fDelay > 0.f)
                m_pGameInstance->Play_OneShot_Delayed(m_pGameInstance->Get_CurrentLevelIndex(), iSoundHash, fDelay, fVolume, fPitch, bSteal);
            else
                m_pGameInstance->Play_OneShot(m_pGameInstance->Get_CurrentLevelIndex(), iSoundHash, fVolume, fPitch, bSteal);
        }

        else
        {
            if (fDelay > 0.f)
                m_pGameInstance->Play_OneShot_Delayed(0 /* static */, iSoundHash, fDelay, fVolume, fPitch, bSteal);
            else
                m_pGameInstance->Play_OneShot(0 /* static */, iSoundHash, fVolume, fPitch, bSteal);
        }
    }
    break;

    case DTO::EAnimSoundCommand::ControlledPlay:
    {
        if (iSoundHash == 0 || iControlledId == INVALID_CONTROLLED_ID)
            return;

        if (bPlayerFootHash)
        {
           m_pGameInstance->Play_Controlled(m_pGameInstance->Get_CurrentLevelIndex(), iSoundHash, iControlledId, fVolume, bLoop, fPitch);
        }

        m_pGameInstance->Play_Controlled(0 /* static */, iSoundHash, iControlledId, fVolume, bLoop, fPitch);
    }
    break;

    case DTO::EAnimSoundCommand::ControlledStop:
    {
        if (iControlledId == INVALID_CONTROLLED_ID)
            return;

        m_pGameInstance->Stop_Controlled(iControlledId);
    }
    break;

    case DTO::EAnimSoundCommand::ControlledVolume:
    {
        if (iControlledId == INVALID_CONTROLLED_ID)
            return;

        m_pGameInstance->Set_ControlledVolume(iControlledId, fVolume);
    }
    break;

    case DTO::EAnimSoundCommand::ControlledPitch:
    {
        if (iControlledId == INVALID_CONTROLLED_ID)
            return;

        m_pGameInstance->Set_ControlledPitch(iControlledId, fPitch);
    }
    break;

    default:
        break;
    }
}

void CSound_Handler::Release_Event()
{
    if (m_pOwnerModel)
        m_pOwnerModel->OnNotify.Unsubscribe(m_EventHandle);
}

CSound_Handler* CSound_Handler::Create()
{
    CSound_Handler* pInstance = new CSound_Handler();

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CSound_Handler");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CSound_Handler::Clone(void* pArg)
{
    CSound_Handler* pInstance = new CSound_Handler(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CSound_Handler");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSound_Handler::Free()
{
    Release_Event();
    Clear_SoundNotifies();

    Safe_Release(m_pOwnerModel);

    Super::Free();
}