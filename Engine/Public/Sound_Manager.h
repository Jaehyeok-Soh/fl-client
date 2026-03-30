#pragma once
#include "Base.h"

#define MAX_SOUND_CHANNEL 32
#define LIMIT_ONESHOT_SOUNDS 5

#define MINPITCH 0.3f
#define MINVOLUME 0.5f

typedef struct tagGlobalMixPulse
{
    _bool bActive = { false };
    TIME_LINE tTimer{};

    _float fMinPitchMul = MINPITCH;
    _float fMinVolumeMul = MINVOLUME;

    void Start(_float fDuration, _float fPitchMul, _float fVolumeMul)
    {
        bActive = true;
        tTimer.Start(fDuration);
        fMinPitchMul = std::clamp(fPitchMul, 0.01f, 1.f);
        fMinVolumeMul = std::clamp(fVolumeMul, 0.f, 1.f);
    }

    void Clear()
    {
        bActive = false;
        tTimer.Clear();
        fMinPitchMul = 1.f;
        fMinVolumeMul = 1.f;
    }

    _bool Is_Active() const
    {
        return bActive && tTimer.Is_Active();
    }
}GLOBAL_MIX_PULSE;

typedef struct tagPendingSound
{
    _uint iLevelID = 0;
    _uint iSoundHash = 0;
    _float fRemainTime = 0.f;
    _float fVolume = 1.f;
    _float fPitch = 1.f;
    _bool bSteal = false;
}PENDING_ONESHOT;

typedef struct tagPendingBGM
{
    _uint   iLevelID = 0;
    _uint   iSoundHash = 0;
    _float  fRemainTime = 0.f;
    _float  fVolume = 1.f;
    _float  fPitch = 1.f;
    _float  fFadeInTime = 0.f;
}PENDING_BGM;

NS_BEGIN(Engine)

typedef struct tagSoundGroup
{
    ESoundCategory            eCategory = ESoundCategory::SFX;
    vector<FMOD::Sound*> vecSounds;
}SOUND_GROUP;

class CSound_Manager : public CBase
{
    using Super = CBase;

private:
    CSound_Manager();
    virtual ~CSound_Manager() = default;

    HRESULT Initialize(_uint iLevelCount);

public:
    void    Update(const _float fTimeDelta);
    // LevelID - 저장할레벨, ESoundCategory - 사운드 카테고리, wstrFolderPath - 폴더 경로
    HRESULT Load_Sounds(_uint iLevelID, ESoundCategory eCategory, const wstring& wstrFolderPath);
    void Clear(_uint iLevelID);
    void Release_LevelSounds(_uint iLevelID);
public:
    // 카테고리별 볼륨
    void   Set_CategoryVolume(ESoundCategory eCategory, _float fVolume);
    _float Get_CategoryVolume(ESoundCategory eCategory) const;

    void   Set_GroupCategory(_uint iLevelID, _uint iSoundHash, ESoundCategory eCategory);
public:
    // Engine_Enum.h의 EControlledChannel에 등록된 컨트롤 할 ID
    void Play_Controlled(_uint iLevelID, _uint iSoundHash, _uint iControlledId, _float fVolume, _bool bLoop = false, _float fPitch = 1.f);
    void Stop_Controlled(_uint iControlledId);
    void Set_ControlledVolume(_uint iControlledId, _float fVolume);
    void Set_ControlledPitch(_uint iControlledId, _float fPitch);

    void PlayBGM(_uint iLevelID, _uint iSoundHash, _float fVolume, _float fPitch = 1.f);
    void PlayBGM_FadeIn(_uint iLevelID, _uint iSoundHash, _float fVolume, _float fFadeInTime, _float fPitch = 1.f);
    void PlayBGM_Delayed(_uint iLevelID, _uint iSoundHash, _float fDelayed, _float fVolume, _float fPitch = 1.f, _float fFadeInTime = 0.f);
    void StopBGM_FadeOut(_float fFadeOutTime);
    void CrossFadeBGM(_uint iLevelID, _uint iSoundHash, _float fVolume, _float fFadeOutTime, _float fFadeInTime, _float fPitch = 1.f);

    // Steal 플래그를 키면 SoundChannel이 꽉찼을때 가장 빨리끝날 Channel을 강제로 종료후 사운드를 재생시킴
    void Play_OneShot(_uint iLevelID, _uint iSoundHash, _float fVolume, _float fPitch = 1.f, _bool bSteal = false);
    void Play_OneShot_Delayed(_uint iLevelID, _uint iSoundHash, _float fDelayedTime, _float fVolume, _float fPitch = 1.f, _bool bSteal = false);
    void Play_RandOneShot(_uint iLevelID, _uint iSoundHash, _float fVolume, _float fPitch = 1.f, _bool bSteal = false);

