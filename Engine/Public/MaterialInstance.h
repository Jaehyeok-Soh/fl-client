#pragma once
#include "ResourceBase.h"

NS_BEGIN(Engine)

class ENGINE_DLL CMaterialInstance final : public CResourceBase
{
	using Super = CResourceBase;
public:
	typedef struct tagMaterialInstanceOrignDesc : public Super::tagResourceBaseDesc
	{
		EMaterialInstanceType eType = { EMaterialInstanceType::END };
		wstring wstrMaterialTag = {L"Material_Default"};
		_float4 vTintColor = { 1.f, 1.f, 1.f, 1.f };
		_float fEmissivePower = { 1.f };
	}MI_ORIGIN_DESC;
private:
	CMaterialInstance(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CMaterialInstance() = default;

	virtual HRESULT Initialize(void* pArg) override;
public:
	HRESULT Bind_ShaderResource(class CShader* pShader);
	EMaterialInstanceType Get_MIType() const { return m_eMIType; }
	void Set_TintColor(const _float4& vColor) { m_shaderDesc.vTintColor = vColor; }
	void Set_EmissivePower(_float fEmissivePower) { m_shaderDesc.fEmissivePower = fEmissivePower; }
private:

private:
	EMaterialInstanceType m_eMIType = { EMaterialInstanceType::END };
	_bool m_bDirty = { false };
	class CMaterial* m_pBaseMaterial = { nullptr };

	SHADER_MI_DESC m_shaderDesc = {};
public:
	static CMaterialInstance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual void Free() override;
};

NS_END