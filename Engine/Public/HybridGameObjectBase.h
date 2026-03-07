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

	END
};

class CEffectBase;

class ENGINE_DLL CHybridGameObjectBase abstract : public CGameObject
{
	using Super = CGameObject;
public:
	typedef struct tagHybridGameObjectDesc
	{
		std::vector<std::pair<_uint/*State*/, _uint /*HashTag*/>>			m_ModuleEffect = {};
	}HYBRID_DESC;
protected:
	// TODO - type 다 지정해줘야됨, 현재 툴때문이 임시로
	CHybridGameObjectBase(EHyBridEffectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CHybridGameObjectBase(const CHybridGameObjectBase& rhs);
	virtual ~CHybridGameObjectBase() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
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
	virtual HRESULT Ready_EffectModule(void* pArg) { return S_OK; }
	virtual HRESULT Ready_ColliderModule(void* pArg) { return S_OK; }

	/////////////
	/// State ///
	/////////////
	// 파생이 상태별 로직 결정
	virtual void Update_HybridState() PURE;

	// 상태 전환시 Start / End
	virtual void Start_HybridState(_uint iState) PURE;
	virtual void End_HybridState(_uint iState) PURE;

	// 모듈 전환 - 타입별 restart / reset을 처리
	virtual void On_ModuleEnter(EHybridModuleType eType, CGameObject* pModule) PURE;
	virtual void On_ModuleExit(EHybridModuleType eType, CGameObject* pModule) PURE;	
protected:
	void Change_HybridState(_uint iState);

	// 상태 바인딩 및 등록
	HRESULT Regist_Module(EHybridModuleType eType, CGameObject* pModule);
	HRESULT Bind_ModuleToState(_uint iState, EHybridModuleType eType, CGameObject* pModule);
	
	// 스테이트별 모듈 활성화 토글
	void Enable_StateModules(_uint iState);
	void Disable_StateModules(_uint iState);
	void Disable_AllModules();

	void Tick_StateModules_Priority(const _float fTimeDelta);
	void Tick_StateModules_Update(const _float fTimeDelta);
	void Tick_StateModules_Late(const _float fTimeDelta);
	void Tick_StateModules_BeforeRender(const _float fTimeDelta);
	HRESULT Tick_State_Modules_Render();

	// 모듈 공통 enable / disable
	void Enable_Module(EHybridModuleType eType, CGameObject* pModule);
	void Disable_Module(EHybridModuleType eType, CGameObject* pModule);
protected:
	EHyBridEffectType															m_eType = {};
	_uint																		m_iCurrentState = { 0 };
	_uint																		m_iPrevState = { 0 };
	
	// 상태별 활성 모듈 참조용
	map<_uint, vector<std::pair<EHybridModuleType,CGameObject*>>>				m_mapStateModules;

	// 소유권
	vector<CGameObject*> m_vecModules;
public:
	virtual void Free() override;
};

NS_END