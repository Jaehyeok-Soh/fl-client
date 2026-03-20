#include "Engine_pch.h"
#include "Sound_Manager.h"
#include "GameInstance.h"

#define FMOD_CALL(x) do { FMOD_RESULT __r = (x); if (__r != FMOD_OK) { MSG_BOX("Failed, FMOD_CALL"); } } while(0)

CSound_Manager::CSound_Manager()
{
}

HRESULT CSound_Manager::Initialize(_uint iLevelCount)
{
    FMOD_RESULT fmodResult = FMOD::System_Create(&m_pSystem);
    m_vecPendingOneShots.reserve(50);
    if (fmodResult != FMOD_OK || m_pSystem == nullptr)
        return E_FAIL;

    fmodResult = m_pSystem->init(MAX_SOUND_CHANNEL, FMOD_INIT_NORMAL, nullptr);
    if (fmodResult != FMOD_OK)
        return E_FAIL;

    for (_uint i = 0; i < MAX_SOUND_CHANNEL; ++i)
    {
        m_pChannelArr[i] = nullptr;
        Reset_ChannelRuntimeState(i);
    }

    for (_uint i = 0; i < ENUM_TO_UINT(ESoundCategory::END); ++i)
        m_arrCategoryVolume[i] = 1.f;

    m_umapSounds.resize(iLevelCount);
    m_vecPendingBGMs.resize(iLevelCount);
    Reset_OneShotPool();
    return S_OK;
}

void CSound_Manager::Update(const _float fTimeDelta)
{
    if (m_pSystem)
        FMOD_CALL(m_pSystem->update());

    Reclaim_ControlledChannels();
    Reclaim_OneShots();
    Update_PendingOneShots(fTimeDelta);
    Update_PendingBGMs(fTimeDelta);
}

HRESULT CSound_Manager::Load_Sounds(_uint iLevelID, ESoundCategory eCategory, const wstring& wstrFolderPath)
{
    if (m_pSystem == nullptr)
        return E_FAIL;

    if (iLevelID >= m_umapSounds.size())
        return E_FAIL;

    std::filesystem::path folderPath{ wstrFolderPath };
    if (std::filesystem::exists(folderPath) == false || std::filesystem::is_directory(folderPath) == false)
        return E_FAIL;

    unordered_map<_uint, SOUND_GROUP>& sounds = m_umapSounds[iLevelID];

    // 폴더 경로에서 .wav 파일 추출
    for (const auto& entry : std::filesystem::recursive_directory_iterator(folderPath))
    {
        if (entry.is_regular_file() == false)
            continue;

        const std::filesystem::path entryPath = entry.path();
        if (entryPath.has_extension() == false)
            continue;

        wstring wstrExtension = entryPath.extension().wstring();
        std::transform(wstrExtension.begin(), wstrExtension.end(), wstrExtension.begin(), ::towlower);
        if (wstrExtension != L".wav")
            continue;

        const wstring wstrStem = entryPath.stem().wstring();

        // 파일명 기준 마지막 숫자를 제외한 GroupKey 추출
        const string strGroupKey = Extract_GroupKeyString(wstrStem);
        if (strGroupKey.empty())
            continue;

        const _uint iSoundHash = Engine_Utils::ToHash(strGroupKey.c_str());
        const std::string strPath = Engine_Utils::ToString(entryPath.wstring());

        FMOD::Sound* pSound = nullptr;
        FMOD_RESULT fmodResult = m_pSystem->createSound(strPath.c_str(), FMOD_DEFAULT, nullptr, &pSound);
        if (fmodResult != FMOD_OK || pSound == nullptr)
            continue;

        FMOD_CALL(pSound->setMode(FMOD_LOOP_OFF));

        // 해시 중복 검사
        auto iter = sounds.find(iSoundHash);
        if (iter != sounds.end())
        {
            if (iter->second.eCategory != eCategory)
            {
                FMOD_CALL(pSound->release());
                MSG_BOX("CSound_Manager::Load_Sounds : Same hash in same level with different category");
                continue;
            }
        }

        SOUND_GROUP& group = sounds[iSoundHash];
        if (group.vecSounds.empty())
            group.eCategory = eCategory;

        group.vecSounds.push_back(pSound);
    }

    return S_OK;
}

