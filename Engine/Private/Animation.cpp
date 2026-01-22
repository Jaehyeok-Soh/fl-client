#include "Texture.h"
#include "GameInstance.h"
#include "Animation.h"

CAnimation::CAnimation(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : Super(RESOURCE_TYPE::ANIMATION, pDevice, pDeviceContext)
{
}

HRESULT CAnimation::Initialize(void* pArg)
{
    if (FAILED(Super::Initialize(pArg)))
        return E_FAIL;

    ANIM_DESC* pDesc = static_cast<ANIM_DESC*>(pArg);
    m_bLoop = pDesc->bLoop;
    m_vecKeyFrames = pDesc->vecKeyFrames;

    if (FAILED(Create_ShaderResourceView(pDesc->wstrTextureTag)))
        return E_FAIL;

    return S_OK;
}

inline _float2 CAnimation::Get_TextureSize()
{
    return m_pTexture->Get_Size();
}

HRESULT CAnimation::Create_ShaderResourceView(const wstring& wstrTag)
{
    if (!(m_pTexture = m_pGameInstance->Get_Resource<CTexture>(wstrTag)))
        return E_FAIL;

    return S_OK;
}

CAnimation* CAnimation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg)
{
    CAnimation* pInstance = new CAnimation(pDevice, pDeviceContext);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("CAnimation::Create, Failed");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CAnimation::Free()
{
    Safe_Release(m_pTexture);
    Super::Free();
}
