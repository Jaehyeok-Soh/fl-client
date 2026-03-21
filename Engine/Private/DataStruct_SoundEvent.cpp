#include "Engine_pch.h"
#include "DataStruct_SoundEvent.h"
#pragma push_macro("new")
#undef new
#include "json.hpp"
using json = nlohmann::json;
#pragma pop_macro("new")

NS_BEGIN(DTO)

    void to_json(json& j, const SOUNDEVENT& d)
    {
        to_json(j, static_cast<const ANIM_EVENT_BASE1&>(d));

        j["eCommand"] = d.eCommand;
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

        if (j.contains("eCommand"))         j.at("eCommand").get_to(d.eCommand);
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
        j["Type"] = ECategory::SOUNDEVENT;
        j["strOwnerTag"] = d.strOwnerTag;
        j["vecSoundEvents"] = d.vecSoundEvents;
    }

    void from_json(const json& j, SOUND_EVENT_INFO_DESC& d)
    {
        if (j.contains("strOwnerTag"))    j.at("strOwnerTag").get_to(d.strOwnerTag);
        if (j.contains("vecSoundEvents")) j.at("vecSoundEvents").get_to(d.vecSoundEvents);
    }

NS_END

NS_BEGIN(Engine)

// 엔진 데이터 구조체 구현부
json CDataStruct_SoundEvent::ToJson() const
{
    json j = json(m_Data);
    j["Type"] = Get_Type();
    return j;
}

HRESULT CDataStruct_SoundEvent::FromJson(const json& j)
{
    m_Data = j.get<DTO::SOUND_EVENT_INFO_DESC>();
    return S_OK;
}

NS_END