void CSound_Manager::Clear(_uint iLevelID)
{
    for (_uint i = 0; i < MAX_SOUND_CHANNEL; ++i)
    {
        if (m_arrChannelLevelID[i] == 0 /* static */ ||
            m_arrChannelLevelID[i] == iLevelID)
        {
            StopSound(i);
        }
    }

    m_vecPendingBGMs.clear();
    m_vecPendingOneShots.clear();
    Release_LevelSounds(iLevelID);
}

void CSound_Manager::Release_LevelSounds(_uint iLevelID)
{
    for (auto& Pair : m_umapSounds[iLevelID])
    {
        for (FMOD::Sound* pSound : Pair.second.vecSounds)
        {
            if (pSound)
                FMOD_CALL(pSound->release());
        }
    }

    m_umapSounds[iLevelID].clear();
}

void CSound_Manager::Set_CategoryVolume(ESoundCategory eCategory, _float fVolume)
{
    const _uint iCategoryIndex = ENUM_TO_UINT(eCategory);
    if (iCategoryIndex >= ENUM_TO_UINT(ESoundCategory::END))
        return;

    m_arrCategoryVolume[iCategoryIndex] = std::clamp(fVolume, 0.f, 1.f);

    for (_uint i = 0; i < MAX_SOUND_CHANNEL; ++i)
        Apply_StoredChannelVolume(i);
}

_float CSound_Manager::Get_CategoryVolume(ESoundCategory eCategory) const
{
    const _uint iCategoryIndex = ENUM_TO_UINT(eCategory);
    if (iCategoryIndex >= ENUM_TO_UINT(ESoundCategory::END))
        return 1.f;

    return m_arrCategoryVolume[iCategoryIndex];
}

void CSound_Manager::Set_GroupCategory(_uint iLevelID, _uint iSoundHash, ESoundCategory eCategory)
{
    SOUND_GROUP* pGroup = FindGroup(iLevelID, iSoundHash);
    if (pGroup == nullptr)
        return;

    pGroup->eCategory = eCategory;

    for (_uint i = 0; i < MAX_SOUND_CHANNEL; ++i)
    {
        if (m_pChannelArr[i] == nullptr)
            continue;

        if (m_arrChannelLevelID[i] != iLevelID)
            continue;

        if (m_arrChannelGroupHash[i] != iSoundHash)
            continue;

        m_arrChannelCategory[i] = eCategory;
        Apply_StoredChannelVolume(i);
    }
}

void CSound_Manager::Play_Controlled(_uint iLevelID, _uint iSoundHash, _uint iControlledId, _float fVolume, _bool bLoop, _float fPitch)
{
    if (m_pSystem == nullptr)
        return;

    if (iControlledId >= ONE_SHOT_BEGIN)
        return;

    SOUND_GROUP* pGroup = FindGroup(iLevelID, iSoundHash);
    if (pGroup == nullptr || pGroup->vecSounds.empty())
        return;

    FMOD::Sound* pSound = Pick_SoundFromGroup(*pGroup);
    if (pSound == nullptr)
        return;

    // 해당 ControllID Channel 정리
    Stop_AndClearChannelSlot(iControlledId);

    FMOD::Channel* pChannel = nullptr;
    FMOD_RESULT fmodResult = m_pSystem->playSound(pSound, nullptr, true, &pChannel);
    if (fmodResult != FMOD_OK || pChannel == nullptr)
        return;

    const _float fFinalVolume = Compute_FinalVolume(fVolume, pGroup->eCategory);
    ApplyChannelParams(pChannel, fFinalVolume, bLoop, fPitch);

    FMOD_CALL(pChannel->setPaused(false));
    m_pChannelArr[iControlledId] = pChannel;

    Set_ChannelRuntimeState(iControlledId, iLevelID, iSoundHash, fVolume, pGroup->eCategory);
}

void CSound_Manager::PlayBGM(_uint iLevelID, _uint iSoundHash, _float fVolume, _float fPitch)
{
    Play_Controlled(iLevelID, iSoundHash, ENUM_TO_UINT(EControlledChannel::BGM), fVolume, true, fPitch);
}

