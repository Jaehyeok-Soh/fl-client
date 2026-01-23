#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CFont_Manager final : public CBase
{
	using Super = CBase;
private:
	CFont_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CFont_Manager() = default;

public:
	HRESULT Initialize();
	HRESULT Add_Font(const _wstring& strFontTag, const _tchar* pFontFilePath);
	HRESULT Draw_Text(const _wstring& strFontTag, const _tchar* pText, const Vec2& vPosition, const Vec4 &vColor);


private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pDeviceContext = { nullptr };
	SpriteBatch* m_pBatch = { nullptr };
	map<const _wstring, class CFont*>		m_Fonts;

private:
	class CFont* Find_Font(const _wstring& strFontTag);

public:
	static CFont_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

NS_END