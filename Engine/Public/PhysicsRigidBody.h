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
	void Update();

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