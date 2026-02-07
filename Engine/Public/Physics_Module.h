#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
class CPhysics_Utils;
class CPhysics_ResourceManager;
class CPhysics_ShapeFactory;
class CPhysics_ActorFactory;
class CPhysics_CCTManager;
class CPhysics_FilterEventCallback;

class CPhysics_Module final : public CBase
{
    using Super = CBase;
private:
    CPhysics_Module(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
    virtual ~CPhysics_Module() = default;

    HRESULT Initialize();

/// <summary>
/// Module Main
/// </summary>
public:
    void StepPhysics(_float fTimeDelta);
    void AddActor(PxRigidActor* actor);
    void ClearPhysics();

/// <summary>
/// Utils
/// </summary>
public:
    PxTransform XMMatrixToPxTransform(Matrix mat);
    Matrix PxTransformToXMMatrix(PxTransform pxTransform);
    _bool HasNegativeScale(Matrix mat);
    _int GetNegativeScaleAxis(const Matrix& mat);
    PxQuat GetPureRotation(Matrix mat);
    PxVec3 GetPureScale(Matrix mat);
#ifdef _DEBUG
    HRESULT Render(PxRigidActor* pActor, XMVECTOR color = DirectX::Colors::White);
#endif // _DEBUG

/// <summary>
/// Resource Manager : 물리 머테리얼, 기하정보 생성 및 캐싱
/// </summary>
public:
    void SerializeStaticMesh(std::filesystem::path path, vector<PxTriangleMesh*> meshes);
    PxCollection* DeserializeStaticMesh(std::filesystem::path path);

    void SerializeConvexMesh(std::filesystem::path path, vector<PxConvexMesh*> meshes);
    PxCollection* SerializeConvexMesh(std::filesystem::path path);

    void SerializeLevel(std::filesystem::path path) {}
    void DeserializeLevel(std::filesystem::path path) {}

    void RegisterPhysicsMesh(_uint levelIndex, _wstring prototypeTag);

/// <summary>
/// Shape Factory : 충돌체 생성
/// </summary>
public:
    vector<PxShape*> GetShape(PHYSICSCOLLIDER_DESC* pDesc);
    vector<PxShape*> GetMeshShape(PHYSICSCOLLIDER_DESC* pDesc);

/// <summary>
/// Actor Factory : RigidBody 생성
/// </summary>
public:
    vector<PxRigidActor*> GetActor(PHYSICSRIGIDBODY_DESC* rigidBodyDesc, PHYSICSCOLLIDER_DESC* colliderDesc, vector<PxShape*>& shapes);

/// <summary>
/// Character Controller Manager
/// CCT
/// </summary>
public:
    PxController* GetController(PHYSICSCCT_DESC* pDesc);

/// <summary>
/// Collision Filter Shader
/// </summary>
public:
    static PxFilterFlags FilterShader(
        PxFilterObjectAttributes attributes0, PxFilterData filterData0,
        PxFilterObjectAttributes attributes1, PxFilterData filterData1,
        PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize);
    
    void Check_Leak();

private:
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pDeviceContext = { nullptr };
    CGameInstance* m_pGameInstance = { nullptr };

private:
    PxDefaultAllocator m_Allocator;
    PxDefaultErrorCallback m_ErrorCallback;
    PxFoundation* m_pFoundation = { nullptr };
    PxPhysics* m_pPhysics = { nullptr };
    PxDefaultCpuDispatcher* m_pDispatcher = { nullptr };
    PxScene* m_pScene = { nullptr };
    PxMaterial* m_pMaterial = { nullptr };
    PxCudaContextManager* m_pCudaContextManager = { nullptr };
    PxPvd* m_pPvd = { nullptr };
#ifdef _DEBUG
    _bool m_bEnabledDebugDraw = { false };
#endif // _DEBUG

private:
    CPhysics_Utils* m_pUtils = { nullptr };
    CPhysics_ResourceManager* m_pResourceManager = { nullptr };
    CPhysics_ShapeFactory* m_pShapeFactory = { nullptr };
    CPhysics_ActorFactory* m_pActorFactory = { nullptr };
    CPhysics_CCTManager* m_pCCTManager = { nullptr };
    CPhysics_FilterEventCallback* m_pFilterEventCallback = { nullptr };

public:
    static CPhysics_Module* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual void Free();
};

NS_END