void CSound_Manager::PlayBGM_FadeIn(_uint iLevelID, _uint iSoundHash, _float fVolume, _float fFadeInTime, _float fPitch)
{
    if (m_pSystem == nullptr)
        return;

    const _uint iBGMChannel = ENUM_TO_UINT(EControlledChannel::BGM);

    SOUND_GROUP* pGroup = FindGroup(iLevelID, iSoundHash);
    if (pGroup == nullptr || pGroup->vecSounds.empty())
        return;

    FMOD::Sound* pSound = Pick_SoundFromGroup(*pGroup);
    if (pSound == nullptr)
        return;

    Stop_AndClearChannelSlot(iBGMChannel);

    FMOD::Channel* pChannel = nullptr;
    FMOD_RESULT fmodResult = m_pSystem->playSound(pSound, nullptr, true, &pChannel);
    if (fmodResult != FMOD_OK || pChannel == nullptr)
        return;

    const _float fFinalVolume = Compute_FinalVolume(fVolume, ESoundCategory::BGM);

    FMOD_CALL(pChannel->setVolume(0.f));
    FMOD_CALL(pChannel->setPitch((fPitch > 0.f) ? fPitch : 0.01f));
    FMOD_CALL(pChannel->setMode(FMOD_LOOP_NORMAL));
    FMOD_CALL(pChannel->setLoopCount(-1));
    FMOD_CALL(pChannel->setPaused(false));

    Setup_FadePoints(pChannel, 0.f, fFinalVolume, fFadeInTime, false);

    m_pChannelArr[iBGMChannel] = pChannel;
    Set_ChannelRuntimeState(iBGMChannel, iLevelID, iSoundHash, fVolume, ESoundCategory::BGM);
}

void CSound_Manager::PlayBGM_Delayed(_uint iLevelID, _uint iSoundHash, _float fDelayed, _float fVolume, _float fPitch, _float fFadeInTime)
{
    if (iLevelID >= m_umapSounds.size())
        return;

    m_vecPendingBGMs.clear();

    if (fDelayed <= 0.f)
    {
        if (fFadeInTime > 0.f)
            PlayBGM_FadeIn(iLevelID, iSoundHash, fVolume, fFadeInTime, fPitch);
        else
            PlayBGM(iLevelID, iSoundHash, fVolume, fPitch);

        return;
    }

    PENDING_BGM tPending;
    tPending.iLevelID = iLevelID;
    tPending.iSoundHash = iSoundHash;
    tPending.fRemainTime = fDelayed;
    tPending.fVolume = fVolume;
    tPending.fPitch = fPitch;
    tPending.fFadeInTime = fFadeInTime;

    m_vecPendingBGMs.push_back(tPending);
}

void CSound_Manager::StopBGM_FadeOut(_float fFadeOutTime)
{
    const _uint iBGMChannel = ENUM_TO_UINT(EControlledChannel::BGM);

    if (iBGMChannel >= MAX_SOUND_CHANNEL)
        return;

    FMOD::Channel* pChannel = m_pChannelArr[iBGMChannel];
    if (pChannel == nullptr)
        return;

    _float fCurrentVolume = Compute_FinalVolume(
        m_arrChannelBaseVolume[iBGMChannel],
        m_arrChannelCategory[iBGMChannel]);

    if (pChannel->getVolume(&fCurrentVolume) != FMOD_OK)
        fCurrentVolume = 1.f;

    Setup_FadePoints(pChannel, fCurrentVolume, 0.f, fFadeOutTime, true);
}

void CSound_Manager::CrossFadeBGM(_uint iLevelID, _uint iSoundHash, _float fVolume, _float fFadeOutTime, _float fFadeInTime, _float fPitch)
{
    StopBGM_FadeOut(fFadeOutTime);
    PlayBGM_Delayed(iLevelID, iSoundHash, fFadeOutTime, fVolume, fPitch, fFadeInTime);
}

void CSound_Manager::Stop_Controlled(_uint iControlledId)
{
    if (iControlledId >= ONE_SHOT_BEGIN)
        return;

    Stop_AndClearChannelSlot(iControlledId);
}

void CSound_Manager::Set_ControlledVolume(_uint iControlledId, _float fVolume)
{
    if (iControlledId >= ONE_SHOT_BEGIN)
        return;

    if (m_pChannelArr[iControlledId] == nullptr)
        return;

    m_arrChannelBaseVolume[iControlledId] = std::clamp(fVolume, 0.f, 1.f);
    Apply_StoredChannelVolume(iControlledId);
}

void CSound_Manager::Set_ControlledPitch(_uint iControlledId, _float fPitch)
{
    if (iControlledId >= ONE_SHOT_BEGIN)
        return;

    FMOD::Channel* pChannel = m_pChannelArr[iControlledId];
    if (pChannel == nullptr)
        return;

    if (fPitch <= 0.f)
        fPitch = 0.01f;

    FMOD_CALL(pChannel->setPitch(fPitch));
}

