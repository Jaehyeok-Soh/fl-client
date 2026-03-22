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
        vector<DTO::SOUNDEVENT> vecSoundEvents;
    } SOUND_HANDLER_DESC;
private:
    constexpr static _uint INVALID_CONTROLLED_ID = static_cast<_uint>(-1);

private:
    CSound_Handler();
    CSound_Handler(const CSound_Handler& rhs);
    virtual ~CSound_Handler() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg) override;

public:
    void Set_Desc(const SOUND_HANDLER_DESC& desc);
    const SOUND_HANDLER_DESC& Get_Desc() const { return m_tDesc; }

    void Setup_ForOwner(CModel* pModel);
    void Release_Event();

private:
    HRESULT Ready_Desc(void* pArg);
    HRESULT Ready_SoundState();
    void Clear_SoundNotifies();

    AnimNotifyKey Build_SoundNotifyKey(const DTO::SOUNDEVENT& event) const;
    void CallbackEvent(const AnimNotifyKey& key);

private:
    SOUND_HANDLER_DESC m_tDesc{};

    CModel* m_pOwnerModel = { nullptr };
    DelegateHandle m_EventHandle{};

    _uint m_iSoundLevelID = 0;

public:
    static CSound_Handler* Create();
    virtual CComponent* Clone(void* pArg) override;
    virtual void Free() override;
};

NS_END