#pragma once
#include "PartObject.h"

NS_BEGIN(Client)

class CWeapon abstract : public CPartObject
{
	using Super = CPartObject;

public:
	enum class Weapon_Type
	{
		SWORD, GUN, SKILL
	};

	enum class Weapon_ModelType
	{
		STATIC, ANIM
	};

	enum class State : _uint
	{
		NONE,
		HOLD,
		HAND
	};

	typedef struct tagWeaponDesc : public CPartObject::PARTOBJ_DESC
	{
		wstring				wstrModelPrototypeName	= { L"" };
		const Matrix*		pMatHandSocket			= { nullptr };
		const Matrix*		pMatSocket				= { nullptr };

		Weapon_ModelType	eModel	= { Weapon_ModelType::STATIC };

		_bool				bMianWeapon = { false };

	}WEAPON_DESC;

protected:


protected:
	CWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, Weapon_Type eWeapon);
	explicit CWeapon(const CWeapon& rhs);
	virtual ~CWeapon() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual HRESULT Awake(const _uint iCurrentLevelIndex) override;
	virtual void Update_Priority(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Update_Late(_float fTimeDelta) override;
	virtual void Ready_Before_Render(_float fTimeDelta) override;
	virtual void OnCollision(_uint iMyColliderLayer, CGameObject* pOther) override;
	virtual void OnCollision_Enter(_uint iMyColliderLayer, CGameObject* pOther) override;
	virtual void OnCollision_Exit(_uint iMyColliderLayer, CGameObject* pOther) override;
	virtual void OnTrigger_Enter(_uint iMyColliderLayer, CGameObject* pOther) override;
	virtual void OnTrigger_Exit(_uint iMyColliderLayer, CGameObject* pOther) override;
	virtual HRESULT Render() override;

	// getter setter funcs
public:
	void	Set_HandSocket();
	void	Set_DefaultSocket();
	_bool	Is_Hand() const { return m_eState == State::HAND; }
	void	Set_WeaponState(State eState) { m_eState = eState; }
	void	Set_WeaponState(_uint iState) { m_eState = static_cast<State>(iState); }

protected:
	State				m_eState		= { State::NONE };
	Weapon_Type			m_eWaeponType	= { Weapon_Type::SWORD };
	Weapon_ModelType	m_eModleType	= { Weapon_ModelType::STATIC };

	const Matrix*		m_pMatHandSocket	= { nullptr };
	const Matrix*		m_pMatSocket		= { nullptr };

	_bool				m_bMainWeapon		= { false };

	Matrix				m_matRotation = {  };

private:
	HRESULT Ready_Components(WEAPON_DESC* pDesc);
	HRESULT Bind_ShaderResources();
	HRESULT Ready_ComputeShaders();

private:
	void	Play_Anim(const _float fTimeDelta);
	void	Update_HoldingPos();

	HRESULT Render_StaticWeap();
	HRESULT Render_AnimWeap();

public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual void Free() override;
};

NS_END