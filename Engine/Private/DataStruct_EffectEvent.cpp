#include "Engine_pch.h"
#include "DataStruct_EffectEvent.h"
#pragma push_macro("new")
#undef new
#include "json.hpp"
using json = nlohmann::json;
#pragma pop_macro("new")

NS_BEGIN(DTO)

void to_json(json& j, const EFFECT_EVENT_SCRIPT& data) {
    j = json{
        { "fDuration", data.fDuration },
        { "strEffectTag", data.strEffectTag },
        { "eSimulationType", data.iSimulationType },
        { "strSocketName", data.strSocketName },
        { "vOffset", { {"x", data.vOffset.x}, {"y", data.vOffset.y}, {"z", data.vOffset.z} } },
        { "bFollowBone", data.bFollowBone }
    };
}

void from_json(const json& j, EFFECT_EVENT_SCRIPT& data) {
    // JSON의 키값("iSimulationType")과 구조체 변수명 매칭 확인
    if (j.contains("fDuration")) j.at("fDuration").get_to(data.fDuration);
    if (j.contains("strEffectTag")) j.at("strEffectTag").get_to(data.strEffectTag);

    // JSON 파일에는 "iSimulationType"으로 되어 있으므로 이를 읽어와야 함
    if (j.contains("iSimulationType")) j.at("iSimulationType").get_to(data.iSimulationType);
    else if (j.contains("eSimulationType")) j.at("eSimulationType").get_to(data.iSimulationType);

    if (j.contains("strSocketName")) j.at("strSocketName").get_to(data.strSocketName);
    if (j.contains("bFollowBone")) j.at("bFollowBone").get_to(data.bFollowBone);

    if (j.contains("vOffset")) {
        const auto& jo = j.at("vOffset");
        if (jo.contains("x")) jo.at("x").get_to(data.vOffset.x);
        if (jo.contains("y")) jo.at("y").get_to(data.vOffset.y);
        if (jo.contains("z")) jo.at("z").get_to(data.vOffset.z);
    }
}

void to_json(json& j, const ANIM_EVENT_BASE& data) {
    j = json{ { "strAnimTag", data.strAnimTag }, { "iAnimIndex", data.iAnimIndex },
              { "fTrackPosition", data.fTrackPosition }, { "vecScript", data.vecScript } };
}

void from_json(const json& j, ANIM_EVENT_BASE& data) {
    if (j.contains("strAnimTag")) j.at("strAnimTag").get_to(data.strAnimTag);
    if (j.contains("iAnimIndex")) j.at("iAnimIndex").get_to(data.iAnimIndex);
    if (j.contains("fTrackPosition")) j.at("fTrackPosition").get_to(data.fTrackPosition);
    if (j.contains("vecScript")) j.at("vecScript").get_to(data.vecScript);
}

void to_json(json& j, const ANIM_EVENT_INFO_DESC& data) {
    json eventObj;
    // 배열 인덱스를 문자열 키로 변환하여 저장 (예: 4번 인덱스 -> "Vfx_Oneshot")
    eventObj["Vfx_Oneshot"] = data.vecAnimEvents[ENUM_TO_UINT(EAnimNotifyId::Vfx_Oneshot)];
    eventObj["Vfx_Attach_On"] = data.vecAnimEvents[ENUM_TO_UINT(EAnimNotifyId::Vfx_Attach_On)];
    eventObj["Vfx_Attach_Off"] = data.vecAnimEvents[ENUM_TO_UINT(EAnimNotifyId::Vfx_Attach_Off)];

    j = json{ { "strOwnerTag", data.strOwnerTag }, { "vecAnimEvents", eventObj } };
}

void from_json(const json& j, ANIM_EVENT_INFO_DESC& data) {
    if (j.contains("strOwnerTag")) j.at("strOwnerTag").get_to(data.strOwnerTag);

    if (j.contains("vecAnimEvents")) {
        const auto& jEvents = j.at("vecAnimEvents");
        // JSON 키 이름과 배열 인덱스 매핑
        if (jEvents.contains("Vfx_Oneshot"))
            jEvents.at("Vfx_Oneshot").get_to(data.vecAnimEvents[ENUM_TO_UINT(EAnimNotifyId::Vfx_Oneshot)]);
        if (jEvents.contains("Vfx_Attach_On"))
            jEvents.at("Vfx_Attach_On").get_to(data.vecAnimEvents[ENUM_TO_UINT(EAnimNotifyId::Vfx_Attach_On)]);
        if (jEvents.contains("Vfx_Attach_Off"))
            jEvents.at("Vfx_Attach_Off").get_to(data.vecAnimEvents[ENUM_TO_UINT(EAnimNotifyId::Vfx_Attach_Off)]);
    }
}
NS_END

NS_BEGIN(Engine)
json CDataStruct_EffectEvent::ToJson() const { return json(m_Data); }
HRESULT CDataStruct_EffectEvent::FromJson(const json& j) {
    m_Data = j.get<DTO::ANIM_EVENT_INFO_DESC>();
    return S_OK;
}
NS_END