#include "Engine_pch.h"
#include "GimmikController.h"
#include "Model.h"
#include "GameObject.h"
#include "GameInstance.h"

CGimmikController::CGimmikController()
    : Super()
{
}

CGimmikController::CGimmikController(const CGimmikController& rhs)
    : Super(rhs)
{
}

HRESULT CGimmikController::Initialize_Prototype()
{
    if (FAILED(Super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CGimmikController::Initialize(void* pArg)
{
    if (pArg == nullptr)
        return E_FAIL;

    if (FAILED(Super::Initialize(pArg)))
        return E_FAIL;

    GIMMIKCTRL_DESC* pDesc = static_cast<GIMMIKCTRL_DESC*>(pArg);
    m_pOwnerModel = pDesc->pOwnerModel;

    if (m_pOwnerModel == nullptr)
        return E_FAIL;

    return S_OK;
}

void CGimmikController::Bind_ModelAnimNotify()
{
    if (m_pOwnerModel == nullptr)
        return;

    Unbind_ModelAnimNotify();
    
    m_hAnimNotifyHandle = m_pOwnerModel->OnNotify.Subscribe(
        [this](const AnimNotifyKey& key)
        {
            this->On_ModelAnimNotify(key);
        });
}

void CGimmikController::Unbind_ModelAnimNotify()
{
    if (m_pOwnerModel == nullptr)
        return;

    m_pOwnerModel->OnNotify.Unsubscribe(m_hAnimNotifyHandle);
    m_hAnimNotifyHandle = {};
}

void CGimmikController::Free()
{
    Unbind_ModelAnimNotify();
    Super::Free();
}
