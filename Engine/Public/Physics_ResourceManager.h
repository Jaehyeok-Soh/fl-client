#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;

class CPhysics_ResourceManager final : public CBase
{
public:
    typedef struct tagHeightFieldInfo
    {
        PxHeightField* pHeightField = { nullptr };
        PxHeightFieldGeometry hfGeom = {};
        PxReal minX = { FLT_MAX }, maxX = { -FLT_MAX };
        PxReal minZ = { FLT_MAX }, maxZ = { -FLT_MAX };
        PxU32 numRows = {}, numCols = {};
        PxReal rowScale = {}, colScale = {};
    }HEIGHTFIELD_INFO;

private:
	using Super = CBase;

private:
	CPhysics_ResourceManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, PxPhysics* pPhysics, PxScene* pScene);
	virtual ~CPhysics_ResourceManager() = default;

	HRESULT Initialize();

public:
	void InitMaterials();
	PxMaterial* GetMaterial(PHYSICSMATERIAL_DESC* pDesc);
	PxMaterial* GetMaterial(EPhysicsMaterial eMaterial);
	PxMaterial* CreateMaterial(PHYSICSMATERIAL_DESC* pDesc);

public:
	vector<PxTriangleMesh*> GetTriangleMeshes(PHYSICSCOLLIDER_DESC* pDesc);
	vector<PxConvexMesh*> GetConvexMeshes(PHYSICSCOLLIDER_DESC* pDesc);
    void RegisterPhysicsMesh(_uint levelIndex, _wstring prototypeTag);

    vector<HEIGHTFIELD_INFO> GetHeightFields(PHYSICSCOLLIDER_DESC* pDesc);
     
public:
    /// <summary>
    /// 지형 물리 모델 생성
    /// </summary>
    vector<PxTriangleMesh*> CreateTriangleMeshes(class CModel* model, _bool skipMeshCleanup = false, _bool skipEdgeData = false, _bool inserted = false, const PxU32 numTrisPerLeaf = 4);
    /// <summary>
    /// 지형 물리 모델 불러오기
    /// </summary>
    vector<PxTriangleMesh*> CreateTriangleMeshes(std::filesystem::path path, _bool skipMeshCleanup = false, _bool skipEdgeData = false, _bool inserted = false, const PxU32 numTrisPerLeaf = 4);
    PxTriangleMesh* CreateTriangleMesh(class CMesh* mesh, _bool skipMeshCleanup = false, _bool skipEdgeData = false, _bool inserted = false, const PxU32 numTrisPerLeaf = 4);

    /// <summary>
    /// 동적 물리 모델 생성
    /// </summary>
    vector<PxConvexMesh*> CreateConvexMeshes(class CModel* model, _bool directionInsertion = true, _uint gaussMapLimit = 16);
    PxConvexMesh* CreateConvexMesh(class CMesh* mesh, PxConvexMeshCookingType::Enum convexMeshCookingType, _bool directionInsertion, PxU32 gaussMapLimit);

    vector<HEIGHTFIELD_INFO> CreateHeightFields(class CModel* model);
    HEIGHTFIELD_INFO CreateHeightField(class CMesh* mesh);

    /// <summary>
    /// 지형 물리 모델 직렬화
    /// </summary>
    void SerializeStaticMesh(std::filesystem::path path, vector<PxTriangleMesh*> meshes);
    /// <summary>
    /// TODO: 레벨 직렬화
    /// </summary>
    void SerializeLevel(std::filesystem::path path);

    /// <summary>
    /// 지형 물리 모델 불러오기
    /// </summary>
    PxCollection* DeserializeStaticMesh(std::filesystem::path path);
    /// <summary>
    /// TODO: 레벨 불러오기
    /// </summary>
    void DeserializeLevel(std::filesystem::path path);

private:
    /// <summary>
    /// 지형 물리 모델 생성 구현부
    /// </summary>
    PxTriangleMesh* CreateBV34TriangleMesh(PxU32 numVertices, const PxVec3* vertices, PxU32 numTriangles, const PxU32* indices,
        _bool skipMeshCleanup, _bool skipEdgeData, _bool inserted, const PxU32 numTrisPerLeaf);
    void SetupCommonCookingParams(PxCookingParams& params, _bool skipMeshCleanup, _bool skipEdgeData);

    /// <summary>
    /// 물리 모델 직렬화 구현부
    /// </summary>
    void SerializeSharedObjects(std::filesystem::path path, PxCollection* sharedCollection);
    void SerializeActorObjects(std::filesystem::path path, PxCollection* sharedCollection, PxCollection* actorCollection);

    /// <summary>
    /// 물리 모델 역직렬화 구현부
    /// </summary>
    PxCollection* DeserializeSharedObjects(std::filesystem::path path);
    PxCollection* DeserializeActorObjects(std::filesystem::path path, PxCollection* sharedCollection);

    /// <summary>
    /// 읽기 주소 128바이트 정렬
    /// </summary>
    void* CreateAlignedBlock(PxU32 size);

private:
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };
    CGameInstance* m_pGameInstance = { nullptr };

private:
    PxPhysics* m_pPhysics = { nullptr };
    PxScene* m_pScene = { nullptr };

    // map<prototype tag, level index>
    map<_wstring, _int> m_MeshResourceTag;

    map<_wstring, vector<PxTriangleMesh*>> m_TriangleMeshes;
    map<_wstring, vector<PxConvexMesh*>> m_ConvexMeshes;
    map<_wstring, vector<HEIGHTFIELD_INFO>> m_HeightFields;

    PxMaterial* m_Materials[ENUM_TO_UINT(EPhysicsMaterial::END)] = { nullptr };
    
private:
    // Deserialize
    vector<void*> m_MemBlocks;

public:
    static CPhysics_ResourceManager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, PxPhysics* pPhysics, PxScene* pScene);
    virtual void Free();
};

NS_END