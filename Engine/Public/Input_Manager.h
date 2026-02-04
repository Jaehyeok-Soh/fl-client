#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CInput_Manager final : public CBase
{
	using Super = CBase;
private:
	CInput_Manager();
	virtual ~CInput_Manager() = default;

public:
	// 현재 마우스의 특정 축 좌표를 반환
	_long	Get_DIMouseMove(MOUSEMOVESTATE eMouseState)
	{
		return *(((_long*)&m_MouseState) + ENUM_TO_UINT(eMouseState));
	}

	_bool KeyButton_Down(_ubyte eKeyID);
	_bool KeyButton_Up(_ubyte eKeyID);
	_bool KeyButton_Pressing(_ubyte eKeyID);
	_bool KeyButton_None(_ubyte eKeyID);

	_bool Mouse_Down(MOUSEKEYSTATE eMouseKeyID);
	_bool Mouse_Up(MOUSEKEYSTATE eMouseKeyID);
	_bool Mouse_Pressing(MOUSEKEYSTATE eMouseKeyID);

	const POINT& Get_MousePos() const { return m_MousePos; }

public:

	HRESULT Initialize(HINSTANCE hInst, HWND hWnd);
	void	Update();
	void	Clear();
	_bool	ShouldIgnoreMouseDelta() noexcept { return m_iIgnoreMouseDeltaFrames >= 1; }
	void	Request_CursorMode(ECursorMode eMode) noexcept { m_eRequstedMode = eMode; }
	void	Force_ReleaseCursor() noexcept;
private:
	void	Apply_CursorMode_IfNeeded();
	void	Apply_CursorMode(ECursorMode eMode);
	void	Force_ShowCursor(_bool bShow);
	void	Clip_ToClient(_bool bEnable);
	void	SetCursor_ToCenter();
private:
	ECursorMode				m_eRequstedMode = ECursorMode::END;
	ECursorMode				m_eAppliedMode = ECursorMode::END;
	_int					m_iIgnoreMouseDeltaFrames = 0;
	HWND					m_hWnd = { NULL };
	LPDIRECTINPUT8			m_pInputSDK = { nullptr };
	LPDIRECTINPUTDEVICE8	m_pKeyboard = { nullptr };
	LPDIRECTINPUTDEVICE8	m_pMouse = { nullptr };

private:
	array<_char, 256>	m_preKeyState = { 0 };
	array<_char, 256>	m_byKeyState = { 0 };
	DIMOUSESTATE		m_PreMouseState = {};
	DIMOUSESTATE		m_MouseState = {};
	POINT				m_MousePos = {};
public:
	static CInput_Manager* Create(HINSTANCE hInstance, HWND hWnd);
	virtual void	Free();
};

NS_END


