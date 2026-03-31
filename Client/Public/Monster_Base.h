#pragma once
#include "ICameraAnchorHost.h"
#include "ContainerObject.h"
#include "Builder_Map.h"

NS_BEGIN(Engine)
class CEffectHandler;
NS_END

NS_BEGIN(Client)

class CMonster_Base abstract : public CContainerObject
	, public ICameraAnchorHost
{
	using Super = CContainerObject;

public:

public:
	typedef struct tagMonsterDesc : public Super::GAMEOBJECT_DESC
	{
		wstring wstrBodyModelTag = {};
		wstring wstrPartBodyPrototypeTag = {};
		wstring wstrAttackOverlapPrototypeTag = {};
		Vec3 vSpawnPosition = {};
		wstring wstrMonsterStateTag = {};
		std::span<std::pair<_uint, string>> spanBoneNames;
		PHYSICSCCT_DESC tCCTDesc{};
	}MONSTER_DESC;

	struct Part
	{
		enum Enum : _uint
		{
			BODY = 0,
			SWORD,
			GUN,
			END
		};
	};

	struct State
	{
		enum Enum : _uint
		{
			IDLE,

			WALK,
			RUN,
			JUMP,

			FALL,
			LAND,

			ATTACK,
			HIT,
			DOWN,
			AIR_BORNE,
			KNOCK_BACK,

			ENCOUNT,
			MISS_TARGET,
			CHASING,

			COMBO,
			GUN,
			CHARGE,
			SKILL1,
			SKILL2,

			LOOPDONE,

			END
		};
	};

	struct SubState
	{
		enum Enum
		{
			PHASE_1,
			PHASE_2,
			END
		};
	};
	struct HitSoundHashNum
	{
		enum Enum
		{
			SWORD_NORMAL,
			SWORD_HEAVY,
			DUAL_NORMAL,
			DUAL_HEAVY,
			GUN_FLESH,
			GUN_FLYBASE,
			GUN_DEATH_HIT,
			SHIELD_HIT,
			ENERGY_HIT,
			LIGHTING_HIT,
			WEAPONHIT_METAL,
			CRITICAL,
			DEATH01_VO,
			DEATH02_VO,
			HURT01_VO,
			HURT02_VO,
			END
		};
	};
protected:
	CMonster_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CMonster_Base(const CMonster_Base& rhs);
	virtual ~CMonster_Base() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual HRESULT		Awake(const _uint iCurrentLevelID) override;
	virtual void		Update_Priority(const _float fTimeDelta) override;
	virtual void		Update(const _float fTimeDelta) override;
	virtual void		Update_Late(const _float fTimeDelta) override;
	virtual void		Ready_Before_Render(const _float fTimeDelta) override;
	virtual HRESULT		Render() override;
	virtual _int		Get_AnimationIndex(const wstring& wstrName) override;
	virtual _wstring	Get_AnimationName(_uint iAniIndex);

public:
	virtual void		OnCollision(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual void		OnCollision_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo) override;
	virtual void		OnCollision_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual void		OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo) override;
	virtual void		OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;

	virtual _bool		On_Hit(const HIT_DESC& hitDesc) override;
	virtual void		Try_Attack(const HIT_DESC& hitDesc) override;
	virtual void		On_Dying();

	void				Set_RootMotion_Apply(_bool bApply);
	
	void				Trigger_Dissolve();
	_bool				Is_DissolveEnded();
public:
	// Camera Interface
	virtual ICameraAnchorProvider* Get_CameraAnchorProvider(_int iPartIndex = 0) override;
	virtual CTransform* Get_CameraAnchorOwnerTransform() override;

public:
	_bool Monster_IsGroggy();

protected:
	HRESULT				Ready_BaseStates();
	HRESULT				Ready_PartObjects(void* pArg);
	HRESULT				Ready_Components(void* pArgs);
	HRESULT				Ready_AttackOverlap(wstring prototypeName);
	HRESULT				Ready_EffectHandler(void* pArgs);
	HRESULT				Ready_CCT(void* pArgs);

	HRESULT				Ready_MonsterEmotion();

protected:
	void				OnHit_Sword(const HIT_DESC& hitDesc);
	void				OnHit_Dual(const HIT_DESC& hitDesc);
	void				OnHit_Gun(const HIT_DESC& hitDesc);
	void				OnHit_Skill(const HIT_DESC& hitDesc);

	void				Hit_Sound(EPlayerAttackFlag eFlag, DTO::EHitType eHitType);

protected:
	void				Compute_MonsterEmotionUV(EMonster_Emontion_State_Type eType);
protected:

	EMonster_Type			m_eMonsterType{ EMonster_Type::END};
	class CEffectHandler*	m_pEffectHandler = { nullptr };
	array<_uint, HitSoundHashNum::END> m_arrHitSoundHash;
public:
	void SetSpawnPos(CTransform::TRANSFORM_DESC tTransformDesc);
public:
	static  HRESULT			Create_Mosnter(EMonster_Type eCreateMonsterType , _uint iFindPrototypeLevelType , _uint iAddLevelType, CTransform::TRANSFORM_DESC* pTransformDesc = nullptr);
	virtual CGameObject*	Clone(void* pArg) PURE;
	virtual void Free() override;
};

NS_END