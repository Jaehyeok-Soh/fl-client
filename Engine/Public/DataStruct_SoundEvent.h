#pragma once
#include "ObjectDataBase.h"
#include "Anim_Event_Base.h"

NS_BEGIN(DTO)

enum class EAnimSoundCommand : unsigned int
{
    OneShot = 0,
    ControlledPlay,
    ControlledStop,
    ControlledVolume,
    ControlledPitch,
    END
};

NLOHMANN_JSON_SERIALIZE_ENUM(EAnimSoundCommand,
    {
        {EAnimSoundCommand::OneShot, "OneShot"},
        {EAnimSoundCommand::ControlledPlay, "ControlledPlay"},
        {EAnimSoundCommand::ControlledStop, "ControlledStop"},
        {EAnimSoundCommand::ControlledVolume, "ControlledVolume"},
        {EAnimSoundCommand::ControlledPitch, "ControlledPitch"},
        {EAnimSoundCommand::END, "END"},
    }
)

typedef struct tagSoundEvent : public ANIM_EVENT_BASE1
{
    EAnimSoundCommand   eCommand = { EAnimSoundCommand::END };
    string  strSoundTag = {};
    _float  fVolume = { 1.f };
    _float  fPitch = { 1.f };
    _float  fDelay = { 0.f };
    _bool   bSteal = { false };
    
    // Controlled Àü¿ë
    _int    iControlledId = { -1 };
    _bool   bLoop = { false };
}SOUNDEVENT;

typedef struct tagSoundEventInfoDesc
{
    string              strOwnerTag = { "Sample" };
    vector<SOUNDEVENT>  vecSoundEvents;
}SOUND_EVENT_INFO_DESC;

void to_json(json& j, const SOUNDEVENT& data);
void from_json(const json& j, SOUNDEVENT& data);
void to_json(json& j, const SOUND_EVENT_INFO_DESC& data);
void from_json(const json& j, SOUND_EVENT_INFO_DESC& data);

NS_END

NS_BEGIN(Engine)

class ENGINE_DLL CDataStruct_SoundEvent final : public IObjectDataBase {
    using Super = IObjectDataBase;
private:
    CDataStruct_SoundEvent() = default;
    virtual ~CDataStruct_SoundEvent() = default;
public:
    virtual _uint Get_Type() const override { return ENUM_TO_UINT(DTO::ECategory::SOUNDEVENT); }
    virtual const _string& Get_Tag() const override { return m_Data.strOwnerTag; }

    virtual json ToJson() const override;
    virtual HRESULT FromJson(const json& j) override;

    const DTO::SOUND_EVENT_INFO_DESC& Get_Data() const { return m_Data; }
    DTO::SOUND_EVENT_INFO_DESC& Get_Data() { return m_Data; }
private:
    DTO::SOUND_EVENT_INFO_DESC m_Data;

public:
    static CDataStruct_SoundEvent* Create() { return new CDataStruct_SoundEvent(); }
    virtual void Free() override { Super::Free(); }
};

NS_END