void CSound_Manager::Play_OneShot(_uint iLevelID, _uint iSoundHash, _float fVolume, _float fPitch, _bool bSteal)
{
    if (m_pSystem == nullptr)
        return;

    SOUND_GROUP* pGroup = FindGroup(iLevelID, iSoundHash);
    if (pGroup == nullptr || pGroup->vecSounds.empty())
        return;

    FMOD::Sound* pSound = Pick_SoundFromGroup(*pGroup);
    if (pSound == nullptr)
        return;

    const _uint iChannelIndex = Acquire_OneShotSlot(iLevelID, iSoundHash, bSteal);
    if (iChannelIndex >= MAX_SOUND_CHANNEL)
        return;

    Stop_AndClearChannelSlot(iChannelIndex);

    FMOD::Channel* pChannel = nullptr;
    FMOD_RESULT fmodResult = m_pSystem->playSound(pSound, nullptr, true, &pChannel);
    if (fmodResult != FMOD_OK || pChannel == nullptr)
    {
        if (std::find(m_vecOneShotStack.begin(), m_vecOneShotStack.end(), iChannelIndex) == m_vecOneShotStack.end())
            m_vecOneShotStack.push_back(iChannelIndex);
        return;
    }

    const _float fFinalVolume = Compute_FinalVolume(fVolume, pGroup->eCategory);
    ApplyChannelParams(pChannel, fFinalVolume, false, fPitch);

    FMOD_CALL(pChannel->setPaused(false));

    m_pChannelArr[iChannelIndex] = pChannel;
    m_vecActiveOneShots.push_back(iChannelIndex);

    Set_ChannelRuntimeState(iChannelIndex, iLevelID, iSoundHash, fVolume, pGroup->eCategory);
}

void CSound_Manager::Play_RandOneShot(_uint iLevelID, _uint iSoundHash, _float fVolume, _float fPitch, _bool bSteal)
{
    Play_OneShot(iLevelID, iSoundHash, fVolume, fPitch, bSteal);
}

void CSound_Manager::Play_OneShot_Delayed(_uint iLevelID, _uint iSoundHash, _float fDelayedTime, _float fVolume, _float fPitch, _bool bSteal)
{
    if (iLevelID >= m_umapSounds.size())
        return;

    if (fDelayedTime <= 0.f)
    {
        Play_OneShot(iLevelID, iSoundHash, fVolume, fPitch, bSteal);
        return;
    }

    PENDING_ONESHOT tPending;
    tPending.iLevelID = iLevelID;
    tPending.iSoundHash = iSoundHash;
    tPending.fRemainTime = fDelayedTime;
    tPending.fVolume = fVolume;
    tPending.fPitch = fPitch;
    tPending.bSteal = bSteal;

    m_vecPendingOneShots.push_back(tPending);
}

void CSound_Manager::StopSound(_uint iChannelIndex)
{
    if (iChannelIndex >= MAX_SOUND_CHANNEL)
        return;

    Stop_AndClearChannelSlot(iChannelIndex);

    if (iChannelIndex >= ONE_SHOT_BEGIN && iChannelIndex < ONE_SHOT_END)
    {
        Remove_ActiveOneShotIfExists(iChannelIndex);

        if (std::find(m_vecOneShotStack.begin(), m_vecOneShotStack.end(), iChannelIndex) == m_vecOneShotStack.end())
            m_vecOneShotStack.push_back(iChannelIndex);
    }
}

void CSound_Manager::StopAll()
{
    for (_uint i = 0; i < MAX_SOUND_CHANNEL; ++i)
        Stop_AndClearChannelSlot(i);

    m_vecPendingBGMs.clear();
    m_vecPendingOneShots.clear();
    Reset_OneShotPool();
}

void CSound_Manager::ApplyChannelParams(FMOD::Channel* pChannel, _float fFinalVolume, _bool bLoop, _float fPitch)
{
    if (pChannel == nullptr)
        return;

    fFinalVolume = std::clamp(fFinalVolume, 0.f, 1.f);
    if (fPitch <= 0.f)
        fPitch = 0.01f;

    FMOD_CALL(pChannel->setVolume(fFinalVolume));
    FMOD_CALL(pChannel->setPitch(fPitch));

    if (bLoop)
    {
        FMOD_CALL(pChannel->setMode(FMOD_LOOP_NORMAL));
        FMOD_CALL(pChannel->setLoopCount(-1));
    }
    else
    {
        FMOD_CALL(pChannel->setMode(FMOD_LOOP_OFF));
        FMOD_CALL(pChannel->setLoopCount(0));
    }
}

