#include "Engine_pch.h"
#include "GameDataManager.h"
#include "GameInstance.h"
#include "Shader.h"
#include <fstream>
#include "Cinematic_Manager.h"
#include "Collider.h"
#include "Bounding_AABB.h"
#include "Bounding_OBB.h"

CGameDataManager::CGameDataManager(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: m_pGameInstance(CGameInstance::GetInstance()), m_pDevice(pDevice), m_pDeviceContext(pDeviceContext)
	, m_pBatch{nullptr}
	, m_pEffect{nullptr}
	, m_pInputLayout{nullptr}
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pDeviceContext);
    Safe_AddRef(m_pGameInstance);
}

HRESULT CGameDataManager::Initialize()
{

	if (FAILED(Make_DefaultTextures()))
		return E_FAIL;

	if (FAILED(Make_MapMinMaxBox()))
		return E_FAIL;


	if (FAILED(Make_Batch()))
		return E_FAIL;

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

HRESULT CGameDataManager::Make_Batch()
{
	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pDeviceContext);
	m_pEffect = new BasicEffect(m_pDevice);
	m_pEffect->SetVertexColorEnabled(true);

	const void* pShaderInput = { nullptr };
	size_t iShaderInputLenght = {};
	m_pEffect->GetVertexShaderBytecode(&pShaderInput, &iShaderInputLenght);

	if (FAILED(m_pDevice->CreateInputLayout(
		VertexPositionColor::InputElements
		, VertexPositionColor::InputElementCount
		, pShaderInput
		, iShaderInputLenght
		, &m_pInputLayout)))
		return E_FAIL;




	return S_OK;
}


#pragma region Map Min Max Box
HRESULT CGameDataManager::Make_MapMinMaxBox()
{
	CBounding_AABB::BOUNDING_AABB_DESC tAABB_Desc{};
	tAABB_Desc.vCenter = {0.f,0.f};
	tAABB_Desc.vExtens = {5.f,5.f};
	m_pMapMinMaxBox = CBounding_AABB::Create(m_pDevice , m_pDeviceContext, &tAABB_Desc);
	if (m_pMapMinMaxBox == nullptr) return E_FAIL;

	return S_OK;
}

void CGameDataManager::Set_MapMinMaxBox(const Vec3& vCenterPos, const Vec3& vExtents)
{
	if (m_pMapMinMaxBox == nullptr) return;

	BoundingBox* pBox = m_pMapMinMaxBox->Get_OriginalDesc();
	pBox->Center = vCenterPos;
	pBox->Extents = vExtents;

	m_pMapMinMaxBox->Update(Matrix::Identity);

	return;
}


#ifdef _DEBUG

HRESULT CGameDataManager::DebugRender_MapMinMaxBox()
{
	if (m_pBatch == nullptr) return E_FAIL;
	if (m_pEffect == nullptr) return E_FAIL;
	if (m_pInputLayout == nullptr) return E_FAIL;

	if (m_pMapMinMaxBox == nullptr) return E_FAIL;

	m_pDeviceContext->GSSetShader(nullptr, nullptr, 0);
	m_pEffect->SetWorld(Matrix::Identity);
	m_pEffect->SetView(m_pGameInstance->Get_ViewMatrix());
	m_pEffect->SetProjection(m_pGameInstance->Get_ProjMatrix());

	m_pEffect->Apply(m_pDeviceContext);
	m_pDeviceContext->IASetInputLayout(m_pInputLayout);

	m_pBatch->Begin();
	m_pMapMinMaxBox->Render(m_pBatch , true );
	m_pBatch->End();

	return S_OK;
}
#endif // _DEBUG
#pragma endregion

#pragma region Texture Splating Info


