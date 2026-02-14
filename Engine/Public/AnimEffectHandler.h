#pragma once
#include "Component.h"
#include "DataStruct_EffectEvent.h"

NS_BEGIN(Engine)

class CModel;

class CAnimEffectHandler :
    public CComponent
{
public:
	typedef struct tagAnimEffectHandlerDesc
	{
		string strOwnerTag; // PlayerMoon 같은 주체 이름을 넣는다

		// Builder 쪽에서 가공해서 넘겨줄 애니메이션 인덱스 기반의 이벤트 맵.
		// Key: 애니메이션 인덱스, Value : 해당 애니메이션 이벤트 리스트.
		unordered_map<_uint, vector<DTO::ANIM_EVENT_BASE>> mapEvents;
	}ANIM_EFFECT_HANDLER_DESC;

private:
	using Super = CComponent;

public:
	constexpr static EComponentType _ID = EComponentType::EF_ANIMHANDLER;

private:
	CAnimEffectHandler();
	CAnimEffectHandler(const CAnimEffectHandler& rhs);
	virtual ~CAnimEffectHandler() = default;

	virtual HRESULT Initialize_Prototype(void* pArg);
	virtual HRESULT Initialize(void* pArg) override;

public:
	void Awake();
	void Update(_float fDT);

private:
	void GetAnimation();
	void CheckAnim();
	void Request_SpawnEffect(const DTO::EFFECT_EVENT_SCRIPT& script);

private:
	_uint m_iPrevAnimIndex = { 999999 };
	_float m_fPrevTrackPosition = { 0.f };

	// 빌더로부터 전달받은 이벤트 데이터 사전
	ANIM_EFFECT_HANDLER_DESC m_tDesc;

	// 빠른 접근을 위한 캐싱
	const Matrix* m_pOwnerMatrix = { nullptr };
	CModel* m_pOwnerModel = { nullptr };

public:
	static CAnimEffectHandler* Create(void* pArg);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END