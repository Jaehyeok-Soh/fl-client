#include "CEffectObject.h"
#include "GameInstance.h"

CEffectObject::CEffectObject(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    :CToolObject(eType, pDevice, pDeviceContext)
{
}

CEffectObject::CEffectObject(const CToolObject& rhs)
    :CToolObject(rhs)
{
}

HRESULT CEffectObject::Initialize_Prototype()
{
    __super::Initialize_Prototype();
    return S_OK;
}

HRESULT CEffectObject::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
    {
        MSG_BOX("Initialize Failed : CEffectObject -__super::Initialize(pArg)");
        return E_FAIL;
    }

    if (FAILED(EffectDesc_Initialize(pArg)))
    {
        MSG_BOX("Initialize Failed : CEffectObject -__super::EffectDesc_Initialize(pArg)");
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CEffectObject::EffectDesc_Initialize(void* pArg)
{
    Effect_Desc* pEffectDesc = static_cast<Effect_Desc*>(pArg);
    
    if (pEffectDesc == nullptr)
        return E_FAIL;

    m_tEffectDesc = *pEffectDesc;
    return S_OK;
}

HRESULT CEffectObject::Awake(const _uint iCurrentLevelID)
{


    return S_OK;
}

void CEffectObject::Update_Priority(const _float fDT)
{
}

void CEffectObject::Update(const _float fTimeDelta)
{
}

void CEffectObject::Update_Late(const _float fTimeDelta)
{
}

HRESULT CEffectObject::Render()
{
    return E_NOTIMPL;
}

_bool CEffectObject::Picking(OUT _float4& vOut)
{
    return _bool();
}

HRESULT CEffectObject::Export_Data(OUT MAPOBJECT_SAVEDATA& data)
{
    return E_NOTIMPL;
}

void CEffectObject::Draw_ImGui()
{
}

void CEffectObject::Set_Dead(const wstring& wstrLayerTag)
{
}

CEffectObject* CEffectObject::Create(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
    return nullptr;
}

void CEffectObject::Free()
{
}
