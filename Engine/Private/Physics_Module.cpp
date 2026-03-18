#include "Engine_pch.h"

#include "GameInstance.h"

#include "EngineConsole.h"

#include "Physics_Module.h"
#include "Physics_ResourceManager.h"
#include "Physics_Utils.h"
#include "Physics_ShapeFactory.h"
#include "Physics_CCTManager.h"
#include "Physics_ActorFactory.h"
#include "Physics_RagdollSystem.h"
#include "Physics_FilterEventCallback.h"

CPhysics_Module::CPhysics_Module(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
	, m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

HRESULT CPhysics_Module::Initialize()
{
	if (!(m_pFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_Allocator, m_ErrorCallback)))
	{
		MSG_BOX("Failed to created : PxFoundation");
		return E_FAIL;
	}

#ifdef _DEBUG
	//m_pPvd = PxCreatePvd(*m_pFoundation);
	////PxPvdTransport* transport = PxDefaultPvdFileTransportCreate("D:\\PVD_Record\\phyXDebug.pxd2");
	//PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	////m_pPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);
	//m_pPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);
#endif // _DEBUG

	if (!(m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, PxTolerancesScale(), true, m_pPvd)))
	{
		MSG_BOX("Failed to created : PxPhysics");
		return E_FAIL;
	}

	PxInitExtensions(*m_pPhysics, m_pPvd);

	///////////////////////////
	/// Create CUDA context ///
	///////////////////////////
	{
		//PxCudaContextManagerDesc cudaContextManagerDesc{};
		//m_pCudaContextManager = PxCreateCudaContextManager(*m_pFoundation, cudaContextManagerDesc, PxGetProfilerCallback());
		//if (m_pCudaContextManager)
		//{
		//	if (!m_pCudaContextManager->contextIsValid())
		//	{
		//		if (m_pCudaContextManager)
		//			PX_RELEASE(m_pCudaContextManager);
		//	}
		//}
	}

	///////////////////
	/// Create PhyX ///
	///////////////////
	{
		PxSceneDesc sceneDesc(m_pPhysics->getTolerancesScale());
		sceneDesc.gravity = PxVec3(0.f, -9.81f, 0.f);
		sceneDesc.flags |= PxSceneFlag::eENABLE_CCD;
		sceneDesc.flags |= PxSceneFlag::eENABLE_PCM;
		sceneDesc.flags |= PxSceneFlag::eENABLE_ACTIVE_ACTORS;
		//sceneDesc.flags |= PxSceneFlag::eREQUIRE_RW_LOCK;

		PxU32 numCores = PxThread::getNbPhysicalCores();
		m_pDispatcher = PxDefaultCpuDispatcherCreate(numCores == 0 ? 0 : numCores - 1);
		sceneDesc.cpuDispatcher = m_pDispatcher;

		if (m_pCudaContextManager)
		{
			sceneDesc.cudaContextManager = m_pCudaContextManager;
			sceneDesc.flags |= PxSceneFlag::eENABLE_GPU_DYNAMICS;
			//sceneDesc.flags |= PxSceneFlag::eENABLE_STABILIZATION; // 물체가 겹실 시 밀어내는 연산 가속
			sceneDesc.broadPhaseType = PxBroadPhaseType::eGPU;
			sceneDesc.gpuMaxNumPartitions = 8;

			// gpu 메모리 // (기본 16MB) // 셋팅 64MB
			sceneDesc.gpuDynamicsConfig.heapCapacity = 64 * 1024 * 1024;
			//sceneDesc.gpuDynamicsConfig.tempBufferCapacity = 64 * 1024 * 1024;

			//처리할 오브젝트가 많다
			//sceneDesc.gpuDynamicsConfig.foundLostPairsCapacity = 16 * 1024;
			//sceneDesc.gpuDynamicsConfig.foundLostAggregatePairsCapacity = 1024;
			//sceneDesc.gpuDynamicsConfig.totalAggregatePairsCapacity = 1024;

			// 충돌 지점 수 (기본 50만) // 셋팅 200만
			//sceneDesc.gpuDynamicsConfig.maxRigidContactCount = 1024 * 1024 * 2;

			// 충돌 면 수 // 16만
			//sceneDesc.gpuDynamicsConfig.maxRigidPatchCount = 160 * 1024;

			// particle
			//sceneDesc.gpuDynamicsConfig.maxSoftBodyContacts = 1024 * 1024;
			//sceneDesc.gpuDynamicsConfig.maxParticleContacts = 1024 * 1024;
		}
		else
		{
			sceneDesc.cudaContextManager = NULL;
			sceneDesc.flags &= ~PxSceneFlag::eENABLE_GPU_DYNAMICS;
			sceneDesc.broadPhaseType = PxBroadPhaseType::ePABP;

#ifdef _DEBUG
			sceneDesc.staticNbObjectsPerNode = 12;
			sceneDesc.dynamicNbObjectsPerNode = 12;
			sceneDesc.dynamicTreeRebuildRateHint = 300;
#endif // _DEBUG
		}

		//////////////////////////////////
		/// Broad Phase Filtering Mode ///
		//////////////////////////////////
		{
			//sceneDesc.kineKineFilteringMode; // eDEFAULT = eSUPPRESS
			sceneDesc.kineKineFilteringMode = PxPairFilteringMode::eKEEP;
			//sceneDesc.staticKineFilteringMode; // eDEFAULT = eSUPPRESS
			sceneDesc.staticKineFilteringMode = PxPairFilteringMode::eKEEP;
		}

		/////////////////////
		/// Filter Shader ///
		/////////////////////
		{
			// Default Setting
			//sceneDesc.filterShader = PxDefaultSimulationFilterShader;
			sceneDesc.filterShader = FilterShader;

			m_pFilterEventCallback = CPhysics_FilterEventCallback::Create();
			sceneDesc.simulationEventCallback = m_pFilterEventCallback;
		}

		if (!(m_pScene = m_pPhysics->createScene(sceneDesc)))
		{
			MSG_BOX("Failed to created : PxScene");
			return E_FAIL;
		}

		m_pScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 0.f);
		m_pScene->setVisualizationCullingBox(PxBounds3(PxVec3(0.f, 0.f, 0.f), PxVec3(0.f, 0.f, 0.f)));
	}

