#include "pch.h"
#include "ToolObject.h"
#include "Model.h"
#include "MaterialInstance.h"
#include "Collider.h"
#include "Picking_ToolManager.h"
#include "MapObject.h"
#include "GameInstance.h"
#include "LevelData.h"

CToolObject::CToolObject(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : Super(pDevice, pDeviceContext)
    , m_eType (eType)
{
}

CToolObject::CToolObject(const CToolObject& rhs)
    : Super(rhs)
    , m_bVisible(rhs.m_bVisible)
    , m_strObjectType(rhs.m_strObjectType)
    , m_eType(rhs.m_eType)
{
}


HRESULT CToolObject::Initialize_Prototype()
{
    if (FAILED(Super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CToolObject::Initialize(void* pArg)
{
    if (FAILED(Super::Initialize(pArg)))
        return E_FAIL;

    TOOLOBJECT_DESC* pDesc = static_cast<TOOLOBJECT_DESC*>(pArg);
    m_wstrLayerTag = pDesc->wstrLayerTag;

    if (FAILED(Set_TypeString()))
        return E_FAIL;

    return S_OK;
}

HRESULT CToolObject::Awake(const _uint iCurrentLevelID)
{
    if (FAILED(Super::Awake(iCurrentLevelID)))
        return E_FAIL;

    Set_Visible();
    return S_OK;
}

void CToolObject::Update_Priority(const _float fTimeDelta)
{
    Super::Update_Priority(fTimeDelta);
}

void CToolObject::Update(const _float fTimeDelta)
{
    Super::Update(fTimeDelta);
}

void CToolObject::Update_Late(const _float fTimeDelta)
{
    Super::Update_Late(fTimeDelta);
    if (Is_Visible())
        CPicking_ToolManager::GetInstance()->Add_PickingGroup(this);
}

void CToolObject::Ready_Before_Render(const _float fTimeDelta)
{
    Super::Ready_Before_Render(fTimeDelta);
    if (Is_Visible())
        CPicking_ToolManager::GetInstance()->Remove_PickingGroup(this);
}

HRESULT CToolObject::Render()
{
    if (FAILED(Super::Render()))
        return E_FAIL;

    return S_OK;
}

void CToolObject::Draw_ImGui()
{
    
}

void CToolObject::Set_Dead(_bool bStatic)
{
    Super::Set_Dead(bStatic);
    m_pGameInstance->Broadcast<ChangeSelectedObject>(nullptr);
}

void CToolObject::Set_WorldMatrix(const Matrix& WorldMatrix)
{
    Get_Component<CTransform>()->Set_WorldMatrix(WorldMatrix);
}

void CToolObject::Set_WorldMatrix(const Vec3& vScale, const Quat& vQuat, const Vec3& vPosition)
{
    Matrix matWorld = Matrix::CreateScale(vScale) * Matrix::CreateFromQuaternion(vQuat) * Matrix::CreateTranslation(vPosition);
    Get_Component<CTransform>()->Set_WorldMatrix(matWorld);
}

Matrix CToolObject::Get_WorldMatrix()
{
    return Get_Component<CTransform>()->Get_WorldMatrix();
}


bool CToolObject::Get_SRT(OUT Vec3& vOutScale, OUT Quat& vQuat, OUT Vec3& vPosition)
{
    Matrix WorldMatrix = Get_Component<CTransform>()->Get_WorldMatrix();
    return WorldMatrix.Decompose(vOutScale , vQuat, vPosition);
}


void CToolObject::Set_Visible()
{
    m_bVisible = true;
}

void CToolObject::Set_Invisible()
{
    m_bVisible = false;
}

void CToolObject::Update_CombinedWorldMatrix(const Matrix& matParent)
{
}

HRESULT CToolObject::Set_TypeString()
{
    m_strObjectType = TypeToString(m_eType);
    HRESULT hr = { S_OK };
    if (::strcmp(m_strObjectType.c_str(), "NONE") == 0)
        hr = E_FAIL;

    return hr;
}

void CToolObject::Free()
{
    Super::Free();
}