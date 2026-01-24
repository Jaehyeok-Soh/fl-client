#include "pch.h"
#include "CameraMan_Free.h"
#include "GodInput.h"
#include "GameInstance.h"

CCameraMan_Free::CCameraMan_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : Super(pDevice, pDeviceContext, CameraType::DYNAMIC)
{
}

CCameraMan_Free::CCameraMan_Free(const CCameraMan_Free& rhs)
    : Super(rhs)
{
}

HRESULT CCameraMan_Free::Initialize_Prototype()
{
    if (FAILED(Super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CCameraMan_Free::Initialize(void* pArg)
{
    if (FAILED(Super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Script_Component(L"Component_GodInput", dynamic_cast<CMonoBehaviour*>(CGodInput::Create()))))
        return E_FAIL;

    Get_Script_Component(L"Component_GodInput")->Initialize(pArg);

    return S_OK;
}

HRESULT CCameraMan_Free::Awake(const _uint iCurrentLevelID)
{
    if (FAILED(Super::Awake(iCurrentLevelID)))
        return E_FAIL;

    return S_OK;
}

void CCameraMan_Free::Update_Priority(const _float fTimeDelta)
{
    Super::Update_Priority(fTimeDelta);
}

void CCameraMan_Free::Update(const _float fTimeDelta)
{
    Super::Update(fTimeDelta);
}

void CCameraMan_Free::Update_Late(const _float fTimeDelta)
{
    Super::Update_Late(fTimeDelta);
}

void CCameraMan_Free::Ready_Before_Render(const _float fTimeDelta)
{
    Super::Ready_Before_Render(fTimeDelta);
}

CCameraMan_Free* CCameraMan_Free::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
    CCameraMan_Free* pInstance = new CCameraMan_Free(pDevice, pDeviceContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("CCameraMan_Free::Create, Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CCameraMan_Free::Clone(void* pArg)
{
    CCameraMan_Free* pInstance = new CCameraMan_Free(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("CCameraMan_Free::Clone, Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CCameraMan_Free::Free()
{
    Super::Free();
}
