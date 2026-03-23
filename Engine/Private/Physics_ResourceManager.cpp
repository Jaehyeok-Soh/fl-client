#include "Engine_pch.h"

#include "GameInstance.h"
#include "Physics_ResourceManager.h"

#include "Model.h"
#include "Mesh.h"

CPhysics_ResourceManager::CPhysics_ResourceManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, PxPhysics* pPhysics, PxScene* pScene)
	: m_pGameInstance{ CGameInstance::GetInstance() },
	m_pDevice{ pDevice },
	m_pContext{ pContext },
	m_pPhysics{ pPhysics },
	m_pScene{ pScene },
	m_Materials{ nullptr }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CPhysics_ResourceManager::Initialize()
{
	InitMaterials();

	return S_OK;
}

void CPhysics_ResourceManager::InitMaterials()
{
	m_Materials[ENUM_TO_UINT(EPhysicsMaterial::PLAYER)] = m_pPhysics->createMaterial(0.f, 0.f, 0.f);

	m_Materials[ENUM_TO_UINT(EPhysicsMaterial::DEFAULT)] = m_pPhysics->createMaterial(0.6f, 0.6f, 0.5f);

	m_Materials[ENUM_TO_UINT(EPhysicsMaterial::CONCRETE)] = m_pPhysics->createMaterial(0.8f, 0.8f, 0.1f);

	m_Materials[ENUM_TO_UINT(EPhysicsMaterial::ICE)] = m_pPhysics->createMaterial(0.1f, 0.05f, 0.1f);
	m_Materials[ENUM_TO_UINT(EPhysicsMaterial::ICE)]->setFrictionCombineMode(PxCombineMode::eMIN);

	m_Materials[ENUM_TO_UINT(EPhysicsMaterial::WOOD)] = m_pPhysics->createMaterial(0.5f, 0.5f, 0.3f);

	m_Materials[ENUM_TO_UINT(EPhysicsMaterial::RUBBER)] = m_pPhysics->createMaterial(1.f, 0.8f, 0.9f);
	m_Materials[ENUM_TO_UINT(EPhysicsMaterial::RUBBER)]->setRestitutionCombineMode(PxCombineMode::eMAX);

	m_Materials[ENUM_TO_UINT(EPhysicsMaterial::METAL)] = m_pPhysics->createMaterial(0.3f, 0.3f, 0.2f);

	m_Materials[ENUM_TO_UINT(EPhysicsMaterial::GLASS)] = m_pPhysics->createMaterial(0.4f, 0.4f, 0.3f);

	m_Materials[ENUM_TO_UINT(EPhysicsMaterial::NONE)] = m_pPhysics->createMaterial(0.f, 0.f, 0.f);
}

PxMaterial* CPhysics_ResourceManager::GetMaterial(PHYSICSMATERIAL_DESC* pDesc)
{
	if (pDesc->eMaterial == EPhysicsMaterial::CUSTOM)
		return CreateMaterial(pDesc);

	return m_Materials[ENUM_TO_UINT(pDesc->eMaterial)];
}

PxMaterial* CPhysics_ResourceManager::GetMaterial(EPhysicsMaterial eMaterial)
{
	return m_Materials[ENUM_TO_UINT(eMaterial)];
}

PxMaterial* CPhysics_ResourceManager::CreateMaterial(PHYSICSMATERIAL_DESC* pDesc)
{
	return m_pPhysics->createMaterial(pDesc->fStaticFriction, pDesc->fDynamicFriction, pDesc->fRestitution);
}

