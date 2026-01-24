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
	
	::GetCursorPos(&m_MousePos);
	::ScreenToClient(m_hWnd, &m_MousePos);

	if (m_bCaptrue)
	{
		POINT center{};
		RECT rc{};
		::GetClientRect(m_hWnd, &rc);
		center.x = (rc.right - rc.left) / 2;
		center.y = (rc.bottom - rc.top) / 2;
		::ClientToScreen(m_hWnd, &center);
		::SetCursorPos(center.x, center.y);
	}	
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

void CInput_Manager::Free()
{
	Clear();
	Safe_Release(m_pKeyboard);
	Safe_Release(m_pMouse);
	Safe_Release(m_pInputSDK);
	Super::Free();
}

