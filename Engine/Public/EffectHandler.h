#pragma once
#include "Component.h"
#include "DataStruct_EffectEvent.h"
#include "MulticastDelegate.h"

NS_BEGIN(Engine)

class CModel;

class ENGINE_DLL CEffectHandler final : public CComponent
{
public:
    enum class E_HANDLER_TYPE
    {
        MODEL_ANIM,   
        SKILL_OBJ,    
        WORLD_STATIC, 
        TYPE_END
    };

    enum class E_OBJ_LIFECYCLE_STATE
    {
        ON_SPAWN,
        ON_CONTINUE,
        ON_DESTROY,
        END
    };

    typedef struct tagStateEffectDesc
    {
        string          EffectPrefabTag = {};     // 생성할 이펙트 Prefab Tag
        _bool           bLocal = {};              // 부모에 붙어서 갈것인가
        const Matrix*   pMatParent = { nullptr }; // 부모 Transform

    }STATE_VFX_DESC;

    typedef struct tagAnimEffectHandlerDesc
    {
        string strOwnerTag;
        E_HANDLER_TYPE eType = { E_HANDLER_TYPE::MODEL_ANIM };
        unordered_map<E_OBJ_LIFECYCLE_STATE, STATE_VFX_DESC> mEffectState;
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
    virtual HRESULT Ready_AnimState();
    virtual void    Ready_State();
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

public:
    // 오브젝트 상태가 변경되서 Effect를 바꿔주어야할때.
    void Trigger_Lifecycle_Effect(E_OBJ_LIFECYCLE_STATE eStage);
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
    // 현재 State에 맞는 이펙트를 하고 이전 State 이펙트의 Loop를 false로 바꾸어준다.
    E_OBJ_LIFECYCLE_STATE       m_eCurrentState = {};
    E_OBJ_LIFECYCLE_STATE       m_ePrevState = {};

    // TODO: State가 변동되었을 때 이펙트 State Loop를 False로 바꾸어줘야하는데 Engine쪽에 Effect가 없기때문에 Delegate로 쏜다.
    // Client쪽에서 파싱 받음.

private:
    DelegateHandle      m_EventHandle = {};
    std::unordered_map<string, CGameObject*> m_ActiveEffects[ENUM_TO_UINT(E_HANDLER_TYPE::TYPE_END)];

public:
    CMulticastDelegate<void(CGameObject*)> OnNotify;

public:
    static CEffectHandler* Create(void* pArg);
    virtual CComponent* Clone(void* pArg) override;
    virtual void Free() override;
};

NS_END