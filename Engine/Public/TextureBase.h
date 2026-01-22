#pragma once
#include "ResourceBase.h"

NS_BEGIN(Engine)

class ENGINE_DLL CTextureBase final : public CResourceBase
{
	using Super = CResourceBase;
private:
	CTextureBase(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CTextureBase() = default;

	virtual HRESULT Initialize(void* pArg) override;
public:
	ID3D11ShaderResourceView* Get_SRV() { return m_pSRV; }
	ID3D11ShaderResourceView** Get_SRV_AddressOf() { return &m_pSRV; }

	const _float2& Get_Size() const { return m_vSize; }
	void Set_Size(const _float2& vSize) { m_vSize = vSize; }
private:
	HRESULT Load_TextureBase();
	HRESULT Load_LegacyXTex();
private:
	ID3D11ShaderResourceView* m_pSRV = { nullptr };
	_float2 m_vSize = { 0.0f, 0.0f };
public:
	static CTextureBase* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual void Free() override;
};

NS_END