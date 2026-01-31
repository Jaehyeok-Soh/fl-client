#include "pch.h"
#include "MapObject.h"
#include "Model.h"
#include "Shader.h"
#include "StaticModel.h"
#include "Engine_Utils.h"
#include "GameInstance.h"
USING(Tool)

CMapObject::CMapObject(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : CToolObject(eType, pDevice, pDeviceContext), m_vImGuiPitchYawRoll{}, m_isLoaded{ false }, m_isRegisterSRT{false}
{
}

CMapObject::CMapObject(const CMapObject& rhs)
    : CToolObject(rhs), m_eMapObjectType(rhs.m_eMapObjectType), m_isLoaded(rhs.m_isLoaded), m_vImGuiPitchYawRoll{}
    , m_isRegisterSRT{false}
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

    m_strModelFileName        = Engine_Utils::ToString(pDesc->wstrModelPath);
    m_strName                 = Engine_Utils::ToString(pDesc->wstrModelName);
    m_isLoaded                = pDesc->isLoaded;
    m_eMapObjectState         = pDesc->eState;

    if (FAILED(CMapObject::Ready_Component()))
        return E_FAIL;


    if (m_isLoaded)
    {
        Register_OriginSRT( EReset_Type::S | EReset_Type::R | EReset_Type::T);
    }

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

void CMapObject::Reset_SRT(Engine::Flags fResetTypeFlag)
{
    if (!m_isLoaded && !m_isRegisterSRT)
    {
        MSG_BOX(" None Load Or None Register SRT");
        return;
    }

    CTransform* pTransfrom = Get_Component<CTransform>();
    if (pTransfrom == nullptr) return;


    if (Engine_Utils::Has_Flag(fResetTypeFlag, ENUM_TO_UINT(EReset_Type::S)))
        pTransfrom->Set_Scale(m_vOriginScale);
    if (Engine_Utils::Has_Flag(fResetTypeFlag, ENUM_TO_UINT(EReset_Type::R)))
        pTransfrom->Rotation(XMConvertToRadians(m_vOriginDegree.x), XMConvertToRadians(m_vOriginDegree.y), XMConvertToRadians(m_vOriginDegree.z));
    if (Engine_Utils::Has_Flag(fResetTypeFlag, ENUM_TO_UINT(EReset_Type::T)))
        pTransfrom->Set_Info(TRANSFORM_INFO_STATE::POS , m_vOriginPosition);

    return;
}

void CMapObject::Register_OriginSRT(Engine::Flags fResetTypeFlag)
{
    m_isRegisterSRT = true;


    CTransform* pTransfrom = Get_Component<CTransform>();
    if (pTransfrom == nullptr) return;

    Vec3 vScale{};
    Quat vRotation{};
    Vec3 vPos{};

    Matrix WorldMatrix = pTransfrom->Get_WorldMatrix();
    WorldMatrix.Decompose(vScale , vRotation , vPos);

    if (Engine_Utils::Has_Flag(fResetTypeFlag, ENUM_TO_UINT(EReset_Type::S)))
        m_vOriginScale = pTransfrom->Get_Scaled();
    if (Engine_Utils::Has_Flag(fResetTypeFlag, ENUM_TO_UINT(EReset_Type::R)))
        m_vOriginDegree = vRotation.ToEuler() * To_DEGREE;
    if (Engine_Utils::Has_Flag(fResetTypeFlag, ENUM_TO_UINT(EReset_Type::T)))
        m_vOriginPosition = vPos;
}

Vec3 CMapObject::Get_OriginScale()
{
    return m_vOriginScale;
}

Vec3 CMapObject::Get_OriginDegree()
{
    return m_vOriginDegree;
}

Vec3 CMapObject::Get_OriginPosition()
{
    return m_vOriginPosition;
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


    Matrix WorldMatrix = pTransfrom->Get_WorldMatrix();
    Vec3 vScale{}, vPosition;
    Quat vQuat{};
    Vec3 vDegree{};
    
    WorldMatrix.Decompose(vScale, vQuat, vPosition);
    vDegree = vQuat.ToEuler() * To_DEGREE;


#pragma region Scale Setting

    if (ImGui::TreeNode(" Scale Setting "))
    {
        ImGui::Separator();

        if (ImGui::DragFloat3(" Scale ", &vScale.x, 0.01f))
            pTransfrom->Set_Scale(vScale);

        ImGui::Separator();

        ImGui::TreePop();
    }

#pragma endregion

#pragma region Rotation Setting

    if (ImGui::TreeNode(" Rotation Setting "))
    {
        ImGui::SeparatorText(" Pitch Yaw Roll ");

        
        if (ImGui::DragFloat3("Pitch Yaw Roll", &vDegree.x))
            pTransfrom->Rotation(Quat::CreateFromYawPitchRoll(vDegree * TO_RAD));

        ImGui::Separator();

        ImGui::SeparatorText(" Quaternion ");

        if (ImGui::DragFloat4(" Quaternion ", &vQuat.x))
            pTransfrom->Rotation(vQuat);

        ImGui::Separator();

        ImGui::TreePop();
    }
#pragma endregion

#pragma region Position Setting

    if (ImGui::TreeNode(" Position Setting "))
    {
        ImGui::Separator();

        if (ImGui::DragFloat3(" Position ", &vPosition.x, 0.05f))
            pTransfrom->Set_Scale(vScale);

        ImGui::Separator();

        ImGui::TreePop();
    }

#pragma endregion

    return;
}




void CMapObject::Free()
{ 
    Super::Free();



}
