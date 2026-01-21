#include "GameInstance.h"
#include "CameraMan.h"

CCameraMan::CCameraMan(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, CameraType eType)
    : Super(pDevice, pDeviceContext)
    , m_eType(eType)
{
}

CCameraMan::CCameraMan(const CCameraMan& rhs)
    : Super(rhs)
    , m_eType(rhs.m_eType)
{
}

HRESULT CCameraMan::Initialize_Prototype()
{
    if (FAILED(Super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CCameraMan::Initialize(void* pArg)
{
    if (FAILED(Super::Initialize(pArg)))
        return E_FAIL;

    GAMEOBJECT_DESC* pDesc = static_cast<GAMEOBJECT_DESC*>(pArg);

    if (FAILED(Add_Component<CCamera>(0/* static */, L"Prototype_Component_Camera", pDesc->pCamera_Desc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CCameraMan::Awake(const _uint iCurrentLevelID)
{
    if (FAILED(Super::Awake(iCurrentLevelID)))
        return E_FAIL;

    return S_OK;
}

void CCameraMan::Update_Priority(const _float fTimeDelta)
{
    Super::Update_Priority(fTimeDelta);
}

void CCameraMan::Update(const _float fTimeDelta)
{
    Super::Update(fTimeDelta);
}

void CCameraMan::Update_Late(const _float fTimeDelta)
{
    Super::Update_Late(fTimeDelta);
}

void CCameraMan::Ready_Before_Render(const _float fTimeDelta)
{
    Super::Ready_Before_Render(fTimeDelta);
    Get_Component<CCamera>()->Update_View();
}

inline void CCameraMan::Change_Actor(CGameObject* pGo)
{
    if (nullptr == pGo)
        return;

    Safe_Release(m_pActor);
    Safe_AddRef(pGo);
    m_pActor = pGo;
}

HRESULT CCameraMan::Ready_Components(void* pArg)
{
    return E_NOTIMPL;
}

void CCameraMan::Free()
{
    Safe_Release(m_pActor);
    Super::Free();
}
