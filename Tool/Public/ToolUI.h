#pragma once
#include "UIObject.h"
#include "UIAction_Registry.h"


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
		_string strLayerName;
		uint32_t iLayerIndex;

		_string strInitTextureTag;
		uint32_t iInitTextureIndex;
		uint32_t iRectTransformType;

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
	void Set_Name(const _string& str) { m_strName = str; }
	ERectTransform Get_RectTransformType() const { return m_eRectTransformType; }
	void Set_RectTransformType(ERectTransform value) { m_eRectTransformType = value; }
	const _wstring& Get_TextureTag() const { return m_wstrTextureTag; }
	void Set_TextureTag(const _wstring& value) { m_wstrTextureTag = value; }
	uint32_t Get_TextureIndex() const { return m_iTextureIndex; }
	_float* Get_WIdth_Ptr() { return &m_fWidth; }
	_float* Get_Height_Ptr() { return &m_fHeight; }
	_float* Get_PosX_Ptr() { return &m_fX; }
	_float* Get_PosY_Ptr() { return &m_fY; }
	_float* Get_PosZ_Ptr() { return &m_fZ; }
	const Vec3& Get_RenderPos() const { return m_vRenderPos; }
	const RECT& Get_RenderRect() const { return m_tRenderRect; }
	void Set_HitTest() { m_isHitTest = TRUE; };
	const DTO::TUI_GenericUIData& Get_Data()const { return m_tUIData; }
	DTO::TUI_GenericUIData& Get_Data_Ref() { return m_tUIData; }

	_float& Get_Alpha_Ref() { return m_fFade_ResultAlpha; }
	_bool& Get_InitVisible() { return m_isVisible; }

	_float& Get_TestProgress_Ref() { return m_fTestProgress; }
	int32_t& Get_FillDir_Ref() { return m_iFillDir; }

 	HRESULT Request_Change_Texture();
	void Request_Chnage_ShaderPass(uint32_t pass);

	_bool Get_isAction() const { return m_isAction; }
	_bool Get_isDisable() const { return m_isDisable; }

	void  Set_MoveOffset(const Vec3& offset) { m_vMoveOffset = offset; }
	void  Set_Progress(const _float fProgress) { m_fTestProgress = fProgress; }

	uint32_t& Get_ComponentFlag() { return m_iComponentFlag; }

	DTO::EUIClassType Get_UIClassType() const { return m_eClassType; }
	DTO::EUIOwnerType Get_UIOwnerType() const { return m_eOwnerType; }
	void Set_UIClassType(DTO::EUIClassType eType) { m_eClassType = eType; }
	void Set_UIOwnerType(DTO::EUIOwnerType eType) { m_eOwnerType = eType; }

#pragma endregion

	/* Action */
public:
	void Set_TextureIndex(uint32_t index) { m_iTextureIndex = index; }
	void Set_isDisable(_bool isDisable);/* 아직 바인드 안함 */

protected:
	uint32_t m_iTextureIndex = {};

	Vec3 m_vMoveOffset = {};
	_float m_fFade_ResultAlpha = {1.f};

	_float m_fTestProgress = { 1.f };
	int32_t m_iFillDir = {};


	/* Set_isDisable */
	_bool m_isDisable = { false };
	/* Set_isDisable */

	/* Start_Fade */
	_bool m_isPlaying_Fade = { false };
	_float m_fFade_StartAlpha = {};
	_float m_fFade_TargetAlpha = {};
	_float m_fFade_Duration = {};
	_float m_fFade_TimeAcc = {};
	/* Start_Fade */

private:	
	PrimitiveBatch<DirectX::VertexPositionColor>* m_pBatch = { nullptr };
	BasicEffect* m_pEffect = { nullptr };
	ID3D11InputLayout* m_pInputLayout = { nullptr };

protected:
	DTO::TUI_GenericUIData m_tUIData = {};
	DTO::EUIClassType m_eClassType = {};

	_string m_strName = {};
	_string m_strCanvasName = {};
	uint32_t m_iCanvasIndex = {};
	_string m_strLayerName = {};
	uint32_t m_iLayerIndex = {};
	CToolCanvas* m_pCacheCanvas = { nullptr };
	ERectTransform m_eRectTransformType = { ERectTransform::C };
	_wstring m_wstrTextureTag = {};
	uint32_t m_iComponentFlag = {};

	DTO::EUIOwnerType m_eOwnerType = {};
	Vec3 m_vRenderPos = {};
	RECT m_tRenderRect = {};
	_bool m_isHitTest = { FALSE };
	_bool m_isAction = { false };

public:
	static CToolUI* Create(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END