vector<PxTriangleMesh*> CPhysics_ResourceManager::GetTriangleMeshes(PHYSICSCOLLIDER_DESC* pDesc)
{
	map<_wstring, vector<PxTriangleMesh*>>::iterator iter;

	_bool bFind = { false };
	if (pDesc->wstrModelPrototypeTag != L"")
	{
		iter = m_TriangleMeshes.find(pDesc->wstrModelPrototypeTag);

		if (iter != m_TriangleMeshes.end())
		{
			bFind = true;
			return iter->second;
		}
	}

	if (pDesc->wstrFilePath != L"")
	{
		iter = m_TriangleMeshes.find(pDesc->wstrFilePath);

		if (iter != m_TriangleMeshes.end())
		{
			bFind = true;
			return iter->second;
		}
	}

	vector<PxTriangleMesh*> result;

	// make triangle mesh
	if (pDesc->wstrModelPrototypeTag != L"")
	{
		CModel* model = static_cast<CModel*>(m_pGameInstance->Find_Prototype(m_MeshResourceTag[pDesc->wstrModelPrototypeTag], pDesc->wstrModelPrototypeTag));
		result = CreateTriangleMeshes(model);
		m_TriangleMeshes.emplace(pDesc->wstrModelPrototypeTag, result);
	}

	return result;
}

vector<PxConvexMesh*> CPhysics_ResourceManager::GetConvexMeshes(PHYSICSCOLLIDER_DESC* pDesc)
{
	map<_wstring, vector<PxConvexMesh*>>::iterator iter;

	_bool bFind = { false };
	if (pDesc->wstrModelPrototypeTag != L"")
	{
		iter = m_ConvexMeshes.find(pDesc->wstrModelPrototypeTag);

		if (iter != m_ConvexMeshes.end())
		{
			bFind = true;
			return iter->second;
		}
	}

	if (pDesc->wstrFilePath != L"")
	{
		iter = m_ConvexMeshes.find(pDesc->wstrFilePath);

		if (iter != m_ConvexMeshes.end())
		{
			bFind = true;
			return iter->second;
		}
	}

	vector<PxConvexMesh*> result;

	// make triangle mesh
	if (pDesc->wstrModelPrototypeTag != L"")
	{
		CModel* model = static_cast<CModel*>(m_pGameInstance->Find_Prototype(m_MeshResourceTag[pDesc->wstrModelPrototypeTag], pDesc->wstrModelPrototypeTag));
		result = CreateConvexMeshes(model);
		m_ConvexMeshes.emplace(pDesc->wstrModelPrototypeTag, result);
	}

	return result;
}

void CPhysics_ResourceManager::RegisterPhysicsMesh(_uint levelIndex, _wstring prototypeTag)
{
	if (m_MeshResourceTag.find(prototypeTag) != m_MeshResourceTag.end())
		m_MeshResourceTag[prototypeTag] = levelIndex;

	m_MeshResourceTag.emplace(prototypeTag, levelIndex);
}

vector<CPhysics_ResourceManager::HEIGHTFIELD_INFO> CPhysics_ResourceManager::GetHeightFields(PHYSICSCOLLIDER_DESC* pDesc)
{
	map<_wstring, vector<HEIGHTFIELD_INFO>>::iterator iter;

	_bool bFind = { false };
	if (pDesc->wstrModelPrototypeTag != L"")
	{
		iter = m_HeightFields.find(pDesc->wstrModelPrototypeTag);

		if (iter != m_HeightFields.end())
		{
			bFind = true;
			return iter->second;
		}
	}

	if (pDesc->wstrFilePath != L"")
	{
		iter = m_HeightFields.find(pDesc->wstrFilePath);

		if (iter != m_HeightFields.end())
		{
			bFind = true;
			return iter->second;
		}
	}

	vector<HEIGHTFIELD_INFO> result;

	// make height field
	if (pDesc->wstrModelPrototypeTag != L"")
	{
		CModel* model = static_cast<CModel*>(m_pGameInstance->Find_Prototype(m_MeshResourceTag[pDesc->wstrModelPrototypeTag], pDesc->wstrModelPrototypeTag));
		result = CreateHeightFields(model);
		m_HeightFields.emplace(pDesc->wstrModelPrototypeTag, result);
	}

	return result;
}

