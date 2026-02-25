#include "pch.h"
#include "LevelData.h"
#include "DataStruct_Map.h"
#include "DataDocument_Map.h"
#include "GameInstance.h"

CLevelData::CLevelData(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : CToolObject(eType, pDevice, pDeviceContext), m_strTextureSplatingInfoName{"None"}
{
}

CLevelData::CLevelData(const CLevelData& rhs)
    : CToolObject(rhs) , m_strTextureSplatingInfoName{rhs.m_strTextureSplatingInfoName }
{
}

HRESULT CLevelData::Initialize_Prototype()
{
    if (FAILED(Super::Initialize_Prototype()))
        return E_FAIL;

    m_strName = "LevelData";

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
}


void CLevelData::Draw_ImGui()
{

    return;
}

_bool CLevelData::Export_Data(DTO::ECategory eCategory, CDataDocumentBase* pDocument)
{
    if (pDocument == nullptr) return false;

    CDataDocument_Map* pMapDoc = static_cast<CDataDocument_Map*>(pDocument);

    DTO::TLevelData tData{};

    /* Scenc Data 구조체 저장 */
    tData.strTag = m_strName + std::to_string(m_iObjectID);
    tData.strTextureSplatingInfoName = m_strTextureSplatingInfoName;


    if (FAILED(pMapDoc->Try_Add(tData)))
        return false;

    return true;
}
