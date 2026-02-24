#include "pch.h"
#include "Mesh.h"
#include "Model.h"
#include "Shader.h"
#include "Bounds.h"
#include "MapObject.h"
#include "InstanceMesh.h"
#include "Engine_Utils.h"
#include "PhysicsCollider.h"
#include "PhysicsRigidBody.h"
#include "GameInstance.h"

CMapObject::CMapObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext), m_eMapObjectType{EMapObject_Type::END}
{
}

CMapObject::CMapObject(const CMapObject& rhs)
	: CGameObject(rhs), m_eMapObjectType(rhs.m_eMapObjectType) 
    , m_eMapObjectDrawType{ rhs.m_eMapObjectDrawType }
    , m_isUELoaded{rhs.m_isUELoaded}
    , m_vecMatrix{rhs.m_vecMatrix }
    , m_iSectionNum{rhs.m_iSectionNum}
{
}


HRESULT	CMapObject::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT	CMapObject::Initialize(void* pArg)
{
	if(FAILED(Super::Initialize(pArg)))
		return E_FAIL;

    MAPOBJECT_DESC* pDesc = static_cast<MAPOBJECT_DESC*>(pArg);


    m_isUELoaded    = pDesc->isUELoaded;
    m_eMapObjectDrawType = pDesc->eMapObjectDrawType;
    m_strName       = path(pDesc->wstrModelPath).filename().stem().string();
    m_iSectionNum  = pDesc->iSectionNum;

    if (FAILED(Ready_Transform(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Component(pDesc)))
        return E_FAIL;

	return S_OK;
}

HRESULT CMapObject::Ready_Transform(MAPOBJECT_DESC* pDesc)
{
    if (pDesc->vecSRT.empty())
        return E_FAIL;

    m_vecMatrix.reserve(pDesc->vecSRT.size());
    m_vecVisibleMatrix.reserve(pDesc->vecSRT.size());
    m_vecVisibleIndex.reserve(pDesc->vecSRT.size());
    for (auto& SRT : pDesc->vecSRT)
        m_vecMatrix.push_back(SRT.Get_World());

    if (m_eMapObjectDrawType != EMapObject_DrawType::Instance)
    {
        /* Transform 에 등록 */
        Get_Component<CTransform>()->Set_WorldMatrix(pDesc->vecSRT.front().Get_World());
    }

    return S_OK;
}


HRESULT	CMapObject::Ready_Component(MAPOBJECT_DESC* pDesc)
{
    m_eMapObjectDrawType == EMapObject_DrawType::Instance ?
        Add_Component<CShader>(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Shader_VtxInstanceMesh", nullptr)
        : Add_Component<CShader>(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Shader_VtxMesh", nullptr);

    const wstring wstrModelTag = L"Prototype_Component_Model_";

    if (m_eMapObjectDrawType != EMapObject_DrawType::Collider)
    {
        wstring wstrFileName = path(pDesc->wstrModelPath).filename().stem();
        wstrFileName = wstrModelTag + wstrFileName;
        CBase* pFinded = { nullptr };

        if (pFinded = m_pGameInstance->Find_Prototype(pDesc->iLevelIndex, wstrFileName))
        {
            Add_Component<CModel>(pDesc->iLevelIndex, wstrFileName, nullptr);
        }
        else
        {
            /* Model 생성 */
            CModel::MODEL_ORIGIN_DESC tModelDesc{};
            tModelDesc.eType = EModelType::STATIC;
            tModelDesc.wstrModelFolderName = pDesc->wstrModelPath;
            tModelDesc.iPrototypeLevelIndex = pDesc->iLevelIndex;
            CModel* pModel = CModel::Create(m_pDevice, m_pDeviceContext, &tModelDesc);
            if (FAILED(m_pGameInstance->Add_Prototype(pDesc->iLevelIndex, wstrFileName, pModel)))
                return E_FAIL;
            else
            {
                m_pGameInstance->RegisterPhysicsMesh(tModelDesc.iPrototypeLevelIndex , wstrFileName);
                Add_Component<CModel>(pDesc->iLevelIndex , wstrFileName  , nullptr );
            }
        }
        /* Bounds 생성 */
        CBounds::BOUND_COMP_DESC desc{};
        desc.fRatio = 1.f;
        desc.pMinMax = Get_Component<CModel>()->Get_StaticModelMinMax();
        if (FAILED(Add_Component<CBounds>(0, L"Prototype_Component_Bounds", &desc)))
            return E_FAIL;
        Get_Component<CBounds>()->Update_BoundingDesc(Get_Component<CTransform>()->Get_WorldMatrix());
    }

    if (m_eMapObjectDrawType == EMapObject_DrawType::Instance)
    {
        /* InstanceMehs  */
        CInstanceMesh::INSTANCEMESH_DESC tInstanceMeshDesc{};
        tInstanceMeshDesc.IB_Usage = D3D11_USAGE_DYNAMIC;
        tInstanceMeshDesc.VB_Usage = D3D11_USAGE_DYNAMIC;
        tInstanceMeshDesc.vecInstanceMatrixPointer = &m_vecMatrix;
        tInstanceMeshDesc.pModelMinMax = Get_Component<CModel>()->Get_StaticModelMinMax();
        Add_Component<CInstanceMesh>(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_VIBuffer_InstanceMesh", &tInstanceMeshDesc);

        CBounds::BOUND_COMP_DESC desc{};
        desc.fRatio = 1.f;
        desc.pMinMax = Get_Component<CInstanceMesh>()->Get_InstanceWorldMinMax();
        if (FAILED(Add_Component<CBounds>(0, L"Prototype_Component_Bounds", &desc)))
            return E_FAIL;

        Get_Component<CBounds>()->Update_BoundingDesc(Matrix::Identity);
        if (FAILED(Get_Component<CBounds>()->Add_SubBounds(Get_Component<CModel>()->Get_StaticModelMinMax(), span<Matrix>(m_vecMatrix.data(), m_vecMatrix.size()), 1.f)))
            return E_FAIL;
    }

    if (FAILED(Ready_PhysicsComponent(pDesc)))
        return E_FAIL;

	return S_OK;
}

HRESULT	CMapObject::Add_MapToolComponent(CMapObject::COMPONENT eType)
{
	return S_OK;
}

HRESULT	CMapObject::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	Get_Component<CPhysicsRigidBody>()->Awake();

	return S_OK;
}

void	CMapObject::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}
void	CMapObject::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}
void	CMapObject::Update_Late(const _float fTimeelta)
{
	Super::Update_Late(fTimeelta);
}

void	CMapObject::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
#ifdef _DEBUG
    if (m_eMapObjectDrawType == EMapObject_DrawType::Instance)
        m_pGameInstance->Push_DebugComponent(Get_Component<CBounds>());
#endif
}

