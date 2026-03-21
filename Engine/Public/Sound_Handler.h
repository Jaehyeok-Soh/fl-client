#pragma once
#include "Component.h"
#include "DataStruct_SoundEvent.h"

NS_BEGIN(Engine)

class CModel;

class ENGINE_DLL CSound_Handler final : public CComponent
{
    using Super = CComponent;
public:
    constexpr static EComponentType _ID = EComponentType::SOUNDHANDLER;
    typedef struct tagSoundHandlerDesc
    {
        string strOwnerTag;
        unordered_map<string, vector<DTO::SOUNDEVENT>> mapEvents;
    }SOUNDHANDLER_DESC;
private:
    CSound_Handler();
    CSound_Handler(const CSound_Handler& rhs);
    virtual ~CSound_Handler() = default;

    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;

public:
    void Bind_Events();
    void Ready_Event();
    void Release_Event();
    void Update(_float fTimeDelta);
    // 툴에서 데이터 교체
public:
    void Modify_AttackOverlap(_uint eventIdx, DTO::SOUNDEVENT event);
    void Modify_AttackOverlap(vector<DTO::SOUNDEVENT> events);
private:
    void CallbackEvent(const AnimNotifyKey& key);
    void Process_SoundEvent(const DTO::SOUNDEVENT& tEvent);
private:
    SOUNDHANDLER_DESC m_tDesc{};
    CModel* m_pOwnerModel = { nullptr };
    DelegateHandle      m_EventHandle = {};
public:
    static CSound_Handler* Create();
    virtual CComponent* Clone(void* pArg) override;
    virtual void Free() override;
};

NS_END