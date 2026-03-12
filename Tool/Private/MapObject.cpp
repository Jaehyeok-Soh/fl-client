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
#include "DataDocument_Map.h"
#include "DataStruct_Map.h"
#include "GameInstance.h"
#include "CameraMan.h"
#include "Collider.h"
#include "Bounding.h"
#include "Bounding_AABB.h"
#include "Bounding_Obb.h"
#include "Bounding_Sphere.h"

USING(Tool)


CMapObject::CMapObject(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : CToolObject(eType, pDevice, pDeviceContext), m_isLoaded{ false }, m_vecOverrideMaterials{} , m_isUseOverrideMaterials{false}
    , m_eMapObjectDrawType{ EMapObject_DrawType::Default }, m_eClientMakePath{ EClientMakePath::END }, m_eClientLevelType{ EClientLevelType::END }
    , m_eMapObjectState{ CMapObject::EState::Default }, m_iSelectedInstanceID{ 0 }, m_vecOriginSRTs{}
    , m_wstrModelPath{L""}
    , m_vecClientMakePathDesc{}
    , m_vecMatrix{}
    , m_vecOriginMatrix{}
    , m_wstrUERawDataPath{L""}
    , m_tUsingModelInfo{}
    , m_fDT{}
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
    , m_isBatced{ rhs .m_isBatced}
    , m_vecMatrix{rhs.m_vecMatrix}
    , m_vecOriginMatrix{rhs.m_vecOriginMatrix}
    , m_pMapToolManager(CMapToolManager::GetInstance())
    , m_wstrUERawDataPath{rhs.m_wstrUERawDataPath }
    , m_tUsingModelInfo{ rhs.m_tUsingModelInfo}
    , m_fDT{rhs.m_fDT }
{
    /*  Description을 어케해주는게 좋을려나...  */
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
    m_iSectionNum               = pDesc->iSectionNumber;
    m_wstrUERawDataPath         = pDesc->wstrUERawDataPath;

    if (m_isUELoaded == true)
        m_tUsingModelInfo = pDesc->tUsingModelInfo;

    if (m_isLoaded == true)
        m_isBatced = true;

    /* Model != None */
    if (m_eMapObjectDrawType != EMapObject_DrawType::Collider)
    {
        /* Path 값과 모델 네임 입력 */
        m_strModelFileName = Engine_Utils::ToString(path(pDesc->tUsingModelInfo.wstrName));
        m_wstrModelPath = pDesc->tUsingModelInfo.wstrPath;
        Set_Name(m_strModelFileName);
    }

    Check_ClientMakePathAndDrawType_TriggerBox();
    
    /* Trigger Box관련 설정 체크하는 함수 */
    if (FAILED(Ready_SRTDatas(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Component()))
        return E_FAIL;

    if (FAILED(Ready_ClientMakePath(pDesc)))
        return E_FAIL;

    if (FAILED(Check_DrawType_ByClientPath()))
        return E_FAIL;

    if (FAILED(Ready_ColliderTypeName()))
        return E_FAIL;


    if (FAILED(Ready_PlusData_ByClientMakePath()))
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
    m_vecMatrix.resize(iCount);

    m_vecOriginSRTs.resize(iCount);
    m_vecOriginMatrix.resize(iCount);

    /* SRT Data 대입 */
    for (_uint i = 0; i < iCount; ++i)
    {
        m_vecSRTs[i] = pDesc->vecSRTs[i];
        m_vecMatrix[i] = m_vecSRTs[i].Get_World();

        m_vecOriginSRTs[i] = pDesc->vecSRTs[i];
        m_vecOriginMatrix[i] = m_vecOriginSRTs[i].Get_World();
    }

    if (m_eMapObjectDrawType != EMapObject_DrawType::Instance)
        Get_Component<CTransform>()->Set_WorldMatrix(m_vecSRTs[m_iSelectedInstanceID].Get_World());
  
    return S_OK;
}

HRESULT CMapObject::Ready_Component()
{
    /* Collider 면 Collider 박스 달아주기 */
    if (m_eMapObjectDrawType == EMapObject_DrawType::Collider)
    {
        if (FAILED(Ready_ColliderTypeComponet()))
            return E_FAIL;
        return S_OK;
    }

    /* ReadyShader Shader */
    m_eMapObjectDrawType == EMapObject_DrawType::Instance ?
                Add_Component<CShader>(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Shader_VtxInstanceMesh_Tool", nullptr)
            :   Add_Component<CShader>(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Shader_VtxMesh_Tool", nullptr);
     

    CModel::MODEL_ORIGIN_DESC tModelDesc{};
    tModelDesc.eType = EModelType::STATIC;
    tModelDesc.wstrModelFolderName = m_wstrModelPath;
    tModelDesc.iPrototypeLevelIndex = /*ENUM_TO_UINT(ELevelType::MAP)*/m_pGameInstance->Get_CurrentLevelIndex();
    CModel* pModel = CModel::Create(m_pDevice, m_pDeviceContext, &tModelDesc);
    if (pModel)
    {
        if (FAILED(m_pGameInstance->Add_Prototype(tModelDesc.iPrototypeLevelIndex, L"Prototype_Component_Model_" + Engine_Utils::ToWString(m_strName), pModel)))
            Safe_Release(pModel);
    }
    CModel::MODEL_COPY_DESC tModelCopyDesc{};
    Add_Component<CModel>(tModelDesc.iPrototypeLevelIndex, L"Prototype_Component_Model_" + Engine_Utils::ToWString(m_strName), &tModelCopyDesc);

    if (m_eMapObjectDrawType == EMapObject_DrawType::Instance)
    {
        /* InstanceMesh */
        CInstanceMesh::INSTANCEMESH_DESC tInstanceMeshDesc{};
        tInstanceMeshDesc.IB_Usage = D3D11_USAGE_DYNAMIC;
        tInstanceMeshDesc.VB_Usage = D3D11_USAGE_DYNAMIC;
        const Vec3* pMinMax = Get_Component<CModel>()->Get_StaticModelMinMax();
        tInstanceMeshDesc.pModelMinMax = pMinMax == nullptr ? m_vDefaultMinMax : pMinMax ;
        tInstanceMeshDesc.vecInstanceMatrixPointer = &m_vecMatrix;
        Add_Component<CInstanceMesh>(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_VIBuffer_InstanceMesh", &tInstanceMeshDesc);
    }

    CBounds::BOUND_COMP_DESC tBoundDesc{};
    tBoundDesc.fRatio = 1.f;
    const Vec3* pMinMax = m_eMapObjectDrawType == EMapObject_DrawType::Instance ? Get_Component<CInstanceMesh>()->Get_InstanceWorldMinMax() :
        Get_Component<CModel>()->Get_StaticModelMinMax();
    tBoundDesc.pMinMax = pMinMax == nullptr ? m_vDefaultMinMax : pMinMax;
    if (FAILED(Add_Component<CBounds>( ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Bounds", &tBoundDesc)))
        return E_FAIL;

    /* Bounding Box 업데이트 , Instnace모델이면 0 0 0월드좌표로 들어가는게 맞다 */
    Get_Component<CBounds>()->Update_BoundingDesc(Get_Component<CTransform>()->Get_WorldMatrix());

    if (m_eMapObjectDrawType == EMapObject_DrawType::Instance)
    {
        /* Add Bounds 해준다 */
        Get_Component<CBounds>()->Add_SubBounds(
            Get_Component<CModel>()->Get_StaticModelMinMax(), span<Matrix>(m_vecMatrix.data(), m_vecMatrix.size()), 1.f);
    }

    return S_OK;
}

HRESULT CMapObject::Ready_ColliderTypeComponet()
{

    /* Collider 면 Collider 박스 달아주기 */
    if (m_eMapObjectDrawType != EMapObject_DrawType::Collider)
        return E_FAIL;

    if (m_vecSRTs.size() > 1)
    {
        MSG_BOX(" Collider Draw Type의 SRT 데이터가 2개를 넘어갈 수 없습니다 ");
        return E_FAIL;
    }

    /* Collider 는 Instance가 될수 없다. 그냥 내 정의 */
    if(m_vecClientMakePathDesc.size() > 1)
    {
        MSG_BOX(" Collider Draw Type의 Desc 데이터가 2개를 넘어갈 수 없습니다 ");
        return E_FAIL;
    }

    if (m_vecClientMakePathDesc.empty())
        return S_OK;

    Engine::TRIGGERBOX_DESC* pTriggerBoxDesc = dynamic_cast<TRIGGERBOX_DESC*>(m_vecClientMakePathDesc.front());
    if (pTriggerBoxDesc == nullptr)
    {
        MSG_BOX(" Trigger Box Desc 정보를 확인 할 수 없습니다 확인해주세요 ");
        return E_FAIL;
    }

    /* 혹시 있을 콜라이더 컴포넌트 삭제시켜주기 */
    CGameObject::Remove_Component<CCollider>();

    /* Extents 지정 */
    /* Center는 Offset이라 상관없음 */
    CBounding_AABB::BOUNDING_AABB_DESC tAABBDesc{};
    tAABBDesc.vExtens = pTriggerBoxDesc->vExtents;

    CCollider::COLLIDER_DESC tColliderDesc{};
    tColliderDesc.pBoundingDesc = &tAABBDesc;

    if (FAILED(CGameObject::Add_Component<CCollider>(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Collider_AABB", &tColliderDesc)))
        return E_FAIL;


    /* Shader 는 기본으로 Mesh를 들고있어준다 */
    Add_Component<CShader>(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Shader_VtxMesh_Tool", nullptr);


    return S_OK;
}

HRESULT CMapObject::Ready_ClientMakePath(CMapObject::MAPOBJECT_DESC* pDesc)
{
    if (nullptr == pDesc) return E_FAIL;

    m_vecClientMakePathDesc.clear();

    bool isHasSource = !pDesc->vecClientMakePathDesc.empty();
    _uint iCount = isHasSource ? (_uint)pDesc->vecClientMakePathDesc.size() : Get_InstanceCount();

    m_vecClientMakePathDesc.reserve(iCount);

    for (_uint i = 0; i < iCount; ++i)
    {
        CLIENT_MAKEPATH_DESC_BASE* pPrototype = isHasSource ? pDesc->vecClientMakePathDesc[i] : nullptr;
        CLIENT_MAKEPATH_DESC_BASE* pNewDesc = CMapToolManager::GetInstance()->Make_Client_MakePathDesc(m_eClientMakePath, pPrototype);

        if (nullptr == pNewDesc)
            return S_OK; //

        m_vecClientMakePathDesc.push_back(pNewDesc);
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

HRESULT CMapObject::Ready_PlusData_ByClientMakePath()
{
    //if (m_vecClientMakePathDesc.empty())
    //    return S_OK;

    switch (m_eClientMakePath)
    {

    case Tool::EClientMakePath::Batch_Player:
        if (FAILED(Ready_Batch_Player()))
            return E_FAIL;
        break;

    case Tool::EClientMakePath::Batch_Monster:
        if (FAILED(Ready_Batch_Monster()))
            return E_FAIL;
        break;

    case Tool::EClientMakePath::Batch_Object:
        if (FAILED(Ready_Batch_Object()))
            return E_FAIL;
        break;

    case Tool::EClientMakePath::TriggerBox_MonsterSpawner:
        if (FAILED(Ready_TriggerBox_MonsterSpawner()))
            return E_FAIL;

        break;
    case Tool::EClientMakePath::Invisible_Wall:
        if (FAILED(Ready_InvisibleWall()))
            return E_FAIL;
        break;

    case Tool::EClientMakePath::Water:
        if (FAILED(Ready_Water()))
            return E_FAIL;
        break;

    case Tool::EClientMakePath::Bush:
    case Tool::EClientMakePath::Grass:
    case Tool::EClientMakePath::Moss:
    case Tool::EClientMakePath::Tree:
    case Tool::EClientMakePath::Vine:
        if (FAILED(Ready_Plants()))
            return E_FAIL;

        break;
    case Tool::EClientMakePath::END:
        break;
    default:
        break;
    }

    return S_OK;
}


HRESULT CMapObject::Ready_Plants()
{
    CModel* pModel = Get_Component<CModel>();
    if (pModel == nullptr)
    {
        MSG_BOX(" Plants인데 모델이 없습니다 확인해주세요 ");
        return S_OK;
    }

    /* Free 흰색으로 지정해주던지 아니면 알아서 색깔지정해줘라 */
    _uint iMeshCount = pModel->Get_MeshCount();
    for (_uint i = 0; i < iMeshCount; ++i )
    {
        /* 모두 Free MI로 변경 */
        pModel->Change_MI(i,EMaterialInstanceType::Free);
    }

    return S_OK;
}

HRESULT CMapObject::Ready_Batch_Player()
{

    Remove_Component<CModel>();
    CModel* pModel = m_pMapToolManager->Get_PlayerPreviewModel();
    if (pModel == nullptr) return E_FAIL;
    Add_Component<CModel>(pModel);


    Remove_Component<CBounds>();

    CBounds::BOUND_COMP_DESC tBoundDesc{};
    tBoundDesc.fRatio = 1.f;
    const Vec3* pMinMax =  Get_Component<CModel>()->Get_StaticModelMinMax();
    tBoundDesc.pMinMax = pMinMax == nullptr ? m_vDefaultMinMax : pMinMax;
    if (FAILED(Add_Component<CBounds>(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Bounds", &tBoundDesc)))
        return E_FAIL;

    /* Bounding Box 업데이트 , Instnace모델이면 0 0 0월드좌표로 들어가는게 맞다 */
    Get_Component<CBounds>()->Update_BoundingDesc(Get_Component<CTransform>()->Get_WorldMatrix());


    m_strName = "Batch_Player";



    return S_OK;
}

HRESULT CMapObject::Ready_Batch_Monster()
{
    /* 현재 Model 무적권 삭제 */
    Remove_Component<CModel>();

    if (m_vecClientMakePathDesc.empty())
        return E_FAIL;

    BATCH_MONSTER_DESC* pDesc = dynamic_cast<BATCH_MONSTER_DESC*>(m_vecClientMakePathDesc.front());
    if (pDesc == nullptr) return E_FAIL;

    CModel* pModel = m_pMapToolManager->Get_MonsterPreviewModel(pDesc->eBatchMonsterType);
    Add_Component<CModel>(pModel);


    Remove_Component<CBounds>();

    CBounds::BOUND_COMP_DESC tBoundDesc{};
    tBoundDesc.fRatio = 1.f;
    const Vec3* pMinMax = Get_Component<CModel>()->Get_StaticModelMinMax();
    tBoundDesc.pMinMax = pMinMax == nullptr ? m_vDefaultMinMax : pMinMax;
    if (FAILED(Add_Component<CBounds>(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Bounds", &tBoundDesc)))
        return E_FAIL;

    /* Bounding Box 업데이트 , Instnace모델이면 0 0 0월드좌표로 들어가는게 맞다 */
    Get_Component<CBounds>()->Update_BoundingDesc(Get_Component<CTransform>()->Get_WorldMatrix());


    m_strName = "Batch_Monster" + DTO::MakeMonsterType_ToString(pDesc->eBatchMonsterType);



    return S_OK;
}

HRESULT CMapObject::Ready_Batch_Object()
{
    if (m_vecClientMakePathDesc.empty())
        return E_FAIL;


    BATCH_OBJECT_DESC* pDesc = dynamic_cast<BATCH_OBJECT_DESC*>(m_vecClientMakePathDesc.front());
    if (pDesc == nullptr) return E_FAIL;

    /* 현재 Model 무적권 삭제 */
    Remove_Component<CModel>();

    /* 이름도 강제조정 */
    CModel* pModel = m_pMapToolManager->Get_BatchObjectModel(pDesc->eBatchObjectType);
    if(pModel)
        Add_Component<CModel>(pModel);



    DTO::EMakeObjectType eMakeObjecType = pDesc->eBatchObjectType;

    switch (eMakeObjecType)
    {
    case DTO::EMakeObjectType::Battle_Field:
    {
        /* Collider 생성해줘야함 */
        BATTLE_FIELD_DESC* pBattleFieldDesc = static_cast<BATTLE_FIELD_DESC*>(pDesc->pBatchObjectDesc);
        if (pBattleFieldDesc == nullptr) return E_FAIL;
        /* Collider 생성전 미리 삭제 */
        Safe_Release(pBattleFieldDesc->pBattleFieldColliderSphere);
        CCollider::COLLIDER_DESC tDesc{};
        CBounding_Sphere::BOUNDING_SPHERE_DESC tBoundingSphere{};
        tBoundingSphere.fRadius =pBattleFieldDesc->fRadius;
        Vec3 vMinMax[2] = { Vec3(-tBoundingSphere.fRadius,-tBoundingSphere.fRadius,-tBoundingSphere.fRadius) , Vec3(tBoundingSphere.fRadius,tBoundingSphere.fRadius,tBoundingSphere.fRadius)};
        tBoundingSphere.pMinMax = vMinMax;
        tBoundingSphere.vCenter = Vec3::Zero;
        tDesc.pBoundingDesc = &tBoundingSphere;
        pBattleFieldDesc->pBattleFieldColliderSphere = 
            static_cast<CCollider*>(m_pGameInstance->Clone_Prototype(EPrototypeType::COMPONENT, ENUM_TO_UINT(ELevelType::STATIC), g_wszCollider_Sphere_Prototype_Tag, &tDesc));
        if (pBattleFieldDesc->pBattleFieldColliderSphere == nullptr) return E_FAIL;


        CBounding_OBB::BOUNDING_OBB_DESC tBoundingObb{};
        tBoundingObb.vExtents = pBattleFieldDesc->vExtents;
        tDesc.pBoundingDesc = &tBoundingObb;
        pBattleFieldDesc->pBattleFieldColliderBox =
            static_cast<CCollider*>(m_pGameInstance->Clone_Prototype(EPrototypeType::COMPONENT, ENUM_TO_UINT(ELevelType::STATIC), g_wszCollider_OBB_Prototype_Tag, &tDesc));
        if (pBattleFieldDesc->pBattleFieldColliderBox == nullptr) return E_FAIL;

        m_strName = "Battle_Field";
    }
    break;
    default:                                    break;
    }


    return S_OK;
}

HRESULT CMapObject::Ready_TriggerBox_MonsterSpawner()
{
    if (m_vecClientMakePathDesc.empty())
        return E_FAIL;

    TRIGGERBOX_MONSTERSPAWNER_DESC* pDesc = dynamic_cast<TRIGGERBOX_MONSTERSPAWNER_DESC*>(m_vecClientMakePathDesc.front());
    if (pDesc == nullptr) return E_FAIL;


    for (auto& MonsterSpawnData : pDesc->vecMonsterSpawnData)
    {
        Safe_Release(MonsterSpawnData.pDebugModel);
        MonsterSpawnData.pDebugModel =  m_pMapToolManager->Get_MonsterPreviewModel(MonsterSpawnData.eMakeMonsterType);
    }

    return S_OK;
}

HRESULT CMapObject::Ready_Water()
{
    CModel* pModel = Get_Component<CModel>();
    if (!pModel) return E_FAIL;

    for (_uint i = 0; i < pModel->Get_MaterialCount(); ++i)
    {
        pModel->Change_MI(i , EMaterialInstanceType::Free);
    }

    return S_OK;
}

HRESULT CMapObject::Ready_InvisibleWall()
{
    /* 현재 Model 무적권 삭제 */
    Remove_Component<CModel>();

    CModel::MODEL_COPY_DESC tModelCopyDesc{};


    Add_Component<CModel>(ENUM_TO_UINT(ELevelType::MAP) , L"Prototype_Component_Model_Plane",&tModelCopyDesc);
    if (Get_Component<CModel>() == nullptr) return E_FAIL;


    if (m_isLoaded == false)
    {
        //Get_Component<CTransform>()->Rotation(XMConvertToRadians(90.f), 0.f, 0.f);
    }

    return S_OK;
}

HRESULT CMapObject::Ready_ColliderTypeName()
{
    /* Collider Type */
    if (m_eMapObjectDrawType != EMapObject_DrawType::Collider)
        return S_OK;


    switch (m_eClientMakePath)
    {
    case Tool::EClientMakePath::TriggerBox_ChangeLevel:
        m_strName = "TriggerBox_ChangeLevel";
        break;
    case Tool::EClientMakePath::TriggerBox_MonsterSpawner:
        m_strName = "TriggerBox_MonsterSpawner";
        break;
    case Tool::EClientMakePath::TriggerBox_GlobalEvent_BroadCaster:
        m_strName = "TriggerBox_GlobalEvent_BroadCaster";
        break;
    case Tool::EClientMakePath::TriggerBox_TutorialUIEvent:
        m_strName = "TriggerBox_TutorialUIEvent";
        break;
    case Tool::EClientMakePath::END:
        break;
    default:
        break;
    }

    return S_OK;
}

_bool CMapObject::Check_OutBound(_int iIndex) const
{
    if (iIndex == -1)
    {
        if (m_iSelectedInstanceID >= m_vecSRTs.size())
            return false;
    }
    else
    {
        if (iIndex >= m_vecSRTs.size())
            return false;
    }

    return true;
}

void CMapObject::Check_ClientMakePathAndDrawType_TriggerBox()
{
    /* Collider Box Draw Type */
    if (m_eMapObjectDrawType != EMapObject_DrawType::Collider)
    {
        switch (m_eClientMakePath)
        {
        case Tool::EClientMakePath::TriggerBox_ChangeLevel:
        case Tool::EClientMakePath::TriggerBox_MonsterSpawner:
        case Tool::EClientMakePath::TriggerBox_GlobalEvent_BroadCaster:
        case Tool::EClientMakePath::TriggerBox_TutorialUIEvent:
            MSG_BOX(" Trigger Box 관련 Client Make Path는 Draw Type으로 Collider로 자동 지정 됩니다 ");
            m_eMapObjectDrawType = EMapObject_DrawType::Collider;
            break;
        default:
            break;
        }
        return;
    }

    return;
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

void CMapObject::Reset_OriginTransform(_int iIndex)
{
    if (!Check_OutBound(iIndex))
        return;

    _int Index = iIndex == -1 ? m_iSelectedInstanceID : iIndex;
    if (Index < 0) return;
   

    m_vecSRTs[Index]  =  m_vecOriginSRTs[Index];
    m_vecMatrix[Index] = m_vecOriginMatrix[Index];

    if (m_eMapObjectDrawType == EMapObject_DrawType::Instance)
        Update_InstanceWorldMatrix(false, Index);
    else
        Get_Component<CTransform>()->Set_WorldMatrix(m_vecSRTs[Index].Get_World());
}

void CMapObject::Override_OriginTransform(_int iIndex)
{
    if (!Check_OutBound(iIndex))
        return;

    _int Index = iIndex == -1 ? m_iSelectedInstanceID : iIndex;
    if (Index < 0) return;

    m_vecOriginSRTs[Index] = m_vecSRTs[Index];
    m_vecOriginMatrix[Index] = m_vecMatrix[Index];
}

void CMapObject::Update_InstanceWorldMatrix(_bool isAllUpdate, _int iIndex)
{
    if (m_eMapObjectDrawType != EMapObject_DrawType::Instance)
        return;

    if (!Check_OutBound(iIndex))
        return;

    _uint Index = iIndex == -1 ? m_iSelectedInstanceID : static_cast<_uint>(iIndex);


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
        memcpy(&pInstance[Index], &m_vecMatrix[Index]._11, sizeof(Matrix));
    }
    else
    {
        for (_uint i = 0; i < static_cast<_uint>(m_vecSRTs.size()); ++i)
            memcpy(&pInstance[i], &m_vecMatrix[i]._11, sizeof(Matrix));
    }

    m_pDeviceContext->Unmap(pInstanaceBuffer , 0);

    return;
}

void CMapObject::Update_Bounds(_uint iIndex)
{
    if (!Check_OutBound(iIndex))
        return;

    /* Instance Type이면 SubBound 업데이트를 시켜주기 */
    if (m_eMapObjectDrawType == EMapObject_DrawType::Instance)
    {
        /* 내가 움직이거나 그 럴떄마다 Update를 해줘야한다 */
        CInstanceMesh* pInstanceMesh = Get_Component<CInstanceMesh>();  if (pInstanceMesh == nullptr) return;
        pInstanceMesh->Update_Instance_WorldMinMax(Get_Component<CModel>()->Get_StaticModelMinMax(), &m_vecMatrix);
        CBounds* pBound = Get_Component<CBounds>(); if (pBound == nullptr) return;
        /* BoundUpdate */
        pBound->Update_SubBound( Get_Component<CInstanceMesh>()->Get_InstanceWorldMinMax(), m_vecMatrix[iIndex], iIndex);
    }
    else
    {
        /*  Instance용 모델이 아니라면?  */
        CBounds* pBound = Get_Component<CBounds>();
        if (pBound == nullptr) return;
        pBound->Update_BoundingDesc(m_vecMatrix[0]);
    }
}

void CMapObject::Update_Collider()
{
    if (m_eMapObjectDrawType != EMapObject_DrawType::Collider)
        return;

    CCollider* pCollider = Get_Component<CCollider>();
    if (pCollider == nullptr) return;


    CBounding* pBounding = pCollider->Get_Bounding();
    if (pBounding == nullptr) return;
    CBounding_AABB* pAABB = dynamic_cast<CBounding_AABB*>(pBounding);
    if (pAABB == nullptr) return;
    BoundingBox* pBoundingBox = pAABB->Get_OriginalDesc();
    if (pBoundingBox == nullptr) return;

    TRIGGERBOX_DESC* pDesc = dynamic_cast<TRIGGERBOX_DESC*>(m_vecClientMakePathDesc.front());
    if (pDesc == nullptr) return;
    pBoundingBox->Extents = pDesc->vExtents;
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
    return m_vecMatrix[m_iSelectedInstanceID];
}

void CMapObject::Set_WorldMatrix(const Vec3& vScale, const Quat& vQuat, const Vec3& vPosition)
{
    Set_SRTData(vScale , vQuat , vPosition);

    Update_Bounds(m_iSelectedInstanceID);
}

void CMapObject::Set_WorldMatrix(const Matrix& WorldMatrix)
{
    if (m_iSelectedInstanceID >= m_vecSRTs.size())
        return;

    m_vecSRTs[m_iSelectedInstanceID].Update_SRT(WorldMatrix);
    m_vecMatrix[m_iSelectedInstanceID] = WorldMatrix;
    
    if (m_eMapObjectDrawType == EMapObject_DrawType::Instance)
        Update_InstanceWorldMatrix(false);
    else
        Get_Component<CTransform>()->Set_WorldMatrix(m_vecSRTs[m_iSelectedInstanceID].Get_World());

    Update_Bounds(m_iSelectedInstanceID);

    return;
}

void CMapObject::Set_Scale(const Vec3& vScale ,_int iIndex)
{
    if (!Check_OutBound(iIndex))
        return;

    _uint Index = iIndex == -1 ?  m_iSelectedInstanceID : static_cast<_uint>(iIndex);

    m_vecSRTs[Index].Update_Scale(vScale);
    m_vecMatrix[Index] = m_vecSRTs[iIndex].Get_World();

    if (m_eMapObjectDrawType == EMapObject_DrawType::Instance)
        Update_InstanceWorldMatrix(false);
    else
        Get_Component<CTransform>()->Set_WorldMatrix(m_vecSRTs[Index].Get_World());


    Update_Bounds(Index);

}

void CMapObject::Set_Position(const Vec3& vPosition , _int iIndex)
{
    if (!Check_OutBound(iIndex))
        return;

    _uint Index = iIndex == -1  ? m_iSelectedInstanceID : static_cast<_int>(iIndex);
    m_vecSRTs[Index].Update_Position(vPosition);
    m_vecMatrix[Index] = m_vecSRTs[Index].Get_World();

    if (m_eMapObjectDrawType == EMapObject_DrawType::Instance)
        Update_InstanceWorldMatrix(false, Index);
    else
        Get_Component<CTransform>()->Set_WorldMatrix(m_vecSRTs[Index].Get_World());

    Update_Bounds(Index);

}

void CMapObject::Set_Quaternion(const Quat& vQuat ,_int iIndex)
{
    if (!Check_OutBound(iIndex))
        return;

    _uint Index = iIndex == -1 ? m_iSelectedInstanceID : static_cast<_int>(iIndex);
    m_vecSRTs[Index].Update_Quat(vQuat);
    m_vecMatrix[Index] = m_vecSRTs[Index].Get_World();

    if (m_eMapObjectDrawType == EMapObject_DrawType::Instance)
        Update_InstanceWorldMatrix(false);
    else
        Get_Component<CTransform>()->Set_WorldMatrix(m_vecSRTs[Index].Get_World());

    Update_Bounds(Index);
}

void CMapObject::Set_ClientMakePath(EClientMakePath eClientMakePath)
{
    if (m_eClientMakePath == eClientMakePath) return;

    for (auto& Desc : m_vecClientMakePathDesc)
        Safe_Delete(Desc);

    m_vecClientMakePathDesc.clear();

    m_eClientMakePath = eClientMakePath;

    /* 현재 Instance 개수만큼 생성된다 */
    for (auto& SRT : m_vecSRTs)
    {
        /* 동적할당 받아서 나온다  */
        CLIENT_MAKEPATH_DESC_BASE* pDesc = CMapToolManager::GetInstance()->Make_Client_MakePathDesc(m_eClientMakePath);
        if (!pDesc)
            return;
        m_vecClientMakePathDesc.push_back(pDesc);
    }

    if (FAILED(Check_DrawType_ByClientPath()))
        return;

    Ready_PlusData_ByClientMakePath();

    return;
}

void CMapObject::Set_MapObjectDrawType(EMapObject_DrawType eDrawType)
{
    if (m_eMapObjectDrawType == eDrawType) return;

    if (eDrawType == EMapObject_DrawType::Collider)
    {
        MSG_BOX(" 콜라이더 타입 은 처음에 생성시점에 생성시 적용 외에 Type으로 지정 할 수 없습니다 ");
        return;
    }


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

    /* Instance로 변경 */
    if (m_eMapObjectDrawType == EMapObject_DrawType::Default)
    {
        /* Draw 에서 Instance로 변경 */
        m_eMapObjectDrawType = eDrawType;
        m_iSelectedInstanceID = 0;
        
        SRT_DATA& tCurSRT = m_vecSRTs.front();

        CInstanceMesh::INSTANCEMESH_DESC tDesc{};
        tDesc.vecInstanceMatrixPointer = &m_vecMatrix;
        tDesc.VB_Usage = D3D11_USAGE_DYNAMIC;
        tDesc.IB_Usage = D3D11_USAGE_DYNAMIC;
        tDesc.pModelMinMax = Get_Component<CModel>()->Get_StaticModelMinMax();
        Add_Component<CInstanceMesh>(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_VIBuffer_InstanceMesh", &tDesc);

        CGameObject::Remove_Component<CShader>();
        Add_Component<CShader>(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Shader_VtxInstanceMesh_Tool", nullptr);
       
        CTransform* pTs = Get_Component<CTransform>();
        pTs->Set_WorldMatrix(Matrix::Identity);

        CBounds* pBounds = Get_Component<CBounds>();
        pBounds->Update_BoundingDesc(m_vecMatrix.front());
        /* 아직 생성을안함 */
        pBounds->Push_SubBounds(Get_Component<CInstanceMesh>()->Get_InstanceWorldMinMax(),Get_Component<CModel>()->Get_StaticModelMinMax(),m_vecMatrix.front());
    }


    if (FAILED(Check_DrawType_ByClientPath()))
        return;

    return;
}

void CMapObject::Add_InstanceData(const SRT_DATA& tData)
{
    if (m_eMapObjectDrawType != EMapObject_DrawType::Instance)
        return;

    /* 데이터를 추가한뒤 버퍼 재할당 */
    m_vecSRTs.push_back(tData);
    m_vecMatrix.push_back(tData.Get_World());

    m_vecOriginSRTs.push_back(tData);
    m_vecOriginMatrix.push_back(tData.Get_World());

    /* 버퍼 재할당 */

    CInstanceMesh* pInstanceMesh = Get_Component<CInstanceMesh>();
    if (pInstanceMesh == nullptr) return;

    if(FAILED(pInstanceMesh->ReMake_InstanceBuffer(&m_vecMatrix , Get_Component<CModel>()->Get_StaticModelMinMax())))
    {
        MSG_BOX(" Add SRT Data is failed ");
    }

    /* Desc 복사 생성 받기 */
    if (!m_vecClientMakePathDesc.empty())
    {
        CLIENT_MAKEPATH_DESC_BASE* pDescBase = CMapToolManager::GetInstance()->Make_Client_MakePathDesc(m_eClientMakePath ,m_vecClientMakePathDesc[m_iSelectedInstanceID]);
        m_vecClientMakePathDesc.push_back(pDescBase);
    }
    if (FAILED(Get_Component<CBounds>()->Push_SubBounds(pInstanceMesh->Get_InstanceWorldMinMax(), Get_Component<CModel>()->Get_StaticModelMinMax(), m_vecMatrix.back())))
    {
        MSG_BOX(" Add_Instance() 안에 CBound::Push_SubBounds 오류 ");
    }
}

void CMapObject::Delete_InstanceData(_int iIndex)
{
    if (m_eMapObjectDrawType != EMapObject_DrawType::Instance)
        return;

    if (Get_InstanceCount() <= 1)
    {
        Set_Dead();
        return;
    }
  
    if (!Check_OutBound(iIndex))
        return;

    _uint iDeleteIndex = iIndex == -1 ? m_iSelectedInstanceID : static_cast<_uint>(iIndex);
    if (iDeleteIndex < 0) return;

   
    vector<Matrix> vecWorldMatrix{};
    vecWorldMatrix.reserve(m_vecSRTs.size());

    /* Cur Data */
    m_vecMatrix.erase(m_vecMatrix.begin() + iDeleteIndex);
    m_vecSRTs.erase(m_vecSRTs.begin() + iDeleteIndex);

    /* Origin Data */
    m_vecOriginSRTs.erase(m_vecOriginSRTs.begin() + iDeleteIndex);
    m_vecOriginMatrix.erase(m_vecOriginMatrix.begin() + iDeleteIndex);


    CInstanceMesh* pInstanceMesh = Get_Component<CInstanceMesh>();
    if (pInstanceMesh == nullptr)  return;
    pInstanceMesh->ReMake_InstanceBuffer(&m_vecMatrix,Get_Component<CModel>()->Get_StaticModelMinMax());

    CBounds* pBouns = Get_Component<CBounds>();
    pBouns->Delete_SubBounds(pInstanceMesh->Get_InstanceWorldMinMax() , iDeleteIndex);
}

void CMapObject::Set_SRTData(const Vec3& vScale, const Quat vQuat, const Vec3 vPosition , _int iIndex)
{
    if (!Check_OutBound(iIndex))
        return;

    _uint iFinalIndex = iIndex == -1.f ? m_iSelectedInstanceID : iIndex;

    /* SRT DATA Update */
    m_vecSRTs[iFinalIndex].Update_SRT(vScale , vQuat , vPosition);
    /* WorldMatrix Update */
    m_vecMatrix[iFinalIndex] = m_vecSRTs[iFinalIndex].Get_World();

    /* Draw Type 에 따라 분기처리 */
    if (m_eMapObjectDrawType == EMapObject_DrawType::Instance)
        Update_InstanceWorldMatrix(false);
    else
        Get_Component<CTransform>()->Set_WorldMatrix(m_vecSRTs[iFinalIndex].Get_World());


    Update_Bounds(iFinalIndex);

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


CLIENT_MAKEPATH_DESC_BASE* CMapObject::Get_ClientMakePathDesc(_int iIndex)
{
    if (m_vecClientMakePathDesc.empty()) return nullptr;

    _int OutIndex = iIndex == -1 ? m_iSelectedInstanceID : iIndex;

    if (OutIndex >= m_vecClientMakePathDesc.size())
        return nullptr;
    
    return m_vecClientMakePathDesc[OutIndex];
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

    /* 나중에 DT값 활용을위해 */



    m_fDT += fTimeDelta;
}

void CMapObject::Update_Late(const _float fTimeDelta)
{
    Super::Update_Late(fTimeDelta);
}

void CMapObject::Ready_Before_Render(const _float fTimeDelta)
{
    Super::Ready_Before_Render(fTimeDelta);

    // m_eMapObjectState == CMapObject::EState::Select ?  RENDER_CATEGORY::NONELIGHT : 

    RENDER_CATEGORY eCategroy = RENDER_CATEGORY::NONEBLEND;

    switch (m_eClientMakePath)
    {
    case Tool::EClientMakePath::Water:  eCategroy = RENDER_CATEGORY::COMPUTELIGHT_BLEND;  break;
    default:                            break;
    }

    m_pGameInstance->Push_RenderObject(eCategroy, this);

#ifdef _DEBUG
    //if (m_eMapObjectDrawType == EMapObject_DrawType::Instance)
        m_pGameInstance->Push_DebugComponent(Get_Component<CBounds>());
#endif
}

HRESULT CMapObject::Render()
{
    if (FAILED(Super::Render()))
        return E_FAIL;

    HRESULT hr{E_FAIL};

    CShader* pShader{nullptr};

    switch (m_eClientMakePath)
    {
    case Tool::EClientMakePath::StaticObject:
        hr = Render_StaticObject();
        break;
    case Tool::EClientMakePath::LandScape:
        hr = Render_LandScape();
        break;
    case Tool::EClientMakePath::Grass:                  
        hr = Render_Grass();
        break;
    case Tool::EClientMakePath::Vine:               
        hr = Render_Vine();
        break;
    case Tool::EClientMakePath::Tree:
        hr = Render_Tree();
        break;
    case Tool::EClientMakePath::Moss:                   
        hr = Render_Moss();
        break;
    case Tool::EClientMakePath::Bush:
        hr = Render_Bush();
        break;
    case Tool::EClientMakePath::Water:                  
        hr = Render_Water();
        break;
    case Tool::EClientMakePath::Rock:                
        hr = Render_Rock();
        break;

    case Tool::EClientMakePath::Batch_Player:
        hr = Render_Batch_Player();
        break;
    case Tool::EClientMakePath::Batch_Monster:
        hr = Render_Batch_Monster();
        break;
    case Tool::EClientMakePath::Batch_Object:
        hr = Render_Batch_Object();
        break;
    case Tool::EClientMakePath::TriggerBox_ChangeLevel:
        hr = Render_TriggerBox_ChangeLevel();
        break;
    case Tool::EClientMakePath::TriggerBox_MonsterSpawner:
        hr = Render_TriggerBox_MonsterSpawner();
        break;
    case Tool::EClientMakePath::TriggerBox_GlobalEvent_BroadCaster:
        hr = Render_TriggerBox_GlobalEvent_BroadCaster();
        break;
    case Tool::EClientMakePath::TriggerBox_TutorialUIEvent:
        hr = Render_Collider();
        break;


    case Tool::EClientMakePath::Invisible_Wall:
        hr = Render_StaticObject();
        break;
    case Tool::EClientMakePath::Static_Light:
        hr = Render_StaticObject();
        break;
    default:
        break;
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

HRESULT CMapObject::Set_GPU_MapObjectState(CShader* pShader)
{
    if (pShader == nullptr) return E_FAIL;

    _uint iState = static_cast<_uint>(m_eMapObjectState);
    ID3DX11EffectVariable* pVariable = pShader->Get_Variable("g_iMapObject_State");
    // ▼▼▼ 이 코드를 넣어서 브레이크포인트를 걸어보세요 ▼▼▼
    if (pVariable->IsValid() == false)
    {
        MSG_BOX("쉐이더 변수 못찾음! 최적화로 삭제됐거나 파일 잘못됨");
        return E_FAIL;
    }
    if (FAILED(pVariable->AsScalar()->SetInt(iState)))
    {
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CMapObject::Set_GPU_BeforeRender(CShader* pShader)
{
    if (pShader == nullptr) return E_FAIL;

    if (FAILED(Set_GPU_MapObjectState(pShader)))
        return E_FAIL;


    return S_OK;
}

bool  CMapObject::IntsersectWithPlane(OUT Vec3& vOut ,const Vec3& vLocalCamPos)
{
    CModel* pModel = Get_Component<CModel>();
    if (pModel == nullptr)  return false;

    _uint iMeshCount = pModel->Get_MeshCount();

    _float fMinDist{FLT_MAX};
    Vec3 vFinalPos{Vec3::Zero};
    _bool isHit{ false };

    for (_uint i = 0; i < iMeshCount; ++i)
    {
        Vec3 vCurHIt{Vec3::Zero};
        if (pModel->Get_Mesh(i)->IntsersectWithPlane_CloseCam(vOut, vLocalCamPos))
        {
            _float fCurDist = Vec3::DistanceSquared(vLocalCamPos , vOut);
            if (fCurDist < fMinDist)
            {
                fMinDist = fCurDist;
                vFinalPos = vOut;
                isHit = true;
            }
        }
    }

    if (!isHit)
        return false;

    vOut = vFinalPos;

    return true;
}

_bool CMapObject::Picking(OUT Vec3& vOut)
{
    /* Type 별로 Picking이 달라진다 */
    if (m_pMapToolManager->Get_Preview() == this)
        return false;

    /* Preview랑 현재 Picking인 맵오브젝트가 똑같다면 거르기 */

    Vec3 vCamWorldPos = m_pGameInstance->Get_MainCamera()->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::POS);

    CBounds* pBounds = Get_Component<CBounds>();
    if (pBounds == nullptr) return false;

    /*  */
    if (pBounds->IntersectWithRay_World(vOut, m_iSelectedInstanceID) == false)
        return false;

    if (m_iSelectedInstanceID >= m_vecMatrix.size())
    {
        m_iSelectedInstanceID = 0;
        return false;
    }

    Matrix InvertWorldMatrix = m_vecMatrix[m_iSelectedInstanceID].Invert();

    m_pGameInstance->TransformRayToLocalSpace(InvertWorldMatrix);
    vCamWorldPos = Vec3::Transform(vCamWorldPos,InvertWorldMatrix);

    if (IntsersectWithPlane(vOut, vCamWorldPos))
    {
        vOut = Vector3::Transform(vOut, m_vecMatrix[m_iSelectedInstanceID]);
        return true;
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
    tData.eMapObjectDrawType    = DTO::EMapObject_DrawType(m_eMapObjectDrawType);
    tData.eClientLevelType      = DTO::EClientLevelType(m_eClientLevelType);
    tData.eClientMakePath       = DTO::EClientMakePath(m_eClientMakePath);
    tData.strUERawDataPath      = Engine_Utils::ToString(m_wstrUERawDataPath);

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

HRESULT CMapObject::Check_DrawType_ByClientPath()
{
    switch (m_eClientMakePath)
    {
    case Tool::EClientMakePath::StaticObject:
    case Tool::EClientMakePath::Grass:
    case Tool::EClientMakePath::Vine:
    case Tool::EClientMakePath::Tree:
    case Tool::EClientMakePath::Moss:
    case Tool::EClientMakePath::Bush:
    case Tool::EClientMakePath::Water:
    case Tool::EClientMakePath::Rock:
        return S_OK;


    case Tool::EClientMakePath::Batch_Player:
    case Tool::EClientMakePath::Batch_Monster:
    case Tool::EClientMakePath::LandScape:
    case Tool::EClientMakePath::TriggerBox_ChangeLevel:
    case Tool::EClientMakePath::TriggerBox_MonsterSpawner:
    case Tool::EClientMakePath::TriggerBox_GlobalEvent_BroadCaster:
    {
        if (m_eMapObjectDrawType == EMapObject_DrawType::Instance)
            MSG_BOX(" 현재 바꾸는 Client MakePth 관련 오브젝트는 Instance 를 지원하지 않습니다 Default Draw로  바꿔주세요 ");
        return S_OK;
    }

    default:                                            return S_OK;
    }

    return S_OK;
}

_int CMapObject::Get_InstanceCount()
{
    CInstanceMesh* pInsMesh  = Get_Component<CInstanceMesh>();
    if (!pInsMesh)
        return 1;
    return pInsMesh->Get_InstanceCount();
}

const SRT_DATA& CMapObject::Get_SRTData(bool isOrigin, _int iIndex) const
{
    _uint Index = iIndex == -1 ? m_iSelectedInstanceID : static_cast<_uint>(iIndex);
    return isOrigin == true ? m_vecOriginSRTs[Index] : m_vecSRTs[Index];
}

Vec3 CMapObject::Get_Scale(_int iIndex) const
{
    if (!Check_OutBound(iIndex))
        return Vec3();
    return iIndex ==-1 ? m_vecSRTs[m_iSelectedInstanceID].vScale : m_vecSRTs[iIndex].vScale;
}

Quat CMapObject::Get_Quaternion(_int iIndex) const
{
    if (!Check_OutBound(iIndex))
        return Quat();

    return iIndex == -1 ? m_vecSRTs[m_iSelectedInstanceID].vQuat : m_vecSRTs[iIndex].vQuat;
}

Vec3 CMapObject::Get_Position(_int iIndex) const
{
    if (!Check_OutBound(iIndex))
        return Vec3();

    return iIndex == -1 ? m_vecSRTs[m_iSelectedInstanceID].vPosition : m_vecSRTs[iIndex].vPosition;
}

Matrix CMapObject::Get_Matrix(_int iIndex) const
{
    if (!Check_OutBound(iIndex))
        return Matrix();

    return iIndex == -1 ? m_vecMatrix[m_iSelectedInstanceID] : m_vecMatrix[iIndex];
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

CMapObject* CMapObject::Clone(CMapObject* pPrototype, const SRT_DATA& tSRT)
{
    if (pPrototype->m_eMapObjectDrawType == EMapObject_DrawType::Instance)
    {
        pPrototype->Add_InstanceData(tSRT);
        return pPrototype;
    }
    else
    {
        CMapObject::MAPOBJECT_DESC tDesc{};
        tDesc.eClientLevelType = pPrototype->m_eClientLevelType;
        tDesc.eClientMakePath = pPrototype->m_eClientMakePath;
        tDesc.eMapObjectDrawType = pPrototype->m_eMapObjectDrawType;
        tDesc.isUELoaded = pPrototype->m_isUELoaded;
        tDesc.isLoaded = false;
        tDesc.iLevelIndex = ENUM_TO_UINT(ELevelType::MAP);
        tDesc.tUsingModelInfo.wstrPath = pPrototype->m_wstrModelPath;
        tDesc.tUsingModelInfo.wstrName = path(pPrototype->m_wstrModelPath).stem();
        tDesc.eState = CMapObject::EState::Preview;

        tDesc.vecSRTs.push_back(tSRT);

        tDesc.wstrLayerTag = g_wszMapObjectLayer;
        tDesc.vecClientMakePathDesc = pPrototype->m_vecClientMakePathDesc;


        return   
            static_cast<CMapObject*>(CGameInstance::GetInstance()->Add_GameObject(
                tDesc.iLevelIndex, L"Prototype_GameObject_MapObject", tDesc.iLevelIndex, g_wszMapObjectLayer, &tDesc));
    }
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
    m_pMapToolManager = nullptr;

    for (auto& OverrideMtl : m_vecOverrideMaterials)
        Safe_Release(OverrideMtl);

    for (auto& Desc : m_vecClientMakePathDesc)
        Safe_Delete(Desc);

    Super::Free();
}


#pragma region 기본적인 Render 함수

HRESULT CMapObject::Render_Default(_int iPass)
{
    CShader*    pShader     = Get_Component<CShader>();     if (pShader == nullptr)         return E_FAIL;
    CModel*     pModel      = Get_Component<CModel>();      if (pModel == nullptr)          return E_FAIL;
    CTransform* pTransform  = Get_Component<CTransform>();  if (pTransform == nullptr)      return E_FAIL;



    pShader->Bind_TransformData(pTransform->Get_WorldMatrix());
    _uint iMeshCount = static_cast<_uint>(pModel->Get_MeshCount());

    /* Client Make Path를 이용한다 */
    pShader->Set_Pass(iPass);
    if (FAILED(Set_GPU_MapObjectState(pShader)))
        return E_FAIL;


    for (_uint i = 0; i < iMeshCount; ++i)
    {
        pModel->Bind_Material(pShader, i);
        pModel->Bind_MaterialInstance(pShader, i);
        pShader->Apply();
        pModel->Render(i);
    }


    return S_OK;
}

HRESULT CMapObject::Render_Instance(_int iPass)
{
    CShader* pShader = Get_Component<CShader>();                    if (pShader == nullptr)             return E_FAIL;
    CModel* pModel = Get_Component<CModel>();                       if (pModel == nullptr)              return E_FAIL;
    CTransform* pTransform = Get_Component<CTransform>();           if (pTransform == nullptr)          return E_FAIL;
    CInstanceMesh* pInstanceMesh = Get_Component<CInstanceMesh>();  if (pInstanceMesh == nullptr)       return E_FAIL;

    pShader->Get_Scalar("g_iSelectInstanceID")->SetRawValue(&m_iSelectedInstanceID, 0, sizeof(m_iSelectedInstanceID));
    _uint iMeshCount = static_cast<_uint>(pModel->Get_MeshCount());


    _uint iInstanceCount = Get_InstanceCount();

    if (FAILED(Set_GPU_MapObjectState(pShader)))
        return E_FAIL;

    pShader->Set_Pass(iPass);

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

#pragma endregion



#pragma region Client Path Type 별 Render 함수

#pragma region 완전 기초 함수

HRESULT CMapObject::Render_MapObject()
{
    if (m_eMapObjectDrawType == EMapObject_DrawType::Instance)
        Render_Instance();
    else
        Render_Default();

    return S_OK;
}

#pragma endregion

#pragma region Static Object

HRESULT CMapObject::Render_StaticObject()
{
    if (m_eMapObjectDrawType == EMapObject_DrawType::Instance)
        return Render_Instance(0);
    else
        return Render_Default(0);

    return S_OK;
}
#pragma endregion

#pragma region LandScape

HRESULT CMapObject::Render_LandScape()
{
    /* LandScape 같은 경우는 Instance를 지원하지않는다  */
    if (m_eMapObjectDrawType == EMapObject_DrawType::Instance)
        return S_OK;

    CShader*        pShader         = Get_Component<CShader>();                         if (pShader == nullptr)             return E_FAIL;
    CModel*         pModel          = Get_Component<CModel>();                          if (pModel == nullptr)              return E_FAIL;
    CTransform*     pTransform      = Get_Component<CTransform>();                      if (pTransform == nullptr)          return E_FAIL;
    

    /* 지형은 Static으로 분류되야한다 */
    LANDSCAPE_DESC* pSelectedDesc = static_cast<LANDSCAPE_DESC*>(m_vecClientMakePathDesc[m_iSelectedInstanceID]);


    if (pSelectedDesc == nullptr) return S_OK;
   
    /* UV 좌표 업데이트 */
    if(FAILED(pShader->Get_Vector(g_szLandScape_TextureUV_LT)->SetRawValue(&pSelectedDesc->vTextureUV_LT, 0, sizeof(pSelectedDesc->vTextureUV_LT)))) return E_FAIL;
    if(FAILED(pShader->Get_Vector(g_szLandScape_TextureUV_RB)->SetRawValue(&pSelectedDesc->vTextureUV_RB, 0, sizeof(pSelectedDesc->vTextureUV_RB)))) return E_FAIL;
    
    _uint iMeshCount = static_cast<_uint>(pModel->Get_MeshCount());

    Set_GPU_MapObjectState(pShader);
   
    pShader->Bind_TransformData(pTransform->Get_WorldMatrix());
    pShader->Set_Pass(ENUM_TO_UINT(EMapObjectShaderPass::LandScape));

    for (_uint i = 0; i < iMeshCount; ++i)
    {
        pModel->Bind_Material(pShader, i);
        pModel->Bind_MaterialInstance(pShader, i);
        pShader->Apply();
        pModel->Render(i);
    }

    return S_OK;
}


#pragma endregion

#pragma region Plants

HRESULT CMapObject::Render_Plants(_uint iPassIndex)
{
    if (m_eMapObjectDrawType == EMapObject_DrawType::Default)
    {

        CShader* pShader = Get_Component<CShader>();                                        if (pShader == nullptr)         return E_FAIL;
        CModel* pModel = Get_Component<CModel>();                                           if (pModel == nullptr)          return E_FAIL;
        CTransform* pTransform = Get_Component<CTransform>();                               if (pTransform == nullptr)      return E_FAIL;
        /* 제일 앞에있는 친구의 Desc을 참고해서 해준다 모든애들을 가져올수없음 */
        PLANTS_DESC* pTreeDesc = static_cast<PLANTS_DESC*>(m_vecClientMakePathDesc.front());    if (pTreeDesc == nullptr)       return E_FAIL;


        pShader->Bind_TransformData(pTransform->Get_WorldMatrix());
        _uint iMeshCount = static_cast<_uint>(pModel->Get_MeshCount());

        /* Client Make Path를 이용한다 */
        pShader->Set_Pass(iPassIndex);


        if (FAILED(Set_GPU_MapObjectState(pShader)))
            return E_FAIL;


        for (_uint i = 0; i < iMeshCount; ++i)
        {
            pModel->Set_MI_TintColor(i, pTreeDesc->vMITint_Color);
            pModel->Bind_Material(pShader, i);
            pModel->Bind_MaterialInstance(pShader, i);
            pShader->Apply();
            pModel->Render(i);
        }

    }
    else if (m_eMapObjectDrawType == EMapObject_DrawType::Instance)
    {
        CShader* pShader = Get_Component<CShader>();                                        if (pShader == nullptr)             return E_FAIL;
        CModel* pModel = Get_Component<CModel>();                                           if (pModel == nullptr)              return E_FAIL;
        CTransform* pTransform = Get_Component<CTransform>();                               if (pTransform == nullptr)          return E_FAIL;
        CInstanceMesh* pInstanceMesh = Get_Component<CInstanceMesh>();                      if (pInstanceMesh == nullptr)       return E_FAIL;
        PLANTS_DESC* pTreeDesc = static_cast<PLANTS_DESC*>(m_vecClientMakePathDesc.front());    if (pTreeDesc == nullptr)           return E_FAIL;

        pShader->Get_Scalar("g_iSelectInstanceID")->SetRawValue(&m_iSelectedInstanceID, 0, sizeof(m_iSelectedInstanceID));
        _uint iMeshCount = static_cast<_uint>(pModel->Get_MeshCount());
        _uint iInstanceCount = Get_InstanceCount();


        pShader->Bind_TransformData(pTransform->Get_WorldMatrix());

        /* Client Make Path를 이용한다 */
        pShader->Set_Pass(iPassIndex);


        if (FAILED(Set_GPU_MapObjectState(pShader)))
            return E_FAIL;

        pInstanceMesh->Bind_Instance(1);
        for (_uint i = 0; i < iMeshCount; ++i)
        {
            pModel->Set_MI_TintColor(i, pTreeDesc->vMITint_Color);
            pModel->Bind_Material(pShader, i);
            pModel->Bind_MaterialInstance(pShader, i);
            pShader->Apply();
            pModel->Render_Instance(i, iInstanceCount);
        }
        pInstanceMesh->Unbind_Resource(1);
    }
    else
        return E_FAIL;

    return S_OK;
}
#pragma endregion

#pragma region Grass

HRESULT CMapObject::Render_Grass()
{
    if (m_eMapObjectDrawType == EMapObject_DrawType::Default)
    {

        CShader* pShader = Get_Component<CShader>();                                        if (pShader == nullptr)         return E_FAIL;
        CModel* pModel = Get_Component<CModel>();                                           if (pModel == nullptr)          return E_FAIL;
        CTransform* pTransform = Get_Component<CTransform>();                               if (pTransform == nullptr)      return E_FAIL;
        /* 제일 앞에있는 친구의 Desc을 참고해서 해준다 모든애들을 가져올수없음 */
        GRASS_DESC* pDesc = static_cast<GRASS_DESC*>(m_vecClientMakePathDesc.front());    if (pDesc == nullptr)       return E_FAIL;

        CB_GrassData tData{};
        tData.g_fGrassDT = m_fDT;

        const Vec3* pMinMax = pModel->Get_StaticModelMinMax(); if (pMinMax == nullptr) return E_FAIL;
        tData.g_fGrassMaxHeight = pMinMax[1].y;
        tData.g_fGrassWaveSize = pDesc->fGrassWaveSize;
        tData.g_fGrassSwaySpeed = pDesc->fGrassSwaySpeed;

       
        if (FAILED(pShader->Get_ConstantBuffer("CB_GrassData")->SetRawValue(&tData, 0, sizeof(CB_GrassData))))
            return E_FAIL;

        pShader->Bind_TransformData(pTransform->Get_WorldMatrix());
        _uint iMeshCount = static_cast<_uint>(pModel->Get_MeshCount());

        /* Client Make Path를 이용한다 */
        pShader->Set_Pass(ENUM_TO_UINT(EMapObjectShaderPass::Grass));


        if (FAILED(Set_GPU_MapObjectState(pShader)))
            return E_FAIL;


        for (_uint i = 0; i < iMeshCount; ++i)
        {
            pModel->Set_MI_TintColor(i, pDesc->vMITint_Color);
            pModel->Bind_Material(pShader, i);
            pModel->Bind_MaterialInstance(pShader, i);
            pShader->Apply();
            pModel->Render(i);
        }

    }
    else if (m_eMapObjectDrawType == EMapObject_DrawType::Instance)
    {
        CShader* pShader = Get_Component<CShader>();                                        if (pShader == nullptr)             return E_FAIL;
        CModel* pModel = Get_Component<CModel>();                                           if (pModel == nullptr)              return E_FAIL;
        CTransform* pTransform = Get_Component<CTransform>();                               if (pTransform == nullptr)          return E_FAIL;
        CInstanceMesh* pInstanceMesh = Get_Component<CInstanceMesh>();                      if (pInstanceMesh == nullptr)       return E_FAIL;
        GRASS_DESC* pDesc = static_cast<GRASS_DESC*>(m_vecClientMakePathDesc.front());      if (pDesc == nullptr)       return E_FAIL;

        CB_GrassData tData{};
        tData.g_fGrassDT = m_fDT;
        const Vec3* pMinMax = pModel->Get_StaticModelMinMax(); if (pMinMax == nullptr) return E_FAIL;
        tData.g_fGrassMaxHeight = pMinMax[1].y;

        tData.g_fGrassWaveSize = pDesc->fGrassWaveSize;
        tData.g_fGrassSwaySpeed = pDesc->fGrassSwaySpeed;

        if (FAILED(pShader->Get_ConstantBuffer("CB_GrassData")->SetRawValue(&tData, 0, sizeof(CB_GrassData))))
            return E_FAIL;


        pShader->Get_Scalar("g_iSelectInstanceID")->SetRawValue(&m_iSelectedInstanceID, 0, sizeof(m_iSelectedInstanceID));
        _uint iMeshCount = static_cast<_uint>(pModel->Get_MeshCount());
        _uint iInstanceCount = Get_InstanceCount();


        pShader->Bind_TransformData(pTransform->Get_WorldMatrix());

        /* Client Make Path를 이용한다 */
        pShader->Set_Pass(ENUM_TO_UINT(EMapObjectShaderPass::Grass));

        if (FAILED(Set_GPU_MapObjectState(pShader)))
            return E_FAIL;

        pInstanceMesh->Bind_Instance(1);
        for (_uint i = 0; i < iMeshCount; ++i)
        {
            pModel->Set_MI_TintColor(i, pDesc->vMITint_Color);
            pModel->Bind_Material(pShader, i);
            pModel->Bind_MaterialInstance(pShader, i);
            pShader->Apply();
            pModel->Render_Instance(i, iInstanceCount);
        }
        pInstanceMesh->Unbind_Resource(1);
    }
    else
        return E_FAIL;

    return S_OK;
}
#pragma endregion

#pragma region Tree

HRESULT CMapObject::Render_Tree()
{
    if (FAILED(Render_Plants(ENUM_TO_UINT(EMapObjectShaderPass::Tree))))
        return E_FAIL;

    return S_OK;
}
#pragma endregion

#pragma region Moss
HRESULT CMapObject::Render_Moss()
{
    if (FAILED(Render_Plants(ENUM_TO_UINT(EMapObjectShaderPass::Moss))))
        return E_FAIL;

    return S_OK;
}
#pragma endregion

#pragma region Vine
HRESULT CMapObject::Render_Vine()
{
    if (FAILED(Render_Plants(ENUM_TO_UINT(EMapObjectShaderPass::Vine))))
        return E_FAIL;

    return S_OK;
}
#pragma endregion 

#pragma region Bush
HRESULT CMapObject::Render_Bush()
{
    if (FAILED(Render_Plants(ENUM_TO_UINT(EMapObjectShaderPass::Bush))))
        return E_FAIL;

    return S_OK;
}
#pragma endregion 

#pragma region Rock
HRESULT CMapObject::Render_Rock()
{
    if (m_eMapObjectDrawType == EMapObject_DrawType::Instance)
        Render_Instance(ENUM_TO_UINT(EMapObjectShaderPass::Rock));
    else
        Render_Default(ENUM_TO_UINT(EMapObjectShaderPass::Rock));

    return S_OK;
}
#pragma endregion

#pragma region Water
HRESULT CMapObject::Render_Water()
{
    if (m_eMapObjectDrawType == EMapObject_DrawType::Default)
    {
        CShader* pShader = Get_Component<CShader>();                                        if (pShader == nullptr)         return E_FAIL;
        CModel* pModel = Get_Component<CModel>();                                           if (pModel == nullptr)          return E_FAIL;
        CTransform* pTransform = Get_Component<CTransform>();                               if (pTransform == nullptr)      return E_FAIL;
        WATER_DESC* pDesc = static_cast<WATER_DESC*>(m_vecClientMakePathDesc.front());      if (pDesc == nullptr)           return E_FAIL;
        

        /* Water CB 가져오기 */
        ID3DX11EffectConstantBuffer* pCB = pShader->Get_ConstantBuffer(g_szCB_WaterData);
        if (!pCB->IsValid())
        {
            MSG_BOX("Water CB 생성 실패");
            return E_FAIL;
        }

        if (m_fDT >= 1000.f)        /* 일정값 이상 넘어가면 다시재생 */
            m_fDT = 0.f;
        CB_WaterData tData{};
        tData.g_fWaterDT = m_fDT;
        tData.g_WaterTexBindingFlags = 0;
        tData.g_vWaterSpeed1            = pDesc->vSpeed1;
        tData.g_vWaterSpeed2            = pDesc->vSpeed2;

        tData.g_vWaterUVPower           = pDesc->vWaterUVPower;

        tData.g_vWaterDistortionSpeed   = pDesc->vDistortionSpeed;
        tData.g_fDistortionPower        = pDesc->fDistortionPower;

        tData.g_fSparklePower           = pDesc->fSparklePower;
        tData.g_vSparkleUVPower         = pDesc->vSparkleUVPower;

        array<ID3D11ShaderResourceView*, ENUM_TO_UINT(EWaterTextureType::END)>  arraySRVs;
        arraySRVs.fill(nullptr);

        /* Water Texture Binding */
        for (_uint i = 0; i < ENUM_TO_UINT(EWaterTextureType::END); ++i)
        {
            /* Texture가 바인딩 되어있다면 */
            if (pDesc->arrayTextureBase[i])
            {
                arraySRVs[i] = pDesc->arrayTextureBase[i]->Get_SRV();
                Engine_Utils::Add_Flag(tData.g_WaterTexBindingFlags, 1 << i); //걍써
            }
        }

        pCB->SetRawValue(&tData,0,sizeof(CB_WaterData));

        pShader->Get_SRV(g_szWaterTexture)->SetResourceArray(&arraySRVs[0] , 0 , ENUM_TO_UINT(EWaterTextureType::END) );

        /* 월드 매트릭스 바인딩 */
        const Matrix& pMatrix = pTransform->Get_WorldMatrix();
        pShader->Bind_TransformData(pMatrix);

        _uint iMeshCount = static_cast<_uint>(pModel->Get_MeshCount());

        /* Client Make Path를 이용한다 */
        pShader->Set_Pass(ENUM_TO_UINT(EMapObjectShaderPass::Water));

        /* MapObject State 바인딩 */
        if (FAILED(Set_GPU_MapObjectState(pShader)))
            return E_FAIL;

        /* Render 호출 */
        for (_uint i = 0; i < iMeshCount; ++i)
        {
            pModel->Set_MI_TintColor(i,pDesc->vMI_TintColor);
            pModel->Bind_Material(pShader, i);
            pModel->Bind_MaterialInstance(pShader, i);
            pShader->Apply();
            pModel->Render(i);
        }
    }

    else if (m_eMapObjectDrawType == EMapObject_DrawType::Instance)
    {
        CShader* pShader = Get_Component<CShader>();                                        if (pShader == nullptr)             return E_FAIL;
        CModel* pModel = Get_Component<CModel>();                                           if (pModel == nullptr)              return E_FAIL;
        CTransform* pTransform = Get_Component<CTransform>();                               if (pTransform == nullptr)          return E_FAIL;
        CInstanceMesh* pInstanceMesh = Get_Component<CInstanceMesh>();                      if (pInstanceMesh == nullptr)       return E_FAIL;
        WATER_DESC* pDesc = static_cast<WATER_DESC*>(m_vecClientMakePathDesc.front());      if (pDesc == nullptr)               return E_FAIL;


        /* Water CB 가져오기 */
        ID3DX11EffectConstantBuffer* pCB = pShader->Get_ConstantBuffer(g_szCB_WaterData);
        if (!pCB->IsValid())
        {
            MSG_BOX("Water CB 생성 실패");
            return E_FAIL;
        }


        if (m_fDT >= 1000.f)        /* 일정값 이상 넘어가면 다시재생 */
            m_fDT = 0.f;

        CB_WaterData tData{};
        tData.g_fWaterDT = m_fDT;
        tData.g_WaterTexBindingFlags = 0;

        array<ID3D11ShaderResourceView*, ENUM_TO_UINT(EWaterTextureType::END)>  arraySRVs;
        arraySRVs.fill(nullptr);

        /* Water Texture Binding */
        for (_uint i = 0; i < ENUM_TO_UINT(EWaterTextureType::END); ++i)
        {
            /* Texture가 바인딩 되어있다면 */
            if (pDesc->arrayTextureBase[i])
            {
                arraySRVs[i] = pDesc->arrayTextureBase[i]->Get_SRV();
                Engine_Utils::Add_Flag(tData.g_WaterTexBindingFlags, 1 << i); //걍써
            }
        }
        pCB->SetRawValue(&tData, 0, sizeof(CB_WaterData));

        pShader->Get_SRV(g_szWaterTexture)->SetResourceArray(&arraySRVs[0], 0, ENUM_TO_UINT(EWaterTextureType::END));

        /* 월드 매트릭스 바인딩 */
        pShader->Bind_TransformData(pTransform->Get_WorldMatrix());

        pShader->Get_Scalar("g_iSelectInstanceID")->SetRawValue(&m_iSelectedInstanceID, 0, sizeof(m_iSelectedInstanceID));
        _uint iMeshCount = static_cast<_uint>(pModel->Get_MeshCount());
        _uint iInstanceCount = Get_InstanceCount();


        pShader->Bind_TransformData(pTransform->Get_WorldMatrix());

        /* Client Make Path를 이용한다 */
        pShader->Set_Pass(ENUM_TO_UINT(EMapObjectShaderPass::Water));

        if (FAILED(Set_GPU_MapObjectState(pShader)))
            return E_FAIL;

        pInstanceMesh->Bind_Instance(1);
        for (_uint i = 0; i < iMeshCount; ++i)
        {
            pModel->Set_MI_TintColor(i, pDesc->vMI_TintColor);
            pModel->Bind_Material(pShader, i);
            pModel->Bind_MaterialInstance(pShader, i);
            pShader->Apply();
            pModel->Render_Instance(i, iInstanceCount);
        }
        pInstanceMesh->Unbind_Resource(1);
    }
    else
        return E_FAIL;

    return S_OK;
}
#pragma endregion


#pragma region Batch Player
HRESULT CMapObject::Render_Batch_Player()
{
    if (m_eMapObjectDrawType != EMapObject_DrawType::Default)
        return S_OK;

    if (FAILED(Render_Default()))
        return S_OK;

    return S_OK;
}
#pragma endregion

#pragma region Batch Monster
HRESULT CMapObject::Render_Batch_Monster()
{
    if (m_eMapObjectDrawType != EMapObject_DrawType::Default)
        return S_OK;

    if (FAILED(Render_Default()))
        return S_OK;

    return S_OK;
}
HRESULT CMapObject::Render_Batch_Object()
{
    if (m_eMapObjectDrawType != EMapObject_DrawType::Default)
        return S_OK;
    

    if (m_vecClientMakePathDesc.empty())
        return E_FAIL;

    BATCH_OBJECT_DESC* pDesc = static_cast<BATCH_OBJECT_DESC*>(m_vecClientMakePathDesc.front());
    if (pDesc == nullptr) return E_FAIL;


    switch (pDesc->eBatchObjectType)
    {
    case DTO::EMakeObjectType::Battle_Field:
    {
        if (FAILED(Render_Default(ENUM_TO_UINT(EMapObjectShaderPass::StaticObject))))
            return E_FAIL;

        /* Battle Field Collider Update */
        BATTLE_FIELD_DESC* pBattelFieldDesc = static_cast<BATTLE_FIELD_DESC*>(pDesc->pBatchObjectDesc);
        if (pBattelFieldDesc == nullptr) return E_FAIL;

        CTransform* pTs = Get_Component<CTransform>();
        pBattelFieldDesc->Update_Collider(&pTs->Get_WorldMatrix());

        if (pBattelFieldDesc->eFieldType == BATTLE_FIELD_DESC::Field_Type::Box)
        {
            pBattelFieldDesc->pBattleFieldColliderBox->Render();
        }
        else
        {
            pBattelFieldDesc->pBattleFieldColliderSphere->Render();
        }


    }
    break;
    default:                            return E_FAIL;;
    }

    return S_OK;
}
#pragma endregion



#pragma region TriggerBox Change Level

HRESULT CMapObject::Render_TriggerBox_ChangeLevel()
{
    if (FAILED(Render_Collider()))
        return E_FAIL;

    return S_OK;
}
#pragma endregion
#pragma region TriggerBox Monster Spawner
HRESULT CMapObject::Render_TriggerBox_MonsterSpawner()
{
    if (FAILED(Render_Collider()))
        return E_FAIL;


    if (m_vecClientMakePathDesc.empty()) return E_FAIL;

    /* Debug Mode가 켜져있다면 Model Render를 해보자.....*/
    TRIGGERBOX_MONSTERSPAWNER_DESC* pDesc = static_cast<TRIGGERBOX_MONSTERSPAWNER_DESC*>(m_vecClientMakePathDesc.front());
    if (pDesc == nullptr) return E_FAIL;

    Matrix WorldMatrix{Matrix::Identity};
    
    CShader* pShader = Get_Component<CShader>();
    if (pShader == nullptr) return E_FAIL;

    Set_GPU_MapObjectState(pShader);

    for (auto& SpawnMonsterData : pDesc->vecMonsterSpawnData)
    {
        if (SpawnMonsterData.isPreviewDebugModel == false)
            continue;
        if (SpawnMonsterData.pDebugModel == nullptr)
            continue;

        WorldMatrix = Matrix::CreateScale(SpawnMonsterData.vScale) * 
            Matrix::CreateFromYawPitchRoll(XMConvertToRadians(SpawnMonsterData.vPitchYawRoll.y), XMConvertToRadians(SpawnMonsterData.vPitchYawRoll.x), XMConvertToRadians(SpawnMonsterData.vPitchYawRoll.z))
            * Matrix::CreateTranslation(SpawnMonsterData.vPosition);

        pShader->Bind_TransformData(WorldMatrix);

        CModel* pModel = SpawnMonsterData.pDebugModel;
        _uint iMeshCount = pModel->Get_MeshCount();

        /* Client Make Path를 이용한다 */
        pShader->Set_Pass(ENUM_TO_UINT(EClientMakePath::StaticObject));


        for (_uint i = 0; i < iMeshCount; ++i)
        {
            pModel->Bind_Material(pShader, i);
            pModel->Bind_MaterialInstance(pShader, i);
            pShader->Apply();
            pModel->Render(i);
        }

    }




    return S_OK;
}

#pragma endregion

#pragma region TriggerBox GlobalEvent BroadCaster

HRESULT CMapObject::Render_TriggerBox_GlobalEvent_BroadCaster()
{
    if (FAILED(Render_Collider()))
        return E_FAIL;

    return S_OK;
}
#pragma endregion

#pragma endregion




#pragma region Collider Draw전용

HRESULT CMapObject::Render_Collider()
{
    //if (m_eMapObjectDrawType != EMapObject_DrawType::Collider)
    //    return E_FAIL;

    CTransform* pTransform = Get_Component<CTransform>();
    CCollider* pCollider = Get_Component<CCollider>();

    if (pTransform == nullptr) return E_FAIL;
    if (pCollider == nullptr) return E_FAIL;

    /* Update전 콜라이더 업데이트해주기 별로안되니까 상관없을듯? */
    pCollider->Update(pTransform->Get_WorldMatrix());
    pCollider->Render();

    return S_OK;
}

#pragma endregion

#pragma endregion




#pragma endregion