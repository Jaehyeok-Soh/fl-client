#include "Engine_pch.h"
#include "Input_Manager.h"

CInput_Manager::CInput_Manager()
{
}

_bool CInput_Manager::KeyButton_Down(_ubyte eKeyID)
{
	if (m_preKeyState[ENUM_TO_CHAR(eKeyID)] != m_byKeyState[ENUM_TO_CHAR(eKeyID)])
	{
		if (m_byKeyState[ENUM_TO_CHAR(eKeyID)] & 0x80)
			return true;
	}

	return false;
}

_bool CInput_Manager::KeyButton_Up(_ubyte eKeyID)
{
	if (m_preKeyState[ENUM_TO_CHAR(eKeyID)] != m_byKeyState[ENUM_TO_CHAR(eKeyID)])
	{
		if (m_preKeyState[ENUM_TO_CHAR(eKeyID)] & 0x80)
			return true;
	}

	return false;
}

_bool CInput_Manager::KeyButton_Pressing(_ubyte eKeyID)
{
	if (m_preKeyState[ENUM_TO_CHAR(eKeyID)] == m_byKeyState[ENUM_TO_CHAR(eKeyID)])
	{
		if (m_preKeyState[ENUM_TO_CHAR(eKeyID)] & 0x80)
			return true;
	}

	return false;
}

bool CInput_Manager::KeyButton_None(_ubyte eKeyID)
{
	if (m_preKeyState[ENUM_TO_CHAR(eKeyID)] == m_byKeyState[ENUM_TO_CHAR(eKeyID)])
	{
		if ((m_preKeyState[ENUM_TO_CHAR(eKeyID)] & 0x80) == 0)
			return true;
	}

	return false;
}

_bool CInput_Manager::Mouse_Down(MOUSEKEYSTATE eMouseKeyID)
{
	if (m_PreMouseState.rgbButtons[ENUM_TO_CHAR(eMouseKeyID)] != m_MouseState.rgbButtons[ENUM_TO_CHAR(eMouseKeyID)])
	{
		if (m_MouseState.rgbButtons[ENUM_TO_CHAR(eMouseKeyID)] & 0x80)
			return true;
	}

	return false;
}

_bool CInput_Manager::Mouse_Up(MOUSEKEYSTATE eMouseKeyID)
{
	if (m_PreMouseState.rgbButtons[ENUM_TO_CHAR(eMouseKeyID)] != m_MouseState.rgbButtons[ENUM_TO_CHAR(eMouseKeyID)])
	{
		if (m_PreMouseState.rgbButtons[ENUM_TO_CHAR(eMouseKeyID)] & 0x80)
			return true;
	}

	return false;
}

_bool CInput_Manager::Mouse_Pressing(MOUSEKEYSTATE eMouseKeyID)
{
	if (m_PreMouseState.rgbButtons[ENUM_TO_CHAR(eMouseKeyID)] == m_MouseState.rgbButtons[ENUM_TO_CHAR(eMouseKeyID)])
	{
		if (m_MouseState.rgbButtons[ENUM_TO_CHAR(eMouseKeyID)] & 0x80)
			return true;
	}

	return false;
}

HRESULT CInput_Manager::Initialize(HINSTANCE hInst, HWND hWnd)
{
	// DInput 컴객체를 생성하는 함수
	if (FAILED(DirectInput8Create(hInst,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&m_pInputSDK,
		NULL)))
		return E_FAIL;

	// 키보드 객체 생성
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyboard, nullptr)))
		return E_FAIL;

	// 생성된 키보드 객체의 대한 정보를 컴 객체에게 전달하는 함수
	m_pKeyboard->SetDataFormat(&c_dfDIKeyboard);

	// 장치에 대한 독점권을 설정해주는 함수, (클라이언트가 떠있는 상태에서 키 입력을 받을지 말지를 결정하는 함수)
	m_pKeyboard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// 장치에 대한 access 버전을 받아오는 함수
	m_pKeyboard->Acquire();

	// 마우스 객체 생성
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
		return E_FAIL;

	// 생성된 마우스 객체의 대한 정보를 컴 객체에게 전달하는 함수
	m_pMouse->SetDataFormat(&c_dfDIMouse);

	// 장치에 대한 독점권을 설정해주는 함수, 클라이언트가 떠있는 상태에서 키 입력을 받을지 말지를 결정하는 함수
	m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// 장치에 대한 access 버전을 받아오는 함수
	m_pMouse->Acquire();

	m_hWnd = hWnd;

	return S_OK;
}

