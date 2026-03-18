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
	m_pFont->SetDefaultCharacter(L'?');
	return S_OK;
}

HRESULT CFont::Draw_Text(SpriteBatch* pBatch, const _tchar* pText, const Vec2& vPosition, const Vec4& vColor, EFontPivotType ePivot, const _float fRotate, const _float fScale)
{
	Vec2 vSize = m_pFont->MeasureString(pText);
	Vec2 vPivot = {};

	switch (ePivot)
	{
	case Engine::EFontPivotType::CENTER:	vPivot = { vSize * 0.5f};				break;
	case Engine::EFontPivotType::LEFT:		vPivot = { 0.f,	vSize.y * 0.5f };		break;
	case Engine::EFontPivotType::RIGHT:		vPivot = { vSize.x, vSize.y * 0.5f };	break;
	case Engine::EFontPivotType::UP:		vPivot = { vSize.x * 0.5f, 0.f };		break;
	case Engine::EFontPivotType::DOWN:		vPivot = { vSize.x * 0.5f, vSize.y };	break;

	case Engine::EFontPivotType::LT:		vPivot = { 0.f, 0.f };					break;
	case Engine::EFontPivotType::RT:		vPivot = { vSize.x, 0.f };				break;
	case Engine::EFontPivotType::LC:		vPivot = { 0.f, vSize.y * 0.5f };		break;
	case Engine::EFontPivotType::RC:		vPivot = { vSize.x, vSize.y * 0.5f };	break;
	case Engine::EFontPivotType::LD:		vPivot = { 0.f, vSize.y };				break;
	case Engine::EFontPivotType::RD:		vPivot = { vSize.x, vSize.y };			break;

	case Engine::EFontPivotType::END:
	default:
		return E_FAIL;
	}

	m_pFont->DrawString(pBatch, pText, vPosition, vColor, fRotate, vPivot, fScale);
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
