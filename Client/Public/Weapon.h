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
		NONE, HOLD, HAND, HAND_ONLY_POS, HAND_ONLY_POS_SCALE, CONDEMN
	};

	enum class AnimState : _uint
	{
		PLAY, STOP, PLAY_ONCE
	};

	enum WeaponDescFlag : Flags
	{
		WF_RGBMappingOn = 0x000001,
		WF_Dissolve		= 0x000002
	};

	typedef struct tagWeaponDesc : public CPartObject::PARTOBJ_DESC
	{
		wstring				wstrModelPrototypeName	= { L"" };
		const Matrix*		pMatHandSocket			= { nullptr };
		const Matrix*		pMatSocket				= { nullptr };

		Weapon_ModelType	eModel		= { Weapon_ModelType::STATIC };
		AnimState			eAnimState	= { AnimState::STOP };
		State				eState		= { State::NONE };

		_bool				bMianWeapon		= { false };

		Flags				FDescFlag = { 0 }; //WeaponDescFlag 참고 해서 설정

		Vec4 vColorR = Vec4::Zero;
		Vec4 vColorG = Vec4::Zero;
		Vec4 vColorB = Vec4::Zero;
		
		Matrix matHoldOffsetMatrix = Matrix::Identity;
		Matrix matHandOffsetMatrix = Matrix::Identity;
		Matrix matConOffsetMatrix	= Matrix::Identity;

		_uint iStartAnimIdx = { 0 };

		Vec3 vDissolveColor = Vec3::Zero;
		Vec3 vDissolveValues = Vec3::Zero; // spawn time, speed, Edge

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
	virtual void OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo) override;
	virtual void OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual HRESULT Render() override;

public:
	void Change_WeaponAnim(_uint iAnimIdx, _bool bLoop, _bool bForce, _bool bBlend =false);

	// getter setter funcs
public:
	void	Set_HandSocket();
	void	Set_DefaultSocket();
	_bool	Is_Hand() const { return m_eState == State::HAND; }
	void	Set_WeaponState(State eState);
	void	Set_WeaponState(_uint iState);

	void	Set_Weapon_PlayState(_uint iPlayState) { m_eAnimState = static_cast<AnimState>(iPlayState); m_bPlayOnceYet = true; }

	_int	Get_AnimationIndex(const wstring& wstrName);

protected:
	State				m_eState = { State::NONE };
	AnimState			m_eAnimState = { AnimState::STOP };

	Weapon_Type			m_eWaeponType = { Weapon_Type::SWORD };
	Weapon_ModelType	m_eModleType = { Weapon_ModelType::STATIC };

	const Matrix*		m_pMatHandSocket = { nullptr };
	const Matrix*		m_pMatSocket = { nullptr };

	_bool				m_bMainWeapon = { false };

	Matrix				m_matHoldOffsetMatrix = {  };
	Matrix				m_matHandOffsetMatrix = {  };
	Matrix				m_matConOffsetMatrix		= {};

	Flags				m_FDescFlags = { 0 };

	_bool				m_bPlayOnceYet = { true };


private:
	SHADER_RGBCOLOR_DESC m_tColorDesc = {};
	CComputeShader* m_pBoneMeshCS{ nullptr };
	CComputeShader* m_pBoneCombineCS{ nullptr };
	CComputeShader* m_pAnimECS{ nullptr };
	CComputeShader* m_pAnimBlendECS{ nullptr };

protected:
	DissolveEffectDesc	m_tDissolveDesc = {};

private:
	HRESULT Ready_Components(WEAPON_DESC* pDesc);
	HRESULT Bind_ShaderResources();
	HRESULT Ready_ComputeShaders();
	HRESULT Ready_DissolveEffect_Setting(WEAPON_DESC* pDesc);

private:
	void	Play_Anim(const _float fTimeDelta);
	void	Update_HoldingPos();

	HRESULT Render_StaticWeap();
	HRESULT Render_AnimWeap();

private:
	void	DissolveStart();
	_bool	Is_Dissolve();

public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual void Free() override;
};

NS_END