#include "pch.h"
#include "SoundEventBinder.h"
#include "GameObject.h"
#include "Model.h"
#include "GameInstance.h"

CSoundEventBinder::CSoundEventBinder()
    : m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CSoundEventBinder::Initialize(_uint iLevelID, CGameObject* pOwner, CModel* pModel, const std::filesystem::path& path)
{
    m_path = path;
    if (FAILED(Load_Document(iLevelID, path)))
        return E_FAIL;

    if (FAILED(Attach_HandlerToOwner(pOwner, pModel)))
        return E_FAIL;

    return S_OK;
}

HRESULT CSoundEventBinder::Load_Document(_uint iLevelID, const std::filesystem::path& path)
{
    m_iLoadedLevelID = iLevelID;

    if (FAILED(m_pGameInstance->Regist_Document<CDataDocument_SoundEvent>(iLevelID, DTO::ECategory::SOUNDEVENT)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Load_File_Json(iLevelID, DTO::ECategory::SOUNDEVENT, path)))
        return E_FAIL;

    CDataDocumentBase* pBase =
        m_pGameInstance->Ensure_Document(iLevelID, DTO::ECategory::SOUNDEVENT, path);

    m_pSoundDoc = static_cast<CDataDocument_SoundEvent*>(pBase);
    if (m_pSoundDoc == nullptr)
        return E_FAIL;

    return S_OK;
}

HRESULT CSoundEventBinder::Build_HandlerDesc(OUT CSound_Handler::SOUND_HANDLER_DESC& outDesc)
{
    if (m_pSoundDoc == nullptr)
        return E_FAIL;

    const DTO::SOUND_EVENT_INFO_DESC* pData = m_pSoundDoc->Find_Data(m_path.stem().string());
    if (pData == nullptr)
        return E_FAIL;

    outDesc.vecSoundEvents = pData->vecSoundEvents;
    return S_OK;
}

CSound_Handler* CSoundEventBinder::Create_HandlerForOwner()
{
    CSound_Handler::SOUND_HANDLER_DESC desc{};
    if (FAILED(Build_HandlerDesc(desc)))
        return nullptr;

    CBase* pComp =
        m_pGameInstance->Clone_Prototype(
            EPrototypeType::COMPONENT,
            ENUM_TO_UINT(ELevelType::STATIC),
            L"Prototype_Component_SoundHandler",
            &desc);

    if (pComp == nullptr)
        return nullptr;

    return static_cast<CSound_Handler*>(pComp);
}

HRESULT CSoundEventBinder::Attach_HandlerToOwner(CGameObject* pOwner, CModel* pModel)
{
    if (pOwner == nullptr || pModel == nullptr)
        return E_FAIL;

    CSound_Handler* pHandler = Create_HandlerForOwner();
    if (pHandler == nullptr)
        return E_FAIL;

    if (FAILED(pOwner->Add_Component<CSound_Handler>(pHandler)))
    {
        Safe_Release(pHandler);
        return E_FAIL;
    }

    pHandler->Setup_ForOwner(pModel);
    return S_OK;
}

CSoundEventBinder* CSoundEventBinder::Create(_uint iLevelID, CGameObject* pOwner, CModel* pModel, const std::filesystem::path& path)
{
    CSoundEventBinder* pInstance = new CSoundEventBinder();
    if (FAILED(pInstance->Initialize(iLevelID, pOwner, pModel, path)))
    {
        Safe_Release(pInstance);
        return nullptr;
    }
    return pInstance;
}

void CSoundEventBinder::Free()
{
    m_pSoundDoc = nullptr;
    Safe_Release(m_pGameInstance);
    Super::Free();
}