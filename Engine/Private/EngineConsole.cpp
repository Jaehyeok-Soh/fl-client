#include "Engine_pch.h"
#include "EngineConsole.h"
#include "Engine_Utils.h"


void CEngineConsole::Initialize(_bool bEnable)
{
    std::lock_guard<std::mutex> lock(s_Mutex);
    s_bEnabled = bEnable;

    if(s_bInitialized)
        return;

    ::AllocConsole();
    if (::GetConsoleWindow() != nullptr)
    {
        FILE* fp;                          
        ::freopen_s(&fp, "CONOUT$", "w", stdout);   // 표준 출력(stdout)을 콘솔로 리디렉션
        ::freopen_s(&fp, "CONOUT$", "w", stderr);   // 표준 에러(stderr)도 콘솔로 리디렉션
        ::freopen_s(&fp, "CONIN$", "r", stdin);     // 표준 입력(stdin)도 콘솔로 리디렉션

        ::SetConsoleOutputCP(CP_UTF8);
        ::SetConsoleCP(CP_UTF8);

        s_hOut = ::GetStdHandle(STD_OUTPUT_HANDLE);
    }

    s_bInitialized = true;
}

void CEngineConsole::Shutdown()
{
    std::lock_guard<std::mutex> lock(s_Mutex);
    if (s_bInitialized == false)
        return;

    if (::GetConsoleWindow() != nullptr)
        ::FreeConsole();

    s_hOut = nullptr;
    s_bInitialized = false;
    s_bEnabled = false;
}

_bool CEngineConsole::Is_Enabled()
{
    std::lock_guard<std::mutex> lock(s_Mutex);
    return s_bEnabled;
}

void CEngineConsole::Set_Enabled(_bool bEnable)
{
    std::lock_guard<std::mutex> lock(s_Mutex);
    s_bEnabled = bEnable;
}

void CEngineConsole::Log(ELogLevel eLevel, std::string_view strMsg)
{
    Log(eLevel, Engine_Utils::ToWString(string(strMsg)));
}
void CEngineConsole::Log(ELogLevel eLevel, std::wstring_view wstrMsg)
{
    std::lock_guard<std::mutex> lock(s_Mutex);

    if (s_bEnabled == false)
        return;

    std::wstring wstrLine;
    wstrLine.reserve(wstrMsg.size() + 64);
    wstrLine += Get_Timestamp();
    wstrLine += L" ";
    wstrLine += LogLevel_To_Wstring(eLevel);
    wstrLine += L" ";
    wstrLine += wstrMsg;
    wstrLine += L"\n";

    Write_To_Console(wstrLine);
}

void CEngineConsole::Write_To_Console(std::wstring_view line)
{
    if (!s_hOut)
        return;

    DWORD written = 0;
    ::WriteConsoleW(s_hOut, line.data(), (DWORD)line.size(), &written, nullptr);
}

void CEngineConsole::Write_To_DebugOutput(std::wstring_view line)
{
    ::OutputDebugStringW(std::wstring(line).c_str());
}

void CEngineConsole::Set_Title(std::wstring_view wstrTitle)
{
    std::lock_guard<std::mutex> lock(s_Mutex);
    if (s_bInitialized == false)
        return;
    ::SetConsoleTitleW(std::wstring(wstrTitle).c_str());
}

wstring CEngineConsole::LogLevel_To_Wstring(ELogLevel eLevel)
{
    switch (eLevel)
    {
    case Engine::ELogLevel::Trace:
        return L"[ TRACE ]";
    case Engine::ELogLevel::Info:
        return L"[ INFO ]";
    case Engine::ELogLevel::Warn:
        return L"[ WARN ]";
    case Engine::ELogLevel::Error:
        return L"[ ERROR ]";
    default:
        return L"[ ????? ]";
    }
}

wstring CEngineConsole::Get_Timestamp()
{
    auto now = std::chrono::system_clock::now();
    auto t = std::chrono::system_clock::to_time_t(now);

    tm localTime{};
    localtime_s(&localTime, &t);

    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    std::wstringstream wstrStream;
    wstrStream << L"["
        << std::setw(2) << std::setfill(L'0') << localTime.tm_hour << L":"
        << std::setw(2) << std::setfill(L'0') << localTime.tm_min << L":"
        << std::setw(2) << std::setfill(L'0') << localTime.tm_sec << L"."
        << std::setw(3) << std::setfill(L'0') << ms.count()
        << L"]";
    return wstrStream.str();
}