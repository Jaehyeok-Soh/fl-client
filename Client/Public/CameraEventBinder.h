#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CGameObject;
class CModel;
class CDataDocumentBase;
class CCameraEvent_Handler;
NS_END

NS_BEGIN(Client)

class CCameraEventBinder final : public CBase
{
    using Super = CBase;

private:
    CCameraEventBinder();
    virtual ~CCameraEventBinder() = default;

public:
    HRESULT Initialize(_uint iLevelID, CGameObject* pOwner, CModel* pModel, const wstring& wstrJsonPath);

private:
    HRESULT Ready_Handler(_uint iLevelID, CGameObject* pOwner, CModel* pModel, const wstring& wstrJsonPath);

public:
    static CCameraEventBinder* Create(_uint iLevelID, CGameObject* pOwner, CModel* pModel, const wstring& wstrJsonPath);
    virtual void Free() override;
};

NS_END