vector<PxTriangleMesh*> CPhysics_ResourceManager::CreateTriangleMeshes(CModel* model, _bool skipMeshCleanup, _bool skipEdgeData, _bool inserted, const PxU32 numTrisPerLeaf)
{
	// [옵션 1: 정석 (오프라인 쿠킹용)]
	// 런타임(게임 실행) 속도를 최우선으로 합니다. 
	// 메쉬를 정리(Clean)하고, '활성 엣지(Active Edges)'를 미리 계산합니다.
	// 결과물은 스트림(파일 등)에 저장합니다.
	// -> 이것이 기본(Default) 설정이며, 미리 구워두는 '오프라인 쿠킹'에 적합합니다.
	// (참고: 리프 노드당 삼각형 수 4개 = 트리가 깊어지지만 충돌 검사는 빠름)
	// ****** createBV34TriangleMesh(..., 4); ******

	// [옵션 2: 용량 절약]
	// 메쉬 용량(메모리 크기) 최적화를 최우선으로 합니다.
	// 메쉬를 정리하고, 활성 엣지를 미리 계산합니다. 스트림에 저장합니다.
	// (참고: 리프 노드당 삼각형 수 15개 = 트리가 얕아져서 용량은 줄지만, 검사는 약간 느려질 수 있음)
	// ****** createBV34TriangleMesh(..., 15); ******

	// [옵션 3: 로딩 속도 중시 (런타임 쿠킹용)]
	// 쿠킹(생성) 속도를 최우선으로 합니다.
	// 메쉬 정리(Cleanup) 과정을 건너뛰지만, 활성 엣지는 미리 계산합니다. 
	// 스트림 저장 없이 PxPhysics 엔진에 바로 집어넣습니다.
	// -> 이 설정은 게임 실행 중 실시간으로 굽는 '런타임 쿠킹'에 적합합니다.
	// -> 다만, 리프당 삼각형 수를 많이(15개) 잡으면 시뮬레이션이나 레이캐스트 속도가 떨어질 수 있습니다.
	// -> 메쉬 정리를 건너뛰었으므로, 데이터가 유효한지 확인하기 위해 Debug/Checked 빌드에서는 유효성 검사를 수행합니다.
	// ****** createBV34TriangleMesh(..., true, false, true, 15); ******

	// [옵션 4: 초고속 생성 (품질 포기)]
	// 쿠킹 속도를 극한으로 올립니다.
	// 메쉬 정리를 건너뛰고, 활성 엣지 계산조차 하지 않습니다. PxPhysics에 바로 넣습니다.
	// -> 런타임 쿠킹 중 가장 빠른 방법입니다.
	// -> 하지만 모든 엣지가 '활성(Active)' 상태로 간주되므로, 게임 성능이 더 떨어질 수 있고 물리 반응이 이상해질 수 있습니다.
	// (예: 평평한 바닥인데 이음새에서 덜컹거리거나 걸리는 현상 발생 가능)
	// ****** createBV34TriangleMesh(..., false, true, true, 15); ******

	vector<PxTriangleMesh*> result{};

	if (model == nullptr)
		return result;

	vector<CMesh*> meshes;
	_uint NumMesh = model->Get_MeshCount();

	meshes.reserve(NumMesh);
	for (_uint i = 0; i < NumMesh; i++)
		meshes.push_back(model->Get_Mesh(i));

	for (auto& mesh : meshes)
	{
		if (mesh->Get_VerticesCount() <= 0)
			continue;

		result.push_back(CreateTriangleMesh(mesh, skipMeshCleanup, skipEdgeData, inserted, numTrisPerLeaf));
	}

	return result;
}

vector<PxTriangleMesh*> CPhysics_ResourceManager::CreateTriangleMeshes(std::filesystem::path path, _bool skipMeshCleanup, _bool skipEdgeData, _bool inserted, const PxU32 numTrisPerLeaf)
{
	return vector<PxTriangleMesh*>();
}

PxTriangleMesh* CPhysics_ResourceManager::CreateTriangleMesh(CMesh* mesh, _bool skipMeshCleanup, _bool skipEdgeData, _bool inserted, const PxU32 numTrisPerLeaf)
{
	PxU32 numVertices = {};
	vector<PxVec3> pxVertices = {};
	PxU32 numTriangles = {};
	const _uint* pxIndices = {};

	const Vec3* vertices = mesh->Get_VertexPositionData();
	numVertices = mesh->Get_VerticesCount();
	{
		pxVertices.reserve(numVertices);

		for (_uint i = 0; i < numVertices; i++)
			pxVertices.push_back(PxVec3(vertices[i].x, vertices[i].y, vertices[i].z));
	}

	_uint numIndices = mesh->Get_IndicesCount();
	{
		pxIndices = mesh->Get_IndicesData();
	}

	return CreateBV34TriangleMesh(numVertices, pxVertices.data(), numIndices / 3, pxIndices, skipMeshCleanup, skipEdgeData, inserted, numTrisPerLeaf);
}

