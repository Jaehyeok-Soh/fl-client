#include "Engine_pch.h"

#include "GameInstance.h"
#include "Physics_CCTManager.h"
#include "Physics_ResourceManager.h"

#include "Physics_CCTHitReport.h"
#include "Physics_CCTBehaviorCallback.h"
#include "Physics_CCTFilterCallback.h"

CPhysics_CCTManager::CPhysics_CCTManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, PxPhysics* pPhysics, PxScene* pScene, CPhysics_ResourceManager* pResourceManager)
	: m_pGameInstance{ CGameInstance::GetInstance() },
	m_pDevice{ pDevice },
	m_pContext{ pContext },
	m_pPhysics{ pPhysics },
	m_pScene{ pScene },
	m_pResourceManager{ pResourceManager }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pResourceManager);
}

HRESULT CPhysics_CCTManager::Initialize()
{
	m_pControllerManager = PxCreateControllerManager(*m_pScene);

	m_pControllerManager->setOverlapRecoveryModule(false);

	m_pCCTHitReport = CPhysics_CCTHitReport::Create();
	m_pCCTBehaviorCallback = CPhysics_CCTBehaviorCallback::Create();
	m_pCCTFilterCallback = CPhysics_CCTFilterCallback::Create();

	return S_OK;
}

PxController* CPhysics_CCTManager::GetController(PHYSICSCCT_DESC* pDesc)
{
	switch (pDesc->eType)
	{
	case EPhysicsCCTType::BOX:
		return MakeBoxController(pDesc);

	case EPhysicsCCTType::CAPSULE:
		return MakeCapsuleController(pDesc);

	default:
		return MakeCapsuleController(pDesc);
	}
}

void CPhysics_CCTManager::SetOverLapRecovery(_bool bFlag)
{
	m_pControllerManager->setOverlapRecoveryModule(bFlag);
}

void CPhysics_CCTManager::SetPreciseSweeps(_bool bFlag)
{
	m_pControllerManager->setPreciseSweeps(bFlag);
}

_int CPhysics_CCTManager::GetNumCharacterControllers()
{
	return m_pControllerManager->getNbControllers();
}

void CPhysics_CCTManager::ReleaseCharacter(PxController* cct)
{
	if (cct)
		PX_RELEASE(cct);
}

void CPhysics_CCTManager::ReleaseCCTManager()
{
	m_pControllerManager->purgeControllers();
}

PxController* CPhysics_CCTManager::MakeBoxController(PHYSICSCCT_DESC* pDesc)
{
	PxBoxControllerDesc desc{};
	desc.halfSideExtent = pDesc->vExtens.x / 2.f;
	desc.halfHeight = pDesc->vExtens.y / 2.f;
	desc.halfForwardExtent = pDesc->vExtens.z / 2.f;
	desc.material = m_pResourceManager->GetMaterial(&pDesc->tMaterial);

	desc.contactOffset = pDesc->fContactOffset;
	desc.stepOffset = pDesc->fStepOffset;
	//desc.slopeLimit = pDesc->fSlopeLimit;
	desc.slopeLimit = PxCos(PxDegToRad(89.f));

	desc.reportCallback = m_pCCTHitReport;
	desc.behaviorCallback = m_pCCTBehaviorCallback;

	desc.nonWalkableMode = PxControllerNonWalkableMode::ePREVENT_CLIMBING;

	PxExtendedVec3 poolPos(m_vPoolingPosition.x, m_vPoolingPosition.y, m_vPoolingPosition.z);
	desc.position = poolPos;

	PxController* pCCT = m_pControllerManager->createController(desc);

	m_vPoolingPosition.z += pDesc->vExtens.z + 5;

	m_iPoolingRaw++;
	if (m_iPoolingRaw == 20)
	{
		m_vPoolingPosition.x += 10.f;
		m_iPoolingRaw = 0;
	}

	return pCCT;
}

PxController* CPhysics_CCTManager::MakeCapsuleController(PHYSICSCCT_DESC* pDesc)
{
	PxCapsuleControllerDesc desc{};
	desc.radius = pDesc->fRadius;
	desc.height = pDesc->fHeight;
	desc.material = m_pResourceManager->GetMaterial(&pDesc->tMaterial);

	desc.contactOffset = pDesc->fContactOffset;
	desc.stepOffset = pDesc->fStepOffset;
	//desc.slopeLimit = pDesc->fSlopeLimit;
	desc.slopeLimit = PxCos(PxDegToRad(89.f));

	desc.climbingMode = PxCapsuleClimbingMode::eCONSTRAINED;

	desc.reportCallback = m_pCCTHitReport;
	desc.behaviorCallback = m_pCCTBehaviorCallback;

	desc.nonWalkableMode = PxControllerNonWalkableMode::ePREVENT_CLIMBING;

	PxExtendedVec3 poolPos(m_vPoolingPosition.x, m_vPoolingPosition.y, m_vPoolingPosition.z);
	desc.position = poolPos;

	PxController* pCCT = m_pControllerManager->createController(desc);

	if (pCCT == nullptr)
	{
		MSG_BOX("Failed to Created : CCT in CCT Manager");
		return nullptr;
	}

	m_vPoolingPosition.z += pDesc->fRadius + 5;
	
	m_iPoolingRaw++;
	if (m_iPoolingRaw == 20)
	{
		m_vPoolingPosition.x += 10.f;
		m_iPoolingRaw = 0;
	}

	return pCCT;
}

CPhysics_CCTManager* CPhysics_CCTManager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, PxPhysics* pPhysics, PxScene* pScene, CPhysics_ResourceManager* pResourceManager)
{
	CPhysics_CCTManager* pInstance = new CPhysics_CCTManager(pDevice, pContext, pPhysics, pScene, pResourceManager);

	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CPhysics_CCTManager");
	}

	return pInstance;
}

void CPhysics_CCTManager::Free()
{
	Safe_Release(m_pCCTHitReport);
	Safe_Release(m_pCCTBehaviorCallback);
	Safe_Release(m_pCCTFilterCallback);

	Safe_Release(m_pResourceManager);

	ReleaseCCTManager();

	if (m_pControllerManager)
		PX_RELEASE(m_pControllerManager);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	Safe_Release(m_pGameInstance);

	Super::Free();
}
