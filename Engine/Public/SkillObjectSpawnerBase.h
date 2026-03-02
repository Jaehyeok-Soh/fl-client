#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CSkillObjectSpawnerBase abstract : public CGameObject
{
	using Super = CGameObject;
public:
	typedef struct tagSpawnerOriginDesc
	{
		// Spawner Visual
		Vec3 vScaleStart{ Vec3::One };
		Vec3 vScaleEnd{ Vec3::One };
		_float fAppearTime{ 0.15f };
		_float fDisappearTime{ 0.15f };
		_float fAfterSpawnHold{ 0.05f };

		// Schedule Default
		_float fStartDelay{ 0.f };
		_float fInterval{ 0.f };
		_uint  iMaxPerTick{ 8 };

		// Warning 이펙트
		_bool bSpawnWarning{ false };
		_uint iWarningPoolLevel{ 0 };
		wstring wstrWarningPoolTag{ L"" };
		_uint iWarningSpawnLevel{ 0 };
		// TODO : warningDesc 추가

		// 스킬 오브젝트 풀
		_uint iPoolLevelIndex{ 0 };
		wstring wstrSkillPoolTag{ L"" };

		_uint iSkillObjectFlags{ 0 };
		_float fSpeed{ 3.f };
		_float fLifeTime{ 4.f };
		_float fMaxDistance{ 10.f };
		_float fHomingStrength{ 5.f };
	}SPAWNER_ORIGIN_DESC;
	typedef struct tagSpawnerCopyDesc : public GAMEOBJECT_DESC
	{
		_uint iSpawnLevelIndex{ 0 };
		Vec3 vOrigin{ Vec3::Zero };
		Vec3 vForward{ Vec3::Zero };
		Vec3 vUp{ Vec3::UnitY };
		CGameObject* pRequester{ nullptr };
		CGameObject* pTarget{ nullptr };
	}SPAWNER_COPY_DESC;
private:
	enum class EState : _uint
	{
		Idle,
		Appear,
		Spawn,
		Disappear
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
private:
	HRESULT Ready_Components();
private:
	// Spawner 렌더관련이 있는가?
	_bool Has_Visual();
	void Change_State(EState eState);
	void Start_State(EState eState);
	void Update_State(const _float fTimeDelta);
	void End_State(EState eState);

	void Start_Appear();
	void Update_Appear(const _float fTimeDelta);
	void End_Appear();

	void Start_Spawn();
	void Update_Spawn(const _float fTimeDelta);
	void End_Spawn();

	void Start_Disappear();
	void Update_Disappear(const _float fTimeDelta);
	void End_Disappear();

	Vec3 Get_Forward();
	Vec3 Get_Up() const;
protected:
	SPAWNER_ORIGIN_DESC* m_pOriginDesc{ nullptr };
	SPAWNER_COPY_DESC m_desc{};
	EState m_eState{ EState::Appear };
	_float m_fStateElapsed{ 0.f };
	_float m_fSpawnAcc{ 0.f };
	_uint  m_iSpawnedCount{ 0 };
public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual void Free() override;
};

NS_END
