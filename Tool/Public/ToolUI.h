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
		uint32_t iInitTextureIndex;
		uint32_t iRectTransformType;
		int32_t iShaderPass;
		_bool isUseColorTint;
		Vec4 vColorTint;
		int32_t iFillDir;
		_float fDelay;
		DTO::EUIOwnerType eOwnerType;
		DTO::TUI_TextData tTextData;
		DTO::TUI_TriggerData tTriggerData;

	}TOOLUI_DESC;

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
	void Stnc_TriggerData();
#pragma region GETTER/SETTER
public:
	const _string& Get_Name()const { return m_strName; }
	ERectTransform Get_RectTransformType() const { return m_eRectTransformType; }
	const _wstring& Get_TextureTag() const { return m_wstrTextureTag; }
	_float* Get_WIdth_Ptr() { return &m_fWidth; }
	_float* Get_Height_Ptr() { return &m_fHeight; }
	_float* Get_PosX_Ptr() { return &m_fX; }
	_float* Get_PosY_Ptr() { return &m_fY; }
	_float* Get_PosZ_Ptr() { return &m_fZ; }
	const DTO::TUI_GenericUIData& Get_Data()const { return m_tUIData; }
	DTO::TUI_GenericUIData& Get_Data_Ref() { return m_tUIData; }
	const DTO::TUI_TextData& Get_TextData()const { return m_tUITextData; }
	const DTO::TUI_TriggerData& Get_TriggerData()const { return m_tUITriggerData; }
	const Vec3& Get_RenderPos() const { return m_vRenderPos; }
	const RECT& Get_RenderRect() const { return m_tRenderRect; }
	_bool& Get_InitVisible() { return m_isVisible; }

	void Set_Name(const _string& str) { m_strName = str; }
	void Set_RectTransformType(ERectTransform value) { m_eRectTransformType = value; }
	void Set_TextureTag(const _wstring& value) { m_wstrTextureTag = value; }
	void Set_HitTest() { m_isHitTest = TRUE; };

	Vec4 Get_ColorTint()			const	{ return m_vColorTint; }
	Vec4& Get_ColorTint_Ref()				{ return m_vColorTint; }
	_float Get_AlphaRatio()			const	{ return m_fTestAlpha; }
	_float& Get_AlphaRatio_Ref()			{ return m_fTestAlpha; }
	_bool Get_isUseColorTint()		const	{ return m_isUseColorTint; }
	_float Get_ProgressRatio()		const	{ return m_fTestProgress; }
	int32_t Get_FillDir()			const	{ return m_iFillDir; }
	_float Get_Delay()				const	{ return m_fDelay; }
	const _wstring& Get_Text()		const	{ return m_wstrText_TextData; }
	Vec4& Get_FontColor()					{ return m_vFontColor_TextData; }

	const vector<_string>& Get_vecHoverEnterTriggerCanvas() const { return m_vecHoverEnterTriggerCanvas; }
	const vector<_string>& Get_vecHoverEnterTriggerUI() const { return m_vecHoverEnterTriggerUI; }
	const vector<_string>& Get_vecHoverExitTriggerCanvas() const { return m_vecHoverExitTriggerCanvas; }
	const vector<_string>& Get_vecHoverExitTriggerUI() const { return m_vecHoverExitTriggerUI; }
	const vector<_string>& Get_vecPressEnterTriggerCanvas() const { return m_vecPressEnterTriggerCanvas; }
	const vector<_string>& Get_vecPressEnterTriggerUI() const { return m_vecPressEnterTriggerUI; }
	const vector<_string>& Get_vecPressExitTriggerCanvas() const { return m_vecPressExitTriggerCanvas; }
	const vector<_string>& Get_vecPressExitTriggerUI() const { return m_vecPressExitTriggerUI; }

	void Set_ColorTint(Vec4 v)				{ m_vColorTint = v; }
	void Set_AlphaRatio(_float f)			{ m_fTestAlpha = f; }
	void Set_isUseColorTint(_bool is)		{ m_isUseColorTint = is; }
	void Set_ProgressRatio(_float f)		{ m_fTestProgress = f; }
	void Set_FillDir(int32_t i)				{ m_iFillDir = i; }
	void Set_Delay(_float f)				{ m_fDelay = f; }
	void Set_Text(const _wstring& wstr)		{ m_wstrText_TextData = wstr; }

	_bool Add_Tag(vector<_string>& vec, const _string& str);
	_bool Remove_Tag(vector<_string>& vec, const _string& str);
	_bool Add_vecHoverEnterTriggerCanvas(const std::string& str) { return Add_Tag(m_vecHoverEnterTriggerCanvas, str); }
	_bool Add_vecHoverEnterTriggerUI(const std::string& str) { return Add_Tag(m_vecHoverEnterTriggerUI, str); }
	_bool Add_vecHoverExitTriggerCanvas(const std::string& str) { return Add_Tag(m_vecHoverExitTriggerCanvas, str); }
	_bool Add_vecHoverExitTriggerUI(const std::string& str) { return Add_Tag(m_vecHoverExitTriggerUI, str); }
	_bool Add_vecPressEnterTriggerCanvas(const std::string& str) { return Add_Tag(m_vecPressEnterTriggerCanvas, str); }
	_bool Add_vecPressEnterTriggerUI(const std::string& str) { return Add_Tag(m_vecPressEnterTriggerUI, str); }
	_bool Add_vecPressExitTriggerCanvas(const std::string& str) { return Add_Tag(m_vecPressExitTriggerCanvas, str); }
	_bool Add_vecPressExitTriggerUI(const std::string& str) { return Add_Tag(m_vecPressExitTriggerUI, str); }

	_bool Remove_vecHoverEnterTriggerCanvas(const std::string& str) { return Remove_Tag(m_vecHoverEnterTriggerCanvas, str); }
	_bool Remove_vecHoverEnterTriggerUI(const std::string& str) { return Remove_Tag(m_vecHoverEnterTriggerUI, str); }
	_bool Remove_vecHoverExitTriggerCanvas(const std::string& str) { return Remove_Tag(m_vecHoverExitTriggerCanvas, str); }
	_bool Remove_vecHoverExitTriggerUI(const std::string& str) { return Remove_Tag(m_vecHoverExitTriggerUI, str); }
	_bool Remove_vecPressEnterTriggerCanvas(const std::string& str) { return Remove_Tag(m_vecPressEnterTriggerCanvas, str); }
	_bool Remove_vecPressEnterTriggerUI(const std::string& str) { return Remove_Tag(m_vecPressEnterTriggerUI, str); }
	_bool Remove_vecPressExitTriggerCanvas(const std::string& str) { return Remove_Tag(m_vecPressExitTriggerCanvas, str); }
	_bool Remove_vecPressExitTriggerUI(const std::string& str) { return Remove_Tag(m_vecPressExitTriggerUI, str); }
	_float& Get_TestProgress_Ref() { return m_fTestProgress; }
	int32_t& Get_FillDir_Ref() { return m_iFillDir; }

 	HRESULT Request_Change_Texture();
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
	DTO::EUIOwnerType Get_UIOwnerType() const { return m_eOwnerType; }
	void Set_UIClassType(DTO::EUIClassType eType) { m_eClassType = eType; }
	void Set_UIOwnerType(DTO::EUIOwnerType eType) { m_eOwnerType = eType; }

