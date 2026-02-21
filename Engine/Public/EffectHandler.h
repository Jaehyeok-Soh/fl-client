#pragma once
#include "Component.h"
#include "DataStruct_EffectEvent.h"

NS_BEGIN(Engine)

class CModel;

class ENGINE_DLL CEffectHandler final : public CComponent
{
public:

    enum class E_EFFECTTYPE
    {
        NONE,
        MODEL_ANIM,   
        SKILL_OBJ,    
        WORLD_STATIC, 
        TYPE_END
    };

    typedef struct tagAnimEffectHandlerDesc
    {
        string strOwnerTag;
        E_EFFECTTYPE eType = { E_EFFECTTYPE::NONE };
        unordered_map<_uint, vector<DTO::EFFECTEVENT>> mapEvents;
    } ANIM_EFFECT_HANDLER_DESC;

private:
    using Super = CComponent;

public:
    constexpr static EComponentType _ID = EComponentType::EF_ANIMHANDLER;

private:
    CEffectHandler();
    CEffectHandler(const CEffectHandler& rhs);
    virtual ~CEffectHandler() = default;

    virtual HRESULT Initialize_Prototype(void* pArg);
    virtual HRESULT Initialize(void* pArg) override;

public:
    void Awake();
    void Update(_float fDT);

public:
    void Ready_Event();
    void Release_Event();
    void CallBackEvent(const AnimNotifyKey& key);

public:
    // 툴 모듈에서 실시간으로 데이터를 교체하기 위한 Getter
    ANIM_EFFECT_HANDLER_DESC& Get_Desc() { return m_tDesc; }
    unordered_map<_uint, vector<DTO::EFFECTEVENT>>& GetEvents();

    void PoolObject_CallBack(CGameObject* pGo);
private:
    void GetAnimation();
    void Request_SpawnEffect(const DTO::EFFECTEVENT& script);
    void Request_SpawnEffect(const DTO::EFFECTEVENT& script, const std::string& UniqueEffectTag);

private:
    _uint m_iPrevAnimIndex = { 999999 };
    _float m_fPrevTrackPosition = { 0.f };

    ANIM_EFFECT_HANDLER_DESC m_tDesc;

    const Matrix* m_pOwnerMatrix = { nullptr };
    CModel* m_pOwnerModel = { nullptr };

private:
    DelegateHandle      m_EventHandle = {};
    std::unordered_map<string, CGameObject*> m_ActiveEffects;

public:
    static CEffectHandler* Create(void* pArg);
    virtual CComponent* Clone(void* pArg) override;
    virtual void Free() override;
};

NS_END