void CInput_Manager::Update(void)
{
	::memcpy(m_preKeyState.data(), m_byKeyState.data(), sizeof(_char) * 256);
	m_pKeyboard->GetDeviceState(256, m_byKeyState.data());
	::memcpy(&m_PreMouseState, &m_MouseState, sizeof(DIMOUSESTATE));
	m_pMouse->GetDeviceState(sizeof(DIMOUSESTATE), &m_MouseState);

	if (::IsWindow(m_hWnd) == false)
		return;

	Apply_CursorMode_IfNeeded();

	if (m_iIgnoreMouseDeltaFrames > 0)
	{
		m_MouseState.lX = 0;
		m_MouseState.lY = 0;
		m_MouseState.lZ = 0;
		--m_iIgnoreMouseDeltaFrames;
	}
	if(m_eAppliedMode != ECursorMode::LockedHiddenCenter && m_eAppliedMode != ECursorMode::Tool)
	{
		m_MouseState.lX = 0;
		m_MouseState.lY = 0;
	}

	::GetCursorPos(&m_MousePos);
	::ScreenToClient(m_hWnd, &m_MousePos);
}

CInput_Manager* CInput_Manager::Create(HINSTANCE hInstance, HWND hWnd)
{
	CInput_Manager* pInstance = new CInput_Manager();

	if (FAILED(pInstance->Initialize(hInstance, hWnd)))
	{
		MSG_BOX("Failed to Created : CInput_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInput_Manager::Clear()
{
	m_preKeyState.fill(0);
	m_byKeyState.fill(0);
}

void CInput_Manager::Force_ReleaseCursor() noexcept
{
	m_eRequstedMode = ECursorMode::VisibleFree;

	if (m_eAppliedMode != ECursorMode::VisibleFree)
	{
		Apply_CursorMode(ECursorMode::VisibleFree);
		m_eAppliedMode = ECursorMode::VisibleFree;
		m_iIgnoreMouseDeltaFrames = 2;
	}
}

void CInput_Manager::Apply_CursorMode_IfNeeded()
{
	const _bool bForcegroundGame = (::GetForegroundWindow() == m_hWnd);
	ECursorMode eFinalWanted = bForcegroundGame ? m_eRequstedMode : ECursorMode::VisibleFree;

	if (eFinalWanted == m_eAppliedMode)
	{
		if (m_eAppliedMode == ECursorMode::LockedHiddenCenter)
			SetCursor_ToCenter();

		return;
	}

	Apply_CursorMode(eFinalWanted);
	m_eAppliedMode = eFinalWanted;

	m_iIgnoreMouseDeltaFrames = 1;

	if (m_eAppliedMode == ECursorMode::LockedHiddenCenter)
		SetCursor_ToCenter();
}

void CInput_Manager::Apply_CursorMode(ECursorMode eMode)
{
	switch (eMode)
	{
	case ECursorMode::LockedHiddenCenter:
	{
		Force_ShowCursor(false);
		Clip_ToClient(true);
	} break;
	case ECursorMode::VisibleClipped:
	{
		Force_ShowCursor(true);
		Clip_ToClient(true);
	} break;
	case ECursorMode::InVisibleClipped:
	{
		Force_ShowCursor(false);
		Clip_ToClient(true);
	}break;
	case ECursorMode::VisibleFree:
	default:
	{
		Force_ShowCursor(true);
		Clip_ToClient(false);
	} break;
	}
}

void CInput_Manager::Force_ShowCursor(_bool bShow)
{
	if (bShow)
	{
		while (::ShowCursor(TRUE) < 0) {}
	}
	else
	{
		while (::ShowCursor(FALSE) >= 0) {}
	}
}

void CInput_Manager::Clip_ToClient(_bool bEnable)
{
	if (bEnable == false)
	{
		::ClipCursor(nullptr);
		return;
	}

	RECT rt{};
	::GetClientRect(m_hWnd, &rt);

	POINT LT{ rt.left, rt.top };
	POINT RB{ rt.right, rt.bottom };
	::ClientToScreen(m_hWnd, &LT);
	::ClientToScreen(m_hWnd, &RB);

	RECT rtClip{ LT.x, LT.y, RB.x, RB.y };
	::ClipCursor(&rtClip);
}

void CInput_Manager::SetCursor_ToCenter()
{
	RECT rt{};
	::GetClientRect(m_hWnd, &rt);

	POINT LT{ rt.left, rt.top };
	POINT RB{ rt.right, rt.bottom };
	::ClientToScreen(m_hWnd, &LT);
	::ClientToScreen(m_hWnd, &RB);

	const _int iX = (LT.x + RB.x) / 2;
	const _int iY = (LT.y + RB.y) / 2;
	::SetCursorPos(iX, iY);
}

void CInput_Manager::Free()
{
	Clear();
	Safe_Release(m_pKeyboard);
	Safe_Release(m_pMouse);
	Safe_Release(m_pInputSDK);
	Super::Free();
}

