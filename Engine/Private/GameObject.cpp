#include "Engine_pch.h"
#include "GameObject.h"
#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"
#include "ActionState.h"
#include "Model.h"
#include "Camera.h"
#include "GameInstance.h"

uint64 CGameObject::s_iNextID = 0;

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
    , m_iObjectID(++s_iNextID)
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
    m_bAwaked = true;
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

/// <summary>
/// <para>스크립트 컴포넌틀을 참조하는 함수</para>
/// RefCount책임은 외부에
/// </summary>
/// <param name="wstrComponentTag"></param>
/// <returns></returns>
inline CMonoBehaviour* CGameObject::Get_Script_Component(const wstring& wstrComponentTag)
{
    unordered_map<wstring, CMonoBehaviour*>::iterator itr = m_ScriptComponents.find(wstrComponentTag);
    if (itr == m_ScriptComponents.end())
        return nullptr;
    else
        return itr->second;
}

/// <summary>
/// <para>스크립트 컴포넌트를 탈착하는 함수</para>
/// <para>Remove_Component와 달리 Safe_Release하지 않고 슬롯에서 인스턴스를 빼옴</para>
/// <para>이때, Owner는 자동적으로 nullptr 처리</para>
/// 해당 Component의 생명주기는 호출자에게 달려있음
/// </summary>
/// <param name="wstrComponentTag"></param>
/// <returns></returns>
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

/// <summary>
/// <para>스크립트 컴포넌트를 추가하는 함수</para>
/// 파라미터로 인스턴스를 받아 추가한다.
/// </summary>
/// <param name="wstrComponentTag">추가할 컴포넌트 태그</param>
/// <param name="pComp"></param>
/// <returns></returns>
HRESULT CGameObject::Add_Script_Component(const wstring& wstrComponentTag, CMonoBehaviour* pComp)
{
    if (nullptr == pComp || wstrComponentTag.empty())
        return E_FAIL;

    m_ScriptComponents.insert(unordered_map<wstring, CMonoBehaviour*>::value_type(wstrComponentTag, pComp));
    pComp->Set_Owner(this);
    return S_OK;
}

/// <summary>
/// <para>스크립트 컴포넌트를 추가하는 함수</para>
/// <para>내부적으로 원본을 복사 생성하여 추가</para>
/// <para>동일한 태그의 컴포넌트가 존재한다면 E_FAIL</para>
/// 스크립트 컴포넌트를 보관하는 LevelID는 STATIC으로 간주
/// </summary>
/// <param name="wstrComponentTag">추가할 컴포넌트 태그</param>
/// <param name="wstrPrototypeTag">원본 태그</param>
/// <param name="pArg">Desc</param>
/// <returns></returns>
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

/// <summary>
/// 스크립트 컴포넌트를 삭제하는 함수
/// </summary>
/// <param name="wstrComponentTag">삭제할 컴포넌트 태그</param>
void CGameObject::Remove_Script_Component(const wstring& wstrComponentTag)
{
    unordered_map<wstring, CMonoBehaviour*>::iterator itr = m_ScriptComponents.find(wstrComponentTag);
    if (itr == m_ScriptComponents.end())
        return;

    m_ScriptComponents.erase(itr);
}

/// <summary>
/// <para> 컴포넌트를 교체하는 함수 </para>
/// Tag와 일치한 컴포넌트를 삭제하고 추가한다.
/// </summary>
/// <param name="wsrtTargetComponentTag"></param>
/// <param name="pComp"></param>
/// <returns></returns>
HRESULT CGameObject::Change_Script_Component(const wstring& wsrtTargetComponentTag, CMonoBehaviour* pComp)
{
    Remove_Script_Component(wsrtTargetComponentTag);
    return Add_Script_Component(wsrtTargetComponentTag, pComp);
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

/// <summary>
/// <para>객체 스스로가 호출하는 지연 삭제 함수, 삭제는 다음프레임에, Is_Dead()는 현재프레임에 확인 가능</para>
/// <para>기본 동작은 GameInstance에 접근해서 현재 레벨의 Index를 파라미터로 전달</para>
/// Static레벨에 속한 오브젝트, 풀오브젝트는 반드시 상속받아서 내부에서 호출하는 Request_DeleteGameObject 파라미터를 변경하여 요청할것을 권장
/// </summary>
/// <param name="wstrLayerTag">내가 속한 레이어의 태그</param>
void CGameObject::Set_Dead(const wstring& wstrLayerTag)
{
    m_bDead = true;
    m_pGameInstance->Request_DeleteGameObject(m_pGameInstance->Get_CurrentLevelIndex(), wstrLayerTag, this);
}

/// <summary>
/// 부착된 Collider의 Center를 호출 해주는 유틸 함수
/// </summary>
/// <param name="eType">콜라이더 타입</param>
/// <param name="pBounding">바운딩 포인터</param>
/// <returns></returns>
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

void CGameObject::Set_Name(const string& strName)
{
    m_strName = strName;
}

void CGameObject::Set_Name(const wstring& wstrName)
{
    m_strName = Engine_Utils::ToString(wstrName);
}

string CGameObject::Get_Name()
{
    return m_strName;
}

wstring CGameObject::Get_WName()
{
    return Engine_Utils::ToWString(m_strName);
}

/// <summary>
/// <para>ScriptComponent를 일괄적으로 Update호출하는 함수</para>
/// GameObject Update에서 Default로 호출되고 있다.
/// </summary>
/// <param name="fTimeDelta"></param>
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
