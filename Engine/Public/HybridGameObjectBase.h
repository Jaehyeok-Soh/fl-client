#pragma once
#include "ColliderModule.h"

NS_BEGIN(Engine)

enum class EHybridModuleType
{
	NONE = -1,
	EFFECT = 0,
	COLLIDER = 1,

	END
};

class CEffectBase;

// 이 객체의 의의는 현재 ContainerObject는 PartObject를 상속받는 GameObject만 소속이 가능
// 때문에 Effect를 Prefab단위, ContainerObject로 선언되어 Prefab 단위로 컨트롤이 가능한 객체가 없음
// 위와 같은 이유로 해당 객체를 만들었으며 GameObject를 모두 소속시킬 수 있으며 소유권은 아래 객체가 소유함
class ENGINE_DLL CHybridGameObjectBase abstract : public CGameObject
{
	using Super = CGameObject;
protected:
	// TODO - type 다 지정해줘야됨, 현재 툴때문이 임시로
	CHybridGameObjectBase(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
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
private:	
	// 상태 바인딩 및 등록
	HRESULT Regist_Module(EHybridModuleType eType, CGameObject* pModule);
	HRESULT Bind_ModuleToState(_uint iState, EHybridModuleType eType, CGameObject* pModule);
protected:
	HRESULT Add_EffectModule(_uint iPrototypeLevelIndex, const string &strEffectName, const wstring &wstrEffectPrototypeTag, _uint iState);
	HRESULT Add_CollideModule(_uint iState, PHYSICSCOLLIDER_DESC* colliderDesc, PHYSICSRIGIDBODY_DESC* rigidbodyDesc);
	/////////////
	/// State ///
	/////////////
	// 파생클래스들이 State를 전환하며 State별 모듈을 제어함
	// ex). EState::Appear - 나타나는 이펙트
	//		EState::Active - 이펙트 Loop상태, Collide On
	//		EState::Disappear - Loop상태가 끝나면 진입, 사라지는 이펙트 On, Collide Off
	//						  - Disappear가 끝나면 Set_Dead()
	void Change_HybridState(_uint iState);

	// 파생이 상태별 로직 결정하여 직접 호출
	virtual void Update_HybridState(const _float fTimeDelta) PURE;

	// "상태" 전환시 - Start / End
	virtual void Start_HybridState(_uint iState) PURE;
	virtual void End_HybridState(_uint iState) PURE;

	//////////////
	/// Module ///
	//////////////
	
	// 초기화
	// "모듈" 전환시 - 타입별 restart / reset을 처리
	virtual void On_ModuleEnter(EHybridModuleType eType, CGameObject* pModule, void* pArg = nullptr) PURE;
	virtual void On_ModuleExit(EHybridModuleType eType, CGameObject* pModule) PURE;	

private:
	// 활성화 비활성화
	// 모듈 공통 enable / disable
	void Enable_Module(EHybridModuleType eType, CGameObject* pModule);
	void Disable_Module(EHybridModuleType eType, CGameObject* pModule);
protected:
	// 상태별 모듈 활성화 토글
	void Enable_StateModules(_uint iState);
	void Disable_StateModules(_uint iState);
	void Disable_AllModules();

	// 현재 State 기준 Tick 함수들
	void Tick_StateModules_Priority(const _float fTimeDelta);
	void Tick_StateModules_Update(const _float fTimeDelta);
	void Tick_StateModules_Late(const _float fTimeDelta);
	void Tick_StateModules_BeforeRender(const _float fTimeDelta);
	HRESULT Tick_State_Modules_Render();
protected:
	_uint																		m_iCurrentState = { 0 };
	_uint																		m_iPrevState = { 0 };
	
	// 상태별 활성 모듈 참조용
	map<_uint, vector<std::pair<EHybridModuleType,CGameObject*>>>				m_mapStateModules;

	// 소유권
	vector<CGameObject*> m_vecModules;
public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual void Free() override;
};

NS_END