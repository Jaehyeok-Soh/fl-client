#include "pch.h"
#include "ToolAnimSoundPlayer.h"
#include "GameObject.h"
#include "Model.h"
#include "ModelAnimation.h"
#include "GameInstance.h"
#include "Event_Sound_Module.h"

CToolAnimSoundPlayer::CToolAnimSoundPlayer()
    : Super()
{
}

CToolAnimSoundPlayer::CToolAnimSoundPlayer(const CToolAnimSoundPlayer& rhs)
    : Super(rhs)
    , m_iSoundLevelID(rhs.m_iSoundLevelID)
{
}

HRESULT CToolAnimSoundPlayer::Initialize_Prototype()
{
    if (FAILED(Super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CToolAnimSoundPlayer::Initialize(void* pArg)
{
    if (FAILED(Super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CToolAnimSoundPlayer::Update(const _float fTimeDelta)
{
}

AnimNotifyKey CToolAnimSoundPlayer::Build_SoundNotifyKey(const DTO::SOUNDEVENT& event)
{
    Engine::AnimNotifyKey key{};
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

HRESULT CToolAnimSoundPlayer::Cache_OwnerModel()
{
    Safe_Release(m_pOwnerModel);

    if (m_pOwner == nullptr)
        return E_FAIL;

    m_pOwnerModel = m_pOwner->Get_Component<Engine::CModel>();
    if (m_pOwnerModel == nullptr)
        return E_FAIL;

    Safe_AddRef(m_pOwnerModel);
    return S_OK;
}

HRESULT CToolAnimSoundPlayer::Rebuild(const vector<DTO::SOUNDEVENT>& events)
{
    Release_Event();

    if (FAILED(Cache_OwnerModel()))
        return E_FAIL;

    Clear_SoundNotifies();
    Register_Notifies(events);
    Bind_Event();

    return S_OK;
}

void CToolAnimSoundPlayer::Clear_SoundNotifies()
{
    if (m_pOwnerModel == nullptr)
        return;

    auto& animations = m_pOwnerModel->Get_Animations();
    for (auto& pAnimation : animations)
    {
        if (pAnimation)
            pAnimation->Clear_Notifies(Engine::EAnimNotifyId::Sound);
    }
}

void CToolAnimSoundPlayer::Register_Notifies(const vector<DTO::SOUNDEVENT>& events)
{
    if (m_pOwnerModel == nullptr)
        return;

    auto& animations = m_pOwnerModel->Get_Animations();

    for (auto& src : events)
    {
        if (src.strAnimTag.empty())
            continue;

        const _int iResolvedAnimIndex =
            m_pOwnerModel->Get_AnimationIndex(Engine_Utils::ToWString(src.strAnimTag));

        // 이름으로 못 찾으면 절대 다른 애니메이션에 억지로 붙이지 않음
        if (iResolvedAnimIndex < 0 || iResolvedAnimIndex >= static_cast<_int>(animations.size()))
            continue;

        auto* pAnimation = animations[iResolvedAnimIndex];
        if (pAnimation == nullptr)
            continue;

        Engine::AnimNotifyKey key = Build_SoundNotifyKey(src);
        const DTO::EAnimSoundCommand eCmd = static_cast<DTO::EAnimSoundCommand>(key.iParam0);

        // 해쉬 기록이 없다면 스킵
        if((eCmd == DTO::EAnimSoundCommand::OneShot || eCmd == DTO::EAnimSoundCommand::ControlledPlay)
            && key.iParam1 == 0)
        {
            continue;
        }

        pAnimation->Pushback_Notifies(src.ePhase, key);
    }

    for (auto& pAnimation : animations)
    {
        if (pAnimation)
            pAnimation->Sort_Notifies();
    }
}

void CToolAnimSoundPlayer::Bind_Event()
{
    Release_Event();

    if (m_pOwnerModel == nullptr)
        return;

    m_EventHandle = m_pOwnerModel->OnNotify.Subscribe(
        [this](const Engine::AnimNotifyKey& key)
        {
            this->CallbackEvent(key);
        });
}

void CToolAnimSoundPlayer::Release_Event()
{
    if (m_pOwnerModel)
        m_pOwnerModel->OnNotify.Unsubscribe(m_EventHandle);
}

void CToolAnimSoundPlayer::CallbackEvent(const Engine::AnimNotifyKey& key)
{
    if (key.eID != Engine::EAnimNotifyId::Sound)
        return;

    const DTO::EAnimSoundCommand eCommand = static_cast<DTO::EAnimSoundCommand>(key.iParam0);
    const _uint iSoundHash = key.iParam1;
    const _uint iControlledId = key.iParam2;
    const _float fDelay = static_cast<_float>(key.iParam3) / 1000.f;
    const _float fVolume = key.fParam0;
    const _float fPitch = key.fParam1;
    const _bool bSteal = key.bParam0;
    const _bool bLoop = key.bParam1;

    switch (eCommand)
    {
    case DTO::EAnimSoundCommand::OneShot:
    {
        if (iSoundHash == 0)
            return;

        if (fDelay > 0.f)
            m_pGameInstance->Play_OneShot_Delayed(m_iSoundLevelID, iSoundHash, fDelay, fVolume, fPitch, bSteal);
        else
            m_pGameInstance->Play_OneShot(m_iSoundLevelID, iSoundHash, fVolume, fPitch, bSteal);
    } break;
    case DTO::EAnimSoundCommand::ControlledPlay:
    {
        if (iSoundHash == 0 || iControlledId < 0)
            return;
        m_pGameInstance->Play_Controlled(m_iSoundLevelID, iSoundHash, iControlledId, fVolume, bLoop, fPitch);
    } break;
    case DTO::EAnimSoundCommand::ControlledStop:
        m_pGameInstance->Stop_Controlled(iControlledId);
        break;
    case DTO::EAnimSoundCommand::ControlledVolume:
        m_pGameInstance->Set_ControlledVolume(iControlledId, fVolume);
        break;
    case DTO::EAnimSoundCommand::ControlledPitch:
        m_pGameInstance->Set_ControlledPitch(iControlledId, fPitch);
        break;
    default:
        break;
    }
}

CToolAnimSoundPlayer* CToolAnimSoundPlayer::Create()
{
    CToolAnimSoundPlayer* pInstance = new CToolAnimSoundPlayer();

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CToolAnimSoundPlayer");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CToolAnimSoundPlayer::Clone(void* pArg)
{
    CToolAnimSoundPlayer* pInstance = new CToolAnimSoundPlayer(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CToolAnimSoundPlayer");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CToolAnimSoundPlayer::Free()
{
    Release_Event();
    Clear_SoundNotifies();

    Safe_Release(m_pOwnerModel);

    Super::Free();
}