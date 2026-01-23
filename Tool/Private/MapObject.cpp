#include "MapObject.h"
#include "Model.h"
#include "Shader.h"
#include "StaticModel.h"

USING(Tool)

CMapObject::CMapObject(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : CToolObject(eType,pDevice,pDeviceContext)
{
}

CMapObject::CMapObject(const CMapObject& rhs)
    : CToolObject(rhs) , m_wstrModelName(rhs.m_wstrModelName) , m_eMapObjectType(rhs.m_eMapObjectType)
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

    /* Mesh 확장자로 바껴있읉첸디 */
    m_wstrModelName = pDesc->wstrModelTag;

    if (FAILED(CMapObject::Ready_Component()))
        return E_FAIL;

    return S_OK;
}

HRESULT CMapObject::Ready_Component()
{
    if (m_eMapObjectType == EMapObject_Type::STATICMODEL)
    {
        /*Static Model 인경우 Model을 바로 생성해준다*/
        CModel::MODEL_ORIGIN_DESC tModelDesc{};
        tModelDesc.wstrModelFolderName  = m_wstrModelName;
        tModelDesc.iPrototypeLevelIndex = ENUM_TO_UINT(ELevelType::MAP);
        tModelDesc.eType = EModelType::STATIC;
        /* Static Model 인경우 */
    }
    else
    {

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

}

HRESULT CMapObject::Render()
{
    if (FAILED(Super::Render()))
        return E_FAIL;

    CModel*     pModel = CGameObject::Get_Component<CModel>();
    CShader*    pShader = CGameObject::Get_Component<CShader>();

    if (!pModel || !pShader ) return E_FAIL;

    UINT32 iMeshCount = pModel->Get_MeshCount();


    for (UINT32 i = 0; i < iMeshCount; ++i)
    {
        pModel->Bind_Material(pShader,i);
        pShader->Apply();
        pModel->Render(i);
    }


    return S_OK;
}


void CMapObject::Free()
{ 
    Super::Free();



}
