#include "Engine_pch.h"
#include "PhysicsSpringArm.h"
#include "GameInstance.h"

#include "GameObject.h"
#include "ContainerObject.h"
#include "PartObject.h"

#include "Physics_QueryFilterCallback_SpringArm.h"

#include "CameraMan.h"

#include "DebugDraw.h"

CPhysicsSpringArm::CPhysicsSpringArm()
	: Super()
{
}

CPhysicsSpringArm::CPhysicsSpringArm(const CPhysicsSpringArm& rhs)
	: Super(rhs)
{
}

HRESULT CPhysicsSpringArm::Initialize_Prototype(void* pArg)
{
	return S_OK;
}

HRESULT CPhysicsSpringArm::Initialize(void* pArg)
{
	m_filterData.data.word0 = 0;
	m_filterData.data.word1 = PHYSICSFILTERGROUP::MAP;
	m_filterData.flags = PxQueryFlag::eSTATIC
		| physx::PxQueryFlag::ePREFILTER;

	m_pxCameraSphere = PxSphereGeometry(0.1f);

	m_pFilterCallback = m_pGameInstance->GetQueryFilterCallback_SpringArm();

	m_pScene = m_pGameInstance->GetPhysicsScene();

	return S_OK;
}

Vec3 CPhysicsSpringArm::CheckResolveCollision(Vec3 vCameraPos, Vec3 vTargetPos)
{
	Vec3 cameraPos = vCameraPos;

	Vec3 vDir = cameraPos - vTargetPos;
	_float fMaxDist = vDir.Length();
	vDir.Normalize();

	m_pxTransform = PxTransform(ToPxVec3(vTargetPos));

	if (m_pScene->sweep(m_pxCameraSphere,
		m_pxTransform,
		ToPxVec3(vDir),
		fMaxDist,
		m_SweepBuffer,
		PxHitFlag::eDEFAULT,
		m_filterData,
		m_pFilterCallback))
	{
		if (m_SweepBuffer.block.hadInitialOverlap())
		{
			return vTargetPos;
		}

		else
		{
			_float fSafeDist = m_SweepBuffer.block.distance - 0.15f;

			if (fSafeDist < 0.f)
				fSafeDist = 0.f;

			cameraPos = vTargetPos + (vDir * fSafeDist);
		}
	}

	return cameraPos;
}

CPhysicsSpringArm* CPhysicsSpringArm::Create(void* pArg)
{
	CPhysicsSpringArm* pInstance = new CPhysicsSpringArm();

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSG_BOX("Failed to Created : CPhysicsSpringArm");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CPhysicsSpringArm::Clone(void* pArg)
{
	CPhysicsSpringArm* pInstance = new CPhysicsSpringArm(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPhysicsSpringArm");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPhysicsSpringArm::Free()
{
	Safe_Release(m_pFilterCallback);
	
	Super::Free();
}