vector<PxConvexMesh*> CPhysics_ResourceManager::CreateConvexMeshes(CModel* model, _bool directionInsertion, _uint gaussMapLimit)
{
	// 기본 볼록 메쉬 생성 방식입니다. 데이터를 스트림(메모리/파일)으로 직렬화합니다.
	// 오프라인 쿠킹(미리 구워서 파일로 저장해두는 방식)에 유용합니다.
	// ****** createRandomConvex<PxConvexMeshCookingType::eQUICKHULL, false, 16>(numVerts, vertices); ******

	// 추가적인 가우스 맵(Gauss map) 데이터 없이 기본 볼록 메쉬를 생성합니다.
	// (보통 정점 제한이 늘어나면 최적화 데이터를 뺄 때가 있습니다)
	// ****** createRandomConvex<PxConvexMeshCookingType::eQUICKHULL, false, 256>(numVerts, vertices); ******

	// 생성된 메쉬를 PhysX 시스템에 바로 삽입(Insert)합니다.
	// 런타임 쿠킹(게임 실행 중에 실시간으로 굽기)에 유용합니다.
	// ****** createRandomConvex<PxConvexMeshCookingType::eQUICKHULL, true, 16>(numVerts, vertices); ******

	// 가우스 맵 데이터 없이 메쉬를 PhysX 시스템에 바로 삽입합니다.
	// 런타임 쿠킹에 유용합니다.
	// ****** createRandomConvex<PxConvexMeshCookingType::eQUICKHULL, true, 256>(numVerts, vertices); ******

	// 가우스 맵
	// Convex 메쉬의 표면 판정을 빠르게 하기 위해 미리 계산해두는 수학적 최적화 데이터
	// 정점 개수가 적을 수록 성능 좋아짐

	vector<PxConvexMesh*> result{};

	if (model == nullptr)
		return result;

	vector<CMesh*> meshes;
	_uint NumMesh = model->Get_MeshCount();

	meshes.reserve(NumMesh);
	for (_uint i = 0; i < NumMesh; i++)
		meshes.push_back(model->Get_Mesh(i));

	for (auto& mesh : meshes)
	{
		if (mesh->Get_VerticesCount() <= 0)
			continue;

		result.push_back(CreateConvexMesh(mesh, PxConvexMeshCookingType::eQUICKHULL, directionInsertion, (PxU32)gaussMapLimit));
	}

	return result;
}

PxConvexMesh* CPhysics_ResourceManager::CreateConvexMesh(CMesh* mesh, PxConvexMeshCookingType::Enum convexMeshCookingType, _bool directionInsertion, PxU32 gaussMapLimit)
{
	PxConvexMesh* convex = NULL;

	PxTolerancesScale tolerances;
	PxCookingParams params(tolerances);

	params.convexMeshCookingType = convexMeshCookingType;

	params.gaussMapLimit = gaussMapLimit;

	params.meshPreprocessParams |= PxMeshPreprocessingFlag::eWELD_VERTICES;
	params.meshWeldTolerance = physx::PxReal(0.01);

	PxU32 numVertices = {};
	vector<PxVec3> pxVertices = {};

	const Vec3* vertices = mesh->Get_VertexPositionData();
	numVertices = mesh->Get_VerticesCount();

	{
		pxVertices.reserve(numVertices);

		for (_uint i = 0; i < numVertices; i++)
			pxVertices.push_back(PxVec3(vertices[i].x, vertices[i].y, vertices[i].z));
	}

	PxConvexMeshDesc desc{};
	desc.points.data = pxVertices.data();
	desc.points.count = numVertices;
	desc.points.stride = sizeof(PxVec3);
	desc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

	PxU32 meshSize = 0;

	if (directionInsertion)
	{
		convex = PxCreateConvexMesh(params, desc, m_pPhysics->getPhysicsInsertionCallback());
		PX_ASSERT(convex);
	}
	else
	{
		// cooking 후 직렬화 내보내기
		// TODO : 내보내는 경로 받기
		// TODO : serialize 분리
		PxDefaultFileOutputStream outStream("");
		bool res = PxCookConvexMesh(params, desc, outStream);
		PX_UNUSED(res);
		PX_ASSERT(res);

		// 직렬화 불러오기
		// TODO : deserialize 분리
		PxDefaultFileInputData instream("");
		convex = m_pPhysics->createConvexMesh(instream);
		PX_ASSERT(convex);
	}

	return convex;
}

