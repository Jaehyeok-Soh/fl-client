#include "pch.h"
#include "LevelData.h"
#include "DataStruct_Map.h"
#include "DataDocument_Map.h"
#include "Shader.h"
#include "MapToolManager.h"

#include "GameInstance.h"

CLevelData::CLevelData(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : CToolObject(eType, pDevice, pDeviceContext)
    , m_strTextureSplatingInfoName{ "None" }
    , m_tCB_EnvData{}
    , m_pMeshShader{nullptr}
    , m_pInstMeshSahder{nullptr}
{
}

CLevelData::CLevelData(const CLevelData& rhs)
    : CToolObject(rhs) 
    , m_strTextureSplatingInfoName{rhs.m_strTextureSplatingInfoName }
    , m_tCB_EnvData{rhs.m_tCB_EnvData}
{
}

HRESULT CLevelData::Initialize_Prototype()
{
    if (FAILED(Super::Initialize_Prototype()))
        return E_FAIL;

    m_strName = "LevelData";


    m_pMeshShader =
        static_cast<CShader*>(m_pGameInstance->Clone_Prototype(EPrototypeType::COMPONENT, ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Shader_VtxMesh_Tool"));
    if (m_pMeshShader == nullptr) return E_FAIL;

    m_pInstMeshSahder =
        static_cast<CShader*>(m_pGameInstance->Clone_Prototype(EPrototypeType::COMPONENT, ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Shader_VtxInstanceMesh_Tool"));
    if (m_pInstMeshSahder == nullptr) return E_FAIL;


    return S_OK;
}

HRESULT CLevelData::Initialize(void* pArg)
{
    if (FAILED(Super::Initialize(pArg)))
        return E_FAIL;




    return S_OK;
}

HRESULT CLevelData::Awake(const _uint iCurrentLevelID)
{
    if (FAILED(Super::Awake(iCurrentLevelID)))
        return E_FAIL;

    return S_OK;
}

void CLevelData::Update_Priority(const _float fTimeDelta)
{
    Super::Update_Priority(fTimeDelta);
}

void CLevelData::Update(const _float fTimeDelta)
{
    Super::Update(fTimeDelta);

}

void CLevelData::Update_Late(const _float fTimeDelta)
{
    Super::Update_Late(fTimeDelta);

}

void CLevelData::Ready_Before_Render(const _float fTimeDelta)
{
    Super::Ready_Before_Render(fTimeDelta);

}

HRESULT CLevelData::Render()
{
    if (FAILED(Super::Render()))
        return E_FAIL;

    return S_OK;
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
}


void CLevelData::Draw_ImGui()
{

    return;
}

HRESULT CLevelData::Apply_Data(const struct DTO::TLevelData* pData)
{
    if (pData == nullptr) return  E_FAIL;

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

    /* Env Data */
    tData.fWindPower = m_tCB_EnvData.fWindPower;
    tData.vWindDirection = m_tCB_EnvData.vWindDirection;

    if (FAILED(pMapDoc->Try_Add(tData)))
        return false;

    return true;
}
