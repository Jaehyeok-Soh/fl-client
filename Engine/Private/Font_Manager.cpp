#include "Engine_pch.h"
#include "d3dcompiler.h"
#include "Constant_Buffer.h"
#include "Font_Manager.h"
#include "Font.h"
#include "TextureBase.h"
#include "GameInstance.h"

#define NOISE_TEXTURE_FILE_NAME wstring{L"Default_Noise"}
#define NOISE_TEXTURE_FILE_PATH wstring{L"../../Resources/Textures/UI/Font/Default_Noise.png"}

CFont_Manager::CFont_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : m_pDevice{ pDevice }
    , m_pDeviceContext{ pDeviceContext }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pDeviceContext);
}

HRESULT CFont_Manager::Initialize()
{
    m_pBatch = new SpriteBatch(m_pDeviceContext);

	for (uint32_t i = 0; i < ENUM_TO_UINT(EFontShaderType::END); ++i)
	{
		m_vecFontData[i].reserve(64);
	}

	if (FAILED(Settig_SpriteBatchStates()))
		return E_FAIL;

	if (FAILED(Setting_PixelShader()))
		return E_FAIL;

	if (FAILED(Settig_FontConstantBuffers()))
		return E_FAIL;

	if (FAILED(Setting_Resource()))
		return E_FAIL;

    return S_OK;
}

