#pragma once
#include "UIObject.h"
#include "DataStruct_UI.h"
#include "UIAction_Registry.h"

NS_BEGIN(Engine)
class IUIActionForMe;
class IUIActionForTarget;
NS_END

NS_BEGIN(Client)
class CCanvas;
class CUILayer;
class CGenericUI final : public CUIObject
{
	using Super = CUIObject;

public:
	typedef struct tagGenericUIDesc : public UIOBJECT_DESC
	{
		uint32_t iUIType;
		uint32_t iRectTransformType;
		_wstring wstrTextureTag;
		uint32_t iTextureIndex;

		CCanvas* pCanvasCache = { nullptr };
		CUILayer* pLayerCache = { nullptr };
	}GENERIC_UI_DESC;

	typedef struct tagScheduleDesc
	{
		_float fRemain = {};
		std::function<void()> Func;
	}SCHEDULE_DESC;

private:
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

public:
	HRESULT Bind_Action(const DTO::TUI_EventBindData& data);
	HRESULT Bind_Action(DTO::EUIEvent EventType, DTO::EUIAction ActType, const json& params);
	HRESULT Remove_Action(DTO::EUIEvent EventType, DTO::EUIAction ActType);
	IUIActionForMe* Get_ActionForMe() const { return m_pActionForMe; }
	HRESULT Excute_Action(DTO::EUIEvent EventType);
	HRESULT Excute_Specific_Action(DTO::EUIEvent EventType, DTO::EUIAction eAction);
	HRESULT ReBind_Action();

	void Delay_Queue(const _float fTimeDelta);
	void Push_DelayAction(const _float fDelay, std::function<void()>&& Func);

private:
	HRESULT Ready_Components(GENERIC_UI_DESC* pDesc);
	HRESULT Bind_ShaderResources();

	/* Action */
public:
	void Set_RectPos(const Vec3& pos) { m_vRectPos = pos; }
	ERectTransform Get_RectTransformType() const { return m_eRectTransformType; }
	void Set_TextureIndex(_uint index) { m_iTextureIndex = index; }
	const _string& Get_Tag() { return m_strName; }

	void Start_Lerp_Movement(const Vec3& vTargetPos, const _float fTargetAlpha, const _float& fDuration, _bool isPin);
	void Start_Return_Lerp_Movement();
	void Lerp_Movement(const _float fTimeDelta);
	void Return_Lerp_Movement(const _float fTimeDelta);
	void Start_Fade(const _float fStartAlpha, const _float fTargetAlpha, const _float fDuration);
	void Fade(const _float fTimeDelta);

	/* Action Variable */
private:
	/*Start_Lerp_Movement*/
	_bool m_isPlaying_Lerp_Movement = { false };
	Vec3 m_vLerpMovement_StartPos = {};
	Vec3 m_vLerpMovement_TargetPos = {};
	_float m_fLerpMovement_TargetAlpha = {};
	_float m_fLerpMovement_Duration = {};
	_float m_fLerpMovement_TimeAcc = {};
	_bool m_isLerpMovement_Pin = {};
	_bool m_isMoved = { false };
	Vec3 m_vMoveOffset = {};
	Vec3 m_vLerpMovement_StartOffset;
	Vec3 m_vLerpMovement_TargetOffset;
	/*Start_Lerp_Movement*/

	/* Start_Return_Lerp_Movement */
	_bool m_isPlaying_Return_Lerp_Movement = { false };
	/* Start_Return_Lerp_Movement */

	/* Set_isDisable */
	_bool m_isDisable = { false };
	/* Set_isDisable */

	/* Start_Fade */
	_bool m_isPlaying_Fade = { false };
	_float m_fFade_StartAlpha = {};
	_float m_fFade_TargetAlpha = {};
	_float m_fFade_ResultAlpha = { 1.f };
	_float m_fFade_Duration = {};
	_float m_fFade_TimeAcc = {};
	/* Start_Fade */

private:
	ERectTransform m_eRectTransformType = { ERectTransform::C };
	_wstring m_wstrTextureTag = {};
	uint32_t m_iTextureIndex = {};
	Vec3 m_vRectPos = {};
	Vec3 m_vRenderPos = {};
	RECT m_tRenderRect = {};
	CCanvas* m_pParentCanvasCache = { nullptr };
	CUILayer* m_pParentLayerCache = { nullptr };

	IUIActionForMe* m_pActionForMe = { nullptr };
	IUIActionForTarget* m_pActionForTarget = { nullptr };

	_bool m_isAction = { false };

	/* 액션들을 이벤트 갯수만큼 정적으로 할당 사실상 vector<ActionFunc>[] 이거임 */
	array< vector<CUIAction_Registry::ActionFunc>, ENUM_TO_UINT(DTO::EUIEvent::END)> m_vecBindingActions;
	array< vector<DTO::TUI_EventBindData>, ENUM_TO_UINT(DTO::EUIEvent::END)> m_vecBindingActionData;

	vector<SCHEDULE_DESC> m_vecActionQueue;

public:
	static CGenericUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGameObject* Clone(void* pArg);
	virtual void Free()override;

};

NS_END