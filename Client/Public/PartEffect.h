#pragma once
#include "PartObject.h"
#include "EffectHandler.h"

/*

container 객체에서 바로 effect를 가지고 있고 호출 할 수 있도록
part effect로 만들어 둔다

container에 달린 effect
skill object는 투사체 effect용

단순히 spawn만 관리 한다.
despawn는..?

상속 받아서 써도 되고 냅다 쓰셔도 되고

근데 거의 깡통이라,,, desc을 늘리거나 상속을 받아서 쓰는 구조로 확장해야할듯!

*/

NS_BEGIN(Client)
class CPartEffect : public CPartObject
{
	using Super = CPartObject;

public:

	// 현재 구조 idle -> spawn -> despawn 고정.
	// 만약 바꾸고 싶다면 의논 필요
	enum class CPartEff_State { IDLE, SPAWN, DESPAWN, END }; // state로 추가적인 effect에 대한 상태를 내린다

	// 어떻게 생성하고 죽일 것인가에 대한 flag들 : 다른 로직에 방해만 되지 않는다면 자유롭게 추가 가능
	enum PartEff_Flag : Flags
	{
			None				= 0
		,	Spawn_CoolTime		= 0x0000001	// 쿨타임이 지나면 자동 spawn
	};

	typedef struct tagDataEffectHandler
	{
		CEffectHandler::E_OBJ_LIFECYCLE_STATE	eEffState		= { CEffectHandler::E_OBJ_LIFECYCLE_STATE::END };
		CEffectHandler::STATE_VFX_DESC			tSkillDesc		= {};
		CEffectHandler::E_HANDLER_TYPE			eHandlerType	= { CEffectHandler::E_HANDLER_TYPE::TYPE_END };
	}DATA_EFFHANDLER;

	typedef struct tagBodyDesc : public CPartObject::PARTOBJ_DESC
	{
		vector<DATA_EFFHANDLER> tEffectHandlerDesc;

		const Matrix*			pMatSocket{ nullptr }; // 부모 행렬 뿐만 아닌 소캣 행렬

		array<_float, ENUM_TO_SZET(CPartEff_State::END)> arrState_DurationTimes;
		array<_float, ENUM_TO_SZET(CPartEff_State::END)> arrState_DelayTimes;

		Flags FPartEff_Flags = { PartEff_Flag::None };

	}PART_EFFECT_DESC;

protected:
	CPartEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CPartEffect(const CPartEffect& rhs);
	virtual ~CPartEffect() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual HRESULT Awake(const _uint iCurrentLevelIndex) override;
	virtual void	Update_Priority(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Update_Late(_float fTimeDelta) override;
	virtual void	Ready_Before_Render(_float fTimeDelta) override;
	virtual void	OnCollision(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual void	OnCollision_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo) override;
	virtual void	OnCollision_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual void	OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual void	OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual _bool	On_Hit(const HIT_DESC& hitDesc) override;
	virtual HRESULT Render() override;

public:
	void Change_State(CPartEff_State eNextState);

	void Start_State(CPartEff_State eState);

	void Update_State(const _float fTimeDelta);
	void Idle_Update(const _float fTimeDelta);
	void Spawn_Update(const _float fTimeDelta);
	void Despawn_Update(const _float fTimeDelta);

	void End_State(CPartEff_State eState);

protected:
	CPartEff_State	m_eState = { CPartEff_State::END };
	const Matrix*	m_pMatSocket { nullptr }; // 소켓 매트릭스. 필요 없다면 사용 할 필요 없음

protected:
	Flags m_FEffFlags = {};
	
	/*
	 Duration	: part effect 상태 지속 시간 
	 Delay		: spawn effect상태 전환 지연 시간
	*/
	// Eff_Float_Data 이거를 통해 접근할것
	array<_float, ENUM_TO_SZET(CPartEff_State::END)> m_arrDurationTime_Origin		= { 0.f,0.f,0.f };
	array<_float, ENUM_TO_SZET(CPartEff_State::END)> m_arrDelayTime_Origin			= { 0.f,0.f,0.f };

	array<_float, ENUM_TO_SZET(CPartEff_State::END)> m_arrDuration_FloatsAcc		= { 0.f,0.f,0.f };
	array<_float, ENUM_TO_SZET(CPartEff_State::END)> m_arrDelay_FloatsAcc			= { 0.f,0.f,0.f };

	_bool m_bSpawnAlready = { false }; // spawn state가 시작된후 한번 spawn이 되어야 true도 변환

protected:
	HRESULT Ready_EffectHandler(PART_EFFECT_DESC* pDesc);

	// 상속 받아서 쓸때 만약 필요하다면 protected로 옮겨도 좋음
private:
	HRESULT			Spawn_Effect();
	HRESULT			Despawn_Effect();

public:
	static	CPartEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};
NS_END
