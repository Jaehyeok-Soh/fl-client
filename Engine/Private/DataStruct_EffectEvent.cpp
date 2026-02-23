#include "Engine_pch.h"
#include "DataStruct_EffectEvent.h"

#pragma push_macro("new")
#undef new
#include "json.hpp"
using json = nlohmann::json;
#pragma pop_macro("new")

NS_BEGIN(DTO)

// 단일 이펙트 이벤트 직렬화 (EFFECTEVENT)
void to_json(json& j, const EFFECTEVENT& data) {
    // 부모 클래스(ANIM_EVENT_BASE1)의 기본 정보 먼저 직렬화
    to_json(j, static_cast<const ANIM_EVENT_BASE1&>(data));

    // 이펙트 전용 데이터 추가
    j["iNotifyId"] = data.iNotifyId;
    j["strEffectTag"] = data.strEffectTag;
    j["iSimulationType"] = data.iSimulationType;
    j["iBoneIndex"] = data.iBoneIndex;
    j["iBoneFlag"] = data.iBoneFlag;
    j["vOffset"] = { {"x", data.vOffset.x}, {"y", data.vOffset.y}, {"z", data.vOffset.z} };
    j["vRotation"] = { {"x", data.vRotation.x}, {"y", data.vRotation.y}, {"z", data.vRotation.z} };
    j["bFollowBone"] = data.bFollowBone;
    j["fDuration"] = data.fDuration;
}

void from_json(const json& j, EFFECTEVENT& data) {
    // 부모 클래스 정보 로드
    from_json(j, static_cast<ANIM_EVENT_BASE1&>(data));

    // 이펙트 전용 데이터 로드 (안전하게 contains 체크)
    if (j.contains("iNotifyId"))       j.at("iNotifyId").get_to(data.iNotifyId);
    if (j.contains("strEffectTag"))    j.at("strEffectTag").get_to(data.strEffectTag);
    if (j.contains("iSimulationType")) j.at("iSimulationType").get_to(data.iSimulationType);
    if (j.contains("iBoneIndex"))      j.at("iBoneIndex").get_to(data.iBoneIndex);
    if (j.contains("iBoneFlag"))       j.at("iBoneFlag").get_to(data.iBoneFlag);
    if (j.contains("bFollowBone"))     j.at("bFollowBone").get_to(data.bFollowBone);
    if (j.contains("fDuration"))       j.at("fDuration").get_to(data.fDuration);

    if (j.contains("vOffset")) {
        const auto& jo = j.at("vOffset");
        if (jo.contains("x")) jo.at("x").get_to(data.vOffset.x);
        if (jo.contains("y")) jo.at("y").get_to(data.vOffset.y);
        if (jo.contains("z")) jo.at("z").get_to(data.vOffset.z);
    }

    if (j.contains("vRotation"))
    {
        const auto& jo = j.at("vRotation");
        if (jo.contains("x")) jo.at("x").get_to(data.vRotation.x);
        if (jo.contains("y")) jo.at("y").get_to(data.vRotation.y);
        if (jo.contains("z")) jo.at("z").get_to(data.vRotation.z); 
    }
}

// 전체 이펙트 정보 묶음 직렬화 (EFFECT_EVENT_INFO_DESC)
void to_json(json& j, const EFFECT_EVENT_INFO_DESC& data) {
    j = json{
        { "strOwnerTag",      data.strOwnerTag },
        { "iNumPool",         data.iNumPool },
        { "vecEffectEvents",  data.vecEffectEvents } // 벡터 직렬화
    };
}

void from_json(const json& j, EFFECT_EVENT_INFO_DESC& data) {
    if (j.contains("strOwnerTag"))     j.at("strOwnerTag").get_to(data.strOwnerTag);
    if (j.contains("iNumPool"))        j.at("iNumPool").get_to(data.iNumPool);
    if (j.contains("vecEffectEvents")) j.at("vecEffectEvents").get_to(data.vecEffectEvents);
}

NS_END

NS_BEGIN(Engine)

// 엔진 데이터 구조체 구현부
json CDataStruct_EffectEvent::ToJson() const {
    json j = json(m_Data);
    j["Type"] = Get_Type(); // Document에서 읽을 때 카테고리 식별용
    return j;
}

HRESULT CDataStruct_EffectEvent::FromJson(const json& j) {
    try {
        m_Data = j.get<DTO::EFFECT_EVENT_INFO_DESC>();
    }
    catch (const std::exception& e) {
        // 로드 실패 시 디버깅을 위해 에러 로그 출력 권장
        return E_FAIL;
    }
    return S_OK;
}

NS_END