HRESULT CGameDataManager::Make_DefaultTextures()
{
	// 공통 텍스처 설정 (1x1 픽셀, 32비트 RGBA)
	D3D11_TEXTURE2D_DESC tDesc = {};
	tDesc.Width = 1;
	tDesc.Height = 1;
	tDesc.MipLevels = 1;
	tDesc.ArraySize = 1;
	tDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	tDesc.SampleDesc.Count = 1;
	tDesc.Usage = D3D11_USAGE_DEFAULT;
	tDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	tDesc.CPUAccessFlags = 0;


	// --- White Texture ---
	{
		uint32_t pixelWhite = 0xFFFFFFFF; // R=255, G=255, B=255, A=255 (0xAABBGGRR)
		D3D11_SUBRESOURCE_DATA tData = {};
		tData.pSysMem = &pixelWhite;
		tData.SysMemPitch = sizeof(uint32_t);

		ID3D11Texture2D* pTexWhite = nullptr;
		if (FAILED(m_pDevice->CreateTexture2D(&tDesc, &tData, &pTexWhite)))
			return E_FAIL;

		if (FAILED(m_pDevice->CreateShaderResourceView(pTexWhite, nullptr, &m_pDefaultWhite)))
		{
			pTexWhite->Release();
			return E_FAIL;
		}
		Safe_Release(pTexWhite);
	}

	// --- Black Texture ---
	{
		uint32_t pixelBlack = 0xFF000000; // R=0, G=0, B=0, A=255 (0xAABBGGRR)
		D3D11_SUBRESOURCE_DATA tData = {};
		tData.pSysMem = &pixelBlack;
		tData.SysMemPitch = sizeof(uint32_t);

		ID3D11Texture2D* pTexBlack = nullptr;
		if (FAILED(m_pDevice->CreateTexture2D(&tDesc, &tData, &pTexBlack)))
			return E_FAIL;

		if (FAILED(m_pDevice->CreateShaderResourceView(pTexBlack, nullptr, &m_pDefaultBlack)))
		{
			pTexBlack->Release();
			return E_FAIL;
		}
		Safe_Release(pTexBlack);
	}

	// --- Red Texture ---
	{
		uint32_t pixelRed = 0xFF0000FF; // R=255, G=0, B=0, A=255 (0xAABBGGRR)
		D3D11_SUBRESOURCE_DATA tData = {};
		tData.pSysMem = &pixelRed;
		tData.SysMemPitch = sizeof(uint32_t);

		ID3D11Texture2D* pTexRed = nullptr;
		if (FAILED(m_pDevice->CreateTexture2D(&tDesc, &tData, &pTexRed)))
			return E_FAIL;

		if (FAILED(m_pDevice->CreateShaderResourceView(pTexRed, nullptr, &m_pDefaultRed)))
		{
			pTexRed->Release();
			return E_FAIL;
		}
		Safe_Release(pTexRed);
	}

	// --- Green Texture ---
	{
		uint32_t pixelGreen = 0xFF00FF00; // R=0, G=255, B=0, A=255 (0xAABBGGRR)
		D3D11_SUBRESOURCE_DATA tData = {};
		tData.pSysMem = &pixelGreen;
		tData.SysMemPitch = sizeof(uint32_t);

		ID3D11Texture2D* pTexGreen = nullptr;
		if (FAILED(m_pDevice->CreateTexture2D(&tDesc, &tData, &pTexGreen)))
			return E_FAIL;

		if (FAILED(m_pDevice->CreateShaderResourceView(pTexGreen, nullptr, &m_pDefaultGreen)))
		{
			pTexGreen->Release();
			return E_FAIL;
		}
		Safe_Release(pTexGreen);
	}

	// --- Blue Texture ---
	{
		uint32_t pixelBlue = 0xFFFF0000; // R=0, G=0, B=255, A=255 (0xAABBGGRR)
		D3D11_SUBRESOURCE_DATA tData = {};
		tData.pSysMem = &pixelBlue;
		tData.SysMemPitch = sizeof(uint32_t);

		ID3D11Texture2D* pTexBlue = nullptr;
		if (FAILED(m_pDevice->CreateTexture2D(&tDesc, &tData, &pTexBlue)))
			return E_FAIL;

		if (FAILED(m_pDevice->CreateShaderResourceView(pTexBlue, nullptr, &m_pDefaultBlue)))
		{
			pTexBlue->Release();
			return E_FAIL;
		}
		Safe_Release(pTexBlue);
	}

	return S_OK;
}

HRESULT CGameDataManager::Load_TextureSplatingInfoData()
{
	/* Texture Splating을 저장시킨 Data들을 Load해준다 */

	std::ifstream ifs(m_wszTextureSplatingInfoDataPath);

	if (ifs.is_open() == false) return E_FAIL;

	if (ifs.peek() == std::ifstream::traits_type::eof())
	{
		return S_OK; // 텅 비어있으니 로드할 것도 없다! 안전하게 리턴.
	}

	nlohmann::json LoadJson{};
	ifs >> LoadJson;



	if (LoadJson.empty())
		return S_OK;

	/* 저장할 Key 값을통해 저장시켜준다 */

	m_mapTextureSplatingInfoDatas.clear();


	for (const auto& item : LoadJson.items())
	{
		TEXTURE_SPLATTING_INFO tInfo{};

		wstring wstrKey = Engine_Utils::ToWString(item.key());
		tInfo.Load_Json(item.value());
		m_mapTextureSplatingInfoDatas.emplace(wstrKey, tInfo);
	}

	return S_OK;
}

