#include "Engine_pch.h"
#include "DataStruct_SoundEvent.h"

namespace DTO
{
    void to_json(json& j, const SOUNDEVENT& d)
    {
        to_json(j, static_cast<const ANIM_EVENT_BASE1&>(d));

        j["iCommand"] = d.iCommand;
        j["strSoundTag"] = d.strSoundTag;
        j["fVolume"] = d.fVolume;
        j["fPitch"] = d.fPitch;
        j["fDelay"] = d.fDelay;
        j["bSteal"] = d.bSteal;
        j["iControlledId"] = d.iControlledId;
        j["bLoop"] = d.bLoop;
    }

    void from_json(const json& j, SOUNDEVENT& d)
    {
        from_json(j, static_cast<ANIM_EVENT_BASE1&>(d));

        if (j.contains("iCommand"))         j.at("iCommand").get_to(d.iCommand);
        if (j.contains("strSoundTag"))      j.at("strSoundTag").get_to(d.strSoundTag);
        if (j.contains("fVolume"))          j.at("fVolume").get_to(d.fVolume);
        if (j.contains("fPitch"))           j.at("fPitch").get_to(d.fPitch);
        if (j.contains("fDelay"))           j.at("fDelay").get_to(d.fDelay);
        if (j.contains("bSteal"))           j.at("bSteal").get_to(d.bSteal);
        if (j.contains("iControlledId"))    j.at("iControlledId").get_to(d.iControlledId);
        if (j.contains("bLoop"))            j.at("bLoop").get_to(d.bLoop);
    }

    void to_json(json& j, const SOUND_EVENT_INFO_DESC& d)
    {
        j["strOwnerTag"] = d.strOwnerTag;
        j["vecSoundEvents"] = d.vecSoundEvents;
    }

    void from_json(const json& j, SOUND_EVENT_INFO_DESC& d)
    {
        if (j.contains("strOwnerTag"))    j.at("strOwnerTag").get_to(d.strOwnerTag);
        if (j.contains("vecSoundEvents")) j.at("vecSoundEvents").get_to(d.vecSoundEvents);
    }
}