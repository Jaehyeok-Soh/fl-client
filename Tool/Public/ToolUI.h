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
	void Set_ColorTint(Vec4 v)				{ m_vColorTint = v; }
	void Set_AlphaRatio(_float f)			{ m_fTestAlpha = f; }
	void Set_isUseColorTint(_bool is)		{ m_isUseColorTint = is; }
	void Set_ProgressRatio(_float f)		{ m_fTestProgress = f; }
	void Set_FillDir(int32_t i)				{ m_iFillDir = i; }
	void Set_Delay(_float f)				{ m_fDelay = f; }

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

	// Local Values
	Vec3 m_vRenderPos		= {};
	RECT m_tRenderRect		= {};
	_bool m_isHitTest		= { FALSE };
	_bool m_isAction		= { false };
	Vec3 m_vMoveOffset		= {};
	_float m_fTestProgress	= { 1.f };
	_bool m_isDisable		= { false };
	_float m_fTestAlpha = {};

	// Func Values
	_bool m_isPlaying_Fade		= { false };
	_float m_fFade_StartAlpha	= {};
	_float m_fFade_TargetAlpha	= {};
	_float m_fFade_Duration		= {};
	_float m_fFade_TimeAcc		= {};

public:
	static CToolUI* Create(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END

