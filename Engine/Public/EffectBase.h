#pragma once
#include "ContainerObject.h"
#include "DataStruct_EffectEvent.h"
#include "DataStruct_Effect.h"

NS_BEGIN(Engine)

class ENGINE_DLL CEffectBase abstract:
    public CContainerObject
{
	using Super = CContainerObject;
public:

	typedef struct tagEffectContainerDesc : public Super::GAMEOBJECT_DESC
	{
		DTO::E_SIMULATION_SPACE _Effect_SimulationType = DTO::E_SIMULATION_SPACE::LOCAL;
		std::vector<DTO::TEFFECT_PartsData>	_childData = {};
		_bool	_IsPoolingEffect = true;
	}EFFECT_CONTAINERDESC;

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
public:
	virtual void LoopStateChange(DTO::E_LoopState EState);
	_bool IsEffectFinished() const { return m_bIsEffectFinish; }

public:
	virtual HRESULT Enable_VFX(void* pArg) PURE;
	virtual HRESULT Disable_VFX() PURE;

public:
	virtual void Free() override;

protected:
	_bool							m_bIsEffectFinish = false;
};

NS_END