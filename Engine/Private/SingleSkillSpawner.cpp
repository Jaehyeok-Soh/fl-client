#include "Engine_pch.h"
#include "SingleSkillSpawner.h"
#include "GameInstance.h"

CSingleSkillSpawner::CSingleSkillSpawner(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : Super(pDevice, pDeviceContext)
{
}

CSingleSkillSpawner::CSingleSkillSpawner(const CSingleSkillSpawner& rhs)
    : Super(rhs)
{
}

HRESULT CSingleSkillSpawner::Initialize_Prototype(SPAWNER_ORIGIN_DESC* pDesc)
{
    if (FAILED(Super::Initialize_Prototype(pDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CSingleSkillSpawner::Initialize(void* pArg)
{
    if (FAILED(Super::Initialize(pArg)))
        return E_FAIL;

	return S_OK;
}

CSingleSkillSpawner* CSingleSkillSpawner::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, SPAWNER_ORIGIN_DESC* pDesc)
{
    CSingleSkillSpawner* pInstance = new CSingleSkillSpawner(pDevice, pDeviceContext);
    if (FAILED(pInstance->Initialize_Prototype(pDesc)))
    {
        MSG_BOX("CSingleSkillSpawner::Create, Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CSingleSkillSpawner::Clone(void* pArg)
{
    CSingleSkillSpawner* pInstance = new CSingleSkillSpawner(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("CSingleSkillSpawner::Clone, Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CSingleSkillSpawner::Free()
{
    Super::Free();
}