SOUND_GROUP* CSound_Manager::FindGroup(_uint iLevelID, _uint iSoundHash)
{
    if (iLevelID >= m_umapSounds.size())
        return nullptr;

    auto it = m_umapSounds[iLevelID].find(iSoundHash);
    if (it == m_umapSounds[iLevelID].end())
        return nullptr;

    return &it->second;
}

const SOUND_GROUP* CSound_Manager::FindGroup(_uint iLevelID, _uint iSoundHash) const
{
    if (iLevelID >= m_umapSounds.size())
        return nullptr;

    auto it = m_umapSounds[iLevelID].find(iSoundHash);
    if (it == m_umapSounds[iLevelID].end())
        return nullptr;

    return &it->second;
}

FMOD::Sound* CSound_Manager::Pick_SoundFromGroup(const SOUND_GROUP& tGroup) const
{
    if (tGroup.vecSounds.empty())
        return nullptr;

    if (tGroup.vecSounds.size() == 1)
        return tGroup.vecSounds.front();

    const _int iRand = CGameInstance::GetInstance()->Rand_Int(0, static_cast<_int>(tGroup.vecSounds.size()) - 1);
    return tGroup.vecSounds[iRand];
}

string CSound_Manager::Extract_GroupKeyString(const wstring& wstrStem)
{
    if (wstrStem.empty())
        return std::string();

    size_t iPos = wstrStem.size();
    while (iPos > 0 && ::iswdigit(wstrStem[iPos - 1]))
        --iPos;

    std::wstring wstrGroupKey = (iPos == wstrStem.size()) ? wstrStem : wstrStem.substr(0, iPos);

    while (wstrGroupKey.empty() == false)
    {
        const wchar_t ch = wstrGroupKey.back();
        if (ch == L'_' || ch == L'-' || ch == L' ')
            wstrGroupKey.pop_back();
        else
            break;
    }

    if (wstrGroupKey.empty())
        wstrGroupKey = wstrStem;

    return Engine_Utils::ToString(wstrGroupKey);
}

_float CSound_Manager::Compute_FinalVolume(_float fBaseVolume, ESoundCategory eCategory) const
{
    fBaseVolume = std::clamp(fBaseVolume, 0.f, 1.f);

    const _uint iCategoryIndex = ENUM_TO_UINT(eCategory);
    if (iCategoryIndex >= ENUM_TO_UINT(ESoundCategory::END))
        return fBaseVolume;

    return std::clamp(fBaseVolume * m_arrCategoryVolume[iCategoryIndex], 0.f, 1.f);
}

void CSound_Manager::Reset_OneShotPool()
{
    m_vecOneShotStack.clear();
    m_vecActiveOneShots.clear();

    m_vecOneShotStack.reserve(ONE_SHOT_END - ONE_SHOT_BEGIN);
    for (_uint i = ONE_SHOT_BEGIN; i < ONE_SHOT_END; ++i)
        m_vecOneShotStack.push_back(i);
}

void CSound_Manager::Reclaim_OneShots()
{
    for (size_t i = 0; i < m_vecActiveOneShots.size(); )
    {
        const _uint iIndex = m_vecActiveOneShots[i];
        FMOD::Channel* pChannel = (iIndex < MAX_SOUND_CHANNEL) ? m_pChannelArr[iIndex] : nullptr;

        _bool bPlaying = false;
        if (pChannel)
        {
            FMOD_RESULT fmodResult = pChannel->isPlaying(&bPlaying);
            if (fmodResult != FMOD_OK)
                bPlaying = false;
        }

        if (pChannel == nullptr || bPlaying == false)
        {
            Stop_AndClearChannelSlot(iIndex);

            const _uint iBackIndex = m_vecActiveOneShots.back();
            m_vecActiveOneShots[i] = iBackIndex;
            m_vecActiveOneShots.pop_back();

            if (std::find(m_vecOneShotStack.begin(), m_vecOneShotStack.end(), iIndex) == m_vecOneShotStack.end())
                m_vecOneShotStack.push_back(iIndex);

            continue;
        }

        ++i;
    }
}

