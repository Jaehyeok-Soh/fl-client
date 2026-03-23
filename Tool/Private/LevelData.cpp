#include "pch.h"
#include "LevelData.h"
#include "DataStruct_Map.h"
#include "DataDocument_Map.h"
#include "Shader.h"
#include "Model.h"
#include "MapToolManager.h"
#include "Bounding_AABB.h"
#include "CameraMan.h"
#include "Transform.h"
#include "GameInstance.h"

CLevelData::CLevelData(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : CToolObject(eType, pDevice, pDeviceContext)
    , m_strTextureSplatingInfoName{ "None" }
    , m_tCB_EnvData{}
    , m_pMeshShader{nullptr}
    , m_pInstMeshSahder{nullptr}
    , m_vMapMinMaxBox_Center{ 0.f,0.f,0.f }
    , m_vMapMinMaxBox_Extents{ 1.f,1.f,1.f }
    , m_pSkyBoxTexture{nullptr}
    , m_strSkyBoxModelName{"None"}
{
}

CLevelData::CLevelData(const CLevelData& rhs)
    : CToolObject(rhs) 
    , m_strTextureSplatingInfoName{rhs.m_strTextureSplatingInfoName }
    , m_tCB_EnvData{rhs.m_tCB_EnvData}
    , m_pSkyBoxTexture(rhs.m_pSkyBoxTexture)
    , m_strSkyBoxModelName{rhs.m_strSkyBoxModelName }
    , m_vMapMinMaxBox_Center{ rhs.m_vMapMinMaxBox_Center }
    , m_vMapMinMaxBox_Extents{ rhs.m_vMapMinMaxBox_Extents }
{
    Safe_AddRef(m_pSkyBoxTexture);
}

HRESULT CLevelData::Initialize_Prototype()
{
    if (FAILED(Super::Initialize_Prototype()))
        return E_FAIL;

    if (FAILED(Ready_Component()))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevelData::Initialize(void* pArg)
{
    if (FAILED(Super::Initialize(pArg)))
        return E_FAIL;

    m_pGameInstance->Set_MapMinMaxBox(m_vMapMinMaxBox_Center,m_vMapMinMaxBox_Extents);

    return S_OK;
}

HRESULT CLevelData::Ready_Component()
{
    /* Shader 생성 */
    m_pMeshShader =
        static_cast<CShader*>(m_pGameInstance->Clone_Prototype(EPrototypeType::COMPONENT, ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Shader_VtxMesh_Tool"));
    if (m_pMeshShader == nullptr) return E_FAIL;

    m_pInstMeshSahder =
        static_cast<CShader*>(m_pGameInstance->Clone_Prototype(EPrototypeType::COMPONENT, ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Shader_VtxInstanceMesh_Tool"));
    if (m_pInstMeshSahder == nullptr) return E_FAIL;


    CTransform* pTs  = CTransform::Create();
    Add_Component<CTransform>(pTs);

    return S_OK;
}

void CLevelData::Change_SkyBoxModel(const wstring& wstrModelName)
{
    if (wstrModelName.empty())
    {
        this->Remove_Component<CModel>();
        this->m_strSkyBoxModelName = "None";
        return;
    }

    CModel::MODEL_COPY_DESC tDesc{};
    CModel* pModel =
        static_cast<CModel*>(
            m_pGameInstance->Clone_Prototype(EPrototypeType::COMPONENT, ENUM_TO_UINT(ELevelType::MAP), L"Prototype_Component_Model_" + wstrModelName, &tDesc));
    if (pModel == nullptr) return;

    m_strSkyBoxModelName = Engine_Utils::ToString(wstrModelName);
    this->Change_Component<CModel>(pModel);
    return;
}

void CLevelData::Change_SkyBoxTexture(const wstring& wstrTextureName)
{
    wstring wstrFullTextureName = L"Texture_" + wstrTextureName;
    CTextureBase* pTexBase = m_pGameInstance->GetOrAddTexture(wstrFullTextureName, nullptr);
    if (pTexBase == nullptr)
    {
        MSG_BOX(" Change SkyBox Texture is Failed To Find");
        return;
    }

    Safe_Release(m_pSkyBoxTexture);

    /* Get으로 가져올떄 이미 Add해준다 */
    m_pSkyBoxTexture = pTexBase;

    return;
}

string CLevelData::Get_SkyBoxModelName()
{
    return m_strSkyBoxModelName;
}

HRESULT CLevelData::Awake(const _uint iCurrentLevelID)
{
    if (FAILED(Super::Awake(iCurrentLevelID)))
        return E_FAIL;

    return S_OK;
}

void CLevelData::Update_Priority(const _float fTimeDelta)
{
    //Super::Update_Priority(fTimeDelta);
}

void CLevelData::Update(const _float fTimeDelta)
{
    Super::Update(fTimeDelta);

    m_pGameInstance->Set_MapMinMaxBox(m_vMapMinMaxBox_Center,m_vMapMinMaxBox_Extents);


    m_tCB_EnvData.fEnvAccDT += fTimeDelta;
    if (m_tCB_EnvData.fEnvAccDT >= 10000.f)
        m_tCB_EnvData.fEnvAccDT = 0.f;
}

void CLevelData::Update_Late(const _float fTimeDelta)
{
    Super::Update_Late(fTimeDelta);

    Set_GPU_EnvData();
}

void CLevelData::Ready_Before_Render(const _float fTimeDelta)
{
    Super::Ready_Before_Render(fTimeDelta);

    m_pGameInstance->Push_RenderObject(RENDER_CATEGORY::ENVIRONMENT,this);
}

HRESULT CLevelData::Render()
{
    if (FAILED(Super::Render()))
        return E_FAIL;

    HRESULT hr{ S_OK };

    CModel* pModel = Get_Component<CModel>();

    if (pModel && m_pMeshShader && m_pSkyBoxTexture)
    {
        CTransform* pTransform = Get_Component<CTransform>();  if (pTransform == nullptr)      return E_FAIL;
        
        CCameraMan* pCam = m_pGameInstance->Get_MainCamera();
        if (pCam == nullptr) return E_FAIL;
        Vec3 vCamPos = pCam->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::POS);
        Matrix matWorld = pTransform->Get_WorldMatrix();
        matWorld._41 += vCamPos.x;
        matWorld._42 += vCamPos.y;
        matWorld._43 += vCamPos.z;

        /* 현재 내 Transform 에다가 카메라 Trasnform Matrix만 곱해줘야겠다 */

        m_pMeshShader->Bind_TransformData(matWorld);
        /* 내 현재 Transform 에 카메라의 위치값만 뽑아와서 던져준다 */

        _uint iMeshCount = pModel->Get_MeshCount();


        hr= m_pMeshShader->Bind_SRV(EFXSRV::Textures , m_pSkyBoxTexture->Get_SRV());


        /* Client Make Path를 이용한다 */
        m_pMeshShader->Set_Pass(ENUM_TO_UINT(EMapObjectShaderPass::SkyBox));

        for (_uint i = 0; i < iMeshCount; ++i)
        {
            hr = pModel->Bind_Material(m_pMeshShader, i);
            hr= pModel->Bind_MaterialInstance(m_pMeshShader, i);
            m_pMeshShader->Apply();
            hr= pModel->Render(i);
        }
    }

#ifdef _DEBUG
    hr = m_pGameInstance->DebugRender_MapMinMaxBox();
#endif // _DEBUG



    return hr;
}

_bool CLevelData::Picking(OUT Vec3& vOut)
{
    return false;
}

HRESULT CLevelData::Set_GPU_EnvData()
{
    if (m_pMeshShader== nullptr) return E_FAIL;
    if (m_pInstMeshSahder == nullptr) return E_FAIL;

    HRESULT hr{ E_FAIL };


    ID3DX11EffectConstantBuffer* pCB = m_pMeshShader->Get_ConstantBuffer("CB_EnvData");
    pCB = m_pMeshShader->Get_ConstantBuffer("CB_EnvData");
    if (pCB->IsValid() == false) return E_FAIL;
    hr = pCB->SetRawValue(&m_tCB_EnvData, 0, sizeof(CB_EnvData));

    pCB = m_pInstMeshSahder->Get_ConstantBuffer("CB_EnvData");
    if (pCB->IsValid() == false) return E_FAIL;
    hr = pCB->SetRawValue(&m_tCB_EnvData, 0, sizeof(CB_EnvData));


    return hr;
}



CLevelData* CLevelData::Create(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLevelData* pLevelData = new CLevelData(eType, pDevice , pContext);

    if(FAILED(pLevelData->Initialize_Prototype()))
    {
        Safe_Release(pLevelData);
        MSG_BOX(" Scene Data is Failed to Create ");
        return nullptr;
    }

    return pLevelData;
}


CGameObject* CLevelData::Clone(void* pArg)
{
    CLevelData* pLevelData = new CLevelData(*this);

    if (FAILED(pLevelData->Initialize(pArg)))
    {
        Safe_Release(pLevelData);
        MSG_BOX(" LevelData is failed to Clone");
        return nullptr;
    }

    return pLevelData;
}

void CLevelData::Free()
{
    Super::Free();
    Safe_Release(m_pMeshShader);
    Safe_Release(m_pInstMeshSahder);
    Safe_Release(m_pSkyBoxTexture);
}


void CLevelData::Draw_ImGui()
{

    return;
}

HRESULT CLevelData::Apply_Data(const struct DTO::TLevelData* pData)
{
    if (pData == nullptr) return E_FAIL;
    if (m_pMeshShader == nullptr)       return E_FAIL;
    if (m_pInstMeshSahder == nullptr)   return E_FAIL;

    /* None이 아니라면 반환한다 */
    if (pData->strTextureSplatingInfoName != "None")
        if (FAILED(CMapToolManager::GetInstance()->Load_TextureSplatingInfoData(Engine_Utils::ToWString(pData->strTextureSplatingInfoName))))
            return E_FAIL;

    /* None => [Don't Use Texture Splating Info] */
    m_strTextureSplatingInfoName = pData->strTextureSplatingInfoName;

    /* Level Type */
    m_eClientLevelType = StringToClientleveltype(pData->strLevelTypeName);



    /* 환경 데이터 */
    m_tCB_EnvData.fWindPower = pData->fWindPower;
    m_tCB_EnvData.vWindDirection = pData->vWindDirection;

    m_tCB_EnvData.vEnvColor = pData->vEnvColor;

    CTransform* pTs = Get_Component<CTransform>();

    Vec3 vPitchYawRoll_Radain = pData->vSkyBoxPitchYawRoll * TO_RAD;

    pTs->Set_WorldMatrix( Matrix::Identity * Matrix::CreateFromYawPitchRoll(vPitchYawRoll_Radain.y , vPitchYawRoll_Radain.x , vPitchYawRoll_Radain.z) * 
        Matrix::CreateTranslation(pData->vSkyBoxPositionOffset.x, pData->vSkyBoxPositionOffset.y, pData->vSkyBoxPositionOffset.z));

    Change_SkyBoxModel(Engine_Utils::ToWString(pData->strSkyBoxModelName));
    Change_SkyBoxTexture(Engine_Utils::ToWString(pData->strSKyBoxTextureName));

    m_tCB_EnvData.vSkyColor = pData->vSkyColor;

    m_tCB_EnvData.vCloudBaseColor = pData->vCloudBaseColor;

    m_tCB_EnvData.vCloudHighlight = pData->vCloudHighlight;
    m_tCB_EnvData.fCloudHighlightPower = pData->fCloudHighlightPower;

    m_tCB_EnvData.fCloudShadowPower = pData->fCloudShadowPower;
    m_tCB_EnvData.vCloudShadowColor = pData->vCloudShadowColor;

    m_tCB_EnvData.isChannelPacking          = pData->isSkyBoxChannelPacking;
    m_tCB_EnvData.fPolarRadiusScale         = pData->fPolarRadiusScale;
    m_tCB_EnvData.iSkyBoxTextureType        = pData->iSkyBoxTextureType;
    m_tCB_EnvData.vSkyBoxTextureUVSpeed     = pData->vSkyBoxTextureUVSpeed;
        

    m_vMapMinMaxBox_Center = pData->vMapMinMaxBox_Center;
    m_vMapMinMaxBox_Extents = pData->vMapMinMaxBox_extents;


    this->Set_GPU_EnvData();

    return S_OK;
}

_bool CLevelData::Export_Data(DTO::ECategory eCategory, CDataDocumentBase* pDocument)
{
    if (pDocument == nullptr) return false;

    CDataDocument_Map* pMapDoc = static_cast<CDataDocument_Map*>(pDocument);

    DTO::TLevelData tData{};

    /* Scenc Data 구조체 저장 */
    tData.strTag = m_strName + std::to_string(m_iObjectID);
    tData.strTextureSplatingInfoName = m_strTextureSplatingInfoName;
    tData.strLevelTypeName = ClientleveltypeToString(m_eClientLevelType);

    CModel* pModel = Get_Component<CModel>();
    tData.strSkyBoxModelName = pModel != nullptr ? m_strSkyBoxModelName : "None";
    tData.strSKyBoxTextureName = m_pSkyBoxTexture != nullptr ? Engine::Engine_Utils::ToString(m_pSkyBoxTexture->Get_Name()) : "None";
    tData.isSkyBoxChannelPacking = m_tCB_EnvData.isChannelPacking;          /* Channel Pakkin Use */
    tData.fPolarRadiusScale = m_tCB_EnvData.fPolarRadiusScale;              /* SKyBox Radius Scale  */
    tData.vSkyBoxTextureUVSpeed = m_tCB_EnvData.vSkyBoxTextureUVSpeed;      /* Sky Box Texture UV Speed */
    tData.iSkyBoxTextureType = m_tCB_EnvData.iSkyBoxTextureType;            /* Sky Box Texture Setting */

    /* 색깔 설정 */
    tData.vEnvColor = m_tCB_EnvData.vEnvColor;
    tData.vSkyColor = m_tCB_EnvData.vSkyColor;

    tData.vCloudBaseColor = m_tCB_EnvData.vCloudBaseColor;
    
    tData.vCloudHighlight = m_tCB_EnvData.vCloudHighlight;
    tData.fCloudHighlightPower = m_tCB_EnvData.fCloudHighlightPower;

    tData.vCloudShadowColor = m_tCB_EnvData.vCloudShadowColor;
    tData.fCloudShadowPower = m_tCB_EnvData.fCloudShadowPower;


    CTransform* pTs = Get_Component<CTransform>();
    Matrix matWorld = pTs->Get_WorldMatrix();
    Vec3 vScale{};
    Quat vQuat{};
    Vec3 vPos{};
    matWorld.Decompose(vScale , vQuat , vPos);
    tData.vSkyBoxPositionOffset = { vPos.x , vPos.y , vPos.z };
    tData.vSkyBoxScale = vScale;
    tData.vSkyBoxPitchYawRoll = vQuat.ToEuler() * TO_RAD;


    /* Env Data */
    tData.fWindPower = m_tCB_EnvData.fWindPower;
    tData.vWindDirection = m_tCB_EnvData.vWindDirection;

    tData.vMapMinMaxBox_Center  = this->m_vMapMinMaxBox_Center;
    tData.vMapMinMaxBox_extents = this->m_vMapMinMaxBox_Extents;




    if (FAILED(pMapDoc->Try_Add(tData)))
        return false;

    return true;
}
