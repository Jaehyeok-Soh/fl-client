#include "Engine_pch.h"
#include "ShaderAsset_Manager.h"
#include "FxEffectAsset.h"
#include "FxShaderVariant.h"
#include "GameInstance.h"

std::size_t FxVariantKeyHasher::operator()(const FxVariantKey& key) const noexcept
{
	std::size_t seed{ 0 };
	Engine_Utils::Hash_HasCombine(seed, std::hash<std::wstring>{}(key.tEffect.wstrPath));
	Engine_Utils::Hash_HasCombine(seed, std::hash<_uint>{}(ENUM_TO_UINT(key.eLayout)));
	return seed;
}

CShaderAsset_Manager::CShaderAsset_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

HRESULT CShaderAsset_Manager::Initialize()
{
	return S_OK;
}

CFxEffectAsset* CShaderAsset_Manager::GetOrCreate_FxEffectAsset(const path& filePath)
{
	FxEffectKey key{};
	key.wstrPath = Engine_Utils::Normalize_PathKey(filePath);

	auto itr = m_umapEffects.find(key);
	if (itr != m_umapEffects.end())
		return itr->second;

	CFxEffectAsset* pAsset = CFxEffectAsset::Create(m_pDevice, m_pDeviceContext);
	if (FAILED(pAsset->Load_EffectFromFile(filePath)))
	{
		MSG_BOX("CShaderAsset_Manager::GetOrCreate_FxEffectAsset, Failed");
		Safe_Release(pAsset);
		return nullptr;
	}

	pAsset->Get_BindingCache()->CB[ENUM_TO_UINT(EFXCB::Global)]->SetConstantBuffer(CGameInstance::GetInstance()->Get_Global_ConstantBuffer());
	pAsset->Get_BindingCache()->CB[ENUM_TO_UINT(EFXCB::Inv)]->SetConstantBuffer(CGameInstance::GetInstance()->Get_Inv_ConstantBuffer());
	pAsset->Get_BindingCache()->CB[ENUM_TO_UINT(EFXCB::Light)]->SetConstantBuffer(CGameInstance::GetInstance()->Get_Light_ConstantBuffer());

	m_umapEffects.emplace(key, pAsset);
	return pAsset;
}

CFxShaderVariant* CShaderAsset_Manager::GetOrCreate_Variant(const path& filePath, EVtxLayout eVertexLayoutID)
{
	FxVariantKey key{};
	key.tEffect.wstrPath = Engine_Utils::Normalize_PathKey(filePath);
	key.eLayout = eVertexLayoutID;

	auto itr = m_umapVariants.find(key);
	if (itr != m_umapVariants.end())
		return itr->second;

	CFxEffectAsset* pOwner = GetOrCreate_FxEffectAsset(filePath);
	CFxShaderVariant* pVariant = CFxShaderVariant::Create(m_pDevice, m_pDeviceContext, pOwner);
	if (FAILED(pVariant->Build_InputLayouts(eVertexLayoutID)))
	{
		MSG_BOX("CShaderAsset_Manager::GetOrCreate_Variant, Failed");
		Safe_Release(pVariant);
		return nullptr;
	}

	pOwner->Get_BindingCache()->CB[ENUM_TO_UINT(EFXCB::Global)]->SetConstantBuffer(CGameInstance::GetInstance()->Get_Global_ConstantBuffer());
	pOwner->Get_BindingCache()->CB[ENUM_TO_UINT(EFXCB::Inv)]->SetConstantBuffer(CGameInstance::GetInstance()->Get_Inv_ConstantBuffer());
	pOwner->Get_BindingCache()->CB[ENUM_TO_UINT(EFXCB::Light)]->SetConstantBuffer(CGameInstance::GetInstance()->Get_Light_ConstantBuffer());
	m_umapVariants.emplace(key, pVariant);
	return pVariant;
}

void CShaderAsset_Manager::Clear()
{
	for (auto& Pair : m_umapEffects)
		Safe_Release(Pair.second);
	m_umapEffects.clear();

	for (auto& Pair : m_umapVariants)
		Safe_Release(Pair.second);
	m_umapVariants.clear();
}

CShaderAsset_Manager* CShaderAsset_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CShaderAsset_Manager* pInstance = new CShaderAsset_Manager(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("CShaderAsset_Manager::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CShaderAsset_Manager::Free()
{
	Clear();
	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);
	Super::Free();
}
