#pragma once
#include "UIObject.h"

NS_BEGIN(Client)
class CUILayer;
class CGenericUI;

class CCanvas final : public CUIObject
{
	using Super = CUIObject;
public:
	typedef struct tagCanvasDesc : public UIOBJECT_DESC
	{
		_string strName;
	}CANVAS_DESC;
private:
	CCanvas(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CCanvas(const CCanvas& rhs);
	virtual ~CCanvas() = default;
public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Awake(const _uint iCurrentLevelID) override;
	void Transmit_for_UI();
	virtual void Update_Priority(const _float fTimeDelta) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual void Update_Late(const _float fTimeDelta) override;
	virtual void Ready_Before_Render(const _float fTimeDelta) override;
	virtual HRESULT Render() override;

	Vec3 Get_LT() { return Vec3{ m_fX - m_fWidth * 0.5f , m_fY - m_fHeight * 0.5f, m_fZ }; }	// Left Top
	Vec3 Get_CT() { return Vec3{ m_fX,  m_fY - m_fHeight * 0.5f, m_fZ }; }						// Center Top
	Vec3 Get_RT() { return Vec3{ m_fX + m_fWidth * 0.5f , m_fY - m_fHeight * 0.5f, m_fZ }; }	// Right Top
	Vec3 Get_LC() { return Vec3{ m_fX - m_fWidth * 0.5f  , m_fY, m_fZ }; }						// Left Center
	Vec3 Get_C()  { return Vec3{ m_fX , m_fY, m_fZ }; }											// Center
	Vec3 Get_RC() { return Vec3{ m_fX + m_fWidth * 0.5f , m_fY, m_fZ }; }						// Right Center
	Vec3 Get_LB() { return Vec3{ m_fX - m_fWidth * 0.5f , m_fY + m_fHeight * 0.5f, m_fZ }; }	// Left Bottom
	Vec3 Get_CB() { return Vec3{ m_fX, m_fY + m_fHeight * 0.5f, m_fZ }; }						// Center Bottom
	Vec3 Get_RB() { return Vec3{ m_fX + m_fWidth * 0.5f , m_fY + m_fHeight * 0.5f, m_fZ }; }	// Right Bottom

	const _string& Get_Name() { return m_strName; }
	vector<CGenericUI*>* Get_UIVector() { return &m_vecUI; }

	virtual void OnCanvasEvent(ETriggerEventType eEvent, CGenericUI* pSender);
	_bool Check_FinEvent();
private:
	HRESULT Ready_Components(CANVAS_DESC* pDesc);
	HRESULT Bind_ShaderResources();

	void Calc_HitUpdate();
	void Sync_Data();
	CGenericUI* Calc_TopUI();

private:
	vector<CGenericUI*> m_vecUI;
	CGenericUI* m_pCaptureUI = { nullptr };
	CGenericUI* m_pHoveringUI = { nullptr };
	array<CGenericUI*, 2> m_ArrReleasedUI = { nullptr };
	_bool m_isPreUIPressing = { FALSE };
	_bool m_isPreHovering = { FALSE };


public:
	static CCanvas* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGameObject* Clone(void* pArg);
	virtual void Free()override;
};

NS_END