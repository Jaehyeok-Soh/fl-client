#pragma once
#include "ObjectDataBase.h"
#include "Anim_Event_Script_Base.h"
#include "Anim_Event_Base.h"

NS_BEGIN(DTO)

enum E_BONE_FLAG
{
    BONE_NONE = 0,
    BONE_SCALE = 1 << 0,
    BONE_ROTATAION= 1 << 1,
    BONE_POS = 1 << 2
};

enum class E_LoopState
{
    LOOP_START,
    LOOP_END,
};

// 이펙트 이벤트 스크립트
typedef struct tagEffectEvent : public ANIM_EVENT_BASE1 {
    _uint       iNotifyId = { 0 };

    string      strEffectTag = {};
    _int        iSimulationType = { 1 };
    Vec3        vOffset = {};
    Vec3        vRotation = {};

    _bool       bFollowBone = { true };
    _int        iBoneIndex = { -1 };
    _uint       iBoneFlag = { 0 };  // Bone Flag는 툴에서 설정한 비트플래그 값. POSITION, SCALE, POSITION 

    _bool       bUseChildBone = { false };
    _int        ChildPartNumber = {-1};
    _int        iChildBoneIndex = { -1 };

    _float      fDuration = {};
}EFFECTEVENT;

// 특정 캐릭터(Owner)의 전체 애니메이션 이벤트 묶음
typedef struct tagEffectEventInfoDesc {
    string              strOwnerTag = { "Sample" };
    _int                iNumPool = { 32 };
    vector<EFFECTEVENT> vecEffectEvents; // 동기처럼 벡터로 관리
} EFFECT_EVENT_INFO_DESC;

// JSON 직렬화 함수 선언
void to_json(json& j, const EFFECTEVENT& data);
void from_json(const json& j, EFFECTEVENT& data);
void to_json(json& j, const EFFECT_EVENT_INFO_DESC& data);
void from_json(const json& j, EFFECT_EVENT_INFO_DESC& data);

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

    const DTO::EFFECT_EVENT_INFO_DESC& Get_Data() const { return m_Data; }
    DTO::EFFECT_EVENT_INFO_DESC& Get_Data() { return m_Data; }
private:
    DTO::EFFECT_EVENT_INFO_DESC m_Data;

public:
    static CDataStruct_EffectEvent* Create() { return new CDataStruct_EffectEvent(); }
    virtual void Free() override { Super::Free(); }
};

NS_END