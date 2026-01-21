#pragma once
#include "ResourceBase.h"

NS_BEGIN(Engine)

class ENGINE_DLL CMaterial final : public CResourceBase
{
	using Super = CResourceBase;
private:
	enum : _short
	{
		FLAG_NONE = 0,
		FLAG_DIFFUSE = 1 << ENUM_TO_UINT(MATERIALSLOT::DIFFUSE),
		FLAG_NORMAL = 1 << ENUM_TO_UINT(MATERIALSLOT::NORMAL),
		FLAG_SPECULAR = 1 << ENUM_TO_UINT(MATERIALSLOT::SPECULAR),
		FLAG_EMISSIVE = 1 << ENUM_TO_UINT(MATERIALSLOT::EMISSIVE)
	};
public:
	typedef struct tagMaterialDesc : public RESOURCE_BASE_DESC
	{
		wstring wstrDiffuseTag = { L"" };
		wstring wstrNormalTag = { L"" };
		wstring wstrSpecularTag = { L"" };
		wstring wstrEmissiveTag = { L"" };
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
private:
	HRESULT Caching_Resource(const _tchar* pTag, MATERIALSLOT eSlot);
	ID3D11ShaderResourceView* Get_ShaderResourceView(const _tchar* pTag);
private:
	_short m_iTextureMask = { 0b0000'0000'0000'0000 };
	std::array<ID3D11ShaderResourceView*, ENUM_TO_SZET(MATERIALSLOT::END)> m_arrSRVs;
	//vector<ID3D11ShaderResourceView*>			m_SRVs[AI_TEXTURE_TYPE_MAX];
public:
	static CMaterial* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual void Free() override;
};

NS_END