#include "Engine_pch.h"
#include "PhysicsRigidBody.h"
#include "GameInstance.h"
#include "GameObject.h"

#include "PhysicsCollider.h"

#include "Transform.h"

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

	if (m_pActors.size() <= 0 || m_pActors.front() == nullptr)
	{
		PHYSICSCOLLIDER_DESC* colDesc = collider->GetDesc();
		vector<PxShape*>* shapes = &collider->GetShapes();

	m_tDesc.pOwnerMatrix = Get_Owner()->Get_Component<CTransform>()->Get_WorldMatrixPtr();
	m_pActors = m_pGameInstance->GetActor(&m_tDesc,
		colDesc,
		*shapes);

		for (auto& actor : m_pActors)
		{
			if (actor)
			{
				m_pGameInstance->AddActor(actor);
				SetUserData(Get_Owner());
			}
		}
	}

	EnableCollision(true);
}

void CPhysicsRigidBody::Update(const Matrix& matWorld)
{
	// TODO : get rigidbody state
	// TODO : set transform
}

CPhysicsRigidBody* CPhysicsRigidBody::SetUserData(_uint iIndex, CGameObject* pObject)
{
	m_pActors[iIndex]->userData = static_cast<void*>(pObject);

	return this;
}

CPhysicsRigidBody* CPhysicsRigidBody::SetUserData(CGameObject* pObject)
{
	void* userData = static_cast<void*>(pObject);

	for (auto& actor : m_pActors)
		actor->userData = userData;

	return this;
}

CPhysicsRigidBody* CPhysicsRigidBody::SetTransform(_uint iIndex, const Matrix& matWorld)
{
	PxTransform pxTf = m_pGameInstance->XMMatrixToPxTransform(matWorld);

	m_pActors[iIndex]->setGlobalPose(pxTf);

	return this;
}

CPhysicsRigidBody* CPhysicsRigidBody::SetTransform(const Matrix& matWorld)
{
	PxTransform pxTf = m_pGameInstance->XMMatrixToPxTransform(matWorld);

	for (auto& actor : m_pActors)
		actor->setGlobalPose(pxTf);

	return this;
}

CPhysicsRigidBody* CPhysicsRigidBody::SetPosition(_uint iIndex, Vec3 vCenter)
{
	if (m_tDesc.eType != EPhysicsActorType::KINEMATIC)
		return this;

	return this;
}

CPhysicsRigidBody* CPhysicsRigidBody::SetPosition(Vec3 vCenter)
{
	if (m_tDesc.eType != EPhysicsActorType::KINEMATIC)
		return this;

	return this;
}

CPhysicsRigidBody* CPhysicsRigidBody::Rotation(_uint iIndex, Quat vQuat)
{
	if (m_tDesc.eType != EPhysicsActorType::KINEMATIC)
		return this;

	return this;
}

CPhysicsRigidBody* CPhysicsRigidBody::Rotation(Quat vQuat)
{
	if (m_tDesc.eType != EPhysicsActorType::KINEMATIC)
		return this;

	return this;
}

CPhysicsRigidBody* CPhysicsRigidBody::Move(_uint iIndex, Vec3 vDist, _float fTimeDelta)
{
	PxTransform pxTf(PxVec3(vDist.x, vDist.y, vDist.z));

	//if (m_pActors[iIndex]->getType() == PxActorType::Enum::eRIGID_DYNAMIC)
	//	m_pActors[iIndex].
	//else
	//	m_pActors[iIndex]->setGlobalPose(pxtf);

	m_pActors[iIndex]->setGlobalPose(pxTf);

	return this;
}

CPhysicsRigidBody* CPhysicsRigidBody::Move(Vec3 vDist, _float fTimeDelta)
{
	PxTransform pxTf(PxVec3(vDist.x, vDist.y, vDist.z));

	for (auto& actor : m_pActors)
		actor->setGlobalPose(pxTf);

	return this;
}

CPhysicsRigidBody* CPhysicsRigidBody::Shot(_uint iIndex, Vec3 vDist, _float fTimeDelta)
{
	return this;
}

CPhysicsRigidBody* CPhysicsRigidBody::EnableCollision(_bool bEnable)
{
	if (m_pActors.size() <= 0)
		return this;

	if (m_pActors.front() == nullptr)
		return this;

	for (auto& actor : m_pActors)
	{
		if (actor)
		{
			PxU32 numShape = actor->getNbShapes();
			vector<PxShape*> vecShape(numShape);
			actor->getShapes(vecShape.data(), numShape);

			if (vecShape.size() > 0)
			{
				for (auto& shape : vecShape)
				{
					PxShapeFlags flags = shape->getFlags();
					if (flags.isSet(PxShapeFlag::eTRIGGER_SHAPE))
					{
						shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, bEnable);
						shape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, false);
						shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
					}
					else
					{
						shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, bEnable);
						shape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, bEnable);
					}
				}
			}
		}

		actor->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, !bEnable);
	}

	return this;
}

#ifdef _DEBUG
void CPhysicsRigidBody::Render()
{
	if (m_pActors[0]->getType() == PxActorType::eRIGID_DYNAMIC)
	{
		auto a = 1;
	}

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
