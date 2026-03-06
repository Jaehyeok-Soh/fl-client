#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)

enum class ESkillObjectFlag : _uint
{
	Move_Straight = 0x0001,
	Move_Homing = 0x0002,
	Move_Return = 0x0004,

	Life_Timer = 0x0010,
	Life_Distance = 0x0020,

	Hit_Single = 0x0100,
	Hit_Multi = 0x0200,

	Render_Off = 0x1000,
	Follow_Owner = 0x2000,
};

typedef struct tagSkillObjectRuntimeDesc
{
	TIME_LINE Life{};
	_float fTravelDistance{ 0.f };

	Vec3 vStartPos{};
	Vec3 vCurDir{};
	Vec3 vFollowOffset{};

	_bool bHitOnce{ false };
}SKILLOBJECT_RUNTIMEDESC;

class ENGINE_DLL CSkillObject_Base abstract : public CGameObject
{
	using Super = CGameObject;
public:
	typedef struct tagSkillObjectDesc : public GAMEOBJECT_DESC
	{
		CGameObject* pRequester = { nullptr };
		CGameObject* pTarget = { nullptr };

		_uint			iFlags = { 0 }; // 행동 플래그 ESkillObjectFlag 참고

		// 이펙트를 회전시킨뒤 날리고싶다면?
		TRANSFORM_INFO_STATE eEffectRotateState = { TRANSFORM_INFO_STATE::END };
		_float fEffectDegree{ 0.00f };

		_float			fSpeed = { 0.f }; // 투사체 속도
		_float          fLifeTime = { 5.f }; // 생명 정도
		_float          fMaxDistance = { 15.f }; // 얼마나 날아가서 죽을래?
		_float          fHomingStrength = { 5.f }; // 호밍 정도
		Vec3            vSpawnPos = {}; // 스폰 위치
		Vec3            vDirection = {}; // 날아가는 방향
	}SKILLOBJECT_DESC;
protected:
	CSkillObject_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CSkillObject_Base(const CSkillObject_Base& rhs);
	virtual ~CSkillObject_Base() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Awake(const _uint iCurrentLevelID) override;
	virtual void Update_Priority(const _float fTimeDelta) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual void Update_Late(const _float fTimeDelta) override;
	virtual void Ready_Before_Render(const _float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual void OnCollision(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override {}
	virtual void OnCollision_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo) override {}
	virtual void OnCollision_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override {}
	virtual void OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo) override;

	virtual _bool On_Hit(const HIT_DESC& hitDesc) override { return false; }
	virtual void Try_Attack(const HIT_DESC& hitDesc) override {}

	virtual HRESULT Spawn_FromPool(void* pArg) override;
	virtual HRESULT Despawn_FromPool() override;

	_uint Get_Flags() const { return m_iFlag; }
protected:
	_bool Has_Flag(ESkillObjectFlag eFlag) const;
	_bool Has_Requester() const { return m_desc.pRequester != nullptr; }
	_bool Has_Target() const { return m_desc.pTarget != nullptr; }
private:
	void Process_Move(const _float fTimeDelta);
	void Process_Life(const _float fTimeDelta);
	Vec3 Compute_MoveDir(const _float fTimeDelta, const Vec3& vCurDir);
protected:
	SKILLOBJECT_DESC m_desc{};
	SKILLOBJECT_RUNTIMEDESC m_runtimeDesc{};
	_uint m_iFlag{ 0 };
public:
	virtual CGameObject* Clone(void *pArg) PURE;
	virtual void Free() override;
};

NS_END
