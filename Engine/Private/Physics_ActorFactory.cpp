#include "Engine_pch.h"

#include "GameInstance.h"
#include "EngineConsole.h"

#include "Physics_ActorFactory.h"

CPhysics_ActorFactory::CPhysics_ActorFactory(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, PxPhysics* pPhysics, PxScene* pScene)
	: m_pGameInstance{ CGameInstance::GetInstance() },
	m_pDevice{ pDevice },
	m_pContext{ pContext },
	m_pPhysics{ pPhysics },
	m_pScene{ pScene }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CPhysics_ActorFactory::Initialize()
{
	return S_OK;
}

vector<PxRigidActor*> CPhysics_ActorFactory::GetActor(PHYSICSRIGIDBODY_DESC* rigidBodyDesc, PHYSICSCOLLIDER_DESC* colliderDesc, vector<PxShape*>& shapes)
{
	switch (rigidBodyDesc->eType)
	{
	case EPhysicsActorType::STATIC:
		return MakeStatics(rigidBodyDesc, colliderDesc, shapes);

	case EPhysicsActorType::DYNAMIC:
		return MakeDynamics(rigidBodyDesc, colliderDesc, shapes);

	case EPhysicsActorType::KINEMATIC:
		return MakeKinematics(rigidBodyDesc, colliderDesc, shapes);

	default:
		return MakeStatics(rigidBodyDesc, colliderDesc, shapes);
	}
}

vector<PxRigidActor*> CPhysics_ActorFactory::MakeStatics(PHYSICSRIGIDBODY_DESC* rigidBodyDesc, PHYSICSCOLLIDER_DESC* colliderDesc, vector<PxShape*>& shapes)
{
	vector<PxRigidActor*> result;

	for (size_t i = 0; i < rigidBodyDesc->pOwnerMatrices.size(); i++)
		result.push_back(MakeStatic(rigidBodyDesc->pOwnerMatrices[i], rigidBodyDesc->vScale_Isolated[i], shapes));

	return result;
}

vector<PxRigidActor*> CPhysics_ActorFactory::MakeDynamics(PHYSICSRIGIDBODY_DESC* rigidBodyDesc, PHYSICSCOLLIDER_DESC* colliderDesc, vector<PxShape*>& shapes)
{
	vector<PxRigidActor*> result;

	for (size_t i = 0; i < rigidBodyDesc->pOwnerMatrices.size(); i++)
		result.push_back(MakeDynamic(rigidBodyDesc->pOwnerMatrices[i], rigidBodyDesc->vScale_Isolated[i], rigidBodyDesc->fDensity, shapes));

	return result;
}

vector<PxRigidActor*> CPhysics_ActorFactory::MakeKinematics(PHYSICSRIGIDBODY_DESC* rigidBodyDesc, PHYSICSCOLLIDER_DESC* colliderDesc, vector<PxShape*>& shapes)
{
	vector<PxRigidActor*> result;

	for (size_t i = 0; i < rigidBodyDesc->pOwnerMatrices.size(); i++)
		result.push_back(MakeKinematic(rigidBodyDesc->pOwnerMatrices[i], rigidBodyDesc->vScale_Isolated[i], rigidBodyDesc->fDensity, shapes));

	return result;
}

PxRigidActor* CPhysics_ActorFactory::MakeStatic(const Matrix world, Vec3 scale, vector<PxShape*>& shapes)
{
	Matrix mat = world;

	PxVec3 pScale(scale.x, scale.y, scale.z);

	PxTransform transform = m_pGameInstance->XMMatrixToPxTransform(mat);
	PxRigidStatic* staticActor = m_pPhysics->createRigidStatic(transform);
	for (auto& shape : shapes)
	{
		PxShape* newShape = nullptr;
		PxMaterial* pMaterial = nullptr;

		PxU32 matCount = shape->getNbMaterials();
		if (matCount > 0)
		{
			vector<PxMaterial*> materials(matCount);
			shape->getMaterials(materials.data(), matCount);
			pMaterial = materials[0];
		}

		if (shape->getGeometry().getType() == PxGeometryType::ePLANE)
		{
			PxTransform localPose(PxVec3(0), PxQuat(PxHalfPi, PxVec3(0, 0, 1)));
			shape->setLocalPose(localPose);
		}

		else if (shape->getGeometry().getType() == PxGeometryType::eTRIANGLEMESH)
		{
			PxGeometryHolder geom = shape->getGeometry();
			PxTriangleMeshGeometry triGeom = geom.triangleMesh();

			triGeom.scale = PxMeshScale(pScale);

			if (pMaterial)
				newShape = m_pPhysics->createShape(triGeom, *pMaterial);
		}
		else
		{
			continue;
		}

		if (newShape)
		{
			newShape->setQueryFilterData(shape->getQueryFilterData());
			newShape->setSimulationFilterData(shape->getSimulationFilterData());
			newShape->setFlags(shape->getFlags());

			staticActor->attachShape(*newShape);
			PX_RELEASE(newShape);
		}
	}

	return staticActor;
}

PxRigidActor* CPhysics_ActorFactory::MakeDynamic(const Matrix world, Vec3 scale, _float density, vector<PxShape*>& shapes)
{
	Matrix mat = world;

	PxVec3 vPos = PxVec3(mat._41, mat._42, mat._43);
	PxQuat pQuat = m_pGameInstance->GetPureRotation(mat);
	PxVec3 pScale(scale.x, scale.y, scale.z);

	PxTransform transform(vPos, pQuat);
	PxRigidDynamic* dynamicActor = m_pPhysics->createRigidDynamic(transform);
	for (auto& shape : shapes)
	{
		if (shape->getGeometry().getType() == PxGeometryType::eCONVEXMESH)
		{
			PxGeometryHolder geom = shape->getGeometry();
			PxConvexMeshGeometry convexGeom = geom.convexMesh();

			convexGeom.scale = PxMeshScale(pScale);

			shape->setGeometry(convexGeom);
		}

		dynamicActor->attachShape(*shape);
	}

	PxRigidBodyExt::updateMassAndInertia(*dynamicActor, density);

	return dynamicActor;
}

PxRigidActor* CPhysics_ActorFactory::MakeKinematic(const Matrix world, Vec3 scale, _float density, vector<PxShape*>& shapes)
{
	PxTransform transform = m_pGameInstance->XMMatrixToPxTransform(world);

	PxRigidDynamic* kinematicActor = m_pPhysics->createRigidDynamic(transform);
	for (auto& shape : shapes)
		kinematicActor->attachShape(*shape);

	PxRigidBodyExt::updateMassAndInertia(*kinematicActor, density);

	kinematicActor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);

	return kinematicActor;
}

CPhysics_ActorFactory* CPhysics_ActorFactory::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, PxPhysics* pPhysics, PxScene* pScene)
{
	CPhysics_ActorFactory* pInstance = new CPhysics_ActorFactory(pDevice, pContext, pPhysics, pScene);

	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CPhysics_ActorFactory");
	}

	return pInstance;
}

void CPhysics_ActorFactory::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	Safe_Release(m_pGameInstance);

	Super::Free();
}
