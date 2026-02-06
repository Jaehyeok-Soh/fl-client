#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CFxShaderVariant;
class CFxEffectAsset;

#pragma region Effect, Variant Key 및 Hasher
struct FxEffectKey
{
	// 정규화된 주소
	wstring wstrPath{ L"" };
	_bool operator==(const FxEffectKey&) const = default;
};
// Hasher 함수 객체
struct FxEffectKeyHasher
{
	std::size_t operator()(const FxEffectKey& key) const noexcept
	{
		return std::hash<std::wstring>{}(key.wstrPath);
	}
};
struct FxVariantKey
{
	FxEffectKey tEffect{};
	EVtxLayout eLayout{ EVtxLayout::NONE };
	_bool operator==(const FxVariantKey&) const = default;
};
// Hasher 함수 객체
struct FxVariantKeyHasher
{
	std::size_t operator()(const FxVariantKey& key) const noexcept;
};
#pragma endregion

class CShaderAsset_Manager final : public CBase
{
	using Super = CBase;
private:
	CShaderAsset_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CShaderAsset_Manager() = default;

	HRESULT Initialize();
public:
	CFxEffectAsset* GetOrCreate_FxEffectAsset(const path& filePath);
	CFxShaderVariant* GetOrCreate_Variant(const path& filePath, EVtxLayout eVertexLayoutID);
private:
	void Clear();
private:
	unordered_map<FxEffectKey, CFxEffectAsset*, FxEffectKeyHasher> m_umapEffects;
	unordered_map<FxVariantKey, CFxShaderVariant*, FxVariantKeyHasher> m_umapVariants;

	ID3D11Device* m_pDevice{ nullptr };
	ID3D11DeviceContext* m_pDeviceContext{ nullptr };
public:
	static CShaderAsset_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

NS_END