#include "Tool_Defines.h"
#include "GameInstance.h"
#include "Model.h"
#include "MaterialInstance.h"
#include "Collider.h"
#include "Engine_Utils.h"
#include "Picking_ToolManager.h"
#include "ToolObject.h"

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

void CToolObject::Set_Dead(const wstring& wstrLayerTag)
{
    Super::Set_Dead(m_wstrLayerTag);
    m_pGameInstance->Broadcast<ChangeSelectedObject>(nullptr);
}

void CToolObject::Set_Visible()
{
    m_bVisible = true;
}

void CToolObject::Set_Invisible()
{
    m_bVisible = false;
}

void CToolObject::Export_TransformData(OUT TRANSFORM_SAVEDATA& data)
{
    _matrix matWorld = ::XMLoadFloat4x4(&Get_Component<CTransform>()->Get_WorldMatrix());
    _vector vPos = {};
    _vector vQuaternion = {};
    _vector vScale = {};
    ::XMMatrixDecompose(&vScale, &vQuaternion, &vPos, matWorld);
    ::XMStoreFloat3(&data.vPos, vPos);
    ::XMStoreFloat3(&data.vScale, vScale);
    ::XMStoreFloat4(&data.vQuaternion, vQuaternion);
}

void CToolObject::Export_ModelData(OUT MODEL_SAVEDATA& data)
{
    data.fileName = m_strModelFileName;
    CModel* pModel = Get_Component<CModel>();
    _uint iMaterialCount = pModel->Get_MaterialCount();
    data.vecMI.reserve(iMaterialCount);
    for (_uint i = 0; i < iMaterialCount; ++i)
    {
        data.vecMI.push_back(pModel->Get_MaterialInstance(i)->Get_MIType());
        data.vecShaderPassesByMesh.push_back(pModel->Get_PassByMesh(i));
    }
}

void CToolObject::Set_Name(const string& strName)
{
    m_strName = strName;
}

void CToolObject::Set_Name(const wstring& wstrName)
{
    m_strName = Engine_Utils::ToString(wstrName);
}

void CToolObject::Set_Name(const string& strName, _uint iValue)
{
    m_strName = strName + "_[" + std::to_string(iValue) + "]";
}

void CToolObject::Set_Name(const wstring& wstrName, _uint iValue)
{
    m_strName = Engine_Utils::ToString(wstrName) + "_[" + std::to_string(iValue) + "]";
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


