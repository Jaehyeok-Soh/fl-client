#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CChattingManager :
    public CBase
{
    using Super = CBase;

private:
    CChattingManager();
    virtual ~CChattingManager();

    HRESULT Initialize();

public:
    _bool Update(_float fTimeDelta);
    HRESULT Render();

    void RecvChat(pair<string, string> msg);

    void On_CharInput(wchar_t ch);
    _bool Is_InputMode() const { return m_bInputMode; }

private:
    void Begin_Input();
    void Cancel_Input();
    void Submit_Input();

    void Update_CursorBlink(_float fTimeDelta);

    string Make_SendMessageUtf8() const;

private:
    class CGameInstance* m_pGameInstance = { nullptr };

    vector<pair<string, string>> m_chattings;

    _bool m_bInputMode = { false };
    wstring m_wstrInputBuffer = L"";

    _float m_fCursorBlinkAcc = 0.f;
    _bool m_bShowCursor = true;

    static constexpr size_t MAX_INPUT_LENGTH = MAX_CHAT_MSG_SIZE;

public:
    static CChattingManager* Create();
    virtual void Free() override;
};

NS_END