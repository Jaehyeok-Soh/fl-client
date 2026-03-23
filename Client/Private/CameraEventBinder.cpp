#include "pch.h"
#include "CameraEventBinder.h"
#include "GameObject.h"
#include "Model.h"
#include "CameraEvent_Handler.h"
#include "DataDocument_CameraControlEvent.h"
#include "GameInstance.h"

CCameraEventBinder::CCameraEventBinder()
{
}

HRESULT CCameraEventBinder::Initialize(_uint iLevelID, CGameObject* pOwner, CModel* pModel, const wstring& wstrJsonPath)
{
    if (pOwner == nullptr || pModel == nullptr)
        return E_FAIL;

    if (FAILED(Ready_Handler(iLevelID, pOwner, pModel, wstrJsonPath)))
        return E_FAIL;

    return S_OK;
}

HRESULT CCameraEventBinder::Ready_Handler(_uint iLevelID, CGameObject* pOwner, CModel* pModel, const wstring& wstrJsonPath)
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    if (pGameInstance == nullptr)
        return E_FAIL;

    const DTO::ECategory eCategory = DTO::ECategory::CAMERACONTROLEVENT;
    const std::filesystem::path path = wstrJsonPath;

    if (FAILED(pGameInstance->Regist_Document<CDataDocument_CameraControlEvent>(iLevelID, eCategory)))
        return E_FAIL;

    if (FAILED(pGameInstance->Load_File_Json(iLevelID, eCategory, path)))
        return E_FAIL;

    CDataDocumentBase* pBase = pGameInstance->Ensure_Document(iLevelID, eCategory, path);
    CDataDocument_CameraControlEvent* pDoc = static_cast<CDataDocument_CameraControlEvent*>(pBase);
    if (pDoc == nullptr)
        return E_FAIL;

    const string ownerTag = path.stem().string();
    const DTO::CAMERACONTROL_EVENT_INFO_DESC* pData = pDoc->Find_Data(ownerTag);
    if (pData == nullptr)
        return E_FAIL;

    CCameraEvent_Handler::CAMERA_EVENT_HANDLER_DESC desc{};
    desc.vecCameraEvents = pData->vecCameraControlEvents;

    CCameraEvent_Handler* pHandler = pOwner->Get_Component<CCameraEvent_Handler>();
    if (pHandler == nullptr)
    {
        if (FAILED(pOwner->Add_Component<CCameraEvent_Handler>(0, L"Prototype_Component_CameraEvent_Handler", &desc)))
            return E_FAIL;

        pHandler = pOwner->Get_Component<CCameraEvent_Handler>();
        if (pHandler == nullptr)
            return E_FAIL;
    }
    else
    {
        pHandler->Set_Desc(desc);
    }

    pHandler->Setup_ForOwner(pModel);

    return S_OK;
}

CCameraEventBinder* CCameraEventBinder::Create(_uint iLevelID, CGameObject* pOwner, CModel* pModel, const wstring& wstrJsonPath)
{
    CCameraEventBinder* pInstance = new CCameraEventBinder();

    if (FAILED(pInstance->Initialize(iLevelID, pOwner, pModel, wstrJsonPath)))
    {
        MSG_BOX("Failed to Created : CCameraEventBinder");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCameraEventBinder::Free()
{
    Super::Free();
}