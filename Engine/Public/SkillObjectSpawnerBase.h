#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CSkillObjectSpawnerBase abstract : public CGameObject
{
	using Super = CGameObject;
public:
	typedef struct tagSpawnerOriginDesc
	{
		// Schedule Default
		_float fStartDelay{ 0.f }; // Spawn 요청후 몇초뒤에 시작?
		_float fInterval{ 0.f }; // 스폰 되는 사이에 Interval
		_uint  iMaxPerTick{ 8 }; // 한 틱 스폰 제한

		// 스킬 오브젝트 풀
		_uint iPoolLevelIndex{ 0 };		// 풀 레벨 인덱스
		wstring wstrSkillPoolTag{ L"" };	// 풀 태그

		_uint iSkillObjectFlags{ 0 };	// 스킬오브젝트의 행동 플래그 ESkillObjectFlag 참고
		_float fSpeed{ 3.f }; // 속도
		_float fLifeTime{ 4.f }; // 생명 시간
		_float fMaxDistance{ 10.f }; // 최대 사거리 ( 이후에 Set_Dead됨 )
		_float fHomingStrength{ 5.f }; // 호밍 새기
	}SPAWNER_ORIGIN_DESC;
	typedef struct tagSpawnerCopyDesc : public GAMEOBJECT_DESC
	{
		_uint iSpawnLevelIndex{ 0 };
		Vec3 vOrigin{ Vec3::Zero };
		Vec3 vForward{ Vec3::Zero };
		Vec3 vUp{ Vec3::UnitY };

		CGameObject* pRequester{ nullptr };
		CGameObject* pTarget{ nullptr };
		
		// 이펙트 회전상태
		TRANSFORM_INFO_STATE eEffectRotationState = TRANSFORM_INFO_STATE::END;
		_float fEffect_Rotation_Degree{ 0.00f };
	}SPAWNER_COPY_DESC;
private:
	enum class EState : _uint
	{
		Idle,
		Spawn
	};
protected:
	CSkillObjectSpawnerBase(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CSkillObjectSpawnerBase(const CSkillObjectSpawnerBase& rhs);
	virtual ~CSkillObjectSpawnerBase() = default;

	virtual HRESULT Initialize_Prototype() override { return S_OK; };
	virtual HRESULT Initialize_Prototype(SPAWNER_ORIGIN_DESC* pDesc);
	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Awake(const _uint iCurrentLevelID) override;
	virtual void Update_Priority(const _float fTimeDelta) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual void Update_Late(const _float fTimeDelta) override;
	virtual void Ready_Before_Render(const _float fTimeDelta) override;
	virtual HRESULT Render() override;
	void Trigger(const SPAWNER_COPY_DESC& desc, _bool bForceRestart = false);
	_bool Is_Idle() const { return m_eState == EState::Idle; }
protected:
	virtual _uint Get_TotalCount() const PURE;
	virtual void  Emit_One(_uint i, const Vec3& vFoward, const Vec3& vUp) PURE;
	void Spawn_SkillObject(const Vec3& vSpawnPos, const Vec3& vDir);
	void Spawn_SkillObject(const Vec3& vSpawnPos, const Vec3& vMoveDir, const Vec3& vLookDir);
private:
	void Change_State(EState eState);
	void Start_State(EState eState);
	void Update_State(const _float fTimeDelta);
	void End_State(EState eState);

	void Start_Spawn();
	void Update_Spawn(const _float fTimeDelta);
	void End_Spawn();

	Vec3 Get_Forward();
	Vec3 Get_Up() const;
protected:
	SPAWNER_ORIGIN_DESC* m_pOriginDesc{ nullptr };
	SPAWNER_COPY_DESC m_desc{};
	EState m_eState{ EState::Idle };
	_float m_fStateElapsed{ 0.f };
	_float m_fSpawnAcc{ 0.f };
	_uint  m_iSpawnedCount{ 0 };
public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual void Free() override;
};

NS_END
