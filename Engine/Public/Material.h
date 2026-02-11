#pragma once
#include "ResourceBase.h"

NS_BEGIN(Engine)

class ENGINE_DLL CMaterial final : public CResourceBase
{
	using Super = CResourceBase;
public:
	typedef struct tagMaterialDesc : public RESOURCE_BASE_DESC
	{
		std::span<string> spanTags;
		SHADER_MATERIALDESC materialInstance = {};
	}MATERIAL_DESC;
	typedef struct tagMaterialCopyDesc
	{
		SHADER_MATERIALDESC materialInstance = {};
	}MATERIAL_COPY_DESC;
private:
	CMaterial(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CMaterial(const CMaterial& rhs);
	virtual ~CMaterial() = default;

	HRESULT Initialize(void *pArg);
public:
	HRESULT Bind_ShaderResource(class CShader* pShader);

	const std::array<ID3D11ShaderResourceView*, ENUM_TO_SZET(EMaterialTextureType::MAX_COUNT)>& Get_ArraySRV() const { return m_arrSRVs; }
private:
	HRESULT Caching_Resource(const wstring &pTag, EMaterialTextureType eType);
	ID3D11ShaderResourceView* Get_ShaderResourceView(const _tchar* pTag);
private:
	_uint m_iTextureMask = { 0 };
	std::array<ID3D11ShaderResourceView*, ENUM_TO_SZET(EMaterialTextureType::MAX_COUNT)> m_arrSRVs;
	//vector<ID3D11ShaderResourceView*>			m_SRVs[AI_TEXTURE_TYPE_MAX];
public:
	static CMaterial* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual void Free() override;
};

NS_END