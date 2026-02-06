#include "Engine_pch.h"
#include "FxShaderVariant.h"
#include "FxEffectAsset.h"
#include "Engine_Utils.h"
#include "Constant_Buffer.h"

CFxShaderVariant::CFxShaderVariant(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, CFxEffectAsset* pOwner)
	: m_pOwner(pOwner)
	, m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pOwner);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

HRESULT CFxShaderVariant::Build_InputLayouts(EVtxLayout layout)
{
	if (m_pOwner == nullptr)
		return E_FAIL;

	// Layout Table 조회
	const LayoutDesc& layoutDesc = g_layoutTable[ENUM_TO_UINT(layout)];
	if (layoutDesc.pElement == nullptr || layoutDesc.iCount == 0)
		return E_FAIL;

	// 기존 InputLayout 정리
	for (auto* pInputLayout : m_vecInputLayoutsPerPass)
	{
		if (pInputLayout)
			Safe_Release(pInputLayout);
	}
	m_vecInputLayoutsPerPass.clear();

	const auto& vecPasses = m_pOwner->Get_Passes();
	m_vecInputLayoutsPerPass.resize(vecPasses.size(), nullptr);

	for (size_t i = 0; i < m_vecInputLayoutsPerPass.size(); ++i)
	{
		const auto& passDesc = vecPasses[i].tDesc;
		// VS가 없는 pass일경우 Siganature가 없으니 방어
		if (passDesc.pIAInputSignature == nullptr || passDesc.IAInputSignatureSize <= 0)
			continue;

		HRESULT hr = m_pDevice->CreateInputLayout(
			layoutDesc.pElement,
			layoutDesc.iCount,
			passDesc.pIAInputSignature,
			passDesc.IAInputSignatureSize,
			&m_vecInputLayoutsPerPass[i]);
		
		if (FAILED(hr))
			return E_FAIL;
	}
	return S_OK;
}

ID3D11InputLayout* CFxShaderVariant::Get_InputLayout(size_t iPassIndex) const
{
	if (m_vecInputLayoutsPerPass.size() <= iPassIndex)
		return nullptr;
	return m_vecInputLayoutsPerPass[iPassIndex];
}

CFxShaderVariant* CFxShaderVariant::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, CFxEffectAsset* pOwner)
{
	return new CFxShaderVariant(pDevice, pDeviceContext, pOwner);
}

void CFxShaderVariant::Free()
{
	for (auto* pElement : m_vecInputLayoutsPerPass)
		Safe_Release(pElement);
	m_vecInputLayoutsPerPass.clear();
	Safe_Release(m_pOwner);
	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);
	Super::Free();
}
