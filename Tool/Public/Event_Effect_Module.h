#pragma once
#include "Base.h"
#include "Animation_Defines.h"
#include "Anim_Event_Info.h"
#include "DataStruct_EffectEvent.h"
#include "EffectHandler.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Tool)

class CEvent_Effect_Module final : public CBase
{
public:
    using Super = CBase;

private:
    CEvent_Effect_Module(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
    virtual ~CEvent_Effect_Module() = default;

    HRESULT Initialize();

public:
    void Update(const _float fTimeDelta);
    void Render();

    // 이펙트 핸들러 세팅
    void SetEFfectEvent(CEffectHandler* pEffectHandler, CAnimObj* pOwner);
    void SetOwner(CAnimObj* pOwner);

    unordered_map<string, vector<DTO::EFFECTEVENT>>& GetEvents();
    CEffectHandler* GetHandler() { return m_pEffectEvent; }

    void SetHandler(CEffectHandler* pHandler) { m_pEffectEvent = pHandler; }

    // 툴에서 수정된 이펙트 데이터를 컴포넌트에 동기화
    void Modify_EFfectEvent(vector<DTO::EFFECTEVENT> events);
    void Awake();

private:
    ID3D11Device* m_pDevice{};
    ID3D11DeviceContext* m_pDeviceContext{};
    CGameInstance* m_pGameInstance{ nullptr };

private:
    // 툴 매니저가 관리하는 이펙트 핸들러 컴포넌트
    CEffectHandler* m_pEffectEvent = { nullptr };

public:
    static CEvent_Effect_Module* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
    virtual void Free() override;
};

NS_END