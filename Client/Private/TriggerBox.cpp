#include "pch.h"
#include "TriggerBox.h"
#include "DataStruct_Map.h"
#include "PhysicsRigidBody.h"
#include "PhysicsCollider.h"
#include "GameInstance.h"
#include "TriggerBox_LevelChange.h"
#include "QuestManager.h"


CTriggerBox::CTriggerBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject(pDevice, pContext), m_eTriggerBoxType{CTriggerBox::Type::CHANGE_LEVEL}
{
}

CTriggerBox::CTriggerBox(const CTriggerBox& rhs)
    : CGameObject(rhs), m_eTriggerBoxType{rhs.m_eTriggerBoxType}
{
}

HRESULT CTriggerBox::Initialize_Prototype()
{
    if (FAILED(Super::Initialize_Prototype()))
        return E_FAIL;

    Set_Object_Enum_Tag(OBJECT_ENUM_TAG::TRIGGER_BOX_DEFAULT);

    return S_OK;
}

HRESULT CTriggerBox::Initialize(void* pArg)
{
    if (FAILED(Super::Initialize(pArg)))
        return E_FAIL;


    CTriggerBox::TRIGGERBOX_DESC* pDesc = static_cast<CTriggerBox::TRIGGERBOX_DESC*>(pArg);

    if (FAILED(Ready_Component(pDesc)))
        return E_FAIL;

    if (m_bHasQuest = pDesc->bHasQuest)
    {
        // 퀘스트 있는 트리거박스는 퀘스트 활성화 시에만 트리거 발동
        SetEnable(false);
        Ready_Quest(&pDesc->tQuestObjectDesc);
    }

    return S_OK;
}

HRESULT CTriggerBox::Ready_Component(TRIGGERBOX_DESC* pDesc)
{

    /* Ready PhysicCollider */
    {
        /* 피직스 콜라이더 */
        {
            PHYSICSCOLLIDER_DESC cloneDesc{};
            cloneDesc.eShape = EPhysicsShape::BOX;
            cloneDesc.eFilterLayer = EPhysicsFilterGroup::TRIGGER_BOX;
            cloneDesc.iFilterMask = 0xFFFFFFFF;
            cloneDesc.bSetOnlyFilter = false;
            cloneDesc.bIsActive = true;
            cloneDesc.bIsTrigger = true;

            /* 맵툴에서 정해준 Trigger Box 크기로 설정 */
            cloneDesc.vExtents = pDesc->vTriggerBox_Extents * 2.f;

            if (FAILED(Add_Component<CPhysicsCollider>(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Physics_Collider", &cloneDesc)))
                return E_FAIL;
        }
    }

    /* Rigid Body */
    {
        PHYSICSRIGIDBODY_DESC desc{};
        desc.eType = EPhysicsActorType::STATIC;
        desc.detection = EPhysicsCollisionDetection::DISCRETE;
        desc.fDensity = 10.f;
        desc.bUseGravity = false;
        desc.bIsKinematic = false;
        desc.fLinearDamping = 0.f;
        desc.fAngularDamping = 0.f;
        if (pDesc->pSRTData == nullptr)
        {
            CTransform* pTs = Get_Component<CTransform>();
            if (pTs == nullptr) return E_FAIL;
            Matrix WorldMatrix = pTs->Get_WorldMatrix();
            desc.pOwnerMatrices = { WorldMatrix };
            desc.vecSRT.resize(1);
            WorldMatrix.Decompose(desc.vecSRT[0].vScale, desc.vecSRT[0].vQuat, desc.vecSRT[0].vPosition);
        }
        else
        {
            desc.pOwnerMatrices = { pDesc->pSRTData->Get_World() };
            desc.vecSRT         = { PHYSICS_SRT(pDesc->pSRTData->vScale , pDesc->pSRTData->vQuat , pDesc->pSRTData->vPosition) };
        }

        if (FAILED(Add_Component<CPhysicsRigidBody>(0, L"Prototype_Component_Physics_RigidBody", &desc)))
            return E_FAIL;
    }


    CTransform* pTs = Get_Component<CTransform>();
    if (pTs == nullptr) return E_FAIL;
    /* 이 오브젝트에 회전값을 적용시켜준다 */
    pTs->Rotation( pDesc->vTriggerBox_Rotation.x , pDesc->vTriggerBox_Rotation.y, pDesc->vTriggerBox_Rotation.z);

    return S_OK;
}

void CTriggerBox::Ready_Quest(vector<DTO::QUEST_CHAPTERDESC>* pQuestDesc)
{
    for (auto desc : *pQuestDesc)
        CQuestManager::GetInstance()->Register_QuestObject(desc, this);
}

HRESULT CTriggerBox::Awake(const _uint iCurrentLevelID)
{
    if (FAILED(Super::Awake(iCurrentLevelID)))
        return E_FAIL;

    Get_Component<CPhysicsRigidBody>()->Awake();

    return S_OK;
}

void CTriggerBox::Update_Priority(const _float fTimeDelta)
{
    Super::Update_Priority(fTimeDelta);

    return;
}

void CTriggerBox::Update(const _float fTimeDelta)
{
    Super::Update(fTimeDelta);
    
    
    
    
    return;
}

void CTriggerBox::Update_Late(const _float fTimeDelta)
{
    Super::Update_Late(fTimeDelta);


    return;
}

void CTriggerBox::Ready_Before_Render(const _float fTimeDelta)
{
    Super::Ready_Before_Render(fTimeDelta);



#ifdef _DEBUG
    /* 일단 무적권 추가 */
    CPhysicsRigidBody* pRigidBody = Get_Component<CPhysicsRigidBody>();
    if (pRigidBody)
        m_pGameInstance->Push_DebugComponent(pRigidBody);

#endif // _DEBUG

    return;
}

HRESULT CTriggerBox::Render()
{
    if(FAILED(Super::Render()))
        return E_FAIL;

    return S_OK;
}

void CTriggerBox::QuestEnter()
{
    SetEnable(true);
}

void CTriggerBox::QuestExit()
{
    SetEnable(false);
}


void CTriggerBox::Free()
{
    Super::Free();
}


