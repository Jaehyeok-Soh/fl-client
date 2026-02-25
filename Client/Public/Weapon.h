#pragma once
#include "PartObject.h"

NS_BEGIN(Engine)
class CComputeShader;
NS_END

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

	enum class AnimState : _uint
	{
		PLAY,
		STOP
	};

	typedef struct tagWeaponDesc : public CPartObject::PARTOBJ_DESC
	{
		wstring				wstrModelPrototypeName = { L"" };
		const Matrix* pMatHandSocket = { nullptr };
		const Matrix* pMatSocket = { nullptr };

		Weapon_ModelType	eModel = { Weapon_ModelType::STATIC };

		_bool				bMianWeapon = { false };
		_bool				bRGBShader = { true };

		Vec4 vColorR = Vec4::Zero;
		Vec4 vColorG = Vec4::Zero;
		Vec4 vColorB = Vec4::Zero;
		
		Matrix matHoldOffsetMatrix = Matrix::Identity;
		Matrix matHandOffsetMatrix = Matrix::Identity;

		
		_uint iStartAnimIdx = { 0 };
	}WEAPON_DESC;

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
	virtual void OnCollision(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual void OnCollision_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo) override;
	virtual void OnCollision_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual void OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual void OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual HRESULT Render() override;

public:
	void Change_WeaponAnim(_uint iAnimIdx, _bool bLoop, _bool bForce, _bool bBlend =false);

	// getter setter funcs
public:
	void	Set_HandSocket();
	void	Set_DefaultSocket();
	_bool	Is_Hand() const { return m_eState == State::HAND; }
	void	Set_WeaponState(State eState) { m_eState = eState; }
	void	Set_WeaponState(_uint iState) { m_eState = static_cast<State>(iState); }

	void	Set_Weapon_PlayState(_uint iPlayState) { m_eAnimState = static_cast<AnimState>(iPlayState); }

protected:
	State				m_eState = { State::NONE };
	AnimState			m_eAnimState = { AnimState::STOP };

	Weapon_Type			m_eWaeponType = { Weapon_Type::SWORD };
	Weapon_ModelType	m_eModleType = { Weapon_ModelType::STATIC };

	const Matrix*		m_pMatHandSocket = { nullptr };
	const Matrix*		m_pMatSocket = { nullptr };

	_bool				m_bMainWeapon = { false };

	Matrix				m_matRotation = {  };

	Matrix				m_matHoldOffsetMatrix = {  };
	Matrix				m_matHandOffsetMatrix = {  };

	_bool m_bColorMapping = { false };

	SHADER_RGBCOLOR_DESC m_tColorDesc = {};
	CComputeShader* m_pBoneMeshCS{ nullptr };
	CComputeShader* m_pBoneCombineCS{ nullptr };
	CComputeShader* m_pAnimECS{ nullptr };
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