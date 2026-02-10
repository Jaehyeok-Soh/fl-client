#include "Engine_pch.h"
#include "Font.h"

CFont::CFont(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: m_pDevice{ pDevice }
	, m_pDeviceContext{ pDeviceContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

HRESULT CFont::Initialize(const _tchar* pFontFilePath)
{
	m_pFont = new SpriteFont(m_pDevice, pFontFilePath);

	return S_OK;
}

HRESULT CFont::Draw_Text(SpriteBatch* pBatch, const _tchar* pText, const Vec2& vPosition, const Vec4 &vColor)
{
	Vec2 vSize = m_pFont->MeasureString(pText);
	Vec2 vFinalPos = vPosition;
	vFinalPos.x -= vSize.x * 0.5f;
	vFinalPos.y -= vSize.y * 0.5f;
	m_pFont->DrawString(pBatch, pText, vFinalPos, vColor, 0.f, Vec2{0.f, 0.f}, 1.f);
	return S_OK;
}

CFont* CFont::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pFontFilePath)
{
	CFont* pInstance = new CFont(pDevice, pDeviceContext);

	/* 메인앱을 완벽히 사용하기 위한 기타 초기화작업을 수행한다. */
	if (FAILED(pInstance->Initialize(pFontFilePath)))
	{
		MSG_BOX("Failed to Created : CFont");
		Safe_Release(pInstance);
	}
	return pInstance;
}


void CFont::Free()
{
	Safe_Delete(m_pFont);
	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);
	Super::Free();
}
