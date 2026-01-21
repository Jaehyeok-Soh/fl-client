#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CSound_Manager : public CBase
{
    using Super = CBase;
private:
    CSound_Manager();
    virtual ~CSound_Manager() = default;

    HRESULT Initialize();
public:
    void    Update();

public:
    HRESULT Load_Sounds(const std::wstring& wstrFolderPath);

public:
    void Play_Controlled(const _tchar* pSoundKey, _uint iControlledId, _float fVolume, _bool  bLoop = false, _float fPitch = 1.f);
    void Stop_Controlled(_uint iControlledId);
    void Set_ControlledVolume(_uint iControlledId, _float fVolume);
    void Set_ControlledPitch(_uint iControlledId, _float fPitch);

    void PlayBGM(const _tchar* pSoundKey, _float fVolume, _bool bLoop = true, _float fPitch = 1.f);
    void Play_OneShot(const _tchar* pSoundKey, _float fVolume, _float fPitch = 1.f);
    void Play_RandOneShot(const _tchar* pSoundKey, _float fVolume, _int iCount, _float fPitch = 1.f);
public:
    void StopSound(_uint iChannelIndex);
    void StopAll();                  
private:
    static void  ApplyChannelParams(FMOD::Channel* pChannel, _float fVolume, _bool bLoop, _float fPitch);
    FMOD::Sound* FindSound(const std::wstring& wstrKey) const;
    void Reset_OneShotPool();
    void Reclaim_OneShots();
    void StopAndClearChannelSlot(_uint iIndex);
    void RemoveActiveOneShotIfExists(_uint iIndex);
private:
    enum { MAXCHANNEL = 32 };
    enum { CONTROLLED_COUNT = 10 };
    const _uint ONE_SHOT_BEGIN = CONTROLLED_COUNT;
    const _uint ONE_SHOT_END = MAXCHANNEL;
private:
    FMOD::System* m_pSystem = nullptr;
    FMOD::Channel* m_pChannelArr[MAXCHANNEL]{};

    std::unordered_map<std::wstring, FMOD::Sound*> m_umapSounds;
    std::vector<_uint> m_vecOneShotStack;
    std::vector<_uint> m_vecActiveOneShots;
public:
    static CSound_Manager* Create();
    virtual void Free() override;
};

NS_END