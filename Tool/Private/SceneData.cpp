#include "pch.h"
#include "SceneData.h"
#include "DataStruct_Map.h"
#include "DataDocument_Map.h"
#include "GameInstance.h"

CSceneData::CSceneData(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : CToolObject(eType, pDevice, pDeviceContext), m_strTextureSplatingInfoName{"None"}
{
}

CSceneData::CSceneData(const CSceneData& rhs)
    : CToolObject(rhs) , m_strTextureSplatingInfoName{rhs.m_strTextureSplatingInfoName }
{
}

HRESULT CSceneData::Initialize_Prototype()
{
    if (FAILED(Super::Initialize_Prototype()))
        return E_FAIL;

    m_strName = "SceneData";

    return S_OK;
}

HRESULT CSceneData::Initialize(void* pArg)
{
    if (FAILED(Super::Initialize(pArg)))
        return E_FAIL;


    return S_OK;
}

HRESULT CSceneData::Awake(const _uint iCurrentLevelID)
{
    if (FAILED(Super::Awake(iCurrentLevelID)))
        return E_FAIL;

    return S_OK;
}

void CSceneData::Update_Priority(const _float fTimeDelta)
{
    Super::Update_Priority(fTimeDelta);
}

void CSceneData::Update(const _float fTimeDelta)
{
    Super::Update(fTimeDelta);

}

void CSceneData::Update_Late(const _float fTimeDelta)
{
    Super::Update_Late(fTimeDelta);

}

void CSceneData::Ready_Before_Render(const _float fTimeDelta)
{
    Super::Ready_Before_Render(fTimeDelta);

}

HRESULT CSceneData::Render()
{
    if (FAILED(Super::Render()))
        return E_FAIL;

    return S_OK;
}

_bool CSceneData::Picking(OUT Vec3& vOut)
{
    return false;
}




CSceneData* CSceneData::Create(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSceneData* pSceneData = new CSceneData(eType, pDevice , pContext);

    if(FAILED(pSceneData->Initialize_Prototype()))
    {
        Safe_Release(pSceneData);
        MSG_BOX(" Scene Data is Failed to Create ");
        return nullptr;
    }

    return pSceneData;
}


CGameObject* CSceneData::Clone(void* pArg)
{
    CSceneData* pSceneData = new CSceneData(*this);

    if (FAILED(pSceneData->Initialize(pArg)))
    {
        Safe_Release(pSceneData);
        MSG_BOX(" SceneData is failed to Clone");
        return nullptr;
    }

    return pSceneData;
}

void CSceneData::Free()
{
    Super::Free();
}


void CSceneData::Draw_ImGui()
{

    return;
}

_bool CSceneData::Export_Data(DTO::ECategory eCategory, CDataDocumentBase* pDocument)
{
    if (pDocument == nullptr) return false;

    CDataDocument_Map* pMapDoc = static_cast<CDataDocument_Map*>(pDocument);

    DTO::TSceneData tData{};

    /* Scenc Data 구조체 저장 */
    tData.strTag = m_strName + std::to_string(m_iObjectID);
    tData.strTextureSplatingInfoName = m_strTextureSplatingInfoName;


    if (FAILED(pMapDoc->Try_Add(tData)))
        return false;

    return true;
}