#ifdef _DEBUG
	//PxPvdSceneClient* pvdClient = m_pScene->getScenePvdClient();
	//if (pvdClient)
	//{
	//	//pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
	//	//pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
	//	pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	//}
#endif // _DEBUG

#ifdef _DEBUG
	//m_pScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.f);
	//m_pScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.f);
	//m_pScene->setVisualizationParameter(PxVisualizationParameter::eACTOR_AXES, 2.f);
	//m_pScene->getVisualizationCullingBox();
#endif // _DEBUG

	{
		if (!(m_pUtils = CPhysics_Utils::Create(m_pDevice, m_pDeviceContext, m_pPhysics, m_pScene)))
		{
			MSG_BOX("Failed to created : physics utils");
			return E_FAIL;
		}

		if (!(m_pResourceManager = CPhysics_ResourceManager::Create(m_pDevice, m_pDeviceContext, m_pPhysics, m_pScene)))
		{
			MSG_BOX("Failed to created : physics resource manager");
			return E_FAIL;
		}

		if (!(m_pShapeFactory = CPhysics_ShapeFactory::Create(m_pDevice, m_pDeviceContext, m_pPhysics, m_pScene, m_pResourceManager)))
		{
			MSG_BOX("Failed to created : physics shape factory");
			return E_FAIL;
		}

		if (!(m_pActorFactory = CPhysics_ActorFactory::Create(m_pDevice, m_pDeviceContext, m_pPhysics, m_pScene)))
		{
			MSG_BOX("Failed to created : physics actor factory");
			return E_FAIL;
		}

		if (!(m_pCCTManager = CPhysics_CCTManager::Create(m_pDevice, m_pDeviceContext, m_pPhysics, m_pScene, m_pResourceManager)))
		{
			MSG_BOX("Failed to created : physics cct manager");
			return E_FAIL;
		}

		if (!(m_pRagdollSystem = CPhysics_RagdollSystem::Create(m_pDevice, m_pDeviceContext, m_pPhysics, m_pScene)))
		{
			MSG_BOX("Failed to created : physics ragdoll system");
			return E_FAIL;
		}
	}

	return S_OK;
}

void CPhysics_Module::StepPhysics(_float fTimeDelta)
{
	m_pScene->lockWrite();

	m_pScene->simulate(std::clamp(fTimeDelta, 1.f / 120.f, 1.f / 30.f));
	m_pScene->fetchResults(true);

	m_pScene->unlockWrite();

	m_pCCTManager->GetPhysicsCCTManager()->computeInteractions(fTimeDelta);

#ifdef _DEBUG
	if (KEY_BUTTON_DOWN(DIK_F1))
		m_bEnabledDebugDraw = !m_bEnabledDebugDraw;
	if (KEY_BUTTON_DOWN(DIK_F2))
		m_pUtils->SetMeshDebugState();
	if (KEY_BUTTON_DOWN(DIK_F3))
		g_ToolDebugRender = !g_ToolDebugRender;
#endif // _DEBUG
}

