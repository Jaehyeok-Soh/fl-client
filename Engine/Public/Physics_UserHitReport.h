#pragma once
#include "Base.h"

#include "GameObject.h"

NS_BEGIN(Engine)

class CPhysics_UserHitReport : public CBase, public PxUserControllerHitReport
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
            if (left)
            {
                leftName = left->Get_WName();
                leftID = left->Get_ID();
                leftObject = left;
                leftColliderDesc = leftDesc;
            }
            
            if (right)
            {
                rightName = right->Get_WName();
                rightID = right->Get_ID();
                rightObject = right;
                rightColliderDesc = rightDesc;
            }
        }
    }GAMEOBJECTINFO;

    typedef struct tagHitEvent
    {
        enum Enum
        {
            ON_SHAPE_HIT,
            ON_CCT_HIT,
            END
        };
    }HITEVENT;

    using Super = CBase;
private:
    CPhysics_UserHitReport();
    virtual ~CPhysics_UserHitReport() = default;

    HRESULT Initialize();

    CGameObject* Conversion_GameObject(void* userData);
    GAMEOBJECTINFO Get_GameObject(void* leftArgs, void* rightArgs);

    void Ready_EventCallChain();
#ifdef _DEBUG
    void Debug_Log(HITEVENT::Enum event, GAMEOBJECTINFO& info);
#endif // _DEBUG

    array<std::function<void(GAMEOBJECTINFO& info)>, HITEVENT::Enum::END> m_arrHitEvent{};
    array<wstring, HITEVENT::Enum::END> m_arrEventString;
public:
    static CPhysics_UserHitReport* Create();
    virtual void Free() override;








    // PxUserControllerHitReport을(를) 통해 상속됨
    void onShapeHit(const PxControllerShapeHit& hit) override;
    void onControllerHit(const PxControllersHit& hit) override;
    
    // PxUserControllerHitReport을(를) 통해 상속됨
    void onObstacleHit(const PxControllerObstacleHit& hit) override { PX_UNUSED(hit); }
};

NS_END