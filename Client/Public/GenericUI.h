#pragma once
#include "UIObject.h"
#include "DataStruct_UI.h"

NS_BEGIN(Client)
class CCanvas;
class CUI_Manager;
class CGenericUI abstract : public CUIObject
{
	using Super = CUIObject;
public:
	typedef struct tagGenericUIDesc : public UIOBJECT_DESC
	{
		_string strName;
		uint32_t iUIType;
		uint32_t iRectTransformType;
		_wstring wstrTextureTag;
		uint32_t iTextureIndex;
		uint32_t iComponentFlag;
		_bool isUseColorTint;
		Vec4 vColorTint;
		int32_t iShaderPass;
		int32_t iFillDir;
		_float fDelay;
		_float fAlpha;
		int32_t iFlip;
		CCanvas* pCanvasCache = { nullptr };
	}GENERIC_UI_DESC;

	enum class ETriggerEventType { HOVER_ENTER, HOVER_EXIT, PRESS_ENTER, PRESS_EXIT, END };

protected :
	CGenericUI(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGenericUI(const CGenericUI& rhs);
	virtual ~CGenericUI() = default;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* pArg) override;

public:
	virtual HRESULT Awake(const _uint iCurrentLevelID) override;
	virtual void Update_Priority(const _float fTimeDelta) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual void Update_Late(const _float fTimeDelta) override;
	virtual void Ready_Before_Render(const _float fTimeDelta) override;
	virtual HRESULT Render() override;

	_bool Calc_HitEvent();
	void Acting_By_InteractState();

	virtual void OnUIEvent(ETriggerEventType eEvent, CGenericUI* pSender);

protected:
	HRESULT Ready_Components(GENERIC_UI_DESC* pDesc);
	HRESULT Bind_ShaderResources();

public:
	void Set_RectPos(const Vec3& pos) { m_vRectPos = pos; }
	ERectTransform Get_RectTransformType() const { return m_eRectTransformType; }
	void Set_TextureIndex(_uint index) { m_iTextureIndex = index; }
	const _string& Get_Tag() { return m_strName; }

protected:
	CUI_Manager* m_pUIManager = { nullptr };	
	uint32_t m_iLevelID = {};

protected:
	ERectTransform m_eRectTransformType = { ERectTransform::C };
	_wstring m_wstrTextureTag			= {};
	uint32_t m_iTextureIndex			= {};
	Vec3 m_vRectPos						= {};
	Vec3 m_vRenderPos					= {};
	RECT m_tRenderRect					= {};
	CCanvas* m_pParentCanvasCache		= { nullptr };
	Vec3 m_vMoveOffset					= {};
	uint32_t m_iComponentFlag			= {};
	uint32_t m_iOwnerType				= {};

	// Shader Bind Values
	_bool m_isUseColorTint				= {false};
	Vec4 m_vColorTint					= {};
	_float m_fAlpha_Ratio				= {};
	_float m_fProgress_Ratio			= {1.f};
	int32_t m_iFillDir					= {};
	_float m_fDelay						= {};
	int32_t m_iFlip						= { ENUM_TO_UINT(EUIFlip::NONE) };

public:
	virtual void Free()override;
};

NS_END