vector<CPhysics_ResourceManager::HEIGHTFIELD_INFO> CPhysics_ResourceManager::CreateHeightFields(CModel* model)
{
	vector<HEIGHTFIELD_INFO> result{};

	if (model == nullptr)
		return result;

	vector<CMesh*> meshes;
	_uint NumMesh = model->Get_MeshCount();

	meshes.reserve(NumMesh);
	for (_uint i = 0; i < NumMesh; i++)
		meshes.push_back(model->Get_Mesh(i));

	for (auto& mesh : meshes)
	{
		if (mesh->Get_VerticesCount() <= 0)
			continue;

		result.push_back(CreateHeightField(mesh));
	}

	return result;
}

CPhysics_ResourceManager::HEIGHTFIELD_INFO CPhysics_ResourceManager::CreateHeightField(CMesh* mesh)
{
	HEIGHTFIELD_INFO heightFieldInfo{};

	PxU32 numVertices = mesh->Get_VerticesCount();
	const Vec3* vertices = mesh->Get_VertexPositionData();

	PxReal minX = FLT_MAX, maxX = -FLT_MAX;
	PxReal minZ = FLT_MAX, maxZ = -FLT_MAX;

	for (_uint i = 0; i < numVertices; i++)
	{
		if (vertices[i].x < minX) minX = vertices[i].x;
		if (vertices[i].x > maxX) maxX = vertices[i].x;
		if (vertices[i].z < minZ) minZ = vertices[i].z;
		if (vertices[i].z > maxZ) maxZ = vertices[i].z;
	}

	PxU32 numRows = 127;
	PxU32 numCols = 127;
	//PxU32 numRows = (PxU32)sqrt((_float)numVertices);
	//PxU32 numCols = numRows;

	PxReal rowScale = (maxZ - minZ) / (_float)(numRows - 1);
	PxReal colScale = (maxX - minX) / (_float)(numCols - 1);
	//PxReal rowScale = 100.f;
	//PxReal colScale = 100.f;

	PxReal heightQuantization = 100.0f;

	vector<PxI16> heightBuffer(numRows * numCols, SHRT_MIN);

	for (_uint i = 0; i < numVertices; i++)
	{
		PxI32 col = (PxI32)floor(((vertices[i].x - minX) / colScale) + 0.5f);
		PxI32 row = (PxI32)floor(((vertices[i].z - minZ) / rowScale) + 0.5f);

		if (col < 0 || col >= (PxI32)numCols || row < 0 || row >= (PxI32)numRows)
			continue;

		PxU32 index = (row * numCols) + col;

		PxI16 h = (PxI16)(vertices[i].y * heightQuantization);

		if (h > heightBuffer[index])
		{
			heightBuffer[index] = h;
		}
	}

	for (PxU32 r = 0; r < numRows; r++)
	{
		for (PxU32 c = 0; c < numCols; c++)
		{
			PxU32 index = r * numCols + c;
			
			if (heightBuffer[index] == SHRT_MIN)
			{
				PxI32 sum = 0;
				PxI32 count = 0;

				if (r > 0 && heightBuffer[index - numCols] > SHRT_MIN)
				{
					sum += heightBuffer[index - numCols];
					count++;
				}

				if (r < numRows - 1 && heightBuffer[index + numCols] > SHRT_MIN)
				{
					sum += heightBuffer[index + numCols];
					count++;
				}

				if (c > 0 && heightBuffer[index - 1] > SHRT_MIN)
				{
					sum += heightBuffer[index - 1];
					count++;
				}

				if (count > 0)
					heightBuffer[index] = (PxI16)(sum / count);
				else
					heightBuffer[index] = 0;
			}
		}
	}

	vector<PxHeightFieldSample> samples(numRows * numCols);
	for (PxU32 i = 0; i < samples.size(); i++)
	{
		samples[i].height = heightBuffer[i];
		samples[i].materialIndex0 = 0;
		samples[i].clearTessFlag();
		samples[i].setTessFlag();
	}

	PxHeightFieldDesc hfDesc;
	hfDesc.format = PxHeightFieldFormat::eS16_TM;
	hfDesc.nbColumns = numCols;
	hfDesc.nbRows = numRows;
	hfDesc.samples.data = samples.data();
	hfDesc.samples.stride = sizeof(PxHeightFieldSample);

	PxHeightField* pHeightField = PxCreateHeightField(hfDesc);
	//PxHeightField* pHeightField = PxCreateHeightField(hfDesc, m_pPhysics->getPhysicsInsertionCallback());
	//PxHeightField* pHeightField = m_pPhysics->createHeightField();
	//PxCookHeightField(hfDesc, );

	if (!pHeightField)
	{
		MSG_BOX("Failed to Created : Physics ShapeFactory Height Field");
		return heightFieldInfo;
	}

	{
		heightFieldInfo.minX = minX;
		heightFieldInfo.maxX = maxX;
		heightFieldInfo.minZ = minZ;
		heightFieldInfo.maxZ = maxZ;
		heightFieldInfo.numRows = numRows;
		heightFieldInfo.numCols = numCols;
		heightFieldInfo.rowScale = rowScale;
		heightFieldInfo.colScale = colScale;
		heightFieldInfo.pHeightField = pHeightField;
	}

	return heightFieldInfo;
}