HRESULT CMapObject::Ready_OverrideMtl(const DTO::USING_MODEL_INFO& tUsingModelInfo)
{

    return S_OK;
}

HRESULT CMapObject::Ready_PhysicsComponent(MAPOBJECT_DESC* pDesc)
{
    HRESULT result{};

    if (FAILED(Ready_PhysicsCollider(pDesc)))
        result = E_FAIL;

    if (FAILED(Ready_PhysicsRigidBody(pDesc)))
        result = E_FAIL;

    return result;
}

HRESULT CMapObject::Ready_PhysicsCollider(MAPOBJECT_DESC* pDesc)
{
    PHYSICSCOLLIDER_DESC pcDesc{};
    wstring wstrModelName = path(pDesc->wstrModelPath).filename().stem().wstring();

    pcDesc.wstrModelPrototypeTag = L"Prototype_Component_Model_" + wstrModelName;
    pcDesc.bIsConvex = false;

    CPhysicsCollider* pCollider = CPhysicsCollider::Create(m_pDevice, m_pDeviceContext, &pcDesc);
    if (pCollider)
    {
        if (FAILED(m_pGameInstance->Add_Prototype(pDesc->iLevelIndex, L"Prototype_Component_Physics_Collider_" + wstrModelName, pCollider)))
            Safe_Release(pCollider);
    }

    PHYSICSCOLLIDER_DESC cloneDesc{};
    cloneDesc.eFilterLayer = PHYSICSFILTERGROUP::MAP;
    cloneDesc.iFilterMask = 0xFFFFFFFF;
    cloneDesc.bSetOnlyFilter = true;

    if (FAILED(Add_Component<CPhysicsCollider>(pDesc->iLevelIndex, L"Prototype_Component_Physics_Collider_" + wstrModelName, &cloneDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CMapObject::Ready_PhysicsRigidBody(MAPOBJECT_DESC* pDesc)
{
    PHYSICSRIGIDBODY_DESC desc{};
    desc.eType = EPhysicsActorType::STATIC;
    desc.detection = EPhysicsCollisionDetection::DISCRETE;
    desc.fDensity = 10.f;
    desc.bUseGravity = false;
    desc.bIsKinematic = false;
    desc.fLinearDamping = 0.f;
    desc.fAngularDamping = 0.f;
    desc.pOwnerMatrices = m_vecMatrix;

    for (auto& srt : pDesc->vecSRT)
        desc.vecSRT.emplace_back(srt.vScale, srt.vQuat, srt.vPosition);

    if (FAILED(Add_Component<CPhysicsRigidBody>(0, L"Prototype_Component_Physics_RigidBody", &desc)))
        return E_FAIL;

    return S_OK;
}

HRESULT	CMapObject::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

    return m_eMapObjectDrawType == EMapObject_DrawType::Instance ? Render_Instance() : m_eMapObjectDrawType == EMapObject_DrawType::Default ? Render_Default() : S_OK ;

}

HRESULT	CMapObject::Render_Instance(_uint iPassIndex)
{
    CShader* pShader = Get_Component<CShader>();                    if (pShader == nullptr)             return E_FAIL;
    CModel* pModel = Get_Component<CModel>();                       if (pModel == nullptr)              return E_FAIL;
    CTransform* pTransform = Get_Component<CTransform>();           if (pTransform == nullptr)          return E_FAIL;
    CInstanceMesh* pInstanceMesh = Get_Component<CInstanceMesh>();  if (pInstanceMesh == nullptr)       return E_FAIL;
    _uint iMeshCount = static_cast<_uint>(pModel->Get_MeshCount());
    _uint iInstanceCount = static_cast<_uint>(pInstanceMesh->Get_InstanceCount());
    Filtering_Visible(iInstanceCount);

    // 그릴거 없으면 그냥 패스
    if (iInstanceCount <= 0)
        return S_OK;

    if (FAILED(Update_InstanceBuffer(pInstanceMesh)))
        return E_FAIL;
    pShader->Set_Pass(iPassIndex);
    pShader->Bind_ObjectInfoData(m_tObjectInfoDesc);
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

HRESULT	CMapObject::Render_Default(_uint iPassIndex)
{
    CShader*    pShader     = Get_Component<CShader>();      if (pShader == nullptr)         return E_FAIL;
    CModel*     pModel      = Get_Component<CModel>();       if (pModel == nullptr)          return E_FAIL;
    CTransform* pTransform  = Get_Component<CTransform>();   if (pTransform == nullptr)      return E_FAIL;


    /* WorldMatrix 바인딩 */
    pShader->Set_Pass(iPassIndex);
    pShader->Bind_ObjectInfoData(m_tObjectInfoDesc);
    pShader->Bind_TransformData(pTransform->Get_WorldMatrix());
    _uint iMeshCount = static_cast<_uint>(pModel->Get_MeshCount());

    for (_uint i = 0; i < iMeshCount; ++i)
    {
        pModel->Bind_Material(pShader, i);
        pModel->Bind_MaterialInstance(pShader, i);
        pShader->Apply();
        pModel->Render(i);
    }

    return S_OK;
}

void CMapObject::Compute_InstanceGroupMinMax(const Vec3* pComputedFinalMinMax, OUT Vec3* pMinMax)
{
    if (m_vecMatrix.size() <= 0)
        return;

    const Vec3& vLocalMin = pComputedFinalMinMax[0];
    const Vec3& vLocalMax = pComputedFinalMinMax[1];

    BoundingBox localBox = Engine_Utils::MakeAABB_FromMinMax(vLocalMin, vLocalMax);
    BoundingBox groupBox;

    // 인스턴스 모델중 첫놈으로 기준 잡기
    localBox.Transform(groupBox, m_vecMatrix[0]);

    Vec3& vOutMin = pMinMax[0];
    Vec3& vOutMax = pMinMax[1];

    vOutMin = groupBox.Center - groupBox.Extents;
    vOutMax = groupBox.Center + groupBox.Extents;

    for (size_t i = 1; i < m_vecMatrix.size(); ++i)
    {
        BoundingBox wBox;
        localBox.Transform(wBox, m_vecMatrix[i]);

        Vec3 vMinMax[2] =
        {
            wBox.Center - wBox.Extents,
            wBox.Center + wBox.Extents
        };

        Engine_Utils::Merge_MinMax(vMinMax, vOutMin, vOutMax);
    }
}

_bool CMapObject::Compute_ModelLocalMinMax(CModel* pModel, OUT Vec3 outMinMax[2])
{
    if (pModel == nullptr)
        return false;

    const _uint iMeshCount = pModel->Get_MeshCount();
    if (iMeshCount <= 0)
        return false;

    {
        CMesh* pMesh0 = pModel->Get_Mesh(0);
        if (pMesh0 == nullptr)
            return false;

        const Vec3* pMinMax = pMesh0->Get_MinMax();
        outMinMax[0] = pMinMax[0];
        outMinMax[1] = pMinMax[1];
    }

    for (_uint i = 1; i < iMeshCount; ++i)
    {
        CMesh* pMesh = pModel->Get_Mesh(i);
        if (pMesh == nullptr)
            continue;

        const Vec3* pMinMax = pMesh->Get_MinMax();
        Engine_Utils::Merge_MinMax(pMinMax, outMinMax[0], outMinMax[1]);
    }


    return true;
}

void CMapObject::Filtering_Visible(OUT _uint& iInstanceCount)
{
    BoundingFrustum* pWorldFrustrum = m_pGameInstance->Get_BoundingFrustrum_World();
    CBounds* pBounds = Get_Component<CBounds>();
    if (pBounds == nullptr || pWorldFrustrum == nullptr)
        return;

    pBounds->IntersectWith_Frustrum_SubBounds(pWorldFrustrum, m_vecVisibleIndex);
    iInstanceCount = (_uint)m_vecVisibleIndex.size();
}

HRESULT CMapObject::Update_InstanceBuffer(CInstanceMesh* pMesh)
{
    size_t iInstanceCount = m_vecVisibleIndex.size();
    m_vecVisibleMatrix.resize(iInstanceCount);
    for (size_t i = 0; i < m_vecVisibleIndex.size(); ++i)
    {
        const _uint& iVisibleIndex = m_vecVisibleIndex[i];
        m_vecVisibleMatrix[i] = m_vecMatrix[iVisibleIndex];
    }

    pMesh->Update_Matrix(m_vecVisibleMatrix);
    return S_OK;
}


void CMapObject::Free()
{
	Super::Free();


}