HRESULT CFont_Manager::Settig_SpriteBatchStates()
{
	////////////////////////////////////////////////////////////
	// Sampler / LinearClamp
	{
		D3D11_SAMPLER_DESC SamplerDesc{};
		SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		SamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		if (FAILED(m_pDevice->CreateSamplerState(&SamplerDesc, &m_pSampler_LinearClamp)))
			return E_FAIL;
	}

	////////////////////////////////////////////////////////////
	// Sampler / PointClamp
	{
		D3D11_SAMPLER_DESC SamplerDesc{};
		SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		SamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		if (FAILED(m_pDevice->CreateSamplerState(&SamplerDesc, &m_pSampler_PointClamp)))
			return E_FAIL;
	}

	////////////////////////////////////////////////////////////
	// Sampler / LinearWrap
	{
		D3D11_SAMPLER_DESC SamplerDesc{};
		SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		SamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		if (FAILED(m_pDevice->CreateSamplerState(&SamplerDesc, &m_pSampler_LinearWrap)))
			return E_FAIL;
	}

	////////////////////////////////////////////////////////////
	// Blend / Premultiplied (ONE, INV_SRC_ALPHA)
	{
		D3D11_BLEND_DESC BlendDesc{};
		BlendDesc.AlphaToCoverageEnable		= FALSE;
		BlendDesc.IndependentBlendEnable	= FALSE;

		auto& rt		= BlendDesc.RenderTarget[0];
		rt.BlendEnable	= TRUE;
		rt.SrcBlend		= D3D11_BLEND_ONE;
		rt.DestBlend	= D3D11_BLEND_INV_SRC_ALPHA;
		rt.BlendOp		= D3D11_BLEND_OP_ADD;
		rt.SrcBlendAlpha	= D3D11_BLEND_ONE;
		rt.DestBlendAlpha	= D3D11_BLEND_INV_SRC_ALPHA;
		rt.BlendOpAlpha		= D3D11_BLEND_OP_ADD;

		rt.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		if (FAILED(m_pDevice->CreateBlendState(&BlendDesc, &m_pBlend_Premultiplied)))
			return E_FAIL;
	}

	////////////////////////////////////////////////////////////
	// Blend / Non-Premultiplied (SRC_ALPHA, INV_SRC_ALPHA)
	{
		D3D11_BLEND_DESC BlendDesc = {};
		BlendDesc.AlphaToCoverageEnable		= FALSE;
		BlendDesc.IndependentBlendEnable	= FALSE;

		auto& rt		= BlendDesc.RenderTarget[0];
		rt.BlendEnable	= TRUE;
		rt.SrcBlend		= D3D11_BLEND_SRC_ALPHA;
		rt.DestBlend	= D3D11_BLEND_INV_SRC_ALPHA;
		rt.BlendOp		= D3D11_BLEND_OP_ADD;

		rt.SrcBlendAlpha	= D3D11_BLEND_ONE;
		rt.DestBlendAlpha	= D3D11_BLEND_INV_SRC_ALPHA;
		rt.BlendOpAlpha		= D3D11_BLEND_OP_ADD;

		rt.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		if (FAILED(m_pDevice->CreateBlendState(&BlendDesc, &m_pBlend_NonPremul)))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CFont_Manager::Setting_PixelShader()
{
	Safe_Release(m_pPS_Outline);
	ID3DBlob* pBlob = nullptr;
	ID3DBlob* pErr	= nullptr;

	UINT flags = 0;
#if defined(_DEBUG)
	flags |= D3DCOMPILE_DEBUG;
	flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	const wchar_t* pFile	= L"../../Shaders/Shader_SpriteFont.hlsl";
	const char* pEntry		= "PS_OUTLINE_NOISE";
	const char* pTarget		= "ps_5_0";

	HRESULT hr = D3DCompileFromFile(pFile, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, pEntry, pTarget, flags, 0, &pBlob, &pErr);
	if (FAILED(hr))
	{
		if (pErr)
		{
			const char* msg = (const char*)pErr->GetBufferPointer();
			_string strmsg = msg;
			_wstring wstrmsg = Engine_Utils::ToWString(strmsg);
			MessageBox(nullptr, wstrmsg.c_str(), L"System Message", MB_OK);
		}
		Safe_Release(pErr);
		Safe_Release(pBlob);
		return E_FAIL;
	}

	hr = m_pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &m_pPS_Outline);
	Safe_Release(pErr);
	Safe_Release(pBlob);
	if (FAILED(hr))
		return E_FAIL;

	return S_OK;
}

HRESULT CFont_Manager::Settig_FontConstantBuffers()
{
	CConstant_Buffer<CB_FONT_OUTLINE_NOISE>* pCB = CConstant_Buffer<CB_FONT_OUTLINE_NOISE>::Create(m_pDevice, m_pDeviceContext);
	if (nullptr == pCB)
		return E_FAIL;
	m_pOutlineNoiseCB = pCB;
	return S_OK;
}

HRESULT CFont_Manager::Setting_Resource()
{
	// Noise Texture
	{
		CTextureBase::RESOURCE_BASE_DESC desc = {};
		desc.wstrName = NOISE_TEXTURE_FILE_NAME;
		desc.wstrPath = NOISE_TEXTURE_FILE_PATH;
		if (FAILED(CGameInstance::GetInstance()->Add_Resource(L"Texture_" + NOISE_TEXTURE_FILE_NAME, CTextureBase::Create(m_pDevice, m_pDeviceContext, &desc))))
			return E_FAIL;

		auto* pTextureBase = CGameInstance::GetInstance()->Get_Resource<CTextureBase>(L"Texture_" + NOISE_TEXTURE_FILE_NAME);
		m_pNoiseSRV = pTextureBase->Get_SRV();
		Safe_Release(pTextureBase);
		if (nullptr == m_pNoiseSRV)
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CFont_Manager::Add_Font(const _wstring& strFontTag, const _tchar* pFontFilePath)
{
    if (nullptr != Find_Font(strFontTag))
        return E_FAIL;

    CFont* pFont = CFont::Create(m_pDevice, m_pDeviceContext, pFontFilePath);
    if (nullptr == pFont)
        return E_FAIL;

    m_Fonts.emplace(strFontTag, pFont);
    return S_OK;
}

HRESULT CFont_Manager::Request_DrawFont(FONT_DESC Desc)
{
	uint32_t iIndex = ENUM_TO_UINT(Desc.eFontShaderType);

	if (iIndex >= ENUM_TO_UINT(EFontShaderType::END))
		return E_FAIL;

	m_vecFontData[iIndex].emplace_back(std::move(Desc));
	return S_OK;
}

HRESULT CFont_Manager::Render_Fonts()
{
	m_pDeviceContext->GSSetShader(nullptr, nullptr, 0);

	if (FAILED(Begin_Draw_Normal()))
		return E_FAIL;
	for (const auto& Desc : m_vecFontData[ENUM_TO_UINT(EFontShaderType::NORMAL)])
		Draw_Text(Desc);
	End_Draw();

	if (FAILED(Begin_Draw_OutlineNoise(true, false, false)))
			return E_FAIL;
	for (const auto& Desc : m_vecFontData[ENUM_TO_UINT(EFontShaderType::OUTLINE)])
		Draw_Text(Desc);
	End_Draw();

	if (FAILED(Begin_Draw_OutlineNoise(false, false, true)))
		return E_FAIL;
	for (const auto& Desc : m_vecFontData[ENUM_TO_UINT(EFontShaderType::NOISE)])
		Draw_Text(Desc);
	End_Draw();

	if (FAILED(Begin_Draw_OutlineNoise(false, true, true)))
		return E_FAIL;
	for (const auto& Desc : m_vecFontData[ENUM_TO_UINT(EFontShaderType::NOISE_KOR)])
		Draw_Text(Desc);
	End_Draw();

	if (FAILED(Begin_Draw_OutlineNoise(true, false, true)))
		return E_FAIL;
	for (const auto& Desc : m_vecFontData[ENUM_TO_UINT(EFontShaderType::OUTLINE_NOISE)])
		Draw_Text(Desc);
	End_Draw();

	if (FAILED(Begin_Draw_OutlineNoise(true, true, true)))
		return E_FAIL;
	for (const auto& Desc : m_vecFontData[ENUM_TO_UINT(EFontShaderType::OUTLINE_NOISE_KOR)])
		Draw_Text(Desc);

	End_Draw();
	Clear_FontQueue();
	return S_OK;
}

void CFont_Manager::Clear_FontQueue()
{
	for (uint32_t i = 0; i < ENUM_TO_UINT(EFontShaderType::END); ++i)
	{
		m_vecFontData[i].clear();
	}
}

HRESULT CFont_Manager::Begin_Draw_Normal()
{
	m_pBatch->Begin();
	return S_OK;
}

HRESULT CFont_Manager::Begin_Draw_OutlineNoise(const _bool isOutline, const _bool isKorean, const _bool isNoise)
{
	CB_FONT_OUTLINE_NOISE cb = {};
	cb.vOutlineColor	= Vec4(0.f, 0.f, 0.f, 1.f);
	cb.fOutlineStrength = 2.0f;
	m_vScrollUV.x		+= 0.0016f;
	if (m_vScrollUV.x > 1.f)
		m_vScrollUV.x = 0.f;

	cb.vNoiseUVScroll	= m_vScrollUV;
	cb.fNoiseStrength	= 1.0f;

	if(isOutline)	{ cb.fOutlineSizePx = 1.f; }
	else			{ cb.fOutlineSizePx = 0.f; }

	if(isKorean)	{ cb.vNoiseUVScale = Vec2(500.f, 500.f); }
	else			{ cb.vNoiseUVScale = Vec2(10.f, 10.f); }

	if (isNoise)	{ cb.fFillMix = 1.0f; }
	else			{ cb.fFillMix = 0.0f; }

	m_pOutlineNoiseCB->Copy_Data(cb);
	m_pBatch->Begin(SpriteSortMode_Deferred, m_pBlend_Premultiplied, m_pSampler_LinearClamp, nullptr, nullptr,
		[this]()
		{
			m_pDeviceContext->PSSetShader(m_pPS_Outline, nullptr, 0);

			ID3D11Buffer* pCB = m_pOutlineNoiseCB->Get_Buffer();
			m_pDeviceContext->PSSetConstantBuffers(1, 1, &pCB);			// b1	// 노이즈 상수버퍼 바인드 

			ID3D11ShaderResourceView* pNoiseSRV = m_pNoiseSRV;
			m_pDeviceContext->PSSetShaderResources(1, 1, &pNoiseSRV);	// t1	// 노이즈 텍스쳐 바인드

			ID3D11SamplerState* pNoiseSamp = m_pSampler_LinearWrap;
			m_pDeviceContext->PSSetSamplers(1, 1, &pNoiseSamp);			// s1	// 노이즈 샘플러 바인드
		});

	return S_OK;
}

void CFont_Manager::End_Draw()
{
	ID3D11ShaderResourceView* nullSRV = nullptr;
	m_pDeviceContext->PSSetShaderResources(1, 1, &nullSRV);
	m_pBatch->End();
}

HRESULT CFont_Manager::Draw_Text(const _wstring& strFontTag, const _tchar* pText, const Vec2& vPosition, const Vec4& vColor, EFontPivotType ePivot, const _float fRotate, const _float fScale)
{
    CFont* pFont = Find_Font(strFontTag);
    if (nullptr == pFont)
        return E_FAIL;

    if (FAILED(pFont->Draw_Text(m_pBatch, pText, vPosition, vColor, ePivot, fRotate, fScale)))
        return E_FAIL;

    return S_OK;
}

HRESULT CFont_Manager::Draw_Text(const FONT_DESC& Desc)
{
	CFont* pFont = Find_Font(Desc.strFontTag);
	if (nullptr == pFont)
		return E_FAIL;

	if (FAILED(pFont->Draw_Text(m_pBatch, Desc.strText.c_str(), Desc.vPosition, Desc.vColor, Desc.ePivot, Desc.fRotate, Desc.fScale)))
		return E_FAIL;

	return S_OK;
}

CFont* CFont_Manager::Find_Font(const _wstring& strFontTag)
{
    auto    iter = m_Fonts.find(strFontTag);

    if (iter == m_Fonts.end())
        return nullptr;

    return iter->second;
}

CFont_Manager* CFont_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
    CFont_Manager* pInstance = new CFont_Manager(pDevice, pDeviceContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CFont_Manager");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CFont_Manager::Free()
{
    for (auto& Pair : m_Fonts)
        Safe_Release(Pair.second);
    m_Fonts.clear();

	{
		ID3D11ShaderResourceView* nullSRV = nullptr;
		m_pDeviceContext->PSSetShaderResources(1, 1, &nullSRV);
		ID3D11SamplerState* nullSamp = nullptr;
		m_pDeviceContext->PSSetSamplers(1, 1, &nullSamp);
		ID3D11Buffer* nullCB = nullptr;
		m_pDeviceContext->PSSetConstantBuffers(1, 1, &nullCB);
		m_pDeviceContext->PSSetShader(nullptr, nullptr, 0);
	}

	CGameInstance::GetInstance()->Remove_Resource<CTextureBase>(L"Texture_" + NOISE_TEXTURE_FILE_NAME);

    Safe_Delete(m_pBatch);

	Safe_Release(m_pBlend_Premultiplied);
	Safe_Release(m_pBlend_NonPremul);
	Safe_Release(m_pSampler_LinearClamp);
	Safe_Release(m_pSampler_LinearWrap);
	Safe_Release(m_pSampler_PointClamp);

	Safe_Release(m_pPS_Outline);
	Safe_Release(m_pOutlineNoiseCB);

    Safe_Release(m_pDevice);
    Safe_Release(m_pDeviceContext);

    Super::Free();
}