void CPhysics_ResourceManager::SerializeStaticMesh(std::filesystem::path path, vector<PxTriangleMesh*> meshes)
{
	PxCollection* sharedCollection = PxCreateCollection();

	for (auto& mesh : meshes)
		sharedCollection->add(*mesh);

	SerializeSharedObjects(path, sharedCollection);
}

void CPhysics_ResourceManager::SerializeLevel(std::filesystem::path path)
{
	// TODO:

	PxCollection* sharedCollection = PxCreateCollection();
	PxCollection* actorCollection = PxCreateCollection();

	SerializeActorObjects(path, sharedCollection, actorCollection);
}

PxCollection* CPhysics_ResourceManager::DeserializeStaticMesh(std::filesystem::path path)
{
	return DeserializeSharedObjects(path);
}

void CPhysics_ResourceManager::DeserializeLevel(std::filesystem::path path)
{
	// TODO:

	PxCollection* collection = NULL;

	DeserializeActorObjects(path, collection);
}

PxTriangleMesh* CPhysics_ResourceManager::CreateBV34TriangleMesh(PxU32 numVertices, const PxVec3* vertices, PxU32 numTriangles, const PxU32* indices, _bool skipMeshCleanup, _bool skipEdgeData, _bool inserted, const PxU32 numTrisPerLeaf)
{
	PxTriangleMeshDesc meshDesc{};
	meshDesc.points.count = numVertices;
	meshDesc.points.data = vertices;
	meshDesc.points.stride = sizeof(PxVec3);
	meshDesc.triangles.count = numTriangles;
	meshDesc.triangles.data = indices;
	meshDesc.triangles.stride = 3 * sizeof(PxU32);

	PxTolerancesScale scale;
	PxCookingParams params(scale);

	params.meshPreprocessParams |= PxMeshPreprocessingFlag::eWELD_VERTICES;
	params.meshWeldTolerance = physx::PxReal(0.01);

	params.midphaseDesc = PxMeshMidPhase::eBVH34;

	SetupCommonCookingParams(params, skipMeshCleanup, skipEdgeData);

	params.midphaseDesc.mBVH34Desc.numPrimsPerLeaf = numTrisPerLeaf;

#if defined(PX_CHECKED) || defined(PX_DEBUG)
	if (skipMeshCleanup)
		PX_ASSERT(PxValidateTriangleMesh(params, meshDesc));
#endif

	PxTriangleMesh* triMesh = NULL;
	PxU32 meshSize = 0;

	// 변환된 메쉬 데이터는 나중에 불러오기 위해 스트림(파일 등)에 저장해 둘 수 있고,
	// 저장하지 않고 곧바로 물리 엔진(PxPhysics)에 집어넣어 사용할 수도 있다

	// 쿠킹(변환)결과를 바이너리 파일로 저장
	// 프로그램 재실행 시 다시 쿠킹 필요없이 파일만 로드 -> 로딩 속도 향상
	if (inserted)
	{
		triMesh = PxCreateTriangleMesh(params, meshDesc, m_pPhysics->getPhysicsInsertionCallback());
	}
	else
	{
		PxDefaultMemoryOutputStream outBuffer;
		PxCookTriangleMesh(params, meshDesc, outBuffer);

		PxDefaultMemoryInputData stream(outBuffer.getData(), outBuffer.getSize());
		triMesh = m_pPhysics->createTriangleMesh(stream);

		meshSize = outBuffer.getSize();
	}

	return triMesh;
}

