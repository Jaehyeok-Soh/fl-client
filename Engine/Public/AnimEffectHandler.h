#pragma once
#include "Component.h"
#include "DataStruct_EffectEvent.h"

NS_BEGIN(Engine)

class CModel;

class ENGINE_DLL CAnimEffectHandler final : public CComponent
{
public:
    typedef struct tagAnimEffectHandlerDesc
    {
        string strOwnerTag;
        unordered_map<_uint, vector<DTO::EFFECTEVENT>> mapEvents;
    } ANIM_EFFECT_HANDLER_DESC;

private:
    using Super = CComponent;

public:
    constexpr static EComponentType _ID = EComponentType::EF_ANIMHANDLER;

private:
    CAnimEffectHandler();
    CAnimEffectHandler(const CAnimEffectHandler& rhs);
    virtual ~CAnimEffectHandler() = default;

    virtual HRESULT Initialize_Prototype(void* pArg);
    virtual HRESULT Initialize(void* pArg) override;

public:
    void Awake();
    void Update(_float fDT);

public:
    // 툴 모듈에서 실시간으로 데이터를 교체하기 위한 Getter
    ANIM_EFFECT_HANDLER_DESC& Get_Desc() { return m_tDesc; }
    unordered_map<_uint, vector<DTO::EFFECTEVENT>>& GetEvents() { return m_tDesc.mapEvents; }

private:
    void GetAnimation();
    void CheckAnim();
    void Request_SpawnEffect(const DTO::EFFECTEVENT& script);

private:
    _uint m_iPrevAnimIndex = { 999999 };
    _float m_fPrevTrackPosition = { 0.f };

    ANIM_EFFECT_HANDLER_DESC m_tDesc;

    const Matrix* m_pOwnerMatrix = { nullptr };
    CModel* m_pOwnerModel = { nullptr };

public:
    static CAnimEffectHandler* Create(void* pArg);
    virtual CComponent* Clone(void* pArg) override;
    virtual void Free() override;
};

NS_END