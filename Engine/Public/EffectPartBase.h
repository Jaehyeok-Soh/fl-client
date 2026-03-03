#pragma once
#include "PartObject.h"
#include "DataStruct_EffectEvent.h"

NS_BEGIN(Engine)

class ENGINE_DLL CEffectPartBase abstract:
public CPartObject
{
     using Super = CPartObject;
protected:
    CEffectPartBase(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
    explicit CEffectPartBase(const CEffectPartBase& rhs);
    virtual ~CEffectPartBase() = default;

    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;

public:
    virtual HRESULT Awake(const _uint iCurrentLevelID) override;
    virtual void Update_Priority(const _float fDT) override;
    virtual void Update(const _float fTimeDelta) override;
    virtual void Update_Late(const _float fTimeDelta) override;
    virtual void Ready_Before_Render(const _float fTimeDelta) override;
    virtual void Set_Dead(const wstring& wstrLayerTag) override;
public:
    virtual void LoopState_Change(DTO::E_LoopState eState) = 0;

public:
    virtual void Free() override;
};

NS_END