void CPhysics_ResourceManager::SetupCommonCookingParams(PxCookingParams& params, _bool skipMeshCleanup, _bool skipEdgeData)
{
	params.suppressTriangleMeshRemapTable = true;

	if (!skipMeshCleanup)
		params.meshPreprocessParams &= ~static_cast<PxMeshPreprocessingFlags>(PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH);
	else
		params.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;

	if (!skipEdgeData)
		params.meshPreprocessParams &= ~static_cast<PxMeshPreprocessingFlags>(PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE);
	else
		params.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE;
}

void CPhysics_ResourceManager::SerializeSharedObjects(std::filesystem::path path, PxCollection* sharedCollection)
{
	// sharedCollection
		// 프로토타입 px객체

		// actorCollection
		// 인스턴스 객체 // ex) 조인트 프리팹(레그돌)

	PxDefaultFileOutputStream sharedOutputStream(path.string().c_str());

	PxSerializationRegistry* sr = PxSerialization::createSerializationRegistry(*m_pPhysics);

	// PxSerialization::complete
	// 의존성 자동 추적
	// PhysX Material(마찰력, 반발계수) 포함 작업
	PxSerialization::complete(*sharedCollection, *sr);

	// PxSerialization::createSerialObjectIds
	// 주소 값으로 참조 중인 정보들 ID로 치환
	// ex) PxSerialObjectId(1000) 1000번 부터 시작하는 ID값으로 구분하겠다 (Base Offset)
	PxSerialization::createSerialObjectIds(*sharedCollection, PxSerialObjectId(1000));

	PxSerialization::serializeCollectionToBinary(sharedOutputStream, *sharedCollection, *sr);

	// To Xml
	//PxSerialization::serializeCollectionToXml(sharedOutputStream, *sharedCollection, *sr);

	sharedCollection->release();
	sr->release();
}

void CPhysics_ResourceManager::SerializeActorObjects(std::filesystem::path path, PxCollection* sharedCollection, PxCollection* actorCollection)
{
	// sharedCollection
		// 프로토타입 px객체

		// actorCollection
		// 인스턴스 객체 // ex) 조인트 프리팹(레그돌)

	PxDefaultFileOutputStream sharedOutputStream(path.string().c_str());

	PxSerializationRegistry* sr = PxSerialization::createSerializationRegistry(*m_pPhysics);

	PxSerialization::complete(*actorCollection, *sr, sharedCollection, true);

	PxSerialization::serializeCollectionToBinary(sharedOutputStream, *actorCollection, *sr, sharedCollection);

	// To Xml
	//PxSerialization::serializeCollectionToXml(sharedOutputStream, *sharedCollection, *sr, NULL, sharedCollection);

	sharedCollection->release();
	actorCollection->release();
	sr->release();
}

