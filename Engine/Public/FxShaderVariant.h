#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CFxEffectAsset;

class CFxShaderVariant final : public CBase
{
	using Super = CBase;
private:
	CFxShaderVariant(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, CFxEffectAsset* pOwner);
	virtual ~CFxShaderVariant() = default;
public:
	HRESULT Build_InputLayouts(EVtxLayout layout);
	ID3D11InputLayout* Get_InputLayout(size_t iPassIndex) const;
	CFxEffectAsset* Get_EffectAsset() { return m_pOwner; }
private:
	CFxEffectAsset* m_pOwner{ nullptr };
	ID3D11Device* m_pDevice{ nullptr };
	ID3D11DeviceContext* m_pDeviceContext{ nullptr };
	vector<ID3D11InputLayout*> m_vecInputLayoutsPerPass;
public:
	static CFxShaderVariant* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, CFxEffectAsset* pOwner);
	virtual void Free() override;
};

NS_END