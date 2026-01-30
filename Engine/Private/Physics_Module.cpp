#include "Engine_pch.h"

#include "GameInstance.h"

#include "Physics_Module.h"
#include "Physics_ResourceManager.h"
#include "Physics_Utils.h"
#include "Physics_ShapeFactory.h"
#include "Physics_CCTManager.h"
#include "Physics_ActorFactory.h"

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
	m_pFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_Allocator, m_ErrorCallback);

#ifdef _DEBUG
	//m_pPvd = PxCreatePvd(*m_pFoundation);
	//PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	////PxPvdTransport* transport = PxDefaultPvdFileTransportCreate(PVD_HOST, 5425, 10);
	////m_pPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);
	//m_pPvd->connect(*transport, PxPvdInstrumentationFlag::ePROFILE);
#endif // _DEBUG

	m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, PxTolerancesScale(), true, m_pPvd);

	PxInitExtensions(*m_pPhysics, m_pPvd);

	///////////////////////////
	/// Create CUDA context ///
	///////////////////////////
	{
		PxCudaContextManagerDesc cudaContextManagerDesc{};
		m_pCudaContextManager = PxCreateCudaContextManager(*m_pFoundation, cudaContextManagerDesc, PxGetProfilerCallback());
		if (m_pCudaContextManager)
		{
			if (!m_pCudaContextManager->contextIsValid())
				PX_RELEASE(m_pCudaContextManager);
		}
	}

	///////////////////
	/// Create PhyX ///
	///////////////////
	{
		PxSceneDesc sceneDesc(m_pPhysics->getTolerancesScale());
		sceneDesc.gravity = PxVec3(0.f, -9.81f, 0.f);
		m_pDispatcher = PxDefaultCpuDispatcherCreate(4);
		sceneDesc.cpuDispatcher = m_pDispatcher;
		sceneDesc.filterShader = PxDefaultSimulationFilterShader;

		if (m_pCudaContextManager)
		{
			sceneDesc.cudaContextManager = m_pCudaContextManager;
			sceneDesc.flags |= PxSceneFlag::eENABLE_GPU_DYNAMICS;
			sceneDesc.flags |= PxSceneFlag::eENABLE_PCM;
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
			sceneDesc.broadPhaseType = PxBroadPhaseType::eSAP;
		}

		m_pScene = m_pPhysics->createScene(sceneDesc);
	}

#ifdef _DEBUG
	//PxPvdSceneClient* pvdClient = m_pScene->getScenePvdClient();
	//if (pvdClient)
	//{
	//	pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
	//	pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
	//	pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	//}
#endif // _DEBUG

#ifdef _DEBUG
	m_pScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.f);
	m_pScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.f);
	m_pScene->setVisualizationParameter(PxVisualizationParameter::eACTOR_AXES, 2.f);
	m_pScene->getVisualizationCullingBox();
#endif // _DEBUG

	{
		m_pUtils = CPhysics_Utils::Create(m_pDevice, m_pDeviceContext, m_pPhysics, m_pScene);
		m_pResourceManager = CPhysics_ResourceManager::Create(m_pDevice, m_pDeviceContext, m_pPhysics, m_pScene);
		m_pShapeFactory = CPhysics_ShapeFactory::Create(m_pDevice, m_pDeviceContext, m_pPhysics, m_pScene, m_pResourceManager);
		m_pActorFactory = CPhysics_ActorFactory::Create(m_pDevice, m_pDeviceContext, m_pPhysics, m_pScene);
		m_pCCTManager = CPhysics_CCTManager::Create(m_pDevice, m_pDeviceContext, m_pPhysics, m_pScene, m_pResourceManager);
	}

	return S_OK;
}

void CPhysics_Module::StepPhysics(_float fTimeDelta)
{
	m_pScene->simulate(fTimeDelta);
	m_pScene->fetchResults(true);

#ifdef _DEBUG
	if (KEY_BUTTON_DOWN(DIK_F1))
		m_bEnabledDebugDraw = !m_bEnabledDebugDraw;
#endif // _DEBUG
}

void CPhysics_Module::AddActor(PxRigidActor* actor)
{
	m_pScene->addActor(*actor);
}

PxTransform CPhysics_Module::XMMatrixToPxTransform(Matrix mat)
{
	return m_pUtils->XMMatrixToPxTransform(mat);
}

Matrix CPhysics_Module::PxTransformToXMMatrix(PxTransform pxTransform)
{
	return m_pUtils->PxTransformToXMMatrix(pxTransform);
}

#ifdef _DEBUG
HRESULT CPhysics_Module::Render(PxRigidActor* pActor, XMVECTOR color)
{
	if (!m_bEnabledDebugDraw)
		return S_OK;

	return m_pUtils->Render(pActor, color);
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

vector<PxShape*> CPhysics_Module::GetShape(PHYSICSCOLLIDER_DESC* pDesc)
{
	return m_pShapeFactory->GetShape(pDesc);
}

vector<PxShape*> CPhysics_Module::GetMeshShape(PHYSICSCOLLIDER_DESC* pDesc)
{
	return m_pShapeFactory->GetMeshShape(pDesc);
}

PxRigidActor* CPhysics_Module::GetActor(PHYSICSRIGIDBODY_DESC* rigidBodyDesc, PHYSICSCOLLIDER_DESC* colliderDesc, vector<PxShape*>& shapes)
{
	return m_pActorFactory->GetActor(rigidBodyDesc, colliderDesc, shapes);
}

PxController* CPhysics_Module::GetController(PHYSICSCCT_DESC* pDesc)
{
	return m_pCCTManager->GetController(pDesc);
}

void CPhysics_Module::ClearPhysics()
{
	PX_RELEASE(m_pScene);
	PX_RELEASE(m_pDispatcher);
	PX_RELEASE(m_pPhysics);

#ifdef _DEBUG
	if (m_pPvd)
	{
		PxPvdTransport* transport = m_pPvd->getTransport();
		PX_RELEASE(m_pPvd);
		PX_RELEASE(transport);
	}

	PxCloseExtensions();
#endif // _DEBUG

	PX_RELEASE(m_pCudaContextManager);
	PX_RELEASE(m_pFoundation);
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
	__super::Free();

	Safe_Release(m_pGameInstance);

	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);

	Safe_Release(m_pCCTManager);
	Safe_Release(m_pActorFactory);
	Safe_Release(m_pShapeFactory);
	Safe_Release(m_pResourceManager);
	Safe_Release(m_pUtils);

	ClearPhysics();
}
