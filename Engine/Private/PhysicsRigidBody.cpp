#include "Engine_pch.h"
#include "PhysicsRigidBody.h"
#include "GameInstance.h"
#include "GameObject.h"

#include "PhysicsCollider.h"

CPhysicsRigidBody::CPhysicsRigidBody(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super()
	, m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

CPhysicsRigidBody::CPhysicsRigidBody(const CPhysicsRigidBody& rhs)
	: Super(rhs)
	, m_pDevice(rhs.m_pDevice)
	, m_pDeviceContext(rhs.m_pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

HRESULT CPhysicsRigidBody::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPhysicsRigidBody::Initialize(void* pArg)
{
	m_tDesc = *static_cast<PHYSICSRIGIDBODY_DESC*>(pArg);

	return S_OK;
}

void CPhysicsRigidBody::Awake()
{
	CPhysicsCollider* collider = m_pOwner->Get_Component<CPhysicsCollider>();

	if (!collider)
	{
		MSG_BOX("Failed to awaken : CPhysicsRigidBody, no collider");
		return;
	}

	PHYSICSCOLLIDER_DESC* colDesc = collider->GetDesc();
	vector<PxShape*>* shapes = &collider->GetShapes();

	m_pActors = m_pGameInstance->GetActor(&m_tDesc,
		colDesc,
		*shapes);

	for (auto& actor : m_pActors)
	{
		if (actor)
			m_pGameInstance->AddActor(actor);
	}
}

void CPhysicsRigidBody::Update()
{
	// TODO : get rigidbody state
	// TODO : set transform
}

#ifdef _DEBUG
void CPhysicsRigidBody::Render()
{
	for (auto& actor : m_pActors)
		m_pGameInstance->Physics_Render(actor);
}
#endif // _DEBUG

CPhysicsRigidBody* CPhysicsRigidBody::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CPhysicsRigidBody* pInstance = new CPhysicsRigidBody(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPhysicsRigidBody");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CPhysicsRigidBody::Clone(void* pArg)
{
	CPhysicsRigidBody* pInstance = new CPhysicsRigidBody(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPhysicsRigidBody");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPhysicsRigidBody::Free()
{
	for (auto& actor : m_pActors)
	{
		if (actor)
			PX_RELEASE(actor);
	}

	m_pActors.clear();

	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);

	Super::Free();
}
