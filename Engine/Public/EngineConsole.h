#pragma once
#include "Base.h"

NS_BEGIN(Engine)

enum class ELogLevel : unsigned int
{
	Trace,
	Info,
	Warn,
	Error
};

class ENGINE_DLL CEngineConsole
{
private:
	CEngineConsole() = delete;
public:
	static void Initialize(_bool bEnable = true);
	static void Shutdown();

	static _bool Is_Enabled();
	static void Set_Enabled(_bool bEnable);

	static void Log(ELogLevel eLevel, std::string_view strMsg);
	static void Log(ELogLevel eLevel, std::wstring_view wstrMsg);

	static void Set_Title(std::wstring_view wstrTitle);
private:
	static void Write_To_Console(std::wstring_view line);
	static void Write_To_DebugOutput(std::wstring_view line);
	static wstring LogLevel_To_Wstring(ELogLevel eLevel);
	static wstring Get_Timestamp();

	static inline mutex s_Mutex;
	static inline _bool s_bInitialized = { false };
	static inline _bool s_bEnabled = { false };
	static inline HANDLE s_hOut = { nullptr };
};

NS_END