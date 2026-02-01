#pragma once
#include "GameObject.h"

NS_BEGIN(Client)

class CPhysics_Terrain final : public CGameObject
{
	using Super = CGameObject;
private:
	CPhysics_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CPhysics_Terrain(const CPhysics_Terrain& rhs);
	virtual ~CPhysics_Terrain() = default;

	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* pArg) override;

public:
	HRESULT Awake(const _uint iCurrentLevelID) override;
	virtual void Update_Priority(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Update_Late(_float fTimeDelta) override;
	virtual void Ready_Before_Render(const _float fTimeDelta) override;
	virtual HRESULT Render() override;

	HRESULT Ready_Components();
	HRESULT Ready_Physics();
	HRESULT Ready_PhysicsCollider();
	HRESULT Ready_PhysicsRigidBody();

public:
	static CPhysics_Terrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END