void CPhysics_Module::AddActor(PxRigidActor* actor)
{
	m_pScene->addActor(*actor);
}

void CPhysics_Module::AddRagdoll(PxArticulationReducedCoordinate* pArticulation)
{
	m_pScene->addArticulation(*pArticulation);
}

void CPhysics_Module::RemoveRagdoll(PxArticulationReducedCoordinate* pArticulation)
{
	m_pScene->removeArticulation(*pArticulation);
}

PxTransform CPhysics_Module::XMMatrixToPxTransform(Matrix mat)
{
	return m_pUtils->XMMatrixToPxTransform(mat);
}

Matrix CPhysics_Module::PxTransformToXMMatrix(PxTransform pxTransform)
{
	return m_pUtils->PxTransformToXMMatrix(pxTransform);
}

PxQuat CPhysics_Module::GetPureRotation(Matrix mat)
{
	return m_pUtils->GetPureRotation(mat);
}

PxVec3 CPhysics_Module::GetPureScale(Matrix mat)
{
	return m_pUtils->GetPureScale(mat);
}

_bool CPhysics_Module::Execute_Overlap(PxGeometry& shape, PxTransform& transform, OUT PxOverlapBuffer& hit, PxQueryFilterData& filterData, PxQueryFilterCallback* filterCallback)
{
	return m_pUtils->Execute_Overlap(shape, transform, hit, filterData, filterCallback);
}

CPhysics_QueryFilterCallback* CPhysics_Module::GetQueryFilterCallback()
{
	return m_pUtils->GetQueryFilterCallback();
}

CPhysics_QueryFilterCallback_Gun* CPhysics_Module::GetQueryFilterCallback_Gun()
{
	return m_pUtils->GetQueryFilterCallback_Gun();
}

CPhysics_QueryFilterCallback_SpringArm* CPhysics_Module::GetQueryFilterCallback_SpringArm()
{
	return m_pUtils->GetQueryFilterCallback_SpringArm();
}

#ifdef _DEBUG
HRESULT CPhysics_Module::Render(PxRigidActor* pActor, XMVECTOR color)
{
	if (!g_ToolDebugRender && !m_bEnabledDebugDraw)
		return S_OK;

	return m_pUtils->Render(pActor, color);
}
HRESULT CPhysics_Module::Render(const PxGeometry& geom, const PxTransform& transform, XMVECTOR color)
{
	if (!g_ToolDebugRender && !m_bEnabledDebugDraw)
		return S_OK;

	return m_pUtils->Render(geom, transform, color);
}
#endif // _DEBUG

void CPhysics_Module::SerializeStaticMesh(std::filesystem::path path, vector<PxTriangleMesh*> meshes)
{
	m_pResourceManager->SerializeStaticMesh(path, meshes);
}

PxCollection* CPhysics_Module::DeserializeStaticMesh(std::filesystem::path path)
{
	return m_pResourceManager->DeserializeStaticMesh(path);
}

void CPhysics_Module::SerializeConvexMesh(std::filesystem::path path, vector<PxConvexMesh*> meshes)
{
}

PxCollection* CPhysics_Module::SerializeConvexMesh(std::filesystem::path path)
{
	return nullptr;
}

void CPhysics_Module::RegisterPhysicsMesh(_uint levelIndex, _wstring prototypeTag)
{
	m_pResourceManager->RegisterPhysicsMesh(levelIndex, prototypeTag);
}

PxMaterial* CPhysics_Module::GetPhysicsMaterial(EPhysicsMaterial eMaterial)
{
	return m_pResourceManager->GetMaterial(eMaterial);
}

vector<PxShape*> CPhysics_Module::GetShape(PHYSICSCOLLIDER_DESC* pDesc)
{
	return m_pShapeFactory->GetShape(pDesc);
}

vector<PxShape*> CPhysics_Module::GetMeshShape(PHYSICSCOLLIDER_DESC* pDesc)
{
	return m_pShapeFactory->GetMeshShape(pDesc);
}

vector<PxShape*> CPhysics_Module::CopyShapes(vector<PxShape*>& shapes)
{
	return m_pShapeFactory->CopyShapes(shapes);
}

