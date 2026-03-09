#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)

// 복합적인 연출을 위해서는 하나 단위로 묶어서 관리해야함.
enum class EHyBridEffectType
{
	NONE = -1,
	WARNINGSPACE,

	END,
};

enum class EHybridModuleType
{
	NONE = -1,
	EFFECT = 0,
	COLLIDER = 1,
};

class CEffectBase;

class ENGINE_DLL CHybridGameObject_Base abstract :
public CGameObject
{
	using Super = CGameObject;
public:
	typedef struct tagOriginHybridDesc
	{
		std::vector<std::pair<_uint/*State*/, _uint /*HashTag*/>>			m_ModuleEffect = {};

	}Origin_HybridDesc;
protected:
	// TODO - type 다 지정해줘야됨, 현재 툴때문이 임시로
	CHybridGameObject_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, EHyBridEffectType eType);
	explicit CHybridGameObject_Base(const CHybridGameObject_Base& rhs);
	virtual ~CHybridGameObject_Base() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

protected:
	virtual HRESULT Ready_EffectModule(void* pArg);
	//virtual HRESULT Ready_ColliderModule(void* pArg);

public:
	  virtual HRESULT Awake(const _uint iCurrentLevelID) override;
	  virtual void Update_Priority(const _float fTimeDelta) override;
	  virtual void Update(const _float fTimeDelta) override;
	  virtual void Update_Late(const _float fTimeDelta) override;
	  virtual void Ready_Before_Render(const _float fTimeDelta) override;
	  virtual HRESULT Render()  override;
	  inline virtual HRESULT Spawn_FromPool(void* pArg)  override;
	  inline virtual HRESULT Despawn_FromPool()  override;

protected:
	virtual void Change_HybridState(_uint iState);
	virtual void Update_CheckState() PURE;

protected:
	// 각 State 별로 활성화 될 객체 리스트를 둬야할 듯?
	// ex) Wait // Warning // Exlosion 
	EHyBridEffectType															eHybridType = {};
	_uint																		m_iCurrentState = { 0 };
	_uint																		m_iPrevState = { 0 };
	std::map<_uint, std::vector<std::pair<EHybridModuleType,CGameObject*>>>		m_StateModules = {};

public:
	virtual void Free() override;

};

NS_END