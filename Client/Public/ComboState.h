#pragma once
#include "StateBase.h"

NS_BEGIN(Client)

class CComboState abstract : public CStateBase
{
	using Super = CStateBase;
protected:
	enum class EDashFlag : _byte
	{
		FIRST = 1 << 0,
		SECOND = 1 << 1,
		THIRD = 1 << 2,
		FOURTH = 1 << 3,
		FIFTH = 1 << 4
	};
	enum class EDir
	{
		FRONT = 0,
		BACKWARD,
		LEFT,
		RIGHT
	};
public:
	typedef struct tagCombostateDesc : public Super::STATE_DESC
	{
		_bool bLeftMouse = { false };
		_uint iRootState = { 0 /* idle */ };
		_int iDIK_input{ -1 };
		_float fChanceTime_Start = { 0.f };
		_float fChanceTime_End = { 0.f };
	}COMBOSTATE_DESC;
protected:
	CComboState(CActionState* pOwnerComponent, const string& strName);
	virtual ~CComboState() = default;

	virtual HRESULT Initialize(void* pArg) override; 
public:
	virtual HRESULT Awake(const _uint iLevelIndex) override;
	virtual HRESULT Start(void* pArg, _bool bForce = false) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual HRESULT End() override;
public:
	_uint Get_AnimIndex() const { return m_vecMainAnims[m_iMainAnimIdx]; }
	_int Get_RootState() const { return m_iRootState; }
	_int Get_ComboIndex() const { return m_iComboIndex; }
	void Set_ComboIndex(_uint iValue) { m_iComboIndex = iValue; }
	_int Get_DIK() const { return m_iDIK_input; }
	_bool Is_LeftMouse() const { return m_bLeftMouse; }
	_bool Is_ChanceTime();
	_bool Has_Dashed(EDashFlag eFlag) { return static_cast<_byte>(eFlag) & m_iDashMask; }
	_bool Can_Dash(EDashFlag eFlag) { return !Has_Dashed(eFlag); }
	void StartForce_ForAnimation(EDashFlag eFlag, EDir eMove = EDir::FRONT, _float fForceAbs = 0.f, _float fDragK = 0.f);
	void Apply_AttackDesc(_int iNewWindow, _int iNewPart, ATTACK_DESC* pDesc);
protected:
	_bool m_bLeftMouse = { false };
	_int m_iActiveHitWindow = { -1 };
	_int m_iActivePart = { -1 };
	_byte m_iDashMask = { 0b0000'0000 };
	_int m_iComboIndex = { -1 };
	_uint m_iRootState = { 0 /* static */};
	_int m_iDIK_input = { 0 };
	_float m_fChanceTime_Start = { 0.f };
	_float m_fChanceTime_End = { 0.f };
public:
	virtual void Free() override;
};

NS_END