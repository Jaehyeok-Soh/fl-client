#pragma once
#include "Player.h"

NS_BEGIN(Engine)
class CRay;
NS_END

NS_BEGIN(Client)

class CMainPlayer final : public CPlayer
{
	using Super = CPlayer;
public:
	enum AbilityState : _uint
	{
		HAND = 0,
		END
	};
private:
	CMainPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CMainPlayer(const CMainPlayer& rhs);
	virtual ~CMainPlayer() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
public:
	// Static Object가 다른 Level에 갈때 호출
	virtual HRESULT Reinitialize(GAMEOBJECT_REINIT_DESC* pDesc) override;
	virtual HRESULT Clear_WhenChangeLevel() override;
	virtual HRESULT Awake(const _uint iCurrentLevelID) override;
	virtual void	Update_Priority(const _float fTimeDelta) override;
	virtual void	Update(const _float fTimeDelta) override;
	virtual void	Update_Late(const _float fTimeDelta) override;
	virtual void	Ready_Before_Render(const _float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void	OnCollision(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual void	OnCollision_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo) override;
	virtual void	OnCollision_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual void	OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual void	OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;

	virtual _bool On_Hit(const HIT_DESC& hitDesc) override;
	virtual void Try_Attack(const HIT_DESC& hitDesc) override;

private:
	// void Movement_Ground(const _float fTimeDelta);
	//_bool Try_AttackHit(ECollideLayer eMyLayer, CCollider* pOther);
private:
	HRESULT Ready_Ability();
	HRESULT Ready_Colliders();
	HRESULT Ready_Ray();
	HRESULT Ready_CCT();
	HRESULT Ready_AttackOverlap();

	HRESULT Ready_AttackStates();

private:
	CRay* m_pFootRay = { nullptr };
	CRay* m_pMoveRay = { nullptr };

public:
	static CMainPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END