#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CGameObject;

class CPhysics_RagdollSystem final : public CBase
{
public:
    typedef struct ERagdollState
    {
        enum Enum
        {
            SLEEP,
            PENDING,
            PROCESSING,
            END
        };
    }ERAGDOLLSTATE;

    using RegisteredItem = std::pair<CGameObject*, ERagdollState::Enum>;

    using Super = CBase;
private:
    CPhysics_RagdollSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, PxPhysics* pPhysics, PxScene* pScene);
    virtual ~CPhysics_RagdollSystem() = default;

    HRESULT Initialize();

public:
    _bool   CheckRagdollState(int64 objID);
    _bool    CheckRagDollState_Processing(uint64 objID);

    RAGDOLLELEMENTS CreateRagdoll(array<RAGDOLLBONEDESC, RAGDOLLJOINT::END> arrRagdollBoneDesc);
    void Register(CGameObject* obj);
    void Unregister(int64 objID);

    void RequestStart(uint64 objID);
    void SyncStates(uint64 objID, vector<class CChannel*>& vecChannels);
    void Finish(uint64 objID);



private:
    void CreateRagdollLink(RAGDOLLELEMENTS* elements, array<RAGDOLLBONEDESC, RAGDOLLJOINT::END> arrRagdollBoneDesc, _int index, PxArticulationLink* parentLink);
    void Awake(uint64 objID, vector<class CChannel*>& vecChannels);
    void Process(uint64 objID, vector<class CChannel*>& vecChannels);

private:
    class CGameInstance* m_pGameInstance = { nullptr };
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };

private:
    unordered_map<uint64, RegisteredItem> m_umapRegisteredMap;

private:
    PxPhysics* m_pPhysics = { nullptr };
    PxScene* m_pScene = { nullptr };

    PxFilterData filterData{};

public:
    static CPhysics_RagdollSystem* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, PxPhysics* pPhysics, PxScene* pScene);
    virtual void Free();
};

NS_END