    // 이벤트
    void Play_GlobalMixPulse(_float fDuration, _bool m_bContainVolume = false);
    void Deactive_Slomo();
    void Active_Slomo();

    // Tool, 디버그용
    vector<SOUND_META> Get_SoundMetas(_uint iLevelID) const;
    const SOUND_META* Find_SoundMeta(_uint iLevelID, _uint iSoundHash) const;
    _bool Has_SoundTag(_uint iLevelID, const string& strTag) const;
public:
    void StopSound(_uint iChannelIndex);
    void StopAll();

private:
    void ApplyChannelParams(FMOD::Channel* pChannel, _float fFinalVolume, _bool bLoop, _float fPitch);

    SOUND_GROUP* FindGroup(_uint iLevelID, _uint iSoundHash);
    const SOUND_GROUP* FindGroup(_uint iLevelID, _uint iSoundHash) const;

    FMOD::Sound* Pick_SoundFromGroup(const SOUND_GROUP& tGroup) const;

    // Player_Example1, ..2, ..3 가능
    // Attack01, ...02 가능
    // Hit-Light-01, ...02 가능
    string    Extract_GroupKeyString(const wstring& wstrStem);

    _float Compute_FinalVolume(_float fBaseVolume, ESoundCategory eCategory) const;

    void Reset_OneShotPool();
    void Reclaim_OneShots();
    void Reclaim_ControlledChannels();

    void Stop_AndClearChannelSlot(_uint iIndex);
    void Remove_ActiveOneShotIfExists(_uint iIndex);

    void Set_ChannelRuntimeState(_uint iIndex, _uint iLevelID, _uint iGroupHash, _float fBaseVolume, ESoundCategory eCategory);
    void Reset_ChannelRuntimeState(_uint iIndex);
    void Apply_StoredChannelVolume(_uint iIndex);

    _uint Acquire_OneShotSlot(_uint iLevelID, _uint iSoundHash, _bool bSteal);
    _uint Find_StealCandidateOneShot() const;

    _uint Count_PlayingOneShot(_uint iLevelID, _uint iSoundHash) const;
    _uint Find_StealCandidate(_uint iLevelID, _uint iSoundHash) const;

    _bool Setup_FadePoints(FMOD::Channel* pChannel, _float fStartVolume, _float fEndVolume, _float fFadeTime, _bool bStopAtEnd);
    _bool Get_ChannelParentClock(FMOD::Channel* pChannel, OUT unsigned long long& iParentClock, OUT _int& iSampleRate);

    void Update_PendingOneShots(_float fTimeDelta);
    void Update_PendingBGMs(_float fTimeDelta);

    // 이벤트
    void Update_GlobalMixPulse(_float fTimeDelta);
    void Apply_GlobalMix(_float fPitchMul, _float fVolumeMul);
    void Reset_GlobalMixPulse();
private:
    const _uint ONE_SHOT_BEGIN = ENUM_TO_UINT(EControlledChannel::COUNT);
    const _uint ONE_SHOT_END = MAX_SOUND_CHANNEL;

private:
    _float m_fGlobalPitch = { 1.f };
    _float m_fGlobalSound = { 1.f };

    FMOD::System* m_pSystem = nullptr;
    FMOD::Channel* m_pChannelArr[MAX_SOUND_CHANNEL]{};

    vector<unordered_map<_uint, SOUND_GROUP>> m_umapSounds;
    vector<unordered_map<_uint, SOUND_META>> m_umapSoundMetaData;

    vector<_uint> m_vecOneShotStack;
    vector<_uint> m_vecActiveOneShots;
    vector<PENDING_ONESHOT> m_vecPendingOneShots;
    vector<PENDING_BGM> m_vecPendingBGMs;

    _float m_arrCategoryVolume[ENUM_TO_UINT(ESoundCategory::END)]{};

    // 이벤트
    FMOD::ChannelGroup* m_pMasterGroup = { nullptr };
    GLOBAL_MIX_PULSE m_tGlobalMixPulse{};


    // 라이브 볼륨 재계산, 그룹 추적용
    _float         m_arrChannelBaseVolume[MAX_SOUND_CHANNEL]{};
    ESoundCategory m_arrChannelCategory[MAX_SOUND_CHANNEL]{};
    _uint          m_arrChannelLevelID[MAX_SOUND_CHANNEL]{};
    _uint          m_arrChannelGroupHash[MAX_SOUND_CHANNEL]{};
public:
    static CSound_Manager* Create(_uint iLevelCount);
    virtual void Free() override;
};

NS_END