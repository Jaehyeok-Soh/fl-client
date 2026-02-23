#pragma once
#include "ContainerObject.h"

NS_BEGIN(Engine)



class ENGINE_DLL CEffectBase abstract:
    public CContainerObject
{
	using Super = CContainerObject;

public:
	enum class E_LoopState
	{
		LOOP_START,
		LOOP_END,
	};

protected:
	CEffectBase(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffectBase(const CEffectBase& rhs);
	virtual ~CEffectBase() = default;

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
	virtual void Set_Dead(const wstring& wstrLayerTag) override;

public:
	virtual void LoopStateChange(E_LoopState EState);

public:
	virtual void Free() override;
};

NS_END