vector<PxRigidActor*> CPhysics_Module::GetActor(PHYSICSRIGIDBODY_DESC* rigidBodyDesc, PHYSICSCOLLIDER_DESC* colliderDesc, vector<PxShape*>& shapes)
{
	return m_pActorFactory->GetActor(rigidBodyDesc, colliderDesc, shapes);
}

RAGDOLLELEMENTS CPhysics_Module::CreateRagdoll(array<RAGDOLLBONEDESC, RAGDOLLJOINT::END> arrRagdollBoneDesc)
{
	return m_pRagdollSystem->CreateRagdoll(arrRagdollBoneDesc);
}

PxController* CPhysics_Module::GetController(PHYSICSCCT_DESC* pDesc)
{
	return m_pCCTManager->GetController(pDesc);
}

CPhysics_CCTFilterCallback* CPhysics_Module::GetCCTFilterCallback()
{
	return m_pCCTManager->GetCCTFilterCallback();
}

_bool CPhysics_Module::CheckRagdollState(int64 objID)
{
	return m_pRagdollSystem->CheckRagdollState(objID);
}

_bool CPhysics_Module::CheckRagDollState_Processing(int64 objID)
{
	return m_pRagdollSystem->CheckRagDollState_Processing(objID);
}

void CPhysics_Module::RagdollRegister(CGameObject* obj)
{
	m_pRagdollSystem->Register(obj);
}

void CPhysics_Module::RagdollUnregister(int64 objID)
{
	m_pRagdollSystem->Unregister(objID);
}

void CPhysics_Module::RagdollRequestStart(uint64 objID)
{
	m_pRagdollSystem->RequestStart(objID);
}

void CPhysics_Module::RagdollSyncStates(uint64 objID, vector<class CChannel*>& vecChannels)
{
	m_pRagdollSystem->SyncStates(objID, vecChannels);
}

void CPhysics_Module::RagdollFinish(uint64 objID)
{
	m_pRagdollSystem->Finish(objID);
}

PxFilterFlags CPhysics_Module::FilterShader(
	PxFilterObjectAttributes attributes0, PxFilterData filterData0,
	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
	if ((filterData0.word0 & PHYSICSFILTERGROUP::RAGDOLL)
		|| (filterData1.word0 & PHYSICSFILTERGROUP::RAGDOLL))
	{
		_bool isMap0 = filterData0.word0 & PHYSICSFILTERGROUP::MAP;
		_bool isMap1 = filterData1.word0 & PHYSICSFILTERGROUP::MAP;

		if (!isMap0 && !isMap1)
			return PxFilterFlag::eSUPPRESS;
	}

	_bool bPass0 = ((filterData0.word0 & filterData1.word1) != 0);
	_bool bPass1 = ((filterData1.word0 & filterData0.word1) != 0);
	// Pair가 아닐 때
	if ((bPass0 && bPass1) == false)
		return PxFilterFlag::eSUPPRESS;

	if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
	{
		pairFlags = PxPairFlag::eTRIGGER_DEFAULT
			| PxPairFlag::eNOTIFY_TOUCH_FOUND
			| PxPairFlag::eNOTIFY_TOUCH_LOST;

		return PxFilterFlag::eDEFAULT;
	}

	if (PxFilterObjectIsKinematic(attributes0) || PxFilterObjectIsKinematic(attributes1))
	{
		pairFlags = PxPairFlag::eDETECT_DISCRETE_CONTACT
			| PxPairFlag::eNOTIFY_TOUCH_FOUND
			| PxPairFlag::eNOTIFY_TOUCH_LOST
			| PxPairFlag::eNOTIFY_TOUCH_PERSISTS;

		return PxFilterFlag::eDEFAULT;
	}

	pairFlags = PxPairFlag::eCONTACT_DEFAULT
		| PxPairFlag::eNOTIFY_TOUCH_FOUND
		| PxPairFlag::eNOTIFY_TOUCH_LOST
		| PxPairFlag::eNOTIFY_TOUCH_PERSISTS;

	// 02/21
	// HitPoint 추출을 위한 Flag 설정
	// Enter에서만 추출하기 위해 사용
	// CPhysics_FilterEventCallback::onContact에서 Flag 체크후 GAMEOBJECTINFO에 넣는중
	if (PHYSICSFILTERGROUP::IsAttackPair(filterData0.word0, filterData1.word0))
		pairFlags |= PxPairFlag::eNOTIFY_CONTACT_POINTS;

	return PxFilterFlag::eDEFAULT;
}

