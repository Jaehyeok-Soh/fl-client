#include "Font.h"
#include "Font_Manager.h"

CFont_Manager::CFont_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : m_pDevice{ pDevice }
    , m_pDeviceContext{ pDeviceContext }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pDeviceContext);
}

HRESULT CFont_Manager::Initialize()
{
    m_pBatch = new SpriteBatch(m_pDeviceContext);

    return S_OK;
}

HRESULT CFont_Manager::Add_Font(const _wstring& strFontTag, const _tchar* pFontFilePath)
{
    if (nullptr != Find_Font(strFontTag))
        return E_FAIL;

    CFont* pFont = CFont::Create(m_pDevice, m_pDeviceContext, pFontFilePath);
    if (nullptr == pFont)
        return E_FAIL;

    m_Fonts.emplace(strFontTag, pFont);

    return S_OK;
}

HRESULT CFont_Manager::Draw_Text(const _wstring& strFontTag, const _tchar* pText, const _float2& vPosition, _fvector vColor)
{
    CFont* pFont = Find_Font(strFontTag);
    if (nullptr == pFont)
        return E_FAIL;

    m_pDeviceContext->GSSetShader(nullptr, nullptr, 0);

    m_pBatch->Begin();

    pFont->Draw_Text(m_pBatch, pText, vPosition, vColor);

    m_pBatch->End();

    return S_OK;
}

CFont* CFont_Manager::Find_Font(const _wstring& strFontTag)
{
    auto    iter = m_Fonts.find(strFontTag);

    if (iter == m_Fonts.end())
        return nullptr;

    return iter->second;
}

CFont_Manager* CFont_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
    CFont_Manager* pInstance = new CFont_Manager(pDevice, pDeviceContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CFont_Manager");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CFont_Manager::Free()
{
    for (auto& Pair : m_Fonts)
        Safe_Release(Pair.second);
    m_Fonts.clear();

    Safe_Delete(m_pBatch);

    Safe_Release(m_pDevice);
    Safe_Release(m_pDeviceContext);
    Super::Free();
}
