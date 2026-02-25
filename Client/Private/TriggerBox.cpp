#include "pch.h"
#include "TriggerBox.h"
#include "PhysicsRigidBody.h"
#include "PhysicsCollider.h"
#include "GameInstance.h"

CTriggerBox::CTriggerBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject(pDevice , pContext)
{
}

CTriggerBox::CTriggerBox(const CTriggerBox& rhs)
    : CGameObject(rhs)
{
}

HRESULT CTriggerBox::Initialize_Prototype()
{
    if (FAILED(Super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CTriggerBox::Initialize(void* pArg)
{
    if (FAILED(Super::Initialize(pArg)))
        return E_FAIL;


    CTriggerBox::TRIGGERBOX_DESC* pDesc = static_cast<CTriggerBox::TRIGGERBOX_DESC*>(pArg);


    if (FAILED(Ready_Transform(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Component(pDesc)))
        return E_FAIL;


    return S_OK;
}

HRESULT CTriggerBox::Ready_Transform(TRIGGERBOX_DESC* pDesc)
{

    return S_OK;
}

HRESULT CTriggerBox::Ready_Component(TRIGGERBOX_DESC* pDesc)
{

    /* Ready PhysicCollider */
    {

        PHYSICSCOLLIDER_DESC pcDesc{};

        /* Collider Box 悼老 */
        pcDesc.eConvexShape = EPhysicsConvexShape::BOX;
        pcDesc.eShape = EPhysicsShape::BOX;

        ///* Trigger Box Prototype阑 刚历积己? */
        //CPhysicsCollider* pCollider = CPhysicsCollider::Create(m_pDevice, m_pDeviceContext, &pcDesc);
        //if (pCollider)
        //{
        //    if (FAILED(m_pGameInstance->Add_Prototype(pDesc->iLevelIndex, L"Prototype_Component_Physics_Collider_" + wstrModelName, pCollider)))
        //        Safe_Release(pCollider);
        //}

        //PHYSICSCOLLIDER_DESC cloneDesc{};
        //cloneDesc.eFilterLayer = PHYSICSFILTERGROUP::MAP;
        //cloneDesc.iFilterMask = 0xFFFFFFFF;
        //cloneDesc.bSetOnlyFilter = true;

        //if (FAILED(Add_Component<CPhysicsCollider>(pDesc->iLevelIndex, L"Prototype_Component_Physics_Collider_" + wstrModelName, &cloneDesc)))
        //    return E_FAIL;

    }

    return S_OK;
}


HRESULT CTriggerBox::Awake(const _uint iCurrentLevelID)
{
    if (FAILED(Super::Awake(iCurrentLevelID)))
        return E_FAIL;

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

    return;
}

HRESULT CTriggerBox::Render()
{
    if(FAILED(Super::Render()))
        return E_FAIL;

    return S_OK;
}


void CTriggerBox::Free()
{
    Super::Free();
}



