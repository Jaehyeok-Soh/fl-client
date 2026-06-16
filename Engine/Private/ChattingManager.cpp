#include "Engine_pch.h"
#include "ChattingManager.h"
#include "GameInstance.h"

#include "RoomChatPacket.h"

CChattingManager::CChattingManager()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);

	m_chattings.reserve(11);
}

CChattingManager::~CChattingManager()
{
}

HRESULT CChattingManager::Initialize()
{
	return S_OK;
}

_bool CChattingManager::Update(_float fTimeDelta)
{
	if (!m_bInputMode)
	{
		if (m_pGameInstance->KeyButton_Down(DIK_RETURN))
		{
			Begin_Input();
			return true;
		}

		return false;
	}

	Update_CursorBlink(fTimeDelta);

	if (m_pGameInstance->KeyButton_Down(DIK_RETURN))
	{
		Submit_Input();
		return true;
	}

	if (m_pGameInstance->KeyButton_Down(DIK_ESCAPE))
	{
		Cancel_Input();
		return true;
	}

	if (m_pGameInstance->KeyButton_Down(DIK_BACK))
	{
		if (!m_wstrInputBuffer.empty())
			m_wstrInputBuffer.pop_back();

		m_fCursorBlinkAcc = 0.f;
		m_bShowCursor = true;

		return true;
	}

	return true;
}

HRESULT CChattingManager::Render()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	const float startX = 250.f;
	const float startY = 40.f;
	const float lineGap = 26.f;

	for (size_t i = 0; i < m_chattings.size(); i++)
	{
		const auto& [userId, message] = m_chattings[i];

		wstring wstrUser = Engine_Utils::ToWString(userId);
		wstring wstrMsg = Engine_Utils::ToWString(message);
		wstring wstrLine = L"[" + wstrUser + L"]" + wstrMsg;

		FONT_DESC desc = {};
		desc.eFontShaderType = EFontShaderType::OUTLINE;
		desc.strFontTag = L"ContentsKO24";
		desc.strText = wstrLine;
		desc.vPosition = Vec2(startX, startY + lineGap * static_cast<float>(i));
		desc.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
		desc.ePivot = EFontPivotType::LT;
		desc.fRotate = 0.f;
		desc.fScale = 0.75f;

		if (FAILED(pGameInstance->Request_DrawFont(desc)))
			return E_FAIL;
	}

	if (m_bInputMode)
	{
		wstring wstrInput = L"> " + m_wstrInputBuffer;

		if (m_bShowCursor)
			wstrInput += L"_";

		FONT_DESC inputDesc = {};
		inputDesc.eFontShaderType = EFontShaderType::OUTLINE;
		inputDesc.strFontTag = L"ContentsKO24";
		inputDesc.strText = wstrInput;
		inputDesc.vPosition = Vec2(30.f, 650.f);
		inputDesc.vColor = Vec4(0.f, 1.f, 0.f, 1.f);
		inputDesc.ePivot = EFontPivotType::LT;
		inputDesc.fRotate = 0.f;
		inputDesc.fScale = 0.85f;

		if (FAILED(pGameInstance->Request_DrawFont(inputDesc)))
			return E_FAIL;
	}

	return S_OK;
}

void CChattingManager::RecvChat(pair<string, string> msg)
{
	m_chattings.push_back(msg);

	if (m_chattings.size() > 10)
		m_chattings.erase(m_chattings.begin());
}

void CChattingManager::On_CharInput(wchar_t ch)
{
	if (!m_bInputMode)
		return;

	if (ch < 0x20)
		return;

	m_wstrInputBuffer.push_back(ch);

	while (Make_SendMessageUtf8().size() > MAX_CHAT_MSG_SIZE)
		m_wstrInputBuffer.pop_back();

	m_fCursorBlinkAcc = 0.f;
	m_bShowCursor = true;
}

void CChattingManager::Begin_Input()
{
	m_bInputMode = true;
	m_wstrInputBuffer.clear();

	m_fCursorBlinkAcc = 0.f;
	m_bShowCursor = true;
}

void CChattingManager::Cancel_Input()
{
	m_bInputMode = false;
	m_wstrInputBuffer.clear();

	m_fCursorBlinkAcc = 0.f;
	m_bShowCursor = true;
}

void CChattingManager::Submit_Input()
{
	string message = Make_SendMessageUtf8();

	if (message.empty())
	{
		Cancel_Input();
		return;
	}

	ROOM_CHAT_REQUEST_PACKET packet = {};
	packet.PacketId = PACKET_ID::ROOM_CHAT_REQUEST;
	packet.PacketLength = sizeof(ROOM_CHAT_REQUEST_PACKET);

	strcpy_s(packet.Message, sizeof(packet.Message), message.c_str());

	m_pGameInstance->SendTCP(reinterpret_cast<char*>(&packet), packet.PacketLength);

	m_bInputMode = false;
	m_wstrInputBuffer.clear();

	m_fCursorBlinkAcc = 0.f;
	m_bShowCursor = true;
}

void CChattingManager::Update_CursorBlink(_float fTimeDelta)
{
	m_fCursorBlinkAcc += fTimeDelta;

	if (m_fCursorBlinkAcc >= 0.5f)
	{
		m_fCursorBlinkAcc = 0.f;
		m_bShowCursor = !m_bShowCursor;
	}
}

string CChattingManager::Make_SendMessageUtf8() const
{
	string utf8 = Engine_Utils::ToString(m_wstrInputBuffer);

	if (utf8.size() <= MAX_CHAT_MSG_SIZE)
		return utf8;

	wstring trimmed = m_wstrInputBuffer;

	while (!trimmed.empty())
	{
		trimmed.pop_back();

		utf8 = Engine_Utils::ToString(trimmed);
		if (utf8.size() <= MAX_CHAT_MSG_SIZE)
			return utf8;
	}

	return "";
}

CChattingManager* CChattingManager::Create()
{
	CChattingManager* pInstance = new CChattingManager();

	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CChattingManager");
	}

	return pInstance;
}

void CChattingManager::Free()
{
	Safe_Release(m_pGameInstance);

	Super::Free();
}