HRESULT CGameDataManager::Bind_SplatingTextureInfo(CShader* pBindShader, const wstring& wstrTextureSplatingInfoDataName)
{
	if (pBindShader == nullptr) return E_FAIL;

	/* Binding Texture */
	if (FAILED(Bind_MapTexture(pBindShader,wstrTextureSplatingInfoDataName)))
		return E_FAIL;

	/* 현재 사용하는 RGBA Map 개수 Binding */
	if (FAILED(Bind_Mix_RGBA_Info(pBindShader, wstrTextureSplatingInfoDataName)))
		return E_FAIL;

	return S_OK;
}

HRESULT CGameDataManager::Bind_MapTexture(CShader* pBindShader, const wstring& wstrTextureSplatingInfoDataName)
{
	/* SRV 바인딩 */
	if (pBindShader == nullptr) return E_FAIL;

	if (m_mapTextureSplatingInfoDatas.find(wstrTextureSplatingInfoDataName) == m_mapTextureSplatingInfoDatas.end())
		return E_FAIL;

	auto& tTextureSplattingInfo = m_mapTextureSplatingInfoDatas.at(wstrTextureSplatingInfoDataName);


	/* Base , RGB , RGBA 텍스처를 바인딩해준다 */
	ID3DX11EffectShaderResourceVariable* pEffectSRV{ nullptr };

	/* Base Texture */
	pEffectSRV = pBindShader->Get_Variable(g_szBase_Texture)->AsShaderResource();
	if (!pEffectSRV->IsValid())
	{
		MSG_BOX(" g_Base_Texture is Can't Find ");
		return E_FAIL;
	}
	pEffectSRV->SetResource(tTextureSplattingInfo.pBase_Texture == nullptr ? m_pDefaultBlack : tTextureSplattingInfo.pBase_Texture->Get_SRV());

	/* DH Tile Texture  */
	pEffectSRV = pBindShader->Get_Variable(g_szMix_DH_Tile_Texture)->AsShaderResource();
	if (!pEffectSRV->IsValid())
	{
		MSG_BOX(" g_Mix_DH_Tile_Texture is Can't Find ");
		return E_FAIL;
	}
	pEffectSRV->SetResource(tTextureSplattingInfo.pMix_DH_Tile_Texture == nullptr ? m_pDefaultBlack : tTextureSplattingInfo.pMix_DH_Tile_Texture->Get_SRV());


	/* NBR Tile Texture */
	pEffectSRV = pBindShader->Get_Variable(g_szMix_NBR_Tile_Texture)->AsShaderResource();
	if (!pEffectSRV->IsValid())
	{
		MSG_BOX(" g_Mix_NBR_Tile_Texture is Can't Find ");
		return E_FAIL;
	}
	pEffectSRV->SetResource(tTextureSplattingInfo.pMix_NBR_Tile_Texture == nullptr ? m_pDefaultBlack : tTextureSplattingInfo.pMix_NBR_Tile_Texture->Get_SRV());

	return S_OK;
}
HRESULT CGameDataManager::Bind_Mix_RGBA_Info(CShader* pBindShader, const wstring& wstrTextureSplatingInfoDataName)
{
	if (FAILED(Bind_Mix_RGBA_Texture(pBindShader, wstrTextureSplatingInfoDataName)))
		return E_FAIL;

	if (FAILED(Bind_Mix_RGBA_Data_And_Count(pBindShader, wstrTextureSplatingInfoDataName)))
		return E_FAIL;

	return S_OK;
}
HRESULT CGameDataManager::Bind_Mix_RGBA_Texture(CShader* pBindShader, const wstring& wstrTextureSplatingInfoDataName)
{
	if (pBindShader == nullptr) return E_FAIL;

	if( m_mapTextureSplatingInfoDatas.find(wstrTextureSplatingInfoDataName) == m_mapTextureSplatingInfoDatas.end())
		return E_FAIL;

	auto& tTextureSplattingInfo = m_mapTextureSplatingInfoDatas.at(wstrTextureSplatingInfoDataName);



	/* Texture 먼저 Binding */
	ID3DX11EffectShaderResourceVariable* pEffectSRV{ nullptr };

	/* Effect SRVs */
	pEffectSRV = pBindShader->Get_Variable(g_szMix_RGBA_Texture)->AsShaderResource();
	if (!pEffectSRV->IsValid())
	{
		MSG_BOX(" g_Mix_RGBA_Texture is Can't Find ");
		return E_FAIL;
	}

	/* SRV를 모아기 */
	array<ID3D11ShaderResourceView*, MAX_RGBA_TEXTURE_COUNT> arraySRVs{};
	arraySRVs.fill(nullptr);

	for (_int i = 0; i < tTextureSplattingInfo.tMix_RGBA_Info.iUse_Mix_RGBA_Count; ++i)
		arraySRVs[i] = tTextureSplattingInfo.tMix_RGBA_Info.vecMixRGBATexture[i] == nullptr ? m_pDefaultBlack
		: tTextureSplattingInfo.tMix_RGBA_Info.vecMixRGBATexture[i]->Get_SRV();

	/* 모은 SRV 던져주기 */
	if (FAILED(pEffectSRV->SetResourceArray(arraySRVs.data(), 0, MAX_RGBA_TEXTURE_COUNT)))
		return E_FAIL;

	return S_OK;
}
HRESULT CGameDataManager::Bind_Mix_RGBA_Data_And_Count(CShader* pBindShader, const wstring& wstrTextureSplatingInfoDataName)
{
	if (pBindShader == nullptr) return E_FAIL;

	if (m_mapTextureSplatingInfoDatas.find(wstrTextureSplatingInfoDataName) == m_mapTextureSplatingInfoDatas.end())
		return E_FAIL;

	auto& tTextureSplattingInfo = m_mapTextureSplatingInfoDatas.at(wstrTextureSplatingInfoDataName);

	CB_MIX_RGBA_INFO	tCB{};

	memcpy(tCB.g_MIX_RGBA_DATA, tTextureSplattingInfo.tMix_RGBA_Info.vecMix_RGBA_Data.data(), sizeof(MIX_RGBA_DATA) * tTextureSplattingInfo.tMix_RGBA_Info.iUse_Mix_RGBA_Count);
	tCB.g_iUse_Mix_RGBA_Count = tTextureSplattingInfo.tMix_RGBA_Info.iUse_Mix_RGBA_Count;

	ID3DX11EffectConstantBuffer* pCB = pBindShader->Get_ConstantBuffer(g_szCB_MIX_RGBA_INFO);
	if (!pCB->IsValid())
	{
		MSG_BOX("CB_MIX_RGBA_INFO 바인딩 실패 문자열 검색 확인");
		return E_FAIL;
	}
	if (FAILED(pCB->SetRawValue(&tCB, 0, sizeof(CB_MIX_RGBA_INFO))))
		return E_FAIL;

	return S_OK;
}

