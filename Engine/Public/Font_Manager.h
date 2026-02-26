#pragma once
#include "Base.h"
#include "Constant_Buffer.h"
NS_BEGIN(Engine)

struct CB_FONT_OUTLINE_NOISE
{
	Vec4  vOutlineColor;		// 아웃라인 색							16
	_float fOutlineSizePx;		// 외곽선 사이즈						4
	_float fOutlineStrength;	// 외곽선 Alpha							4
	Vec2  vNoiseUVScale;		// Noise 패턴의 크기					8
	Vec2  vNoiseUVScroll;		// Noise를 흐르게 하는 Offset			8
	_float fNoiseStrength;		// Noise가 내부 색에 얼마나 영향을 줄지	4
	_float fFillMix;			// Noise랑 원래 글자 색을 얼마나 섞을지	4
};

class CFont_Manager final : public CBase
{
	using Super = CBase;
private:
	CFont_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CFont_Manager() = default;

public:
	HRESULT Initialize();
	HRESULT Settig_SpriteBatchStates();
	HRESULT Setting_PixelShader();
	HRESULT Settig_FontConstantBuffers();
	HRESULT Setting_Resource();

	HRESULT Add_Font(const _wstring& strFontTag, const _tchar* pFontFilePath);

	HRESULT Request_DrawFont(FONT_DESC Desc);
	HRESULT Render_Fonts();
	void Clear_FontQueue();

	HRESULT Begin_Draw_Normal();

	/// <summary>
	/// Batch Begin And Pixel Shader Bind
	/// </summary>
	/// <param name="isOutline"> / 아웃라인인가? </param>
	/// <param name="isKorean"> / 한국어인가? </param>
	/// <param name="isNoise"> / 노이즈인가? </param>
	/// <param name="isHit"> / 히트 데미지 폰트인가? </param>
	/// <returns></returns>
	HRESULT Begin_Draw_OutlineNoise(const _bool isOutline, const _bool isKorean, const _bool isNoise, const _bool isHit = false);
	HRESULT Begin_Draw_Gradation(const _bool isOutline);


	void End_Draw();

	HRESULT Draw_Text(const _wstring& strFontTag, const _tchar* pText, const Vec2& vPosition, const Vec4 &vColor, EFontPivotType ePivot, const _float fRotate, const _float fScale);
	HRESULT Draw_Text(const FONT_DESC& Desc);

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pDeviceContext = { nullptr };
	SpriteBatch* m_pBatch = { nullptr };
	map<const _wstring, class CFont*>	m_Fonts;

private:
	// Shader State
	ID3D11BlendState*	m_pBlend_Premultiplied	= nullptr; // Src=ONE
	ID3D11BlendState*	m_pBlend_NonPremul		= nullptr; // Src=SRC_ALPHA
	ID3D11SamplerState* m_pSampler_LinearClamp	= nullptr;
	ID3D11SamplerState* m_pSampler_LinearWrap	= nullptr;
	ID3D11SamplerState* m_pSampler_PointClamp	= nullptr;

	// Pixel Shader & Constant Buffer
	ID3D11PixelShader* m_pPS_Outline = nullptr;
	ID3D11PixelShader* m_pPS_OutlineGrad = nullptr;

	CConstant_Buffer<CB_FONT_OUTLINE_NOISE>* m_pOutlineNoiseCB = { nullptr };
	ID3D11ShaderResourceView* m_pNoiseSRV = { nullptr };

	// 폰트 Draw를 한번에 하기 위해서 
	array<vector<FONT_DESC>, ENUM_TO_UINT(EFontShaderType::END)> m_vecFontData;

	Vec2 m_vScrollUV = {};

private:
	class CFont* Find_Font(const _wstring& strFontTag);

public:
	static CFont_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

NS_END