#pragma endregion

private:
	// Debug
	PrimitiveBatch<DirectX::VertexPositionColor>* m_pBatch = { nullptr };
	BasicEffect* m_pEffect = { nullptr };
	ID3D11InputLayout* m_pInputLayout = { nullptr };

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
	uint32_t m_iComponentFlag			= {};
	_bool m_isUseColorTint				= {};
	Vec4 m_vColorTint					= {};
	int32_t m_iShaderPass				= {};
	DTO::EUIOwnerType m_eOwnerType		= {};
	int32_t m_iFillDir					= {};
	_float m_fDelay						= {};

	// Client Bind Values Text Data
	DTO::TUI_TextData m_tUITextData		= {};
	std::wstring m_wstrText_TextData	= {};
	Vec4 m_vFontColor_TextData			= {};

	// Client Bind Values Trigger Data
	DTO::TUI_TriggerData m_tUITriggerData = {};
	vector<std::string> m_vecHoverEnterTriggerCanvas;
	vector<std::string> m_vecHoverEnterTriggerUI;
	vector<std::string> m_vecHoverExitTriggerCanvas;
	vector<std::string> m_vecHoverExitTriggerUI;
	vector<std::string> m_vecPressEnterTriggerCanvas;
	vector<std::string> m_vecPressEnterTriggerUI;
	vector<std::string> m_vecPressExitTriggerCanvas;
	vector<std::string> m_vecPressExitTriggerUI;

	// Local Values
	Vec3 m_vRenderPos		= {};
	RECT m_tRenderRect		= {};
	_bool m_isHitTest		= { FALSE };
	_bool m_isAction		= { false };
	Vec3 m_vMoveOffset		= {};
	_float m_fTestProgress	= { 1.f };
	_bool m_isDisable		= { false };
	_float m_fTestAlpha		= {};

public:
	static CToolUI* Create(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END