#pragma endregion

#pragma region ATTACK_PRESET
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

_uint CGameDataManager::Get_AttackPresetIdByTag(const string& strTag) const
{
	if (strTag.empty() == true)
	{
		MSG_BOX("CGameDataManager::Get_AttackPresetIdByTag, Empty tag");
		return UINT_MAX;
	}

	auto itr = m_umapAttackPresetTagToKey.find(strTag);
	if (itr == m_umapAttackPresetTagToKey.end())
	{
		//MSG_BOX("CGameDataManager::Get_AttackPresetIdByTag, Invalid tag");
		return UINT_MAX;
	}

	return itr->second;
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
#pragma endregion

CGameDataManager* CGameDataManager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CGameDataManager* pInstance = new CGameDataManager(pDevice , pDeviceContext);
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("CGameDataManager::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGameDataManager::Free()
{
	Safe_Release(m_pDefaultBlack);
	Safe_Release(m_pDefaultWhite);
	Safe_Release(m_pDefaultRed);
	Safe_Release(m_pDefaultBlue);
	Safe_Release(m_pDefaultGreen);
	Safe_Release(m_pMapMinMaxBox);

	Safe_Delete(m_pBatch);
	Safe_Delete(m_pEffect);
	Safe_Release(m_pInputLayout);


	Clear_AttackPreset();
	for (auto& Pair : m_mapTextureSplatingInfoDatas)
		Pair.second.Free();
	m_mapTextureSplatingInfoDatas.clear();

	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pGameInstance);

    Super::Free();
}
