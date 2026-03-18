#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
class CPhysics_Utils;
class CPhysics_ResourceManager;
class CPhysics_ShapeFactory;
class CPhysics_ActorFactory;
class CPhysics_CCTManager;
class CPhysics_RagdollSystem;
class CPhysics_FilterEventCallback;
class CPhysics_QueryFilterCallback_SpringArm;

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
    void AddRagdoll(PxArticulationReducedCoordinate* pArticulation);
    void RemoveRagdoll(PxArticulationReducedCoordinate* pArticulation);
    void ClearPhysics();
    void FlushScene();
    void RemoveActor(PxRigidActor* actor);
    void ResetActorFilter(PxRigidActor* actor);

    PxScene* GetPhysicsScene() { return m_pScene; }
    PxControllerManager* GetPhysicsCCTManager();
/// <summary>
/// Utils
/// </summary>
public:
    PxTransform XMMatrixToPxTransform(Matrix mat);
    Matrix PxTransformToXMMatrix(PxTransform pxTransform);
    PxQuat GetPureRotation(Matrix mat);
    PxVec3 GetPureScale(Matrix mat);
    _bool Execute_Overlap(PxGeometry& shape, PxTransform& transform, OUT PxOverlapBuffer& hit, PxQueryFilterData& filterData, PxQueryFilterCallback* filterCallback);
    CPhysics_QueryFilterCallback* GetQueryFilterCallback();
    CPhysics_QueryFilterCallback_Gun* GetQueryFilterCallback_Gun();
    CPhysics_QueryFilterCallback_SpringArm* GetQueryFilterCallback_SpringArm();
#ifdef _DEBUG
    HRESULT Render(PxRigidActor* pActor, XMVECTOR color = DirectX::Colors::White);
    HRESULT Render(const PxGeometry& geom, const PxTransform& transform, XMVECTOR color = DirectX::Colors::White);
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

    PxMaterial* GetPhysicsMaterial(EPhysicsMaterial eMaterial);

/// <summary>
/// Shape Factory : 충돌체 생성
/// </summary>
public:
    vector<PxShape*> GetShape(PHYSICSCOLLIDER_DESC* pDesc);
    vector<PxShape*> GetMeshShape(PHYSICSCOLLIDER_DESC* pDesc);

    vector<PxShape*> CopyShapes(vector<PxShape*>& shapes);

/// <summary>
/// Actor Factory : RigidBody 생성
/// </summary>
public:
    vector<PxRigidActor*> GetActor(PHYSICSRIGIDBODY_DESC* rigidBodyDesc, PHYSICSCOLLIDER_DESC* colliderDesc, vector<PxShape*>& shapes);
    RAGDOLLELEMENTS CreateRagdoll(array<RAGDOLLBONEDESC, RAGDOLLJOINT::END> arrRagdollBoneDesc);

/// <summary>
/// Character Controller Manager
/// CCT
/// </summary>
public:
    PxController* GetController(PHYSICSCCT_DESC* pDesc);
    CPhysics_CCTFilterCallback* GetCCTFilterCallback();

/// <summary>
/// Ragdoll system
/// </summary>
public:
    _bool CheckRagdollState(int64 objID);
    _bool CheckRagDollState_Processing(int64 objID);
    void RagdollRegister(CGameObject* obj);
    void RagdollUnregister(int64 objID);

    void RagdollRequestStart(uint64 objID);
    void RagdollSyncStates(uint64 objID, vector<class CChannel*>& vecChannels);
    void RagdollFinish(uint64 objID);

/// <summary>
/// Collision Filter Shader
/// </summary>
public:
    static PxFilterFlags FilterShader(
        PxFilterObjectAttributes attributes0, PxFilterData filterData0,
        PxFilterObjectAttributes attributes1, PxFilterData filterData1,
        PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize);
    
    void Check_Leak();
    void GetActiveActors();

/// <summary>
/// Scene query
/// </summary>
public:
    void Overlap_EventCallback(CGameObject* pOwner, const PxVec3& vOverlapPoint, PxOverlapHit* pOverlapHit, PxPairFlag::Enum event, DTO::HITBOX_DESC* hitboxDesc);
    void Raycast_EventCallback(CGameObject* pOwner, PxRaycastBuffer* pRaycastHitBuffer, CPhysicsAttackRaycast::ATTACKRAYCASTDESC* raycastDesc);

    _bool RayCast(Vec3 vWorldPos, Vec3 vDir, _float fMaxDist, CPhysics_QueryFilterCallback* pFilterCall); // 월드 좌표 기준으로 // 방향으로 max dist까지 범위까지 검사

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
    CPhysics_RagdollSystem* m_pRagdollSystem = { nullptr };
    CPhysics_FilterEventCallback* m_pFilterEventCallback = { nullptr };

public:
    static CPhysics_Module* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual void Free();
};

NS_END