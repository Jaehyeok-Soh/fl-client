#pragma once
#include "Component.h"
#include "TextureBase.h"


NS_BEGIN(Engine)

#define TEXTURE_MAGICNUMBER 8

class ENGINE_DLL CTexture final : public CComponent
{
	using Super = CComponent;
public:
	constexpr static EComponentType _ID = EComponentType::TEXTURE;
	typedef struct tagTextureOriginDesc
	{
		std::wstring wstrTexturePath;
		_uint iTextureCount = { 0 };
	}TEXTURE_COMPONENT_ORIGIN_DESC;
private:
	CTexture();
	CTexture(const CTexture& rhs);
	virtual ~CTexture() = default;

public:
	virtual HRESULT Initialize_Prototype(void* pArg);
	virtual HRESULT Initialize(void* pArg) override;
public:
	HRESULT Add_DefaultTexture(const wstring& wstrTextureTag, _uint iTextureIndex);
	HRESULT Release_DefaultTexture(_uint iTextureIndex);
	_bool Is_Valid(_uint iTextureIndex);
	HRESULT Bind_ShaderResource(class CShader* pShader, _uint iIndex = 0);
	HRESULT Bind_ShaderResourceBuffer(class CShader* pShader);
	HRESULT Bind_ShaderResource_Cube(class CShader* pShader);
	_uint Get_TextureCount() const { return m_iTextureCount; }
	const Vec2& Get_TextureSize(_uint iIndex = 0) const { return m_vecTextures[iIndex]->Get_Size(); }
private:
	_uint m_iTextureCount = { 0 };
	vector<CTextureBase*>						m_vecTextures;
	ID3D11ShaderResourceView* m_arrSRV[TEXTURE_MAGICNUMBER]{nullptr};
public:
	static CTexture* Create(void* pArg);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END