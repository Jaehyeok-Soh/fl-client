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
		_wstring wstrNoiseTextureTag;
		_wstring wstrAlphaMaskTextureTag;
		uint32_t iTextureIndex;
		uint32_t iComponentFlag;
		_bool isUseColorTint;
		Vec4 vColorTint;
		Vec4 vGradiantColorTint;
		int32_t iShaderPass;
		int32_t iFillDir;
		_float fDelay;
		_float fAlpha;
		int32_t iFlip;
		CCanvas* pCanvasCache = { nullptr };
		CGameObject* pTarget = { nullptr };
	}GENERIC_UI_DESC;

	enum EUITextureSlot { DEFAULT, NOISE, ALPHA_MASK };

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
	virtual void Acting_By_InteractState();
	virtual void OnUIEvent(ETriggerEventType eEvent, CGenericUI* pSender);

protected:
	HRESULT Ready_Components(GENERIC_UI_DESC* pDesc);
	HRESULT Bind_ShaderResources();
	virtual HRESULT Spawn_FromPool(void* pArg)override;

public:
	void Set_RectPos(const Vec3& pos) { m_vRectPos = pos; }
	ERectTransform Get_RectTransformType() const { return m_eRectTransformType; }
	void Set_TextureIndex(_uint index) { m_iTextureIndex = index; }
	const _string& Get_Tag() { return m_strName; }
	_bool Get_FinEvent()const { return m_isFin_Event; }

	void Ready_Lerp_Movement(const Vec2& vStartOffset, const Vec2& vTargetOffset, const _float fDuration, const _float fEaseValue, const _float fDelay);
	_bool Tick_Lerp_Movement(const _float fTimeDelta);
	
	void Ready_Fade(const _float fDuration, const _float fStartAlpha, const _float fTargetAlpha, const _float fDelay);
	void Ready_ExplosionFade(const _float fDuration, const _float fStartAlpha,const _float fExplosionAlpha, const _float fTargetAlpha, const _float fDelay);
	_bool Tick_Fade(const _float fTimeDelta);

	void Ready_LerpChange(const _float fDuration, const _float fStartAlpha, const _float fTargetAlpha, const _float fEaseValue, const _float fDelay);
	_bool Tick_LerpChange(_float* p, const _float fTimeDelta);

	// 해야될 이벤트가 끝나면 Request SetDead 호출 -> 나중에 캔버스에서 일괄적으로 SetDead를 해줌
	void Request_SetDead();

protected:
	CUI_Manager* m_pUIManager = { nullptr };	
	uint32_t m_iLevelID = {};
	class CWorldUI_Component* m_pWorldUIComp = { nullptr };

protected:
	ERectTransform m_eRectTransformType = { ERectTransform::C };
	_wstring m_wstrTextureTag			= {};
	_wstring m_wstrNoiseTextureTag		= {};
	_wstring m_wstrAlphaMaskTextureTag	= {};
	uint32_t m_iTextureIndex			= {};
	Vec3 m_vRectPos						= {};
	Vec3 m_vRenderPos					= {};
	RECT m_tRenderRect					= {};
	CCanvas* m_pParentCanvasCache		= { nullptr };
	Vec2 m_vMoveOffset					= {};
	uint32_t m_iComponentFlag			= {};
	uint32_t m_iOwnerType				= {};
	_bool m_isFin_Event					= { true };

	// Shader Bind Values
	_bool m_isUseColorTint				= { false };
	Vec4 m_vColorTint					= {};
	Vec4 m_vGradiantColorTint			= {};
	_float m_fAlpha_Ratio				= {};
	_float m_fProgress_Ratio			= {1.f};
	int32_t m_iFillDir					= {};
	_float m_fDelay						= {};
	int32_t m_iFlip						= { ENUM_TO_UINT(EUIFlip::NONE) };

	_float m_fBrightness				= {};

	// Lerp Movement Valuse
	Vec2 m_vMoveOffsetBase	= {};
	Vec2 m_vStartOffset		= {};
	Vec2 m_vTargetOffset	= {};
	_float m_fDuration		= {};
	_float m_fEaseValue		= {};
	_float m_fTimeAcc		= {};
	_float m_fDelayTimeAcc	= {};
	_float m_fLerpDelay		= {};

	// Fade 
	_float m_fFadeDelay = {};
	_float m_fFadeDelayTimeAcc = {};
	_float m_fFadeDuration = {};
	_float m_fFadeTimeAcc = {};
	_float m_fStartAlphaRatio = {};
	_float m_fTargetAlphaRatio = {};
	_float m_fExplosionAlphaRatio = {};

public:
	virtual void Free()override;
};

NS_END