PxCollection* CPhysics_ResourceManager::DeserializeSharedObjects(std::filesystem::path path)
{
	PxDefaultFileInputData sharedInputStream(path.string().c_str());

	PxTolerancesScale scale;
	const PxCookingParams params(scale);

	PxSerializationRegistry* sr = PxSerialization::createSerializationRegistry(*m_pPhysics);

	PxCollection* sharedCollection = NULL;
	{
		void* alignedBlock = CreateAlignedBlock(sharedInputStream.getLength());
		sharedInputStream.read(alignedBlock, sharedInputStream.getLength());
		sharedCollection = PxSerialization::createCollectionFromBinary(alignedBlock, *sr);

		// From Xml
		//sharedCollection = PxSerialization::createCollectionFromXml(sharedInputStream, params, *sr);
	}

	m_pScene->addCollection(*sharedCollection);

	return sharedCollection;
}

PxCollection* CPhysics_ResourceManager::DeserializeActorObjects(std::filesystem::path path, PxCollection* sharedCollection)
{
	PxDefaultFileInputData actorInputStream(path.string().c_str());

	PxTolerancesScale scale;
	const PxCookingParams params(scale);

	PxSerializationRegistry* sr = PxSerialization::createSerializationRegistry(*m_pPhysics);

	PxCollection* collection = NULL;
	{
		actorInputStream.seek(0);

		void* alignedBlock = CreateAlignedBlock(actorInputStream.getLength());
		actorInputStream.read(alignedBlock, actorInputStream.getLength());
		collection = PxSerialization::createCollectionFromBinary(alignedBlock, *sr, sharedCollection);

		// From Xml
		//sharedCollection = PxSerialization::createCollectionFromXml(sharedInputStream, params, *sr, sharedCollection);
	}

	m_pScene->addCollection(*collection);

	return collection;
}

void* CPhysics_ResourceManager::CreateAlignedBlock(PxU32 size)
{
	PxU8* baseAddr = static_cast<PxU8*>(malloc(size + PX_SERIAL_FILE_ALIGN - 1));
	m_MemBlocks.push_back(baseAddr);

	// 임의의 메모리 주소를 강제로 128의 배수로 딱 맞게 올림(Ceiling) 처리하는 비트 연산 공식
	// baseAddr를 Align으로 나눈 뒤 올림하고, 다시 Align을 곱함 = 다음 배수로 이동
	void* alignedBlock = reinterpret_cast<void*>((size_t(baseAddr) + PX_SERIAL_FILE_ALIGN - 1) & ~(PX_SERIAL_FILE_ALIGN - 1));
	return alignedBlock;
}

CPhysics_ResourceManager* CPhysics_ResourceManager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, PxPhysics* pPhysics, PxScene* pScene)
{
	CPhysics_ResourceManager* pInstance = new CPhysics_ResourceManager(pDevice, pContext, pPhysics, pScene);

	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CPhysics_ResourceManager");
	}

	return pInstance;
}

void CPhysics_ResourceManager::Free()
{
	for (size_t i = 0; i < ENUM_TO_UINT(EPhysicsMaterial::END); i++)
	{
		if (m_Materials[i])
			PX_RELEASE(m_Materials[i]);
	}

	{
		for (auto& triMeshes : m_TriangleMeshes)
		{
			for (auto& triMesh : triMeshes.second)
			{
				if (triMesh)
					PX_RELEASE(triMesh);
			}
			triMeshes.second.clear();
		}
		m_TriangleMeshes.clear();
	}

	{
		for (auto& conMeshes : m_ConvexMeshes)
		{
			for (auto& conMesh : conMeshes.second)
			{
				if (conMesh)
					PX_RELEASE(conMesh);
			}
			conMeshes.second.clear();
		}
		m_ConvexMeshes.clear();
	}

	{
		for (auto& heightFields : m_HeightFields)
		{
			for (auto& heightField : heightFields.second)
			{
				if (heightField.pHeightField)
					PX_RELEASE(heightField.pHeightField);
			}

			heightFields.second.clear();
		}
		m_HeightFields.clear();
	}

	for (void* ptr : m_MemBlocks)
	{
		//Safe_Delete(ptr);
		free(ptr);
	}
	m_MemBlocks.clear();

	Safe_Release(m_pGameInstance);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	Super::Free();
}
