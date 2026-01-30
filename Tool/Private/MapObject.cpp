#include "pch.h"
#include "MapObject.h"
#include "Model.h"
#include "Shader.h"
#include "StaticModel.h"
#include "Engine_Utils.h"
#include "GameInstance.h"
USING(Tool)

CMapObject::CMapObject(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : CToolObject(eType, pDevice, pDeviceContext), m_vImGuiPitchYawRoll{}
{
}

CMapObject::CMapObject(const CMapObject& rhs)
    : CToolObject(rhs), m_eMapObjectType(rhs.m_eMapObjectType), m_isLoaded(rhs.m_isLoaded), m_vImGuiPitchYawRoll{}
{
}

HRESULT CMapObject::Initialize_Prototype()
{
    if (FAILED(Super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CMapObject::Initialize(void* pArg)
{
    if(FAILED(Super::Initialize(pArg)))
        return E_FAIL;

    CMapObject::MAPOBJECT_DESC* pDesc = static_cast<CMapObject::MAPOBJECT_DESC*>(pArg);

    m_strModelFileName = Engine_Utils::ToString(pDesc->wstrModelPath);
    m_strName          = Engine_Utils::ToString(pDesc->wstrModelName);
    m_isLoaded         = pDesc->isLoaded;

    if (FAILED(CMapObject::Ready_Component()))
        return E_FAIL;

    return S_OK;
}

HRESULT CMapObject::Ready_Component()
{
    CModel::MODEL_COPY_DESC tDesc{};

    if (FAILED(Add_Component<CShader>(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Shader_VtxMesh", nullptr)))
        return E_FAIL;

    /* CStatic_Model Type ÀÌ¶ó¸é */
    if (m_eMapObjectType == EMapObject_Type::STATICMODEL)
    {
        CModel::MODEL_ORIGIN_DESC tModelDesc{};
        tModelDesc.eType = EModelType::STATIC;
        tModelDesc.wstrModelFolderName = Engine_Utils::ToWString(m_strModelFileName);
        tModelDesc.iPrototypeLevelIndex = ENUM_TO_UINT(ELevelType::MAP);
        CModel* pModel = CModel::Create(m_pDevice, m_pDeviceContext , &tModelDesc);
        if (pModel)
        {
            if (FAILED(m_pGameInstance->Add_Prototype(tModelDesc.iPrototypeLevelIndex, L"Prototype_Component_Model_" + Engine_Utils::ToWString(m_strName), pModel)))
                Safe_Release(pModel);
        }
        CModel::MODEL_COPY_DESC tModelCopyDesc{};
        CGameObject::Add_Component<CModel>(tModelDesc.iPrototypeLevelIndex, L"Prototype_Component_Model_" + Engine_Utils::ToWString(m_strName), &tModelCopyDesc);
    }


    return S_OK;
}

HRESULT CMapObject::Awake(const _uint iCurrentLevelID)
{
    if (FAILED(Super::Awake(iCurrentLevelID)))
        return E_FAIL;


    return S_OK;
}

void CMapObject::Update_Priority(const _float fTimeDelta)
{
    Super::Update_Priority(fTimeDelta);
}

void CMapObject::Update(const _float fTimeDelta)
{
    Super::Update(fTimeDelta);

}

void CMapObject::Update_Late(const _float fTimeDelta)
{
    Super::Update_Late(fTimeDelta);

}

void CMapObject::Ready_Before_Render(const _float fTimeDelta)
{
    Super::Ready_Before_Render(fTimeDelta);


    m_pGameInstance->Push_RenderObject(RENDER_CATEGORY::NONEBLEND , this);
}

HRESULT CMapObject::Render()
{
    if (FAILED(Super::Render()))
        return E_FAIL;

    CModel*     pModel = CGameObject::Get_Component<CModel>();
    CShader*    pShader = CGameObject::Get_Component<CShader>();

    if (!pModel || !pShader ) return S_OK;

    pShader->Bind_TransformData(CGameObject::Get_Component<CTransform>()->Get_WorldMatrix());

    UINT32 iMeshCount = pModel->Get_MeshCount();
    

    for (UINT32 i = 0; i < iMeshCount; ++i)
    {
        pModel->Bind_Material(pShader,i);
        pModel->Bind_MaterialInstance(pShader,i);
        pShader->Apply();
        pModel->Render(i);
    }


    return S_OK;
}

void CMapObject::Draw_ImGui()
{
    Super::Draw_ImGui();

    
    CTransform* pTransfrom = Get_Component<CTransform>();

    if (!pTransfrom) return;


    if (ImGui::TreeNode("Quaternion"))
    {
        Matrix WorldMatrix = pTransfrom->Get_WorldMatrix();
        Vec3 vScale{}, vPosition;
        Quat vQuat{};
        WorldMatrix.Decompose(vScale, vQuat, vPosition);
        ImGui::InputFloat4( "Quat" , &m_vImGuiQuat.x);
        if (ImGui::Button("Ratattion From Quat"))
        {
            pTransfrom->Rotation(m_vImGuiQuat);
        }
        ImGui::TreePop();
    }


    ImGui::InputFloat3("Pitch Yaw Roll", &m_vImGuiPitchYawRoll.x);

    if (ImGui::Button("Rotation"))
    {
        pTransfrom->Rotation(XMConvertToRadians(m_vImGuiPitchYawRoll.x), XMConvertToRadians(m_vImGuiPitchYawRoll.y), XMConvertToRadians(m_vImGuiPitchYawRoll.z));
        ZeroMemory(&m_vImGuiPitchYawRoll, sizeof(Vec3));
    }




    return;
}




void CMapObject::Free()
{ 
    Super::Free();



}
