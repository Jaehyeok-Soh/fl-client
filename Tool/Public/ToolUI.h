#pragma once
#include "UIObject.h"
#include "DataStruct_UI.h"

NS_BEGIN(Tool)
class CToolCanvas;
class CToolUI final : public CUIObject
{
	using Super = CUIObject;

public:
	typedef struct tagToolUIDesc : public Super::UIOBJECT_DESC
	{
		CToolCanvas* pCacheCanvas = { nullptr };
		DTO::EUIClassType eClassType;
		_string strName;
		_string strCanvasName;
		uint32_t iCanvasIndex;
		_string strInitTextureTag;
		uint32_t iRectTransformType;
		int32_t iShaderPass;
		_bool isUseColorTint;
		Vec4 vColorTint;
		Vec4 vGradiantColorTint;
		int32_t iFillDir;
		_float fDelay;
		int32_t iFlip;
		_float fAlpha;
		DTO::EUISubClassType eSubClassType;
		_string strNoiseTextureTag;
		_string strAlphaMaskTextureTag;
		_string strGlowTextureTag;
		DTO::TUI_TextData tTextData;
		DTO::TUI_DImageData tDImageData;
		int32_t iIndex;
	}TOOLUI_DESC;

	enum EUITextureSlot {DEFAULT = 0, NOISE, ALPHA_MASK, GLOW};

protected:
	CToolUI(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CToolUI(const CToolUI& rhs);
	virtual ~CToolUI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;

	virtual HRESULT Awake(const _uint iCurrentLevelID) override;
	virtual void Update_Priority(const _float fTimeDelta) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual void Update_Late(const _float fTimeDelta) override;
	virtual void Ready_Before_Render(const _float fTimeDelta) override;
	virtual HRESULT Render() override;
	_bool Calc_HitEvent();

protected:
	HRESULT Ready_Components(TOOLUI_DESC* pDesc);
	HRESULT Bind_ShaderResources();

	void SetUp_RectTransform_Position();
	Vec2 Calc_RectTransformPosition();
	void SetUp_Visible();
	void Acting_About_State();
	void Sync_Data();
	void Sync_TextData();
	void Sync_DImageData();
#pragma region GETTER/SETTER
public:
	const _string& Get_Name()const { return m_strName; }
	ERectTransform Get_RectTransformType() const { return m_eRectTransformType; }
	const _wstring& Get_TextureTag() const { return m_wstrTextureTag; }
	const _wstring& Get_NoiseTextureTag() const { return m_wstrNoiseTextureTag; }
	const _wstring& Get_AlphaMaskTextureTag() const { return m_wstrAlphaMaskTextureTag; }
	const _wstring& Get_GlowTextureTag() const { return m_wstrGlowTextureTag; }
	_float* Get_WIdth_Ptr() { return &m_fWidth; }
	_float* Get_Height_Ptr() { return &m_fHeight; }
	_float* Get_PosX_Ptr() { return &m_fX; }
	_float* Get_PosY_Ptr() { return &m_fY; }
	_float* Get_PosZ_Ptr() { return &m_fZ; }
	const DTO::TUI_GenericUIData& Get_Data()const { return m_tUIData; }
	DTO::TUI_GenericUIData& Get_Data_Ref() { return m_tUIData; }
	const DTO::TUI_TextData& Get_TextData()const { return m_tUITextData; }
	const DTO::TUI_DImageData& Get_DImageData()const { return m_tDImageData; }
	const Vec3& Get_RenderPos() const { return m_vRenderPos; }
	const RECT& Get_RenderRect() const { return m_tRenderRect; }
	_bool& Get_InitVisible() { return m_isVisible; }
	_bool& Get_InitInteractable() { return m_isInteract; }
	_bool& Get_InitActivate() { return m_isActive; }

	const Vec2& Get_NoiseUVScale() const { return m_vNoiseUVScale; }
	const Vec2& Get_NoiseUVScroll() const { return m_vNoiseUVScroll; }
	_float Get_Time() const { return m_fTime; }
	_float Get_GlowDistort() const { return m_fGlowDistort; }
	_float Get_GlowPulseSpeed() const { return m_fGlowPulseSpeed; }
	_float Get_GlowIntensity() const { return m_fGlowIntensity; }

	_uint Get_iParmas0() const { return m_iParam0; }
	_uint Get_iTextParmas0() const { return m_iTextParam0; }

	const Vec2& Get_UVScale() const { return m_vUVScale; }
	const Vec2& Get_UVOffset() const { return m_vUVOffset; }


	void Set_Name(const _string& str) { m_strName = str; }
	void Set_RectTransformType(ERectTransform value) { m_eRectTransformType = value; }
	void Set_TextureTag(const _wstring& value) { m_wstrTextureTag = value; }
	void Set_NoiseTextureTag(const _wstring& value) { m_wstrNoiseTextureTag = value; }
	void Set_AlphaMaskTextureTag(const _wstring& value) { m_wstrAlphaMaskTextureTag = value; }
	void Set_GlowTextureTag(const _wstring& value) { m_wstrGlowTextureTag = value; }
	void Set_HitTest() { m_isHitTest = TRUE; };

	void Set_NoiseUVScale(const Vec2& vNoiseUVScale) { m_vNoiseUVScale = vNoiseUVScale; }
	void Set_NoiseUVScroll(const Vec2& vNoiseUVScroll) { m_vNoiseUVScroll = vNoiseUVScroll; }
	void Set_Time(_float fTime) { m_fTime = fTime; }
	void Set_GlowDistort(_float fGlowDistort) { m_fGlowDistort = fGlowDistort; }
	void Set_GlowPulseSpeed(_float fGlowPulseSpeed) { m_fGlowPulseSpeed = fGlowPulseSpeed; }
	void Set_GlowIntensity(_float fGlowIntensity) { m_fGlowIntensity = fGlowIntensity; }

	void Set_iParams0(_uint i) { m_iParam0 = i; }
	void Set_iTextParams0(_uint i) { m_iTextParam0 = i; }

	void Set_UVScale(const Vec2& v) { m_vUVScale = v; }
	void Set_UVOffset(const Vec2& v) { m_vUVOffset = v; }

	Vec4 Get_ColorTint()			const	{ return m_vColorTint; }
	Vec4 Get_GradiantColorTint()	const	{ return m_vGradiantColorTint; }
	Vec4& Get_ColorTint_Ref()				{ return m_vColorTint; }
	_float Get_AlphaRatio()			const	{ return m_fTestAlpha; }
	_float& Get_AlphaRatio_Ref()			{ return m_fTestAlpha; }
	_bool Get_isUseColorTint()		const	{ return m_isUseColorTint; }
	_float Get_ProgressRatio()		const	{ return m_fTestProgress; }
	int32_t Get_FillDir()			const	{ return m_iFillDir; }
	_float Get_Delay()				const	{ return m_fDelay; }
	const _wstring& Get_Text()		const	{ return m_wstrText_TextData; }
	Vec4& Get_FontColor()					{ return m_vFontColor_TextData; }
	_float Get_FontScale() const { return m_fScale_TextData; }
	const _string& Get_FontName() const { return m_strFontName_TextData; }
	_float Get_FontRotate() const { return m_fRotate_TextData; }
	EFontPivotType Get_FontPivotType() const { return m_ePivot_TextData; }
	EFontShaderType Get_FontShaderType() const { return m_eFontShaderType; }

	int32_t Get_Flip()				const	{ return m_iFlip; }
	void Set_ColorTint(Vec4 v)				{ m_vColorTint = v; }
	void Set_GradiantColorTint(Vec4 v)				{ m_vGradiantColorTint = v; }
	void Set_AlphaRatio(_float f)			{ m_fTestAlpha = f; }
	void Set_isUseColorTint(_bool is)		{ m_isUseColorTint = is; }
	void Set_ProgressRatio(_float f)		{ m_fTestProgress = f; }
	void Set_FillDir(int32_t i)				{ m_iFillDir = i; }
	void Set_Delay(_float f)				{ m_fDelay = f; }
	void Set_Text(const _wstring& wstr)		{ m_wstrText_TextData = wstr; }
	void Set_Flip(int32_t i)				{ m_iFlip = i; }
	void Set_FontScale(const _float fScale) { m_fScale_TextData = fScale; }
	void Set_FontName(const _string& strName) { m_strFontName_TextData = strName; }
	void Set_FontRotate(const _float fRotate) { m_fRotate_TextData = fRotate; }
	void Set_FontPivotType(EFontPivotType ePivot) { m_ePivot_TextData = ePivot; }
	void Set_FontShaderType(EFontShaderType e) { m_eFontShaderType = e; }

	_bool Add_Tag(vector<_string>& vec, const _string& str);
	_bool Remove_Tag(vector<_string>& vec, const _string& str);
	_float& Get_TestProgress_Ref() { return m_fTestProgress; }
	int32_t& Get_FillDir_Ref() { return m_iFillDir; }

 	HRESULT Request_Change_Texture();
	HRESULT Request_Change_NoiseTexture();
	HRESULT Request_Change_AlphaMaskTexture();
	HRESULT Request_Change_GlowTexture();

	void Request_Chnage_ShaderPass(uint32_t pass);

	_bool Get_isAction() const { return m_isAction; }
	_bool Get_isDisable() const { return m_isDisable; }

	void  Set_MoveOffset(const Vec3& offset) { m_vMoveOffset = offset; }
	void  Set_Progress(const _float fProgress) { m_fTestProgress = fProgress; }

	uint32_t& Get_ComponentFlag() { return m_iComponentFlag; }
	_bool& Get_UseColorTint() { return m_isUseColorTint; }
	int32_t Get_ShaderPass()const { return m_iShaderPass; }
	void Set_ShaderPass(int32_t pass) { m_iShaderPass = pass; }

	DTO::EUIClassType Get_UIClassType() const { return m_eClassType; }
	DTO::EUISubClassType Get_UISubClassType() const { return m_eSubClassType; }
	DTO::EUITextSubClassType Get_UITextSubClassType() const { return m_eTextSubClassType; }
	DTO::EUIDImageSubClassType Get_UIDImageSubClassType()const { return m_eDImageSubClassType; }
	void Set_UIClassType(DTO::EUIClassType eType) { m_eClassType = eType; }
	void Set_UISubClassType(DTO::EUISubClassType eType) { m_eSubClassType = eType; }
	void Set_UITextSubClassType(DTO::EUITextSubClassType eType) { m_eTextSubClassType = eType; }
	void Set_UIDImageSubClassType(DTO::EUIDImageSubClassType eType) { m_eDImageSubClassType = eType; }


	_uint Get_iHorizontal() const { return m_iHorizontal; }
	_uint Get_iVeriacal() const { return m_iVeriacal; }
	_float Get_fCurFrame() const { return m_fCurFrame; }

	void Set_iHorizontal(_uint iHorizontal) { m_iHorizontal = iHorizontal; }
	void Set_iVeriacal(_uint iVeriacal) { m_iVeriacal = iVeriacal; }
	void Set_fCurFrame(_float fCurFrame) { m_fCurFrame = fCurFrame; }
	_float Get_fFrameSpeed() const { return m_fFrameSpeed; }

	void Set_fFrameSpeed(_float fFrameSpeed) { m_fFrameSpeed = fFrameSpeed; }

	const Vec2& Get_NoiseTexelScale() const { return m_vNoiseTexelScale; }
	const Vec2& Get_NoiseFlow() const { return m_vNoiseFlow; }
	_float Get_NoiseAspect() const { return m_fNoiseAspect; }

	void Set_NoiseTexelScale(const Vec2& vNoiseTexelScale) { m_vNoiseTexelScale = vNoiseTexelScale; }
	void Set_NoiseFlow(const Vec2& vNoiseFlow) { m_vNoiseFlow = vNoiseFlow; }
	void Set_NoiseAspect(const _float fNoiseAspect) { m_fNoiseAspect = fNoiseAspect; }
#pragma endregion

private:
	// Debug
	PrimitiveBatch<DirectX::VertexPositionColor>* m_pBatch	= { nullptr };
	BasicEffect* m_pEffect									= { nullptr };
	ID3D11InputLayout* m_pInputLayout						= { nullptr };

protected:
	// Client Bind Values
	DTO::TUI_GenericUIData m_tUIData	= {};
	DTO::EUIClassType m_eClassType		= {};
	_string m_strName					= {};
	_string m_strCanvasName				= {};
	uint32_t m_iCanvasIndex				= {};
	CToolCanvas* m_pCacheCanvas			= { nullptr };
	ERectTransform m_eRectTransformType = { ERectTransform::C };
	_wstring m_wstrTextureTag			= {};
	_wstring m_wstrNoiseTextureTag		= {};
	_wstring m_wstrGlowTextureTag = {};
	_wstring m_wstrAlphaMaskTextureTag	= {};
	uint32_t m_iComponentFlag			= {};
	_bool m_isUseColorTint				= {};
	Vec4 m_vColorTint					= {};
	Vec4 m_vGradiantColorTint			= {};
	int32_t m_iShaderPass				= {};
	DTO::EUISubClassType m_eSubClassType= {};
	int32_t m_iFillDir					= {};
	_float m_fDelay						= {};
	int32_t m_iFlip						= { ENUM_TO_UINT(EUIFlip::NONE) };

	Vec2	m_vNoiseUVScale				= {};
	Vec2	m_vNoiseUVScroll			= {};
	_float	m_fTime						= {};
	_float	m_fGlowDistort				= {};
	_float	m_fGlowPulseSpeed			= {};
	_float	m_fGlowIntensity			= {};

	Vec2 m_vUVScale = {};
	Vec2 m_vUVOffset = {};

	_uint m_iHorizontal = {1};
	_uint m_iVeriacal = {1};
	_float m_fCurFrame = {};
	_float m_fFrameSpeed = { 1.f };

	Vec2 m_vNoiseTexelScale;
	Vec2 m_vNoiseFlow;
	_float  m_fNoiseAspect;
	
	// Client Bind Values Text Data
	DTO::TUI_TextData m_tUITextData					= {};
	std::wstring m_wstrText_TextData				= {};
	DTO::EUITextSubClassType m_eTextSubClassType	= {};
	EFontShaderType m_eFontShaderType				= { EFontShaderType::NORMAL };
	Vec4 m_vFontColor_TextData						= {};
	_float m_fScale_TextData						= {};
	EFontPivotType m_ePivot_TextData				= { EFontPivotType::CENTER };
	_string m_strFontName_TextData					= {"SemiBold"};
	_float m_fRotate_TextData						= {};
	_uint m_iTextParam0 = {};

	// Client Bind Values Button Trigger Data
	_string m_strKeyMapping = {};
	vector<std::string> m_vecButtonTriggerCanvas;
	vector<std::string> m_vecButtonTriggerUI;

	// Client Bind Values Dynamic Image Data
	DTO::TUI_DImageData m_tDImageData = {};
	DTO::EUIDImageSubClassType m_eDImageSubClassType = {};
	_uint m_iParam0 = {};

	// Local Values
	Vec3 m_vRenderPos		= {};
	RECT m_tRenderRect		= {};
	_bool m_isHitTest		= { FALSE };
	_bool m_isAction		= { false };
	Vec3 m_vMoveOffset		= {};
	_float m_fTestProgress	= { 1.f };
	_bool m_isDisable		= { false };
	_float m_fTestAlpha		= {};
	int32_t m_iIndex		= {};
	_float m_fBrightness	= { 1.f };
	FONT_DESC m_tFontDesc	= {};


public:
	static CToolUI* Create(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END

