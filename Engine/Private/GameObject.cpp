#include "GameObject.h"
#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"
#include "ActionState.h"
#include "Model.h"
#include "Camera.h"
#include "GameInstance.h"

CGameObject::CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : m_pDevice(pDevice)
    , m_pDeviceContext(pDeviceContext)
    , m_pGameInstance(CGameInstance::GetInstance())
    , m_bClone(false)
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pDeviceContext);
    Safe_AddRef(m_pGameInstance);
    m_Components.fill(nullptr);
}

CGameObject::CGameObject(const CGameObject& rhs)
    : m_pDevice(rhs.m_pDevice)
    , m_pDeviceContext(rhs.m_pDeviceContext)
    , m_pGameInstance(CGameInstance::GetInstance())
    , m_bClone(true)
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pDeviceContext);
    Safe_AddRef(m_pGameInstance);
    m_Components.fill(nullptr);
}

HRESULT CGameObject::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CGameObject::Initialize(void* pArg)
{
    if (FAILED(Add_Component<CTransform>(0/* STATIC */, L"Prototype_Component_Transform", pArg)))
        return E_FAIL;

    return S_OK;
}

HRESULT CGameObject::Awake(const _uint iCurrentLevelID)
{
    return S_OK;
}

void CGameObject::Update_Priority(const _float fTimeDelta)
{
    Get_Component<CTransform>()->Update_PrevPosition();
}

void CGameObject::Update(const _float fTimeDelta)
{
    Update_Script_Components(fTimeDelta);
}

void CGameObject::Update_Late(const _float fTimeDelta)
{
}

void CGameObject::Ready_Before_Render(const _float fTimeDelta)
{

}

HRESULT CGameObject::Render()
{
    return S_OK;
}

inline CMonoBehaviour* CGameObject::Get_Script_Component(const wstring& wstrComponentTag)
{
    unordered_map<wstring, CMonoBehaviour*>::iterator itr = m_ScriptComponents.find(wstrComponentTag);
    if (itr == m_ScriptComponents.end())
        return nullptr;
    else
        return itr->second;
}

inline CMonoBehaviour* CGameObject::Detach_Script_Component(const wstring& wstrComponentTag)
{
    if (wstrComponentTag.empty())
        return nullptr;

    unordered_map<wstring, CMonoBehaviour*>::iterator itr = m_ScriptComponents.find(wstrComponentTag);
    if (itr == m_ScriptComponents.end())
        return nullptr;

    CMonoBehaviour* pDetached = itr->second;
    if (!pDetached)
        return nullptr;

    m_ScriptComponents.erase(itr);
    pDetached->Set_Owner(nullptr);
    return pDetached;
}

HRESULT CGameObject::Add_Script_Component(const wstring& wstrComponentTag, CMonoBehaviour* pComp)
{
    if (nullptr == pComp || wstrComponentTag.empty())
        return E_FAIL;

    m_ScriptComponents.insert(unordered_map<wstring, CMonoBehaviour*>::value_type(wstrComponentTag, pComp));
    pComp->Set_Owner(this);
    return S_OK;
}

HRESULT CGameObject::Add_Script_Component(const wstring &wstrComponentTag, const wstring& wstrPrototypeTag, void* pArg)
{
    CMonoBehaviour* pComp = { nullptr };
    if ( nullptr == (pComp = dynamic_cast<CMonoBehaviour*>(m_pGameInstance->Clone_Prototype(EPrototypeType::COMPONENT, 0/* STATIC */, wstrPrototypeTag, pArg))))
        return E_FAIL;

    unordered_map<wstring, CMonoBehaviour*>::iterator itr = m_ScriptComponents.find(wstrComponentTag);
    if (itr != m_ScriptComponents.end())
        return E_FAIL;

    m_ScriptComponents.insert(unordered_map<wstring, CMonoBehaviour*>::value_type(wstrComponentTag, pComp));
    pComp->Set_Owner(this);
    return S_OK;
}

void CGameObject::Remove_Script_Component(const wstring& wstrComponentTag)
{
    unordered_map<wstring, CMonoBehaviour*>::iterator itr = m_ScriptComponents.find(wstrComponentTag);
    if (itr == m_ScriptComponents.end())
        return;

    m_ScriptComponents.erase(itr);
}

_int CGameObject::Get_AnimationIndex(const wstring& wstrName)
{
    if (CModel* pModel = Get_Component<CModel>())
    {
        return pModel->Get_AnimationIndex(wstrName);
    }
    return -1;
}

HRESULT CGameObject::Change_State(_uint iIndex)
{
    if (CActionState* pActionState = Get_Component<CActionState>())
    {
        return pActionState->Change_State(iIndex);
    }
    return E_FAIL;
}

void CGameObject::Set_Dead(const wstring& wstrLayerTag)
{
    m_bDead = true;
    m_pGameInstance->Request_DeleteGameObject(m_pGameInstance->Get_CurrentLevelIndex(), wstrLayerTag, this);
}

Vec3 CGameObject::Get_CenterFromCollider(EColliderType eType, CBounding* pBounding)
{
    switch (eType)
    {
    case Engine::EColliderType::SPHERE:
        return static_cast<CBounding_Sphere*>(pBounding)->Get_Desc()->Center;
    case Engine::EColliderType::AABB:
        return static_cast<CBounding_AABB*>(pBounding)->Get_Desc()->Center;
    case Engine::EColliderType::OBB:
        return static_cast<CBounding_OBB*>(pBounding)->Get_Desc()->Center;
    default:
        return { -100.f, -100.f, -100.f };
    }
}

void CGameObject::Update_Script_Components(const _float fTimeDelta)
{
    for (auto& Pair : m_ScriptComponents)
    {
        if (Pair.second)
            Pair.second->Update(fTimeDelta);
    }
}

void CGameObject::Safe_Release_Component()
{
    for (CComponent*& pElement : m_Components)
    {
        Safe_Release(pElement);
    }
    m_Components.fill(nullptr);
}

void CGameObject::Safe_Release_ScriptComponent()
{
    for (auto itr = m_ScriptComponents.begin();
        itr != m_ScriptComponents.end();
        ++itr)
    {
        if (itr->second)
        {
            CMonoBehaviour* pMono = itr->second;
            Safe_Release(pMono);
        }
    }
    m_ScriptComponents.clear();
}

void CGameObject::Free()
{
    Super::Free();
    Safe_Release_Component();
    Safe_Release_ScriptComponent();
    Safe_Release(m_pGameInstance);
    Safe_Release(m_pDeviceContext);
    Safe_Release(m_pDevice);
}
