#include "Engine_pch.h"
#include "MonoBehaviour.h"

CMonoBehaviour::CMonoBehaviour()
    : Super()
{
}

CMonoBehaviour::CMonoBehaviour(const CMonoBehaviour& rhs)
    : Super(rhs)
{
}

HRESULT CMonoBehaviour::Initialize_Prototype()
{
    if (FAILED(Super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CMonoBehaviour::Initialize(void* pArg)
{
    if (FAILED(Super::Initialize(pArg)))
        return E_FAIL;
    
    return S_OK;
}

void CMonoBehaviour::Free()
{
    Super::Free();
}
