#include "Engine_pch.h"

#include "GameInstance.h"
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

PxRigidActor* CPhysics_ActorFactory::GetActor(PHYSICSRIGIDBODY_DESC* rigidBodyDesc, PHYSICSCOLLIDER_DESC* colliderDesc, vector<PxShape*>& shapes)
{
	switch (rigidBodyDesc->eType)
	{
	case EPhysicsActorType::STATIC:
		return MakeStatic(rigidBodyDesc, colliderDesc, shapes);

	case EPhysicsActorType::DYNAMIC:
		return MakeDynamic(rigidBodyDesc, colliderDesc, shapes);

	case EPhysicsActorType::KINEMATIC:
		return MakeKinematic(rigidBodyDesc, colliderDesc, shapes);

	default:
		return MakeStatic(rigidBodyDesc, colliderDesc, shapes);
	}
}

PxRigidActor* CPhysics_ActorFactory::MakeStatic(PHYSICSRIGIDBODY_DESC* rigidBodyDesc, PHYSICSCOLLIDER_DESC* colliderDesc, vector<PxShape*>& shapes)
{
	if (m_pGameInstance->HasNegativeScale(*rigidBodyDesc->pOwnerMatrix))
		return MakeStatic_NegativeScale(rigidBodyDesc, colliderDesc, shapes);

	PxTransform transform = m_pGameInstance->XMMatrixToPxTransform(*rigidBodyDesc->pOwnerMatrix);
	PxRigidStatic* staticActor = m_pPhysics->createRigidStatic(transform);
	for (auto& shape : shapes)
	{
		if (shape->getGeometry().getType() == PxGeometryType::ePLANE)
		{
			PxTransform localPose(PxVec3(0), PxQuat(PxHalfPi, PxVec3(0, 0, 1)));
			shape->setLocalPose(localPose);
		}

		staticActor->attachShape(*shape);
	}

	return staticActor;
}

PxRigidActor* CPhysics_ActorFactory::MakeDynamic(PHYSICSRIGIDBODY_DESC* rigidBodyDesc, PHYSICSCOLLIDER_DESC* colliderDesc, vector<PxShape*>& shapes)
{
	PxTransform transform = m_pGameInstance->XMMatrixToPxTransform(*rigidBodyDesc->pOwnerMatrix);

	PxRigidDynamic* dynamicActor = m_pPhysics->createRigidDynamic(transform);
	for (auto& shape : shapes)
		dynamicActor->attachShape(*shape);

	PxRigidBodyExt::updateMassAndInertia(*dynamicActor, rigidBodyDesc->fDensity);

	return dynamicActor;
	return nullptr;
}

PxRigidActor* CPhysics_ActorFactory::MakeKinematic(PHYSICSRIGIDBODY_DESC* rigidBodyDesc, PHYSICSCOLLIDER_DESC* colliderDesc, vector<PxShape*>& shapes)
{
	PxTransform transform = m_pGameInstance->XMMatrixToPxTransform(*rigidBodyDesc->pOwnerMatrix);

	PxRigidDynamic* kinematicActor = m_pPhysics->createRigidDynamic(transform);
	for (auto& shape : shapes)
		kinematicActor->attachShape(*shape);

	PxRigidBodyExt::updateMassAndInertia(*kinematicActor, rigidBodyDesc->fDensity);

	kinematicActor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);

	return kinematicActor;
	return nullptr;
}

PxRigidActor* CPhysics_ActorFactory::MakeStatic_NegativeScale(PHYSICSRIGIDBODY_DESC* rigidBodyDesc, PHYSICSCOLLIDER_DESC* colliderDesc, vector<PxShape*>& shapes)
{
	Matrix mat = *rigidBodyDesc->pOwnerMatrix;

	// 1. 축 벡터 추출
	Vec3 u = Vec3(mat._11, mat._12, mat._13); // Right
	Vec3 v = Vec3(mat._21, mat._22, mat._23); // Up
	Vec3 w = Vec3(mat._31, mat._32, mat._33); // Look
	Vec3 pos = Vec3(mat._41, mat._42, mat._43);

	// 2. 스케일 추출
	Vec3 scale(u.Length(), v.Length(), w.Length());
	if (scale.x < 1e-4f) scale.x = 1.f;
	if (scale.y < 1e-4f) scale.y = 1.f;
	if (scale.z < 1e-4f) scale.z = 1.f;

	// 3. 정규화
	u.Normalize();
	v.Normalize();
	w.Normalize();

	// 4. 미러링 체크 (u, v, w로 만든 박스의 부피가 음수인가?)
	// (X cross Y) dot Z
	float det = u.Cross(v).Dot(w);
	bool bMirrored = (det < -1e-4f);

	if (bMirrored)
	{
		// ★ 중요: Decompose 따위 쓰지 않고, 그냥 우리가 X축을 뒤집어 버림
		u *= -1.0f;
		scale.x *= -1.0f; // 나중에 쉐이프에 넣을 스케일
	}

	// 5. PhysX Matrix (3x3) 직접 생성
	// Decompose를 거치지 않고, 우리가 만든 축(u,v,w)을 바로 쿼터니언으로 변환
	// (찌그러져 있어도 그나마 제일 비슷한 회전을 PhysX가 알아서 만듦)
	PxMat33 pMat(
		PxVec3(u.x, u.y, u.z), // Corrected Right
		PxVec3(v.x, v.y, v.z), // Up
		PxVec3(w.x, w.y, w.z)  // Look
	);

	PxQuat pQuat(pMat); // 3x3 행렬 -> 쿼터니언 변환

	// 6. 액터 생성
	PxTransform transform(PxVec3(pos.x, pos.y, pos.z), pQuat);
	PxRigidStatic* staticActor = m_pPhysics->createRigidStatic(transform);

	// 7. 쉐이프 적용
	for (auto& shape : shapes)
	{
		if (shape->getGeometry().getType() == PxGeometryType::eTRIANGLEMESH)
		{
			PxGeometryHolder geom = shape->getGeometry();
			PxTriangleMeshGeometry triGeom = geom.triangleMesh();

			// 추출한 스케일 적용 (bMirrored면 x에 -1 들어있음)
			triGeom.scale.scale.x *= scale.x * triGeom.scale.scale.x;
			triGeom.scale.scale.y *= scale.y * triGeom.scale.scale.y;
			triGeom.scale.scale.z *= scale.z * triGeom.scale.scale.z;

			shape->setGeometry(triGeom);
		}
		// ... Convex 등 ...
		staticActor->attachShape(*shape);
	}

	return staticActor;
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
