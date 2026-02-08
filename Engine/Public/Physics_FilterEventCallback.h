#pragma once
#include "Base.h"

#include "GameObject.h"

NS_BEGIN(Engine)

class CPhysics_FilterEventCallback final : public CBase , public PxSimulationEventCallback
{
    typedef struct tagGameObjectInfo
    {
        wstring leftName = {};
        uint64 leftID = {};
        CGameObject* leftObject = { nullptr };
        PHYSICSCOLLIDER_DESC* leftColliderDesc = { nullptr };

        wstring rightName = {};
        uint64 rightID = {};
        CGameObject* rightObject = { nullptr };
        PHYSICSCOLLIDER_DESC* rightColliderDesc = { nullptr };

        tagGameObjectInfo(CGameObject* left, PHYSICSCOLLIDER_DESC* leftDesc, CGameObject* right, PHYSICSCOLLIDER_DESC* rightDesc)
        {
            leftName = left->Get_WName();
            leftID = left->Get_ID();
            leftObject = left;
            leftColliderDesc = leftDesc;

            rightName = right->Get_WName();
            rightID = right->Get_ID();
            rightObject = right;
            rightColliderDesc = rightDesc;
        }
    }GAMEOBJECTINFO;

    typedef struct tagCollisionEvent
    {
        enum Enum
        {
            ON_COLLISION_ENTER,
            ON_COLLISION_STAY,
            ON_COLLISION_EXIT,
            ON_TRIGGER_ENTER,
            ON_TRIGGER_EXIT,
            END
        };
    }COLLISIONEVENT;

    using Super = CBase;
private:
    CPhysics_FilterEventCallback();
    virtual ~CPhysics_FilterEventCallback() = default;

    HRESULT Initialize();

    CGameObject* Conversion_GameObject(void* userData);
    GAMEOBJECTINFO Get_GameObject(void* leftArgs, void* rightArgs);
    
    void Ready_EventCallChain();
#ifdef _DEBUG
    void Debug_Log(COLLISIONEVENT::Enum event, GAMEOBJECTINFO& info);
#endif // _DEBUG

    array<std::function<void(GAMEOBJECTINFO& info)>, COLLISIONEVENT::Enum::END> m_arrCollisionEvent{};
    array<wstring, COLLISIONEVENT::Enum::END> m_arrEventString;
public:
    static CPhysics_FilterEventCallback* Create();
    virtual void Free() override;








private:
    // PxSimulationEventCallback을(를) 통해 상속됨
    void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) override;
    void onTrigger(PxTriggerPair* pairs, PxU32 count) override;

    // PxSimulationEventCallback을(를) 통해 상속됨
    void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) override { PX_UNUSED(constraints); PX_UNUSED(count); }
    void onWake(PxActor** actors, PxU32 count) override { PX_UNUSED(actors); PX_UNUSED(count); }
    void onSleep(PxActor** actors, PxU32 count) override { PX_UNUSED(actors); PX_UNUSED(count); }
    void onAdvance(const PxRigidBody* const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count) override {}
};

NS_END