void CSound_Manager::Reclaim_ControlledChannels()
{
    for (_uint i = 0; i < ONE_SHOT_BEGIN; ++i)
    {
        FMOD::Channel* pChannel = m_pChannelArr[i];
        if (pChannel == nullptr)
            continue;

        _bool bPlaying = false;
        if (pChannel->isPlaying(&bPlaying) != FMOD_OK || bPlaying == false)
        {
            m_pChannelArr[i] = nullptr;
            Reset_ChannelRuntimeState(i);
        }
    }
}

void CSound_Manager::Stop_AndClearChannelSlot(_uint iIndex)
{
    if (iIndex >= MAX_SOUND_CHANNEL)
        return;

    FMOD::Channel* pChannel = m_pChannelArr[iIndex];
    if (pChannel)
    {
        _bool bPlaying = false;
        if (pChannel->isPlaying(&bPlaying) == FMOD_OK && bPlaying)
            FMOD_CALL(pChannel->stop());
    }

    m_pChannelArr[iIndex] = nullptr;
    Reset_ChannelRuntimeState(iIndex);
}

void CSound_Manager::Remove_ActiveOneShotIfExists(_uint iIndex)
{
    auto it = std::find(m_vecActiveOneShots.begin(), m_vecActiveOneShots.end(), iIndex);
    if (it == m_vecActiveOneShots.end())
        return;

    *it = m_vecActiveOneShots.back();
    m_vecActiveOneShots.pop_back();
}

void CSound_Manager::Set_ChannelRuntimeState(_uint iIndex, _uint iLevelID, _uint iGroupHash, _float fBaseVolume, ESoundCategory eCategory)
{
    if (iIndex >= MAX_SOUND_CHANNEL)
        return;

    m_arrChannelBaseVolume[iIndex] = std::clamp(fBaseVolume, 0.f, 1.f);
    m_arrChannelCategory[iIndex] = eCategory;
    m_arrChannelLevelID[iIndex] = iLevelID;
    m_arrChannelGroupHash[iIndex] = iGroupHash;
}

void CSound_Manager::Reset_ChannelRuntimeState(_uint iIndex)
{
    if (iIndex >= MAX_SOUND_CHANNEL)
        return;

    m_arrChannelBaseVolume[iIndex] = 1.f;
    m_arrChannelCategory[iIndex] = ESoundCategory::SFX;
    m_arrChannelLevelID[iIndex] = 0;
    m_arrChannelGroupHash[iIndex] = 0;
}

void CSound_Manager::Apply_StoredChannelVolume(_uint iIndex)
{
    if (iIndex >= MAX_SOUND_CHANNEL)
        return;

    FMOD::Channel* pChannel = m_pChannelArr[iIndex];
    if (pChannel == nullptr)
        return;

    const _float fFinalVolume = Compute_FinalVolume(
        m_arrChannelBaseVolume[iIndex],
        m_arrChannelCategory[iIndex]);

    FMOD_CALL(pChannel->setVolume(fFinalVolume));
}

_uint CSound_Manager::Acquire_OneShotSlot(_uint iLevelID, _uint iSoundHash, _bool bSteal)
{
    const _uint iSameGroupCount = Count_PlayingOneShot(iLevelID, iSoundHash);

    if (iSameGroupCount >= LIMIT_ONESHOT_SOUNDS)
    {
        const _uint iStealSameGroup = Find_StealCandidate(iLevelID, iSoundHash);
        if (iSameGroupCount < MAX_SOUND_CHANNEL)
        {
            Stop_AndClearChannelSlot(iStealSameGroup);
            Remove_ActiveOneShotIfExists(iStealSameGroup);
            return iStealSameGroup;
        }
    }

    if (m_vecOneShotStack.empty() == false)
    {
        const _uint iIndex = m_vecOneShotStack.back();
        m_vecOneShotStack.pop_back();
        return iIndex;
    }

    if (bSteal == false)
        return MAX_SOUND_CHANNEL;

    const _uint iStealIndex = Find_StealCandidateOneShot();
    if (iStealIndex >= MAX_SOUND_CHANNEL)
        return MAX_SOUND_CHANNEL;

    Stop_AndClearChannelSlot(iStealIndex);
    Remove_ActiveOneShotIfExists(iStealIndex);
    return iStealIndex;
}

