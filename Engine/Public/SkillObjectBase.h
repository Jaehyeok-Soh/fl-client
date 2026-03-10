#pragma once
#include "HybridGameObjectBase.h"

NS_BEGIN(Engine)

enum class ESkillObjectFlag : _uint
{
	Move_Straight = 0x0001,
	Move_Homing = 0x0002,
	Move_Return = 0x0004,

	Life_Timer = 0x0010,
	Life_Distance = 0x0020,

	Render_Off = 0x0100,
	Follow_Owner = 0x0200,
};

typedef struct tagSkillObjectRuntimeDesc
{
	TIME_LINE Life{};
	_float fTravelDistance{ 0.f };

	Vec3 vStartPos{};
	Vec3 vCurDir{};
	Vec3 vFollowOffset{};
}SKILLOBJECT_RUNTIMEDESC;

class ENGINE_DLL CSkillObjectBase abstract : public CHybridGameObjectBase
{
	using Super = CHybridGameObjectBase;
public:
	typedef struct tagSkillObjectSpawnDesc : public GAMEOBJECT_DESC
	{
		// Safe_AddRef X
		CGameObject* pRequester = { nullptr };
		CGameObject* pTarget = { nullptr };

		// 행동 플래그 ESkillObjectFlag 참고
		_uint			iFlags = { 0 };

		// 이펙트를 회전시킨뒤 날리고싶다면?
		TRANSFORM_INFO_STATE eEffectRotateState = { TRANSFORM_INFO_STATE::END };
		_float fEffectDegree = { 0.00f };

		_float			fSpeed = { 0.f }; // 투사체 속도
		_float          fLifeTime = { 5.f }; // 생명 정도
		_float          fMaxDistance = { 15.f }; // 얼마나 날아가서 죽을래?
		_float          fHomingStrength = { 5.f }; // 호밍 정도

		Vec3            vSpawnPos = {}; // 스폰 위치
		Vec3            vDirection = {}; // 날아가는 방향
		Vec3			vScale = {};
	}SKILLOBJECT_SPAWN_DESC;
protected:
	CSkillObjectBase(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CSkillObjectBase(const CSkillObjectBase& rhs);
	virtual ~CSkillObjectBase() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Spawn_FromPool(void* pArg) final override;
	virtual HRESULT Despawn_FromPool() final override;
protected:
	// Hybrid PURE들
	virtual void Update_HybridState(const _float fTimeDelta) PURE;
	// 파생을 위해 래핑
	virtual void Start_HybridState(_uint iState) final override { On_StateEnter(iState); }
	virtual void End_HybridState(_uint iState) final override { On_StateExit(iState); }
	virtual void On_ModuleEnter(EHybridModuleType eType, CGameObject* pModule) final override;
	virtual void On_ModuleExit(EHybridModuleType eType, CGameObject* pModule) final override;
	
	/////////////////////////////////
	/// 파생이 상속받아 쓰면 되는것들 ///
	/////////////////////////////////
	// HybirdState라는 이름 헷갈리는것 떄문에 간소화를 위해 이렇게 함
	
	// Spawn_FromPool / Despawn_FromPool 내부에서 호출
	virtual void On_Spawned(const SKILLOBJECT_SPAWN_DESC&) {}
	virtual void On_Despawned() {}
	// State의 Enter / Exit
	virtual void On_StateEnter(_uint iState) {}
	virtual void On_StateExit(_uint iState) {}
	// Effect 모듈의 Enter / Exit
	// Effect의경우 Desc를 Enable할때마다 전달해야 하므로 해당 초기화값을 이펙트 투사체마다 정의
	virtual void On_EffectModuleEnter(CGameObject* pModule);
	virtual void On_EffectModuleExit(CGameObject* pModule);
	// Collider 모듈의 Enter / Exit
	virtual void On_ColliderModuleEnter(CGameObject* pModule);
	virtual void On_ColliderModuleExit(CGameObject* pModule);
	// Life가 다됬을때 호출 ( ChangeState 혹은 Set_Dead )
	virtual void On_LifeOver() {};
protected:
	// Helper들
	// 특정 State의 EffectModule 일괄 Loop 끄기
	void LoopEnd_EffectModules(_uint iState);
	// void Apply_MoveStep(const _float fTimeDelta, const Vec3& vDir);
	// Compute_MoveDir의 결과로 
	void Process_Move(const _float fTimeDelta);
	// 수명 태그를 사용한다면 계산
	void Process_Life(const _float fTimeDelta);
	// 플래그에 의한 MoveDir 계산
	Vec3 Compute_MoveDir(const _float fTimeDelta, const Vec3& vCurDir);
	_bool Has_Flag(ESkillObjectFlag eFlag) const { return Engine_Utils::Has_Flag(m_iFlag, ENUM_TO_UINT(eFlag)); }
	_bool Is_RequesterAlive() const { return (m_tDesc.pRequester != nullptr) && (m_tDesc.pRequester->IsDead() == false); }
	_bool Is_TargetAlive() const { return (m_tDesc.pTarget != nullptr) && (m_tDesc.pTarget->IsDead() == false); }
	_bool Is_LifeOver() const;
	// 하나라도 Active거나 끝나지않으면
	_bool Is_StateEffectFinished(_uint iState) const;
private:
	void Sync_ModuleWorldToOwner(CGameObject* pModel);
	void Reset_RuntimeDesc();
	// Desc에 저장된 회전량으로 이펙트를 회전시켜 스폰하기 위함
	void Compute_Rotate(CGameObject* pEffectModule);
public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual void Free() override;
protected:
	_uint m_iFlag{ 0 };
	SKILLOBJECT_SPAWN_DESC m_tDesc{};
	SKILLOBJECT_RUNTIMEDESC m_tRuntimeDesc{};
	EFFECT_SPAWN_DESC m_tDefaultEffectDesc{};
	const Matrix* m_pMyMatrix{ nullptr };
};

NS_END