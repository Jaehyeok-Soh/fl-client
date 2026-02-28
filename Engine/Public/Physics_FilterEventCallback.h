#pragma once
#include "Base.h"

#include "GameObject.h"
#include <Anim_Event_Hitbox.h>

NS_BEGIN(Engine)

class CPhysics_FilterEventCallback final : public CBase , public PxSimulationEventCallback
{
    typedef struct tagGameObjectInfo
    {
        wstring leftName = {};
        uint64 leftID = {};
        CGameObject* leftObject = { nullptr };
        PHYSICSCOLLIDER_DESC leftColliderDesc{};

        wstring rightName = {};
        uint64 rightID = {};
        CGameObject* rightObject = { nullptr };
        PHYSICSCOLLIDER_DESC rightColliderDesc{};

        _bool bHasHitPoint = { false };
        SimpleMath::Vector3 vHitPoint = { SimpleMath::Vector3::Zero };
        SimpleMath::Vector3 vRawNormal = { SimpleMath::Vector3::Zero };
        _float fDepth = { 0.f };

        tagGameObjectInfo(CGameObject* left, PHYSICSCOLLIDER_DESC leftDesc, CGameObject* right, PHYSICSCOLLIDER_DESC rightDesc)
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

    using Super = CBase;
private:
    CPhysics_FilterEventCallback();
    virtual ~CPhysics_FilterEventCallback() = default;

    HRESULT Initialize();

    CGameObject* Conversion_GameObject(void* userData);
    GAMEOBJECTINFO Get_GameObject(void* leftArgs, void* rightArgs);
    GAMEOBJECTINFO Get_GameObject(CGameObject* leftObj, CGameObject* rightObj);
    
    void Ready_EventCallChain();
#ifdef _DEBUG
    void Debug_Log(COLLISIONEVENT::Enum event, GAMEOBJECTINFO& info);
#endif // _DEBUG

    array<std::function<void(GAMEOBJECTINFO& info)>, COLLISIONEVENT::Enum::END> m_arrCollisionEvent{};

#ifdef _DEBUG
    array<wstring, COLLISIONEVENT::Enum::END> m_arrEventString;
#endif // _DEBUG

public:
    static CPhysics_FilterEventCallback* Create();
    virtual void Free() override;



private:
    void OnCollisionEnter(GAMEOBJECTINFO& info);
    void OnCollision(GAMEOBJECTINFO& info);
    void OnCollisionExit(GAMEOBJECTINFO& info);

public:
    void ProcessOverlap(CGameObject* pOwner, const PxVec3& vOverlapPoint, PxOverlapHit* pOverlapHit, PxPairFlag::Enum event, DTO::HITBOX_DESC* hitboxDesc);

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