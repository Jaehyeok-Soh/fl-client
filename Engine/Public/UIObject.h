#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CUIObject abstract : public CGameObject
{
	using Super = CGameObject;

public:
	typedef struct tagUIObjectDesc : public Super::GAMEOBJECT_DESC
	{
		_bool isWorld;
		_bool isAlpha;
		_bool isInitVisible;
		_bool isInitInteract;
		_bool isInitActivate;
		_float fX;
		_float fY;
		_float fZ;
		_float fWidth;
		_float fHeight;
		_float fScale = {1.f};
		_float fRotate;

	}UIOBJECT_DESC;

protected:
	CUIObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CUIObject(const CUIObject& rhs);
	virtual ~CUIObject() = default;

	virtual HRESULT Initialize_Prototype() PURE;
	virtual HRESULT Initialize(void* pArg) PURE;

public:
	virtual HRESULT Awake(const _uint iCurrentLevelID)			override;
	virtual void Update_Priority(const _float fTimeDelta)		override;
	virtual void Update(const _float fTimeDelta)				override;
	virtual void Update_Late(const _float fTimeDelta)			override;
	virtual void Ready_Before_Render(const _float fTimeDelta)	override;
	virtual HRESULT Render() override;

public:
	_float Get_Width() const { return m_fWidth; }
	_float Get_Height() const { return m_fHeight; }
	_float Get_PosX() const { return m_fX; }
	_float Get_PosY() const { return m_fY; }
	_float Get_PosZ() const { return m_fZ; }
	_float Get_Scale() const { return m_fScale; }
	_float Get_Rotate()const { return m_fRotate; }

	void Set_Size(_float fWidth, _float fHeight);
	void Set_Size(const Vec2 &vSize);
	void Move_Size(_float fWidth, _float fHeight);

	void Set_Position(const Vec3& vPosition);
	void Set_Position(_float fX, _float fY, _float fZ);
	void Move_Position(_float fX, _float fY, _float fZ);

	void Set_Scale(const _float fScale);

	void Set_Rotate(const _float fRad);
	void Move_Rotate(const _float fRad);

	_bool IsVisible() const { return m_isVisible; }
	uint32_t Get_InteractState() const { return m_iInteractState; }
	uint32_t& Get_InteractState_Ref() { return m_iInteractState; }
	void Located_Left_In_Viewport();
	void Located_Right_In_Viewport();
	void Located_Top_In_Viewport();
	void Located_Bottom_In_Viewport();
	void Located_LeftTop_In_Viewport();
	void Located_RightTop_In_Viewport();
	void Located_LeftBottom_In_Viewport();
	void Located_RightBottom_In_Viewport();
	void Set_SizeToTextureScale();

	inline virtual HRESULT Spawn_FromPool(void* pArg)override { if (FAILED(Super::Spawn_FromPool(pArg)))return E_FAIL; return S_OK; };
	inline virtual HRESULT Despawn_FromPool()override { if (FAILED(Super::Despawn_FromPool()))return E_FAIL; return S_OK; };

protected:
	void SetUp_Rect();

public:
	void Set_Pass(_uint iPass) { m_iShaderPass = iPass; }

	void Set_Visible()			{ m_isVisibleTrigger = true; }
	void Set_Invisible()		{ m_isVisibleTrigger = false;	}
	void Set_Activate()			{ m_isActive = true;	}
	void Set_InActivate()		{ m_isActive = false;	}
	void Set_Interactable()		{ m_isInteractTrigger = true;	}
	void Set_NonInteractable()	{ m_isInteractTrigger = false; }

	/// <summary>
	/// Update 에서 실행
	/// </summary>
	virtual void Initialize_Visible_Event()			{}
	virtual void Initialize_InVisible_Event()		{}
	virtual void Initialize_Activate_Event()		{}
	virtual void Initialize_InActivate_Event()		{}
	virtual void Initialize_Interactable_Event()	{}
	virtual void Initialize_NonInteractable_Event()	{}


	/// <summary>
	/// Update Late 에서 실행 / 이벤트가 끝나면 True 반환할 것
	/// </summary>
	/// <param name="fTimeDelta"></param>
	/// <returns></returns>
	virtual _bool Tick_Visible_Event(const _float fTimeDelta)			{return true;}
	virtual _bool Tick_InVisible_Event(const _float fTimeDelta)			{return true;}
	virtual _bool Tick_Activate_Event(const _float fTimeDelta)			{return true;}
	virtual _bool Tick_InActivate_Event(const _float fTimeDelta)		{return true;}
	virtual _bool Tick_Interactable_Event(const _float fTimeDelta)		{return true;}
	virtual _bool Tick_NonInteractable_Event(const _float fTimeDelta)	{return true;}
protected:
	/* 렌더 상태를 제어 */
	_bool m_isPreVisible = { false };
	_bool m_isVisible = { false };
	_bool m_isVisibleTrigger = { false };
	_bool m_isPlaying_VisibleEvent = { false };

	/* 업데이트 상태를 제어 */
	_bool m_isPreActive = { false };
	_bool m_isActive = { false };
	_bool m_isPlaying_ActiveEvent = { false };

	/* 입력 상태를 제어 */
	_bool m_isPreInteract = { true };
	_bool m_isInteract = { true };
	_bool m_isInteractTrigger = { true };
	_bool m_isPlaying_InteractEvent = { false };

protected:
	RECT m_tRect = {};
	RENDER_CATEGORY m_eCategory = { RENDER_CATEGORY::BLEND };
	_uint m_iViewportWidth = { 0 };
	_uint m_iViewportHeight = { 0 };
	_uint m_iShaderPass = { 0 };
	_float m_fX = {};
	_float m_fY = {};
	// 렌더 정렬 순서 // 0이 가장 앞에 그려짐
	_float m_fZ = {};
	
	_float m_fWidth = {};
	_float m_fHeight = {};

	_float m_fAspect = {};
	
	_float m_fScale = { 1.f };
	_float m_fRotate = { 0.f };

	uint32_t m_iInteractState = {};

public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual void Free() override;
};

NS_END