void CPhysics_Module::Check_Leak()
{
	_uint staticActorCount = { 0 };
	_uint dynamicActorCount = { 0 };

	if (m_pScene)
	{
		staticActorCount = m_pScene->getNbActors(PxActorTypeFlag::eRIGID_STATIC);
		dynamicActorCount = m_pScene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC);
	}

	_uint triMeshCount = m_pPhysics->getNbTriangleMeshes();
	_uint convexMeshCount = m_pPhysics->getNbConvexMeshes();

	wstring logMsg = L" [PhysX Leak Check] \n";
	logMsg += L"Static Actors: " + std::to_wstring(staticActorCount) + L"\n";
	logMsg += L"Dynamic Actors: " + std::to_wstring(dynamicActorCount) + L"\n";

	logMsg += L"Tri Meshes: " + std::to_wstring(triMeshCount) + L"\n";
	logMsg += L"Convex Meshes: " + std::to_wstring(convexMeshCount) + L"\n";

	CLOG_INFO(logMsg);

	OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
	OutputDebugStringW(L"                                                                            PhysX Leak Checker \r ");
	OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");

	OutputDebugStringW(logMsg.c_str());

	OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
	OutputDebugStringW(L"                                                                          PhysX Leak Checker END \r ");
	OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
}

void CPhysics_Module::GetActiveActors()
{
	PxU32 nbActiveActors;
	PxActor** activeActors = m_pScene->getActiveActors(nbActiveActors);
	for (PxU32 i = 0; i < nbActiveActors; ++i)
	{
	}
}

void CPhysics_Module::Overlap_EventCallback(CGameObject* pOwner, const PxVec3& vOverlapPoint, PxOverlapHit* pOverlapHit, PxPairFlag::Enum event, DTO::HITBOX_DESC* hitboxDesc)
{
	m_pFilterEventCallback->ProcessOverlap(pOwner, vOverlapPoint, pOverlapHit, event, hitboxDesc);
}

void CPhysics_Module::Raycast_EventCallback(CGameObject* pOwner, PxRaycastBuffer* pRaycastHitBuffer, CPhysicsAttackRaycast::ATTACKRAYCASTDESC* raycastDesc)
{
	m_pFilterEventCallback->ProcessRaycast(pOwner, pRaycastHitBuffer, raycastDesc);
}

_bool CPhysics_Module::RayCast(Vec3 vWorldPos, Vec3 vDir, _float fMaxDist, CPhysics_QueryFilterCallback* pFilterCall)
{
	return m_pUtils->RayCast(vWorldPos, vDir, fMaxDist, pFilterCall);
}

void CPhysics_Module::ClearPhysics()
{
	if (m_pScene)
		PX_RELEASE(m_pScene);

	Check_Leak();

	if (m_pFilterEventCallback)
		Safe_Release(m_pFilterEventCallback);

	if (m_pDispatcher)
		PX_RELEASE(m_pDispatcher);

	if (m_pPhysics)
		PX_RELEASE(m_pPhysics);

#ifdef _DEBUG
	if (m_pPvd)
	{
		PxPvdTransport* transport = m_pPvd->getTransport();
		PX_RELEASE(m_pPvd);

		if (transport)
			PX_RELEASE(transport);
	}
#endif // _DEBUG

	PxCloseExtensions();

	if (m_pCudaContextManager)
		PX_RELEASE(m_pCudaContextManager);

	if (m_pFoundation)
		PX_RELEASE(m_pFoundation);
}

void CPhysics_Module::FlushScene()
{
	m_pScene->flushQueryUpdates();
	//m_pScene->flushSimulation();
	m_pScene->flushUpdates();
}

void CPhysics_Module::RemoveActor(PxRigidActor* actor)
{
	m_pScene->removeActor(*actor);
}

void CPhysics_Module::ResetActorFilter(PxRigidActor* actor)
{
	m_pScene->resetFiltering(*actor);
}

PxControllerManager* CPhysics_Module::GetPhysicsCCTManager()
{
	return m_pCCTManager->GetPhysicsCCTManager();
}

CPhysics_Module* CPhysics_Module::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPhysics_Module* pInstance = new CPhysics_Module(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CPhysics_Module");
	}

	return pInstance;
}

void CPhysics_Module::Free()
{
	Safe_Release(m_pRagdollSystem);
	Safe_Release(m_pCCTManager);
	Safe_Release(m_pActorFactory);
	Safe_Release(m_pShapeFactory);
	Safe_Release(m_pUtils);

	PX_RELEASE(m_pScene);
	Safe_Release(m_pResourceManager);

	ClearPhysics();

	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);

	Safe_Release(m_pGameInstance);

	Super::Free();
}
