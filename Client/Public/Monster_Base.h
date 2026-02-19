#pragma once
#include "ContainerObject.h"

NS_BEGIN(Engine)
class CRay;
NS_END

NS_BEGIN(Client)

class CMonster_Base abstract : public CContainerObject
{
	using Super = CContainerObject;
public:
	typedef struct tagMonsterDesc : public Super::GAMEOBJECT_DESC
	{
		wstring wstrBodyModelTag = { L"" };
		wstring wstrPartBodyPrototypeTag = { L"" };
		wstring wstrNavigationPrototypeTag = { L"" };
		wstring wstrAttackOverlapPrototypeTag = { L"" };
		_int iNavigationCellIndex = { -1 };
		Vec3 vSpawnPosition = {};

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
	virtual void OnCollision(_uint iMyColliderLayer, CGameObject* pOther) override;
	virtual void OnCollision_Enter(_uint iMyColliderLayer, CGameObject* pOther) override;
	virtual void OnCollision_Exit(_uint iMyColliderLayer, CGameObject* pOther) override;
	virtual void OnTrigger_Enter(_uint iMyColliderLayer, CGameObject* pOther) override;
	virtual void OnTrigger_Exit(_uint iMyColliderLayer, CGameObject* pOther) override;

protected:
	HRESULT Ready_BaseStates();
	HRESULT Ready_PartObjects(void* pArg);
	HRESULT Ready_Components(void* pArgs);
	HRESULT Ready_Ray();

protected:
	HRESULT Ready_CCT(void* pArgs);

protected:
private:
	CRay* m_pFootRay = { nullptr };
	CRay* m_pMoveRay = { nullptr };

public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual void Free() override;
};

NS_END