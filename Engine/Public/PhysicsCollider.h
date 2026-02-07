#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CPhysicsCollider final : public CComponent
{
	using Super = CComponent;
public:
	constexpr static EComponentType _ID = EComponentType::PX_COLLIDER;

private:
	CPhysicsCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CPhysicsCollider(const CPhysicsCollider& rhs);
	virtual ~CPhysicsCollider() = default;

	virtual HRESULT Initialize_Prototype(void* pArg);
	virtual HRESULT Initialize(void* pArg) override;

public:
	void Awake();
	void Update(const Matrix& matWorld);

	void OnCollision(PxRigidActor* _pOther);
	void OnCollisionEnter(PxRigidActor* _pOther);
	void OnCollisionExit(PxRigidActor* _pOther);

public:
	void Set_Active(_bool bVal) { m_tDesc.bIsActive = bVal; }
	_bool Get_Active() const { return m_tDesc.bIsActive; }

	vector<PxShape*>& GetShapes() { return m_pColliderShapes; }
	PHYSICSCOLLIDER_DESC* GetDesc() { return &m_tDesc; }
	void SetCenter(Vec3 vCenter);

	void SetCollisionFilter();

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pDeviceContext = { nullptr };

private:
	PHYSICSCOLLIDER_DESC m_tDesc = {};
	_bool m_isColl = { false };

	vector<PxShape*> m_pColliderShapes;

#ifdef _DEBUG
public:
	virtual void Render() override;
#endif

public:
	static CPhysicsCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END