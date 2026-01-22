#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CFont final : public CBase
{
	using Super = CBase;
private:
	CFont(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CFont() = default;

public:
	HRESULT Initialize(const _tchar* pFontFilePath);
	HRESULT Draw_Text(SpriteBatch* pBatch, const _tchar* pText, const _float2& vPosition, _fvector vColor);

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pDeviceContext = { nullptr };
	SpriteFont* m_pFont = { nullptr };

public:
	static CFont* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pFontFilePath);
	virtual void Free() override;
};

NS_END