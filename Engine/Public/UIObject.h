#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CUIObject abstract : public CGameObject
{
	using Super = CGameObject;
public:
	typedef struct tagUIObjectDesc : public Super::GAMEOBJECT_DESC
	{
		_bool bAlpha = { false };
		_float fX = { 0.f };
		_float fY = { 0.f };
		_float fSizeX = { 1.f };
		_float fSizeY = { 1.f };
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
	virtual _bool IsPicked();
	virtual void OffPicked();
	_float Get_SizeX() const { return m_fSizeX; }
	_float Get_SizeY() const { return m_fSizeY; }
	_float Get_PosX() const { return m_fX; }
	_float Get_PosY() const { return m_fY; }
	_float Get_Ratio() const { return m_fRatio; }
	void Set_Size(_float fX, _float fY);
	void Set_Size(_float2 vSize);
	void Set_Position(_float2 vPosition);
	void Set_Position(_float fX, _float fY);
	void Move_Position_Lerp(const _float2 vTargetPosition);
	void Set_Pass(_uint iPass) { m_iPass = iPass; }
	_bool IsVisible() const { return m_bVisible; }
	virtual void Set_Visible() { m_bVisible = true; }
	virtual void Set_Invisible() { m_bVisible = false; }
	_uint Get_DivisionIndex() const { return m_iDivisionIndex; }
	void Set_DivisionIndex(_uint iIndex) { m_iDivisionIndex = iIndex; }

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
protected:
	_bool m_bVisible = { false };
	_bool m_bPicked = { false };
	_uint m_iDivisionIndex = { 0 }; // 여러 UI에 포함되었을때 Index
	_float m_fRatio = { 1.f };
	RECT m_tRect = {};
	RENDER_CATEGORY m_eCategory = { RENDER_CATEGORY::UI };
	_uint m_iViewportWidth = { 0 };
	_uint m_iViewportHeight = { 0 };
	_uint m_iPass = { 0 };
	_float m_fX = { 0.f };
	_float m_fY = { 0.f };
	_float m_fSizeX = { 1.f };
	_float m_fSizeY = { 1.f };
	_uint m_iZIndex = { 0 };
	class CShader* m_pShader = { nullptr };
	class CMesh* m_pMesh = { nullptr };
	class CTexture* m_pTexture = { nullptr };
public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual void Free() override;
};

NS_END