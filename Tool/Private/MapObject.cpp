#include "pch.h"
#include "MapObject.h"
#include "Material.h"
#include "AsTypes.h"
#include "Shader.h"
#include "Model.h"
#include "Mesh.h"
#include "MapToolManager.h"
#include "Bounds.h"
#include "InstanceMesh.h"
#include "Engine_Utils.h"
#include "DataDocument_Map.h"
#include "DataStruct_Map.h"
#include "GameInstance.h"


USING(Tool)

CMapObject::CMapObject(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : CToolObject(eType, pDevice, pDeviceContext), m_isLoaded{ false }, m_vecOverrideMaterials{} , m_isUseOverrideMaterials{false}
    , m_eMapObjectDrawType{ EMapObject_DrawType::Default }, m_eClientMakePath{ EClientMakePath::END }, m_eClientLevelType{ EClientLevelType::END }
    , m_eMapObjectState{ CMapObject::EState::Default }, m_iSelectedInstanceID{ 0 }, m_vecOriginSRTs{}
    , m_wstrModelPath{L""}
    , m_vecClientMakePathDesc{}
{

}


CMapObject::CMapObject(const CMapObject& rhs)
    : CToolObject(rhs), m_eMapObjectType(rhs.m_eMapObjectType), m_isLoaded(rhs.m_isLoaded), m_vecOverrideMaterials{rhs.m_vecOverrideMaterials}
     , m_isUseOverrideMaterials(rhs.m_isUseOverrideMaterials) , m_eMapObjectDrawType{rhs.m_eMapObjectDrawType }
    , m_eClientMakePath{rhs.m_eClientMakePath}
    , m_eClientLevelType{rhs.m_eClientLevelType }
    , m_vecClientMakePathDesc{rhs.m_vecClientMakePathDesc }
    , m_eMapObjectState{rhs.m_eMapObjectState }
    , m_iSelectedInstanceID{rhs.m_iSelectedInstanceID }
    , m_vecOriginSRTs{rhs.m_vecOriginSRTs }
    , m_vecSRTs{rhs.m_vecSRTs }
    , m_wstrModelPath(rhs.m_wstrModelPath)
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
    m_isUELoaded                = pDesc->isUELoaded;
    m_isLoaded                  = pDesc->isLoaded;
    m_eMapObjectState           = pDesc->eState;
    m_eMapObjectDrawType        = pDesc->eMapObjectDrawType;
    m_eClientMakePath           = pDesc->eClientMakePath;
    m_eClientLevelType          = pDesc->eClientLevelType;
    m_eMapObjectDrawType        = pDesc->eMapObjectDrawType;
    

    /* Model != None */
    if (m_eMapObjectDrawType != EMapObject_DrawType::Collider)
    {
        /* Path 값과 모델 네임 입력 */
        m_strModelFileName = Engine_Utils::ToString(path(pDesc->tUsingModelInfo.wstrName));
        m_wstrModelPath = pDesc->tUsingModelInfo.wstrPath;
        Set_Name(m_strModelFileName);
    }
    

    if (FAILED(Ready_SRTDatas(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Component()))
        return E_FAIL;

    if (FAILED(Ready_ClientMakePath(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_OverrideMtl(pDesc->tUsingModelInfo)))
        return E_FAIL;

    return S_OK;
}

HRESULT CMapObject::Ready_SRTDatas(CMapObject::MAPOBJECT_DESC* pDesc)
{

    /* 초기값 두개 보존 */
    m_vecSRTs.insert(m_vecSRTs.end() , pDesc->vecSRTs.begin() , pDesc->vecSRTs.end());
    m_vecOriginSRTs.insert(m_vecOriginSRTs.end() , pDesc->vecSRTs.begin() , pDesc->vecSRTs.end());

    _uint iCount = ENUM_TO_UINT(pDesc->vecSRTs.size());

    m_vecSRTs.resize(iCount);
    m_vecOriginSRTs.resize(iCount);


    /* SRT Data 대입 */
    for (_uint i = 0; i < iCount; ++i)
    {
        m_vecSRTs[i] = pDesc->vecSRTs[i];
        m_vecSRTs[i].Update_World();
        m_vecOriginSRTs[i] = pDesc->vecSRTs[i];
        m_vecOriginSRTs[i].Update_World();
    }

    if (m_eMapObjectDrawType != EMapObject_DrawType::Instance)
        Get_Component<CTransform>()->Set_WorldMatrix(m_vecSRTs[m_iSelectedInstanceID].Get_World());
   

    return S_OK;
}

HRESULT CMapObject::Ready_Component()
{
    CModel::MODEL_COPY_DESC tDesc{};

    /* ReadyShader Shader */

    m_eMapObjectDrawType == EMapObject_DrawType::Instance ?
                Add_Component<CShader>(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Shader_VtxInstanceMesh", nullptr)
            :   Add_Component<CShader>(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Shader_VtxMesh", nullptr);
     

    /* 먼저 File Name으로 가져오고 */

    CModel::MODEL_ORIGIN_DESC tModelDesc{};
    tModelDesc.eType = EModelType::STATIC;
    tModelDesc.wstrModelFolderName = m_wstrModelPath;
    tModelDesc.iPrototypeLevelIndex = ENUM_TO_UINT(ELevelType::MAP);
    CModel* pModel = CModel::Create(m_pDevice, m_pDeviceContext, &tModelDesc);
    if (pModel)
    {
        if (FAILED(m_pGameInstance->Add_Prototype(tModelDesc.iPrototypeLevelIndex, L"Prototype_Component_Model_" + Engine_Utils::ToWString(m_strName), pModel)))
            Safe_Release(pModel);
    }
    CModel::MODEL_COPY_DESC tModelCopyDesc{};
    Add_Component<CModel>(tModelDesc.iPrototypeLevelIndex, L"Prototype_Component_Model_" + Engine_Utils::ToWString(m_strName), &tModelCopyDesc);


    /* 인스턴싱 용이면 Instance Mesh 생성 */
    if (m_eMapObjectDrawType == EMapObject_DrawType::Instance)
    {
        /* InstanceMehs  */
        CInstanceMesh::INSTANCEMESH_DESC tInstanceMeshDesc{};
        tInstanceMeshDesc.IB_Usage = D3D11_USAGE_DYNAMIC;
        tInstanceMeshDesc.VB_Usage = D3D11_USAGE_DYNAMIC;

        vector<Matrix> vecMatrix{};
        vecMatrix.reserve(m_vecSRTs.size());
        for (auto& SRTs : m_vecSRTs)
            vecMatrix.push_back(SRTs.Get_World());
        tInstanceMeshDesc.vecInstanceMatrixPointer = &vecMatrix;
        Add_Component<CInstanceMesh>(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_VIBuffer_InstanceMesh", &tInstanceMeshDesc);
    }


    return S_OK;
}

HRESULT CMapObject::Ready_ClientMakePath(CMapObject::MAPOBJECT_DESC* pDesc)
{
    if (pDesc == nullptr) return E_FAIL;

    m_vecClientMakePathDesc.clear();

    if (m_isLoaded == true)
    {
        /* Desc이 Empty인 상태는 이 Make Path 가 Desc이 필요로 하지않는다 */
        if (pDesc->vecClientMakePathDesc.empty())
            return S_OK;

        for (auto& Desc : pDesc->vecClientMakePathDesc)
        {
            m_vecClientMakePathDesc.push_back(Desc);
        }
    }

    /* Load된게 아니라면 */
    else
    {
        for (auto& SRT : m_vecSRTs)
        {
            /* 동적할당 받아서 나온다  */
            CLIENT_MAKEPATH_DESC_BASE* pDesc = CMapToolManager::GetInstance()->Make_Client_MakePathDesc(m_eClientMakePath);
            if (!pDesc)
                return S_OK;
            m_vecClientMakePathDesc.push_back(pDesc);
        }
    }

    return S_OK;
}


HRESULT CMapObject::Ready_OverrideMtl(const USING_MODEL_INFO& tUsingModelInfo)
{
    if (m_isUELoaded != true)
        return S_OK;

    if (tUsingModelInfo.vecOverrideMaterial.empty())
        m_isUseOverrideMaterials = false;
    else
    {
        for (auto& OverrideMtl : tUsingModelInfo.vecOverrideMaterial)
        {
            if (!OverrideMtl.isNull)
                m_isUseOverrideMaterials = true;
        }
    }

    if (!m_isUseOverrideMaterials) return S_OK;

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
        /* Mtl Json 파일이 있는지 확인 */
        if(!std::filesystem::exists(OverrideMtl.wstrMtl_JsonFile_Path))
        {
            //MessageBox(nullptr  , OverrideMtl.wstrMtl_JsonFile_Path.c_str() , MB_OK , 0);
            continue;
        }

        bool isFailed{false};
        for (auto& pairTexturePath : OverrideMtl.vecUsingTextureInfo)
        {
            tResourceTextureOriginDecs.wstrPath = pairTexturePath.second;
            tResourceTextureOriginDecs.wstrName = path(pairTexturePath.second).filename().stem().wstring();
            CTextureBase* pBase = m_pGameInstance->GetOrAddTexture(tResourceTextureOriginDecs.wstrName, &tResourceTextureOriginDecs);
            if (!pBase)
            {
                /* Mtl Json 파일은 있어서 텍스처경로를 읽었지만 텍스처에 그 경로가 없을경우  */
                MessageBox(nullptr, wstring(L" Ovrride Mtl Make Texture Failed(Check File): " + tResourceTextureOriginDecs.wstrPath).c_str() , MB_OK, 0);
                isFailed = true;
                break;
            }
            else
                Safe_Release(pBase);

            vecMateiralTexturePath[Get_IndexByMaterialSlotName(pairTexturePath.first)]
                = Engine_Utils::ToString(tResourceTextureOriginDecs.wstrName);
        }
        if (!isFailed)
        {
            CMaterial* pMtl = m_pGameInstance->Get_Resource<CMaterial>(OverrideMtl.wstrMtl_JsonFile_Name);

            if (pMtl == nullptr)
            {
                tDesc.wstrName = OverrideMtl.wstrMtl_JsonFile_Name;
                tDesc.wstrPath = OverrideMtl.wstrMtl_JsonFile_Path;
                tDesc.spanTags = vecMateiralTexturePath;
                m_pGameInstance->Add_Resource<CMaterial>(tDesc.wstrName, CMaterial::Create(m_pDevice, m_pDeviceContext, &tDesc));
                pMtl = m_pGameInstance->Get_Resource<CMaterial>(OverrideMtl.wstrMtl_JsonFile_Name);
            }
            m_vecOverrideMaterials[iIndex] = pMtl;
        }

        std::fill(vecMateiralTexturePath.begin(), vecMateiralTexturePath.end(), "");
        ++iIndex;
    }

    return S_OK;

}

HRESULT CMapObject::Change_Instance_To_Default()
{
    /* InstanceMesh 제거 */
    CGameObject::Remove_Component<CInstanceMesh>();
   
    /* Desc션은 삭제하지않는다 */

    return S_OK;
}

HRESULT CMapObject::Add_MapToolComponent(CMapObject::COMPONENT eType)
{
    return S_OK;
}


void CMapObject::Reset_OriginTransform()
{
    if (m_eMapObjectDrawType == EMapObject_DrawType::Collider || m_eMapObjectDrawType == EMapObject_DrawType::Default)
        Get_Component<CTransform>()->Set_WorldMatrix(m_vecOriginSRTs[0].Get_World());

    else if (m_eMapObjectDrawType == EMapObject_DrawType::Instance)
    {
        if (m_iSelectedInstanceID >= m_vecOriginSRTs.size())
            return;

        m_vecSRTs[m_iSelectedInstanceID] = m_vecOriginSRTs[m_iSelectedInstanceID];
        Update_InstanceWorldMatrix(false);
    }

}

void CMapObject::Override_OriginTransform()
{
    if (m_eMapObjectDrawType == EMapObject_DrawType::Collider || m_eMapObjectDrawType == EMapObject_DrawType::Default)
    {
        /* 내 SRT값으로 Origin SRT값 갱신 */
        m_vecOriginSRTs[0] = m_vecSRTs[0];
    }

    else if (m_eMapObjectDrawType == EMapObject_DrawType::Instance)
    {
        if (m_iSelectedInstanceID >= m_vecOriginSRTs.size())
            return;
        m_vecOriginSRTs[m_iSelectedInstanceID] = m_vecSRTs[m_iSelectedInstanceID];
    }
}

void CMapObject::Update_InstanceWorldMatrix(_bool isAllUpdate)
{
    if (m_eMapObjectDrawType != EMapObject_DrawType::Instance)
        return;

    D3D11_MAPPED_SUBRESOURCE SubResource{ nullptr };

    ID3D11Resource* pInstanaceBuffer = Get_Component<CInstanceMesh>()->Get_VBBuffer();
    m_pDeviceContext->Map(pInstanaceBuffer, 0 , D3D11_MAP_WRITE_NO_OVERWRITE , 0 , &SubResource);


    VTX_INSTANCE* pInstance = reinterpret_cast<VTX_INSTANCE*>(SubResource.pData);
    if (!pInstance)
        return;

    if (!isAllUpdate)
    {
        if (m_iSelectedInstanceID >= m_vecSRTs.size())
            return;
        memcpy(&pInstance[m_iSelectedInstanceID], &m_vecSRTs[m_iSelectedInstanceID].WorldMatrix._11, sizeof(Matrix));
    }
    else
    {
        for (_uint i = 0; i < static_cast<_uint>(m_vecSRTs.size()); ++i)
            memcpy(&pInstance[i], &m_vecSRTs[i].WorldMatrix._11, sizeof(Matrix));
    }

    m_pDeviceContext->Unmap(pInstanaceBuffer , 0);

    return;
}

bool CMapObject::Get_SRT(OUT Vec3& vOutScale, OUT Quat& vQuat, OUT Vec3& vPosition)
{
    if (m_iSelectedInstanceID >= m_vecSRTs.size())
        return false;
    vOutScale       = m_vecSRTs[m_iSelectedInstanceID].vScale;
    vQuat           = m_vecSRTs[m_iSelectedInstanceID].vQuat;
    vPosition       = m_vecSRTs[m_iSelectedInstanceID].vPosition;

    return true;
}

Matrix CMapObject::Get_WorldMatrix()
{
    if (m_iSelectedInstanceID >= m_vecSRTs.size())
        return Matrix::Identity;
    return m_vecSRTs[m_iSelectedInstanceID].Get_World();
}

void CMapObject::Set_WorldMatrix(const Vec3& vScale, const Quat& vQuat, const Vec3& vPosition)
{
    Set_SRTData(vScale , vQuat , vPosition);
}

void CMapObject::Set_WorldMatrix(const Matrix& WorldMatrix)
{
    if (m_iSelectedInstanceID >= m_vecSRTs.size())
        return;

    m_vecSRTs[m_iSelectedInstanceID].Update_World(WorldMatrix);

    
    if (m_eMapObjectDrawType == EMapObject_DrawType::Instance)
        Update_InstanceWorldMatrix(false);
    else
        Get_Component<CTransform>()->Set_WorldMatrix(m_vecSRTs[m_iSelectedInstanceID].Get_World());

    return;
}



void CMapObject::Set_Scale(const Vec3& vScale)
{
    if (m_iSelectedInstanceID >= m_vecSRTs.size())
        return;
    
    m_vecSRTs[m_iSelectedInstanceID].Update_Scale(vScale);

    if (m_eMapObjectDrawType == EMapObject_DrawType::Instance)
        Update_InstanceWorldMatrix(false);
    else
        Get_Component<CTransform>()->Set_WorldMatrix(m_vecSRTs[m_iSelectedInstanceID].Get_World());
}

void CMapObject::Set_Position(const Vec3& vPosition)
{
    if (m_iSelectedInstanceID >= m_vecSRTs.size())
        return;

    m_vecSRTs[m_iSelectedInstanceID].Update_Position(vPosition);

    if (m_eMapObjectDrawType == EMapObject_DrawType::Instance)
        Update_InstanceWorldMatrix(false);
    else
        Get_Component<CTransform>()->Set_WorldMatrix(m_vecSRTs[m_iSelectedInstanceID].Get_World());
}

void CMapObject::Set_Quaternion(const Quat& vQuat)
{
    if (m_iSelectedInstanceID >= m_vecSRTs.size())
        return;

    m_vecSRTs[m_iSelectedInstanceID].Update_Quat(vQuat);

    if (m_eMapObjectDrawType == EMapObject_DrawType::Instance)
        Update_InstanceWorldMatrix(false);
    else
        Get_Component<CTransform>()->Set_WorldMatrix(m_vecSRTs[m_iSelectedInstanceID].Get_World());
}



void CMapObject::Set_ClientMakePath(EClientMakePath eClientMakePath)
{
    if (m_eClientMakePath == eClientMakePath) return;

    for (auto& Desc : m_vecClientMakePathDesc)
        Safe_Delete(Desc);

    m_vecClientMakePathDesc.clear();

    m_eClientMakePath = eClientMakePath;

    for (auto& SRT : m_vecSRTs)
    {
        /* 동적할당 받아서 나온다  */
        CLIENT_MAKEPATH_DESC_BASE* pDesc = CMapToolManager::GetInstance()->Make_Client_MakePathDesc(m_eClientMakePath);
        if (!pDesc)
            return;
        m_vecClientMakePathDesc.push_back(pDesc);
    }

    return;
}

void CMapObject::Set_MapObjectDrawType(EMapObject_DrawType eDrawType)
{

    if (m_eMapObjectDrawType == eDrawType) return;

    if (m_eMapObjectDrawType == EMapObject_DrawType::Instance)
    {
        int iResult = MessageBox(NULL, L"인스턴싱 데이터를 일반 오브젝트렌더 변경하면 개별 객체로 분리됩니다. 계속 하시겠습니까?", L"경고: 타입 변경", MB_OKCANCEL | MB_ICONWARNING | MB_SETFOREGROUND);
        if (iResult)
        {
            if (FAILED(CMapToolManager::GetInstance()->Change_Instance_To_OtherDrawType(this, eDrawType)))
            {
                MSG_BOX(" 인스턴싱 데이터를 일반 오브젝트로 변경 중에 오류가 발생했습니다 ");
                return;
            }
        }

    }

    return;
}

void CMapObject::Set_SRTData(const Vec3& vScale, const Quat vQuat, const Vec3 vPosition)
{
    if ( m_iSelectedInstanceID >= m_vecSRTs.size() )  return;

    /* SRT DATA Update */
    m_vecSRTs[m_iSelectedInstanceID].Update_World(vScale , vQuat , vPosition);

    /* Draw Type 에 따라 분기처리 */
    if (m_eMapObjectDrawType == EMapObject_DrawType::Instance)
        Update_InstanceWorldMatrix(false);
    else
        Get_Component<CTransform>()->Set_WorldMatrix(m_vecSRTs[m_iActiveIndex].Get_World());
}


vector<wstring> CMapObject::Get_OverrideMtlsName() const
{
    if (m_isUseOverrideMaterials == false)
        return vector<wstring>();

    vector<wstring> vecResult{};

    for (auto& Mtl : m_vecOverrideMaterials)
    {
        if (Mtl == nullptr)
            vecResult.push_back(L"");
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
    

    for (_uint i = 0; i < iCount; ++i)
        vecResult.push_back(pModel->Get_MaterialName(i));
    return vecResult;


    //if (m_iUseOverrideMaterials == false)
    //{
    //    for (_uint i = 0; i < iCount; ++i)
    //        vecResult.push_back(pModel->Get_MaterialName(i));
    //    return vecResult;
    //}


    //for (_uint i = 0; i < iCount; ++i)
    //{
    //    if (m_vecOverrideMaterials[i])
    //        vecResult.push_back(m_vecOverrideMaterials[i]->Get_Name());
    //    else
    //        vecResult.push_back(pModel->Get_MaterialName(i));
    //}

    return vecResult;
}

CLIENT_MAKEPATH_DESC_BASE* CMapObject::Get_ClientMakePathDesc(_uint iIndex)
{
    return iIndex != g_Uint_NoneIndex ?
        iIndex >= m_vecClientMakePathDesc.size() ? nullptr : m_vecClientMakePathDesc[iIndex]
        :
        m_iSelectedInstanceID >= m_vecClientMakePathDesc.size() ?  nullptr : m_vecClientMakePathDesc[m_iSelectedInstanceID];
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


    m_pGameInstance->Push_RenderObject( m_eMapObjectState == CMapObject::EState::Select ?  RENDER_CATEGORY::NONELIGHT : RENDER_CATEGORY::NONEBLEND, this);
}

HRESULT CMapObject::Render()
{
    if (FAILED(Super::Render()))
        return E_FAIL;


    CShader* pShader{nullptr};


    switch (m_eMapObjectDrawType)
    {
    case Tool::EMapObject_DrawType::Collider:                                   return S_OK;
    case Tool::EMapObject_DrawType::Default:    if (FAILED(Render_Default()))   return E_FAIL; break;
    case Tool::EMapObject_DrawType::Instance:   if (FAILED(Render_Instance()))  return E_FAIL;   break;
    default:                                                                    return E_FAIL;
    }

    return S_OK;
}

void CMapObject::Draw_ImGui()
{
    Super::Draw_ImGui();

    CTransform* pTransfrom = Get_Component<CTransform>();
    
    if (!pTransfrom) return;

    return;
}


bool  CMapObject::IntsersectWithPlane(OUT Vec3& vOut)
{
    CModel* pModel = Get_Component<CModel>();
    if (pModel == nullptr)  return false;

    _uint iMeshCount = pModel->Get_MeshCount();
    for (_uint i = 0; i < iMeshCount; ++i)
    {
        if (pModel->Get_Mesh(i)->IntsersectWithPlane(vOut))
        {
            return true;
        }
    }
    return false;
}

_bool CMapObject::Picking(OUT Vec3& vOut)
{
    SimpleMath::Matrix InvWorldMatrix{};

    /* Type 별로 Picking이 달라진다 */

    _uint iIndex = 0;
    for (auto& SRT : m_vecSRTs)
    {
        InvWorldMatrix = SRT.Get_World().Invert();

        m_pGameInstance->TransformRayToLocalSpace(InvWorldMatrix);

        if (IntsersectWithPlane(vOut))
        {
            vOut = Vector3::Transform(vOut, SRT.Get_World());
            m_iSelectedInstanceID = iIndex;
            return true;
        }
        iIndex++;
    }

    m_iSelectedInstanceID = 0;

    return false;
}

_bool CMapObject::Export_Data(DTO::ECategory eCategory, CDataDocumentBase* pDocument)
{
    if (eCategory != DTO::ECategory::MAP)
        return false;

    CDataDocument_Map* pMapDoc = static_cast<CDataDocument_Map*>(pDocument);

    if (pMapDoc == nullptr) 
        return false;

    DTO::TMap_MapObjectData tData{};

    tData.strTag = m_strName + std::to_string(m_iObjectID);

    tData.isUELoaded = m_isUELoaded;
    tData.eMapObjectDrawType = ENUM_TO_UINT(m_eMapObjectDrawType);
    tData.eClientLevelType = ENUM_TO_UINT(m_eClientLevelType);
    tData.eClientMakePath = ENUM_TO_UINT(m_eClientMakePath);
    

    tData.strModelPath = Engine_Utils::ToString(m_wstrModelPath);


    if (!m_vecClientMakePathDesc.empty())
    {
        for (auto& Desc : m_vecClientMakePathDesc)
        {
            /* 안쪽에서 new로 동적할당해서 복사생성해준다 필수 관문 */
            tData.vecClientMakePathDesc.push_back(CMapToolManager::GetInstance()->Make_Client_MakePathDesc(m_eClientMakePath, Desc));
        }
    }


    for (auto& My_SRT : m_vecSRTs)
    {
        DTO::SRT_DATA tSRT{};
        tSRT.vScale_Isolated = My_SRT.vScale_Isolated;
        tSRT.vScale = My_SRT.vScale;
        tSRT.vQuat = My_SRT.vQuat;
        tSRT.vPosition = My_SRT.vPosition;
        tData.vecSRTs.push_back(tSRT);
    }

    if (FAILED(pMapDoc->Try_Add(tData)))
        return false;

    return true;
}


_int CMapObject::Get_InstanceCount()
{
    CInstanceMesh* pInsMesh  = Get_Component<CInstanceMesh>();
    if (!pInsMesh)
        return 1;
    return pInsMesh->Get_InstanceCount();
}

const Vec3& CMapObject::Get_Scale() const
{
    return m_vecSRTs[m_iSelectedInstanceID].vScale;
}

const Quat& CMapObject::Get_Quaternion() const
{
    return m_vecSRTs[m_iSelectedInstanceID].vQuat;
}

const Vec3& CMapObject::Get_Position() const
{
    return m_vecSRTs[m_iSelectedInstanceID].vPosition;
}



HRESULT CMapObject::Render_Default()
{
    CShader* pShader        = Get_Component<CShader>();     if (pShader == nullptr)         return E_FAIL;
    CModel* pModel          = Get_Component<CModel>();      if (pModel == nullptr)          return E_FAIL;
    CTransform* pTransform  = Get_Component<CTransform>();  if (pTransform == nullptr)      return E_FAIL;


    /* WorldMatrix 바인딩 */
    pShader->Bind_TransformData(pTransform->Get_WorldMatrix());
    _uint iMeshCount = static_cast<_uint>(pModel->Get_MeshCount());
    pShader->Set_Pass(ENUM_TO_UINT(m_eMapObjectState));
  
    for (_uint i = 0; i < iMeshCount; ++i)
    {
        pModel->Bind_Material(pShader,i);
        pModel->Bind_MaterialInstance(pShader,i);
        pShader->Apply();
        pModel->Render(i);
    }

    return S_OK;
}

HRESULT CMapObject::Render_Instance()
{
    CShader* pShader = Get_Component<CShader>();                    if (pShader == nullptr)             return E_FAIL;
    CModel* pModel = Get_Component<CModel>();                       if (pModel == nullptr)              return E_FAIL;
    CTransform* pTransform = Get_Component<CTransform>();           if (pTransform == nullptr)          return E_FAIL;
    CInstanceMesh* pInstanceMesh = Get_Component<CInstanceMesh>();  if (pInstanceMesh == nullptr)       return E_FAIL;

    pShader->Get_Scalar("g_iSelectInstanceID")->SetRawValue(&m_iSelectedInstanceID, 0, sizeof(m_iSelectedInstanceID));
    _uint iMeshCount = static_cast<_uint>(pModel->Get_MeshCount());
    pShader->Set_Pass(ENUM_TO_UINT(m_eMapObjectState));

    _uint iInstanceCount = Get_InstanceCount();

    pInstanceMesh->Bind_Instance(1);
    for (_uint i = 0; i < iMeshCount; ++i)
    {
        pModel->Bind_Material(pShader, i);
        pModel->Bind_MaterialInstance(pShader, i);
        pShader->Apply();
        pModel->Render_Instance(i, iInstanceCount);
    }
    pInstanceMesh->Unbind_Resource(1);

    return S_OK;
}


CMapObject* CMapObject::Create(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMapObject* pMapObject = new CMapObject(eType, pDevice,pContext);

    if (FAILED(pMapObject->Initialize_Prototype()))
    {
        Safe_Release(pMapObject);
        MSG_BOX( " Map Object is Failed To Create " );
        return nullptr;
    }

    return pMapObject;
}

CGameObject* CMapObject::Clone(void* pArg)
{
    CMapObject* pMapObject = new CMapObject(*this);

    if (FAILED(pMapObject->Initialize(pArg)))
    {
        Safe_Release(pMapObject);
        MSG_BOX(" Map Object is Failed To Craete ");
        return nullptr;
    }
    return pMapObject;
}




void CMapObject::Free()
{ 
    for (auto& OverrideMtl : m_vecOverrideMaterials)
        Safe_Release(OverrideMtl);

    for (auto& Desc : m_vecClientMakePathDesc)
        Safe_Delete(Desc);

    Super::Free();
}
