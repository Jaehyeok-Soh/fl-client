#include "Client_Defines.h"
#include "ComboContainer.h"
#include "PlayerActionState.h"

CPlayerActionState::CPlayerActionState()
    : Super()
{
}

CPlayerActionState::CPlayerActionState(const CPlayerActionState& rhs)
    : Super(rhs)
{
}

HRESULT CPlayerActionState::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerActionState::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

CPlayerActionState* CPlayerActionState::Create()
{
    CPlayerActionState* pInsatnce = new CPlayerActionState();
    if (FAILED(pInsatnce->Initialize_Prototype()))
    {
        MSG_BOX("CPlayerActionState::Create, Failed");
        Safe_Release(pInsatnce);
    }
    return pInsatnce;
}

CComponent* CPlayerActionState::Clone(void* pArg)
{
    CPlayerActionState* pClone = new CPlayerActionState(*this);
    if (FAILED(pClone->Initialize(pArg)))
    {
        MSG_BOX("CPlayerActionState::Clone, Failed");
        Safe_Release(pClone);
    }
    return pClone;
}

void CPlayerActionState::Free()
{
    Super::Free();
}