_uint CSound_Manager::Find_StealCandidateOneShot() const
{
    if (m_vecActiveOneShots.empty())
        return MAX_SOUND_CHANNEL;

    _uint iBestIndex = MAX_SOUND_CHANNEL;
    _uint iBestRemainMs = UINT_MAX;

    for (const _uint iIndex : m_vecActiveOneShots)
    {
        if (iIndex >= MAX_SOUND_CHANNEL)
            continue;

        FMOD::Channel* pChannel = m_pChannelArr[iIndex];
        if (pChannel == nullptr)
            return iIndex;

        _bool bPlaying = false;
        if (pChannel->isPlaying(&bPlaying) != FMOD_OK || bPlaying == false)
            return iIndex;

        FMOD::Sound* pCurrentSound = nullptr;
        if (pChannel->getCurrentSound(&pCurrentSound) != FMOD_OK || pCurrentSound == nullptr)
            return iIndex;

        _uint iLengthMs = 0;
        _uint iPositionMs = 0;

        if (pCurrentSound->getLength(&iLengthMs, FMOD_TIMEUNIT_MS) != FMOD_OK)
            iLengthMs = 0;
        if (pChannel->getPosition(&iPositionMs, FMOD_TIMEUNIT_MS) != FMOD_OK)
            iPositionMs = 0;

        const _uint iRemainMs = (iLengthMs > iPositionMs) ? (iLengthMs - iPositionMs) : 0;
        if (iRemainMs < iBestRemainMs)
        {
            iBestRemainMs = iRemainMs;
            iBestIndex = iIndex;
        }
    }

    return iBestIndex;
}

_uint CSound_Manager::Count_PlayingOneShot(_uint iLevelID, _uint iSoundHash) const
{
    _uint iCount = 0;

    for (const _uint iIndex : m_vecActiveOneShots)
    {
        if (iIndex >= MAX_SOUND_CHANNEL)
            continue;

        if (m_arrChannelLevelID[iIndex] != iLevelID)
            continue;

        if (m_arrChannelGroupHash[iIndex] != iSoundHash)
            continue;

        FMOD::Channel* pChannel = m_pChannelArr[iIndex];
        if (pChannel == nullptr)
            continue;

        _bool bPlaying = { false };
        if (pChannel->isPlaying(&bPlaying) != FMOD_OK || bPlaying == false)
            continue;

        ++iCount;
    }

    return iCount;
}

_uint CSound_Manager::Find_StealCandidate(_uint iLevelID, _uint iSoundHash) const
{
    _uint iBestIndex = MAX_SOUND_CHANNEL;
    _uint iBestRemainMs = UINT_MAX;

    for (const _uint iIndex : m_vecActiveOneShots)
    {
        if (iIndex >= MAX_SOUND_CHANNEL)
            continue;

        if (m_arrChannelLevelID[iIndex] != iLevelID)
            continue;

        if (m_arrChannelGroupHash[iIndex] != iSoundHash)
            continue;

        FMOD::Channel* pChannel = m_pChannelArr[iIndex];
        if (pChannel == nullptr)
            return iIndex;

        _bool bPlaying{ false };
        if (pChannel->isPlaying(&bPlaying) != FMOD_OK || bPlaying == false)
            return iIndex;

        FMOD::Sound* pCurrentSound = { nullptr };
        if (pChannel->getCurrentSound(&pCurrentSound) != FMOD_OK || pCurrentSound == nullptr)
            return iIndex;

        _uint iLengthMS = 0;
        _uint iPositionMs = 0;

        if (pCurrentSound->getLength(&iLengthMS, FMOD_TIMEUNIT_MS) != FMOD_OK)
            iLengthMS = 0;
        if (pChannel->getPosition(&iPositionMs, FMOD_TIMEUNIT_MS) != FMOD_OK)
            iPositionMs = 0;

        const _uint iReaminMs = (iLengthMS > iPositionMs) ? (iLengthMS - iPositionMs) : 0;
        if (iReaminMs < iBestRemainMs)
        {
            iBestRemainMs = iReaminMs;
            iBestIndex = iIndex;
        }
    }

    return iBestIndex;
}

