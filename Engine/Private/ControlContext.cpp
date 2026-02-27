#include "Engine_pch.h"
#include "ControlContext.h"
#include "GameInstance.h"

CControlContext::CControlContext()
    : Super()
{
}

CControlContext::CControlContext(const CControlContext& rhs)
    : Super(rhs)
{
}

HRESULT CControlContext::Initialize_Prototype()
{
    if (FAILED(Super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CControlContext::Initialize(void* pArg)
{
    if (FAILED(Super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CControlContext::Clear_WhenChangeLevel()
{
    m_pTarget = nullptr;
}

void CControlContext::Free()
{
    Super::Free();
}