#pragma once
#include "ObjectDataBase.h"

NS_BEGIN(DTO)

// [상속] 툴 담당자 공통 스크립트 베이스
typedef struct tagAnimEventScriptBase {
    _float fDuration = {};
    _float Get_Duration() { return fDuration; }
} ANIM_EVENT_SCRIPT_BASE;

// [확장] 정우님 전용 이펙트 이벤트 스크립트
typedef struct tagEffectEventScript : public ANIM_EVENT_SCRIPT_BASE {
    string              strEffectTag = {};      // "Attack_1" 등 컨테이너 이름
    _int                iSimulationType = { 1 }; // LOCAL(1), WORLD(0)
    string              strSocketName = {};     // 부착될 본/소켓 이름
    Vec3                vOffset = {};           // 오프셋
    _bool               bFollowBone = { true };  // 실시간 본 추적 여부
} EFFECT_EVENT_SCRIPT;

// 애니메이션 1개 프레임에 걸리는 이벤트 정보
typedef struct tagAnimEventBase {
    EAnimNotifyId               eNotifyId = EAnimNotifyId::Vfx_Oneshot; // 엔진 Enum 사용
    string                      strAnimTag = {};
    _uint                       iAnimIndex = {};
    _float                      fTrackPosition = {};
    vector<EFFECT_EVENT_SCRIPT> vecScript;
} ANIM_EVENT_BASE;

// 특정 캐릭터(Owner)의 전체 애니메이션 이벤트 묶음
typedef struct tagAnimEventInfoDesc {
    string                      strOwnerTag = { "Sample" };
    // [핵심] EAnimNotifyId::END 크기의 배열로 카테고리별 관리
    vector<ANIM_EVENT_BASE>     vecAnimEvents[ENUM_TO_UINT(EAnimNotifyId::END)];
} ANIM_EVENT_INFO_DESC;

// JSON 직렬화 함수 선언
void to_json(json& j, const EFFECT_EVENT_SCRIPT& data);
void from_json(const json& j, EFFECT_EVENT_SCRIPT& data);

void to_json(json& j, const ANIM_EVENT_BASE& data);
void from_json(const json& j, ANIM_EVENT_BASE& data);

void to_json(json& j, const ANIM_EVENT_INFO_DESC& data);
void from_json(const json& j, ANIM_EVENT_INFO_DESC& data);

NS_END

NS_BEGIN(Engine)

class ENGINE_DLL CDataStruct_EffectEvent final : public IObjectDataBase {
    using Super = IObjectDataBase;
private:
    CDataStruct_EffectEvent() = default;
    virtual ~CDataStruct_EffectEvent() = default;
public:
    // 특정 Enum ID가 아닌, Document 카테고리인 EFFECTEVENT를 리턴
    virtual _uint Get_Type() const override { return ENUM_TO_UINT(DTO::ECategory::EFFECTEVENT); }
    virtual const _string& Get_Tag() const override { return m_Data.strOwnerTag; }

    virtual json ToJson() const override;
    virtual HRESULT FromJson(const json& j) override;

    const DTO::ANIM_EVENT_INFO_DESC& Get_Data() const { return m_Data; }
    DTO::ANIM_EVENT_INFO_DESC& Get_Data() { return m_Data; }
private:
    DTO::ANIM_EVENT_INFO_DESC m_Data;
public:
    static CDataStruct_EffectEvent* Create() { return new CDataStruct_EffectEvent(); }
    virtual void Free() override { Super::Free(); }
};

NS_END