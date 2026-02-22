#include "Engine_pch.h"
#include "GameDataManager.h"
#include "GameInstance.h"

CGameDataManager::CGameDataManager()
    : m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CGameDataManager::Initialize()
{
    return S_OK;
}

ID3D11ShaderResourceView* CGameDataManager::Make_ShaderResourceViewColor(_uint A, _uint R, _uint G, _uint B, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	D3DCOLOR Color = (D3DCOLOR)((((A) & 0xff) << 24) | (((B) & 0xff) << 16) | (((G) & 0xff) << 8) | ((R) & 0xff));

	ID3D11Texture2D* pTexture2D = { nullptr };
	ID3D11ShaderResourceView* pSRV{ nullptr };


	D3D11_TEXTURE2D_DESC tDesc{};
	tDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
	/* 2 의 n 제곱 값 추천*/
	tDesc.Width = 128;
	tDesc.Height = 128;
	tDesc.MipLevels = 1;
	tDesc.ArraySize = 1;

	tDesc.SampleDesc.Quality = 0;
	tDesc.SampleDesc.Count = 1;

	/* 바인드값은 이대로 두기 */
	tDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	tDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	tDesc.CPUAccessFlags = 0;
	tDesc.MiscFlags = 0;

	_uint* pInitializePixel = new _uint[tDesc.Width * tDesc.Height];
	ZeroMemory(pInitializePixel, sizeof(_uint) * (tDesc.Width * tDesc.Height));
	D3D11_SUBRESOURCE_DATA tData{};

	for (_uint i = 0; i < tDesc.Width * tDesc.Height; ++i)
		pInitializePixel[i] = (D3DCOLOR)((((255) & 0xff) << 24) | (((255) & 0xff) << 16) | (((255) & 0xff) << 8) | ((255) & 0xff));

	tData.pSysMem = pInitializePixel;
	tData.SysMemPitch = sizeof(_int32) * tDesc.Width;

	if (FAILED(pDevice->CreateTexture2D(&tDesc, &tData, &pTexture2D)))
	{
		if (pInitializePixel != nullptr)
			delete[] pInitializePixel;
		pInitializePixel = nullptr;

		Safe_Release(pTexture2D);

		return nullptr;
	}

	if (FAILED(pDevice->CreateShaderResourceView(pTexture2D, nullptr, &pSRV)))
		return nullptr;

	delete[] pInitializePixel;
	pInitializePixel = nullptr;

	Safe_Release(pTexture2D);

	return pSRV;
}

const DTO::TAttackPreset_Data* CGameDataManager::Find_AttackPrseet(_uint iPresetKey) const
{
	auto itr = m_umapAttackPresetDatas.find(iPresetKey);
	return (itr == m_umapAttackPresetDatas.end()) ? nullptr : &itr->second;
}

const DTO::TAttackPreset_Data* CGameDataManager::Find_AttackPresetByTag(const string& strTag) const
{
	auto itr = m_umapAttackPresetTagToKey.find(strTag);
	if (itr == m_umapAttackPresetTagToKey.end())
		return nullptr;

	return Find_AttackPrseet(itr->second);
}

void CGameDataManager::Clear_AttackPreset()
{
	m_bAttackPresetLoaded = false;
	m_umapAttackPresetDatas.clear();
	m_umapAttackPresetTagToKey.clear();
}

HRESULT CGameDataManager::Upsert_AttackPresetData(const DTO::TAttackPreset_Data& inData)
{
	DTO::TAttackPreset_Data data = inData;
	data.Make_Key();

	if (data.strTag.empty())
		return E_FAIL;

	auto itTag = m_umapAttackPresetTagToKey.find(data.strTag);
	if (itTag != m_umapAttackPresetTagToKey.end())
	{
		if (itTag->second != data.iPresetKey)
			return E_FAIL;
	}

	auto it = m_umapAttackPresetDatas.find(data.iPresetKey);
	if (it != m_umapAttackPresetDatas.end())
	{
		if (it->second.strTag != data.strTag)
		{
			m_umapAttackPresetTagToKey.erase(it->second.strTag);
			m_umapAttackPresetTagToKey[data.strTag] = data.iPresetKey;
		}
		it->second = data;
	}
	else
	{
		m_umapAttackPresetDatas.emplace(data.iPresetKey, data);
		m_umapAttackPresetTagToKey.emplace(data.strTag, data.iPresetKey);
	}

	return S_OK;
}

CGameDataManager* CGameDataManager::Create()
{
    CGameDataManager* pInstance = new CGameDataManager;
    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("CGameDataManager::Create, Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CGameDataManager::Free()
{
	Clear_AttackPreset();
	Safe_Release(m_pGameInstance);
    Super::Free();
}
