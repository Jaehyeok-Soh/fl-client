#pragma once
#include "DataDocumentBase.h"
#include "DataStruct_EffectEvent.h"

NS_BEGIN(Engine)

class ENGINE_DLL CDataDocument_EffectEvent final : public CDataDocumentBase
{
    using Super = CDataDocumentBase;
public:
    CDataDocument_EffectEvent();
    virtual ~CDataDocument_EffectEvent() = default;

    HRESULT Initialize();

public:
    // 정우님이 지정하신 EFFECTEVENT 카테고리
    virtual DTO::ECategory Get_Category() const override { return DTO::ECategory::EFFECTEVENT; }

    virtual json ToJson() const override;
    virtual HRESULT FromJson(const json& j) override;

    // 특정 캐릭터의 전체 이펙트 이벤트 데이터를 추가
    HRESULT Try_Add(const DTO::ANIM_EVENT_INFO_DESC& data);

private:
    // 데이터 구조체 생성 (DataStruct_EffectEvent 활용)
    virtual IObjectDataBase* Create_ObjectData(DTO::ECategory eType);

    // Repository에 실제 등록하는 내부 함수
    HRESULT Try_Add(IObjectDataBase* pObject);

public:
    static CDataDocument_EffectEvent* Create();
    virtual void Free() override;
};

NS_END