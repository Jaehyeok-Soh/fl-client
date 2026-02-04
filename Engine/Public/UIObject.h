#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CUIObject abstract : public CGameObject
{
	using Super = CGameObject;

public:


	typedef struct tagUIObjectDesc : public Super::GAMEOBJECT_DESC
	{
		_bool isAlpha;
		_bool isInitVisible;
		_float fX;
		_float fY;
		_float fZ;
		_float fWidth;
		_float fHeight;

	}UIOBJECT_DESC;

protected:
	CUIObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CUIObject(const CUIObject& rhs);
	virtual ~CUIObject() = default;

	virtual HRESULT Initialize_Prototype() PURE;
	virtual HRESULT Initialize(void* pArg) PURE;

public:
	virtual HRESULT Awake(const _uint iCurrentLevelID) override;
	virtual void Update_Priority(const _float fTimeDelta) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual void Update_Late(const _float fTimeDelta) override;
	virtual void Ready_Before_Render(const _float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	_float Get_Width() const { return m_fWidth; }
	_float Get_Height() const { return m_fHeight; }
	_float Get_PosX() const { return m_fX; }
	_float Get_PosY() const { return m_fY; }
	_float Get_PosZ() const { return m_fZ; }
	void Set_Size(_float fWidth, _float fHeight);
	void Set_Size(const Vec2 &vSize);
	void Set_Position(const Vec3& vPosition);
	void Set_Position(_float fX, _float fY, _float fZ);
	void Move_Position(_float fX, _float fY, _float fZ);
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

protected:
	void SetUp_Rect();

	/* Action */
public:
	void Set_Pass(_uint iPass) { m_iShaderPass = iPass; }/* 아직 안함 */
	virtual void Set_Visible() { m_isVisible = true; }
	virtual void Set_Invisible() { m_isVisible = false; }

protected:
	_bool m_isVisible = { false };
	RECT m_tRect = {};
	RENDER_CATEGORY m_eCategory = { RENDER_CATEGORY::UI };
	_uint m_iViewportWidth = { 0 };
	_uint m_iViewportHeight = { 0 };
	_uint m_iShaderPass = { 0 };
	_float m_fX = {};
	_float m_fY = {};
	_float m_fZ = {};
	
	_float m_fWidth = {};
	_float m_fHeight = {};
	_float m_fAspect = {};
	
	uint32_t m_iInteractState = {};

public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual void Free() override;
};

NS_END