_bool CSound_Manager::Setup_FadePoints(FMOD::Channel* pChannel, _float fStartVolume, _float fEndVolume, _float fFadeTime, _bool bStopAtEnd)
{
    if (pChannel == nullptr)
        return false;

    fStartVolume = std::clamp(fStartVolume, 0.f, 1.f);
    fEndVolume = std::clamp(fEndVolume, 0.f, 1.f);

    if (fFadeTime <= 0.f)
    {
        FMOD_CALL(pChannel->setVolume(std::clamp(fEndVolume, 0.f, 1.f)));

        if (bStopAtEnd)
            FMOD_CALL(pChannel->stop());

        return true;
    }

    unsigned long long ullParentClock = 0;
    _uint iSampleRate = 0;
    if (Get_ChannelParentClock(pChannel, ullParentClock, iSampleRate) == false)
        return false;

    const unsigned long long ullFadeSamples = static_cast<unsigned long long>(fFadeTime * static_cast<_float>(iSampleRate));
    const unsigned long long ullEndClock = ullParentClock + ullFadeSamples;

    FMOD_CALL(pChannel->removeFadePoints(0ULL, ULLONG_MAX));
    FMOD_CALL(pChannel->addFadePoint(ullParentClock, fStartVolume));
    FMOD_CALL(pChannel->addFadePoint(ullEndClock, fEndVolume));

    if (bStopAtEnd)
        FMOD_CALL(pChannel->setDelay(0ULL, ullEndClock, true));

    return true;
}

_bool CSound_Manager::Get_ChannelParentClock(FMOD::Channel* pChannel, OUT unsigned long long& iParentClock, OUT _uint& iSampleRate)
{
    if (m_pSystem == nullptr || pChannel == nullptr)
        return false;

    unsigned long long ullDSPClock = 0;
    unsigned long long ullParentDSPClock = 0;

    if (pChannel->getDSPClock(&ullDSPClock, &ullParentDSPClock) != FMOD_OK)
        return false;

    int iRate = 0;
    if (m_pSystem->getSoftwareFormat(&iRate, nullptr, nullptr) != FMOD_OK)
        return false;

    iParentClock = ullParentDSPClock;
    iSampleRate = static_cast<_uint>(iRate);
    return true;
}

void CSound_Manager::Update_PendingOneShots(_float fTimeDelta)
{
    for (size_t i = 0; i < m_vecPendingOneShots.size(); )
    {
        PENDING_ONESHOT& tPending = m_vecPendingOneShots[i];
        tPending.fRemainTime -= fTimeDelta;

        if (tPending.fRemainTime <= 0.f)
        {
            Play_OneShot(
                tPending.iLevelID,
                tPending.iSoundHash,
                tPending.fVolume,
                tPending.fPitch,
                tPending.bSteal
            );

            m_vecPendingOneShots[i] = m_vecPendingOneShots.back();
            m_vecPendingOneShots.pop_back();
            continue;
        }

        ++i;
    }
}

void CSound_Manager::Update_PendingBGMs(_float fTimeDelta)
{
    if (fTimeDelta <= 0.f)
        return;

    for (size_t i = 0; i < m_vecPendingBGMs.size(); )
    {
        PENDING_BGM& tPending = m_vecPendingBGMs[i];
        tPending.fRemainTime -= fTimeDelta;

        if (tPending.fRemainTime <= 0.f)
        {
            if (tPending.fFadeInTime > 0.f)
                PlayBGM_FadeIn(tPending.iLevelID, tPending.iSoundHash, tPending.fVolume, tPending.fFadeInTime, tPending.fPitch);
            else
                PlayBGM(tPending.iLevelID, tPending.iSoundHash, tPending.fVolume, tPending.fPitch);

            m_vecPendingBGMs[i] = m_vecPendingBGMs.back();
            m_vecPendingBGMs.pop_back();
            continue;
        }

        ++i;
    }
}

CSound_Manager* CSound_Manager::Create(_uint iLevelCount)
{
    CSound_Manager* pInstance = new CSound_Manager();
    if (FAILED(pInstance->Initialize(iLevelCount)))
    {
        Safe_Release(pInstance);
        MSG_BOX("CSound_Manager::Create, Failed");
    }
    return pInstance;
}

void CSound_Manager::Free()
{
    StopAll();

    for (auto& sounds : m_umapSounds)
    {
        for (auto& Pair : sounds)
        {
            for (FMOD::Sound* pSound : Pair.second.vecSounds)
            {
                if (pSound)
                    FMOD_CALL(pSound->release());
            }
            Pair.second.vecSounds.clear();
        }
        sounds.clear();
    }
    m_umapSounds.clear();

    if (m_pSystem)
    {
        FMOD_CALL(m_pSystem->close());
        FMOD_CALL(m_pSystem->release());
        m_pSystem = nullptr;
    }

    Super::Free();
}