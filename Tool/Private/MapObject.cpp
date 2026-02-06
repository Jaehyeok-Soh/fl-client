#include "MapObject.h"
#include "pch.h"
#include "Model.h"
#include "Shader.h"
#include "StaticModel.h"
#include "Engine_Utils.h"
#include "GameInstance.h"
#include "Material.h"
#include "AsTypes.h"

USING(Tool)

CMapObject::CMapObject(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : CToolObject(eType, pDevice, pDeviceContext), m_isLoaded{ false }, m_vecOverrideMaterials{} , m_iUseOverrideMaterials{false}
{
    m_arrayMapToolComponent.fill(nullptr);
}


CMapObject::CMapObject(const CMapObject& rhs)
    : CToolObject(rhs), m_eMapObjectType(rhs.m_eMapObjectType), m_isLoaded(rhs.m_isLoaded), m_vecOverrideMaterials{rhs.m_vecOverrideMaterials}
     ,m_iUseOverrideMaterials(rhs.m_iUseOverrideMaterials)
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
    m_isLoaded                = pDesc->isLoaded;
    m_eMapObjectState         = pDesc->eState;


    if (FAILED(CMapObject::Ready_Component()))
        return E_FAIL;


    return S_OK;
}

HRESULT CMapObject::Ready_Component()
{
    return S_OK;
}


HRESULT CMapObject::Ready_OverrideMtl(const USING_MODEL_INFO& tUsingModelInfo)
{

    if (tUsingModelInfo.vecOverrideMaterial.empty())
        m_iUseOverrideMaterials = false;
    else
    {
        for (auto& OverrideMtl : tUsingModelInfo.vecOverrideMaterial)
        {
            if (!OverrideMtl.isNull)
                m_iUseOverrideMaterials = true;
        }
    }

    if (!m_iUseOverrideMaterials) return S_OK;

    size_t iSizeMtl = Get_Component<CModel>()->Get_MaterialCount();
    size_t iSizeeOverrideMtl = tUsingModelInfo.vecOverrideMaterial.size();

    m_vecOverrideMaterials.resize(max(iSizeMtl, iSizeeOverrideMtl));


    CTextureBase::RESOURCE_BASE_DESC tResourceTextureOriginDecs{};

    CMaterial::MATERIAL_DESC tDesc{};

    /* 여기다가 경로를 집어넣어서 진행해주면된다 */
    vector<std::string> vecMateiralTexturePath{};
    vecMateiralTexturePath.resize(ENUM_TO_UINT(EMaterialTextureType::MAX_COUNT));

    _uint iIndex{};
    for (auto& OverrideMtl : tUsingModelInfo.vecOverrideMaterial)
    {
        if (OverrideMtl.isNull)
        {
            iIndex++;
            continue;
        }

        for (auto& pairTexturePath : OverrideMtl.vecUsingTextureInfo)
        {
            tResourceTextureOriginDecs.wstrPath = pairTexturePath.second;
            tResourceTextureOriginDecs.wstrName = path(pairTexturePath.second).filename().stem().wstring();
            CTextureBase* pBase = m_pGameInstance->GetOrAddTexture(tResourceTextureOriginDecs.wstrName, &tResourceTextureOriginDecs);
            Safe_Release(pBase);

            vecMateiralTexturePath[Get_IndexByMaterialSlotName(pairTexturePath.first)]
                = Engine_Utils::ToString(tResourceTextureOriginDecs.wstrName);
        }

        CMaterial* pMtl = m_pGameInstance->Get_Resource<CMaterial>(OverrideMtl.wstrMtl_JsonFile_Name);
        if (pMtl == nullptr)
        {
            tDesc.wstrName = OverrideMtl.wstrMtl_JsonFile_Name;
            tDesc.wstrPath = OverrideMtl.wstrMtl_JsonFile_Path;
            tDesc.spanTags = vecMateiralTexturePath;
            m_pGameInstance->Add_Resource<CMaterial>(tDesc.wstrName, CMaterial::Create(m_pDevice, m_pDeviceContext, &tDesc));
            pMtl = m_pGameInstance->Get_Resource<CMaterial>(OverrideMtl.wstrMtl_JsonFile_Name);
        }

        if (pMtl == nullptr)  return E_FAIL;
        /* Override Mtl 값 가져오기 */
        m_vecOverrideMaterials[iIndex++] = pMtl;

        /* 비우기 */
        std::fill(vecMateiralTexturePath.begin(), vecMateiralTexturePath.end(), "");
    }

    return S_OK;

}

HRESULT CMapObject::Add_MapToolComponent(CMapObject::COMPONENT eType)
{
    return S_OK;
}

void CMapObject::Reset_OriginTransform(_uint iIndex)
{
    if (iIndex >= m_vecOriginSRTs.size())
        return;

    CTransform* pTransform = Get_Component<CTransform>();

    pTransform->Set_WorldMatrix( m_vecOriginSRTs[iIndex].Get_World() );
}

void CMapObject::Override_OriginTransform(_uint iIndex)
{
    if (iIndex >= m_vecOriginSRTs.size())
        return;

    CTransform* pTransform = Get_Component<CTransform>();

    SimpleMath::Matrix WorldMatrix = pTransform->Get_WorldMatrix();
    SRT_DATA& tChangeSRT = m_vecOriginSRTs[iIndex];
    WorldMatrix.Decompose(tChangeSRT.vScale,tChangeSRT.vQuat,tChangeSRT.vPosition);
}

SimpleMath::Matrix CMapObject::Get_OriginTransform(_uint iIndex)
{
    return m_vecOriginSRTs[iIndex].Get_World();
}



vector<wstring> CMapObject::Get_OverrideMtlsName() const
{
    if (m_iUseOverrideMaterials == false)
        return vector<wstring>();

    vector<wstring> vecResult{};

    for (auto& Mtl : m_vecOverrideMaterials)
    {
        if (Mtl == nullptr)
            vecResult.push_back(0);
        else
            vecResult.push_back(Mtl->Get_Name());
    }

    return vecResult;
}

vector<wstring> CMapObject::Get_TotalUseMtlsName()
{
    vector<wstring> vecResult{};
    
    CModel* pModel = Get_Component<CModel>();
    _uint iCount = Get_Component<CModel>()->Get_MaterialCount();
    
    if (m_iUseOverrideMaterials == false)
    {
        for (_uint i = 0; i < iCount; ++i)
            vecResult.push_back(pModel->Get_MaterialName(i));
        return vecResult;
    }


    for (_uint i = 0; i < iCount; ++i)
    {
        if (m_vecOverrideMaterials[i])
            vecResult.push_back(m_vecOverrideMaterials[i]->Get_Name());
        else
            vecResult.push_back(pModel->Get_MaterialName(i));
    }

    return vecResult;
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
    for (auto& OverrideMtl : m_vecOverrideMaterials)
        Safe_Release(OverrideMtl);

    Super::Free();


}
