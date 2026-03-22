#pragma once
#include "Base.h"
#include "DataDocument_SoundEvent.h"
#include "Sound_Handler.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)

class CSoundEventBinder final : public CBase
{
    using Super = CBase;

private:
    CSoundEventBinder();
    virtual ~CSoundEventBinder() = default;

public:
    HRESULT Initialize(_uint iLevelID, CGameObject* pOwner, CModel* pModel, const std::filesystem::path& path);

private:
    HRESULT Build_HandlerDesc(OUT CSound_Handler::SOUND_HANDLER_DESC& outDesc);
    HRESULT Load_Document(_uint iLevelID, const std::filesystem::path& path);
    CSound_Handler* Create_HandlerForOwner();
    HRESULT Attach_HandlerToOwner(CGameObject* pOwner, CModel* pModel);
private:
    std::filesystem::path m_path;
    CGameInstance* m_pGameInstance = nullptr;
    CDataDocument_SoundEvent* m_pSoundDoc = nullptr;
    _uint m_iLoadedLevelID = 0;

public:
    static CSoundEventBinder* Create(_uint iLevelID, CGameObject* pOwner, CModel* pModel, const std::filesystem::path& path);
    virtual void Free() override;
};

NS_END