#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CPhysicsRigidBody final : public CComponent
{
	using Super = CComponent;
public:
	constexpr static EComponentType _ID = EComponentType::PX_RIGIDBODY;

private:
	CPhysicsRigidBody(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CPhysicsRigidBody(const CPhysicsRigidBody& rhs);
	virtual ~CPhysicsRigidBody() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	void Awake();
	void Update(const Matrix& matWorld);
	
	CPhysicsRigidBody* SetUserData(_uint iIndex, CGameObject* pObject);
	CPhysicsRigidBody* SetUserData(CGameObject* pObject);

	PHYSICSRIGIDBODY_DESC* GetDesc() { return &m_tDesc; }

	//////////////////////
	/// Only kinematic ///
	CPhysicsRigidBody* SetTransform(_uint iIndex, const Matrix& matWorld);
	CPhysicsRigidBody* SetTransform(const Matrix& matWorld);

	CPhysicsRigidBody* SetPosition(_uint iIndex, Vec3 vCenter);
	CPhysicsRigidBody* SetPosition(Vec3 vCenter);

	CPhysicsRigidBody* Rotation(_uint iIndex, Quat vQuat);
	CPhysicsRigidBody* Rotation(Quat vQuat);

	CPhysicsRigidBody* Move(_uint iIndex, Vec3 vDist, _float fTimeDelta);
	CPhysicsRigidBody* Move(Vec3 vDist, _float fTimeDelta);
	/// Only kinematic ///
	//////////////////////

	////////////////////
	/// Only dynamic ///
	CPhysicsRigidBody* Shot(_uint iIndex, Vec3 vDist, _float fTimeDelta);
	//가속도
	//각가속도
	//질량
	//중력
	//힘
	/// Only dynamic ///
	////////////////////

	CPhysicsRigidBody* EnableCollision(_bool bEnable);

#ifdef _DEBUG
	void Render();
#endif // _DEBUG

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pDeviceContext = { nullptr };

private:
	vector<PxRigidActor*> m_pActors = { nullptr };
	PHYSICSRIGIDBODY_DESC m_tDesc = {};

public:
	static CPhysicsRigidBody* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END