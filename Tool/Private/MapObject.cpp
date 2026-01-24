#include "MapObject.h"
#include "Model.h"
#include "Shader.h"
#include "GameInstance.h"
#include "StaticModel.h"

USING(Tool)

CMapObject::CMapObject(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : CToolObject(eType,pDevice,pDeviceContext)
{
}

CMapObject::CMapObject(const CMapObject& rhs)
    : CToolObject(rhs) , m_wstrModelPath(rhs.m_wstrModelPath) , m_eMapObjectType(rhs.m_eMapObjectType) , m_isLoaded(rhs.m_isLoaded)
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

    /* Mesh È®ÀåÀÚ·Î ¹Ù²¸ÀÖŸ¿Ã¾µð */
    m_wstrModelPath = pDesc->wstrModelPath;
    m_isLoaded = pDesc->isLoaded;


    if (FAILED(CMapObject::Ready_Component()))
        return E_FAIL;





    return S_OK;
}

HRESULT CMapObject::Ready_Component()
{
    CModel::MODEL_COPY_DESC tDesc{};

    if (FAILED(Add_Component<CShader>(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Shader_VtxMesh", nullptr)))
        return E_FAIL;

    if (FAILED(Add_Component<CModel>(ENUM_TO_UINT(ELevelType::MAP), L"Prototype_Component_Model_" + m_wstrModelPath, &tDesc)))
        return S_OK;


    CTransform* pTransform = Get_Component<CTransform>();

    //Vec3 vPos = pTransform->Get_Info(TRANSFORM_INFO_STATE::POS);
    //std::swap(vPos.y,vPos.z);
    //pTransform->Set_Info(TRANSFORM_INFO_STATE::POS , vPos);



    //if (m_eMapObjectType == EMapObject_Type::STATICMODEL)
    //{
    //    if (m_isLoaded = true)
    //    {
    //    }
    //    else
    //    {
    //        Add_Component<CModel>(ENUM_TO_UINT(ELevelType::MAP), L"Prototype_Component_Model_" + m_wstrModelPath, &tDesc);
    //    }
    //}
    //else
    //{

    //}

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

    Matrix WorldMatrix = pTransfrom->Get_WorldMatrix();
    
    Vec3 vPosition  = WorldMatrix.Translation();
    Vec3 vRotation  = WorldMatrix.ToEuler();
    Vec3 vScale     = pTransfrom->Get_Scaled();


    if (ImGui::InputFloat3("Position" , &vPosition.x ))
        pTransfrom->Set_Info(TRANSFORM_INFO_STATE::POS, vPosition);
    if (ImGui::InputFloat3("Rotation", &vRotation.x))
        return;
    if (ImGui::InputFloat3("Scale", &vScale.x))
        pTransfrom->Set_Scale(vScale);

    return;
}




void CMapObject::Free()
{ 
    Super::Free();



}
