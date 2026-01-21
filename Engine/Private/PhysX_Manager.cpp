#include "PhysX_Manager.h"

CPhysX_Manager::CPhysX_Manager()
{
}

HRESULT CPhysX_Manager::Initialize()
{
	// Foundation 객체 생성
	// 물리 시뮬레이션의 기본 세팅 및 초기화, 오류처리, 디버깅에 사용
	if (!(m_pFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_DefaultAllocatorCallback, m_DefaultErrorCallback)))
	{
		MSG_BOX("PxCreateFoundation, Failed!");
		return E_FAIL;
	}

	// physX Visual Debugger에서 사용하는 네트워크 전송 객체 생성
	physx::PxPvdTransport* pTransport = { nullptr };
	if (!(pTransport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10)))
	{
		MSG_BOX("PxDefaultPvdSocketTransportCreate, Failed!");
		return E_FAIL;
	}

	// pvd생성
	if (!(m_pPvd = PxCreatePvd(*m_pFoundation)))
	{
		MSG_BOX("PxCreatePvd, Failed!");
		return E_FAIL;
	}

	// Pvd를 사용해서 물리 시뮬레이션을 디버깅하고 모니터링할 수 있게 설정하는 코드
	m_pPvd->connect(*pTransport, physx::PxPvdInstrumentationFlag::eALL);

	// 길이 단위에 대한 공차값을 100으로 설정한다.
	// 이 값은 물리 시뮬레이션의 정확도와 성능에 영향을 미친다.
	m_ToleranceScale.length = 100;

	// 길이 단위 당 초당 이동 거리를 나타낸다.
	// 일반적으로는 중력가속도를 나타냄
	m_ToleranceScale.speed = 981;

	if (!(m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, m_ToleranceScale, true, m_pPvd)))
	{
		MSG_BOX("PxCreatePhysics, Failed!");
		return E_FAIL;
	}

	// 허용 오차값으로 sceneDesc를 초기화한다.
	// sceneDesc는 Scene을 만드는 데 필요한 구조체
	physx::PxSceneDesc sceneDesc(m_pPhysics->getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);

	// CPU 디스패처를 생성하는 코드
	// 디스패터는 물체 간 충도 검출 및 충돌 결과 처리를 수행한다.
	// 인자로 넘기는 2는 CPU 코어 갯수
	if(!(m_pDispatcher = physx::PxDefaultCpuDispatcherCreate(2)))
	{
		MSG_BOX("PxDefaultCpuDispatcherCreate, Failed!");
		return E_FAIL;
	}
	
	// physX 시뮬레이션에서 cpu연산을 처리하기위한 디스패처를 설정한다.
	// 이렇게 설정하면 시뮬레이션에서 물리연산은 지정된 디스패처에서 처리된다.
	sceneDesc.cpuDispatcher = m_pDispatcher;

	// PxDefaultSimulationFilterShader은 기본 필터링 콜백 함수
	// 이 필터링 함수는 물리 시뮬레이션에서 충돌과 관련된 처리를 수행하며,
	// 충돌 그룹 및 마스크를 설정하며 물체 간 상호작용을 제어한다.
	sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;

	// 시뮬레이션에서 사용할 새로운 장면을 생성하는 코드
	// Scene은 시뮬레이션에서 사용할 물리 객체 (바디, 조인트, 콜리전 모양)을 추가 할 수 있다.
	if (!(m_pScene = m_pPhysics->createScene(sceneDesc)))
	{
		MSG_BOX("createScene, Failed!");
		return E_FAIL;
	}

	if (physx::PxPvdSceneClient* pPvdClient = m_pScene->getScenePvdClient())
	{
		pPvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pPvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pPvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}

	pTransport->release();
	return S_OK;
}

CPhysX_Manager* CPhysX_Manager::Create()
{
	CPhysX_Manager* pInstance = new CPhysX_Manager();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("CPhysX_Manager::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPhysX_Manager::Free()
{
	if (m_pMaterial)
	{
		m_pMaterial->release();
		m_pMaterial = nullptr;
	}
	if (m_pScene)
	{
		m_pScene->release();
		m_pScene = nullptr;
	}
	if (m_pDispatcher)
	{
		m_pDispatcher->release();
		m_pDispatcher = nullptr;
	}
	if (m_pPhysics)
	{
		m_pPhysics->release();
		m_pPhysics = nullptr;
	}
	if (m_pPvd)
	{
		m_pPvd->release();
		m_pPvd = nullptr;
	}
	if (m_pFoundation)
	{
		m_pFoundation->release();
		m_pFoundation = nullptr;
	}
	Super::Free();
}
