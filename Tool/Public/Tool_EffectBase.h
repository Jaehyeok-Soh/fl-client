#pragma once
#include "Tool_ContainerObject.h"

NS_BEGIN(Tool)

class CTool_EffectBase :
    public Tool_ContainerObject
{
	using Super = Tool_ContainerObject;

protected:
	CTool_EffectBase(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CTool_EffectBase(const CTool_EffectBase& rhs);
	virtual ~CTool_EffectBase() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual HRESULT Awake(const _uint iCurrentLevelID) override;
	virtual void Update_Priority(const _float fTimeDelta) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual void Update_Late(const _float fTimeDelta) override;
	virtual void Ready_Before_Render(const _float fTimeDelta) override;
	virtual HRESULT Render() override;
public:
	virtual void Free() override;
};

NS_END