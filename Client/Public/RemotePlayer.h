#pragma once
#include "Player.h"
#include "UserModel.h"

NS_BEGIN(Client)

class CRemotePlayer : public CPlayer
{
	using Super = CPlayer;
public:
	typedef struct tagRemotePlayerDesc : public Super::PLAYER_DESC
	{
		UserModel tUserModel = {};
	}REMOTE_PLAYER_DESC;

private:
	CRemotePlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CRemotePlayer(const CRemotePlayer& rhs);
	virtual ~CRemotePlayer() = default;

	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Awake(const _uint iCurrentLevelID) override;
	virtual HRESULT Clear_WhenChangeLevel() override;
	virtual void Update_Priority(const _float fTimeDelta) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual void Update_Late(const _float fTimeDelta) override;
	virtual void Ready_Before_Render(const _float fTimeDelta);
	virtual HRESULT Render() override;

public:
	void Synchronize_State();

private:
	HRESULT Ready_Ability();
	HRESULT Ready_EffectEvent();
	HRESULT Ready_SoundHandler();
	HRESULT Ready_AttackStates();

private:
	UserModel m_tagUserData = {};
	Vec3 m_vServerTargetpos = { 0.f, 0.f, 0.f };
	_float m_fServerTargetYaw = { 0.f };
	_float m_fInterpolationSpeed = { 10.f };
public:
	static CRemotePlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END