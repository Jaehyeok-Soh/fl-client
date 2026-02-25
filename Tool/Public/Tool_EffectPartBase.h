#pragma once
#include "Tool_PartObject.h"

NS_BEGIN(Tool)

class CTool_EffectPartBase :
    public Tool_PartObject
{
    using Super = Tool_PartObject;

protected:
    CTool_EffectPartBase(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
    explicit CTool_EffectPartBase(const CTool_EffectPartBase& rhs);
    virtual ~CTool_EffectPartBase() = default;

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
    virtual void Free() override;

};

NS_END