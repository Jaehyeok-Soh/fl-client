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
		_wstring wstrGlowTextureTag;
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

public:
	_bool Calc_HitEvent();

protected:
	HRESULT Ready_Components(GENERIC_UI_DESC* pDesc);
	HRESULT Bind_ShaderResources();

	virtual HRESULT Attach_Personal_Info() { return S_OK; }
	virtual void Tick_By_Type(const _float fTimedelta) {}
	virtual void Trigger_By_InteractState(){}

	// 끝에서 천천히 isEaseOut = true / 끝에서 빠르게 isEaseOut = false
	void Ready_Lerp_Movement(const Vec2& vStartOffset, const Vec2& vTargetOffset, const _float fDuration, const _float fEaseValue, const _float fDelay, _bool isEaseOut = false);
	void Ready_Fade(const _float fDuration, const _float fStartAlpha, const _float fTargetAlpha, const _float fDelay);
	void Ready_LerpChange(const _float fDuration, const _float fStartAlpha, const _float fTargetAlpha, const _float fEaseValue, const _float fDelay);
	_bool Tick_Lerp_Movement(const _float fTimeDelta);
	_bool Tick_Fade(const _float fTimeDelta);
	_bool Tick_LerpChange(_float* p, const _float fTimeDelta);
	void Request_SetDead();
	virtual void Bind_Events() {};

public:
	void Set_RectPos(const Vec3& pos) { m_vRectPos = pos; }
	ERectTransform Get_RectTransformType() const { return m_eRectTransformType; }
	const _string& Get_Tag() { return m_strName; }
	_bool Get_FinEvent()const { return m_isFin_Event; }
	_bool Get_DeadRequest()const { return m_isDeadRequest; }

	inline virtual HRESULT Spawn_FromPool(void* pArg)override { if (FAILED(Super::Spawn_FromPool(pArg)))return E_FAIL; return S_OK; };
	inline virtual HRESULT Despawn_FromPool()override { if (FAILED(Super::Despawn_FromPool()))return E_FAIL; return S_OK; };;

protected:
	CUI_Manager* m_pUIManager = { nullptr };
	uint32_t m_iLevelID = {};
	class CWorldUI_Component* m_pWorldUIComp = { nullptr };

protected:
	ERectTransform m_eRectTransformType = { ERectTransform::C };
	array<vector<_wstring>, ENUM_TO_UINT(EUITextureSlot::END)> m_ArrTextures = {};
	
	Vec3 m_vRectPos						= {};
	Vec3 m_vRenderPos					= {};
	RECT m_tRenderRect					= {};
	CCanvas* m_pParentCanvasCache		= { nullptr };
	Vec2 m_vMoveOffset					= {};
	uint32_t m_iComponentFlag			= {};
	uint32_t m_iOwnerType				= {};
	_bool m_isFin_Event					= { true };
	_bool m_isDeadRequest				= { false };

	vector<DelegateHandle> m_vecEventHandles;

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

private:
	_float m_fTimeAcc = {};

	// Lerp Movement Values
	Vec2 m_vMoveOffsetBase						= {};

	Vec2	m_vLerpMove_StartOffset				= {};
	Vec2	m_vLerpMove_TargetOffset			= {};
	_float	m_fLerpMove_Duration				= {};
	_float	m_fLerpMove_EaseValue				= {};
	_float	m_fLerpMove_TimeAcc					= {};
	_float	m_fLerpMove_DelayTimeAcc			= {};
	_float	m_fLerpMove_Delay					= {};
	_bool m_isLerpMove_EaseOut					= { false };
	// Fade Values
	_float m_fFade_Delay						= {};
	_float m_fFade_DelayTimeAcc					= {};
	_float m_fFade_Duration						= {};
	_float m_fFade_TimeAcc						= {};
	_float m_fFade_StartAlphaRatio				= {};
	_float m_fFade_TargetAlphaRatio				= {};
	_float	m_fFade_EaseValue					= {};

	// Lerp Change Values
	_float m_fLerpChange_TimeAcc				= {};
	_float m_fLerpChange_DelayTimeAcc			= {};
	_float m_fLerpChange_Delay					= {};
	_float m_fLerpChange_Duration				= {};
	_float m_fLerpChange_StartValue				= {};
	_float m_fLerpChange_TargetValue			= {};
	_float m_fLerpChange_EaseValue				= {};

public:
	virtual void Free()override;
};

NS_END