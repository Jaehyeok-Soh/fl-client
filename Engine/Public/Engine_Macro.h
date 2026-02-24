#ifndef Engine_Macro_h__
#define Engine_Macro_h__

#define			ENUM_TO_SZET(ENUM)			static_cast<size_t>(ENUM)
#define			ENUM_TO_UINT(ENUM)			static_cast<unsigned int>(ENUM)
#define			ENUM_TO_CHAR(ENUM)			static_cast<unsigned char>(ENUM)
#define			PURE						= 0

#if defined(_DEBUG)
#define CLOG_TRACE(msg) CEngineConsole::Log(ELogLevel::Trace, (msg))
#define CLOG_INFO(msg)  CEngineConsole::Log(ELogLevel::Info,  (msg))
#define CLOG_WARN(msg)  CEngineConsole::Log(ELogLevel::Warn,  (msg))
#define CLOG_ERROR(msg) CEngineConsole::Log(ELogLevel::Error, (msg))
#else
#define CLOG_TRACE(msg) ((void)0)
#define CLOG_INFO(msg)  ((void)0)
#define CLOG_WARN(msg)  ((void)0)
#define CLOG_ERROR(msg) ((void)0)
#endif

#ifndef			MSG_BOX
#define			MSG_BOX(_message)			OutputDebugString(TEXT(_message));MessageBox(nullptr, TEXT(_message), L"System Message", MB_OK)
#endif

#define			NS_BEGIN(NAMESPACE)			namespace NAMESPACE {
#define			NS_END						}

#define			USING(NAMESPACE)			using namespace NAMESPACE;



#ifdef	ENGINE_EXPORTS
#define ENGINE_DLL		_declspec(dllexport)
#else
#define ENGINE_DLL		_declspec(dllimport)
#endif

#define NO_COPY(CLASSNAME)										\
		private:												\
		CLASSNAME(const CLASSNAME&) = delete;					\
		CLASSNAME& operator = (const CLASSNAME&)= delete;		

#define DECLARE_SINGLETON(CLASSNAME)							\
		NO_COPY(CLASSNAME)										\
		private:												\
		static CLASSNAME*	m_pInstance;						\
		public:													\
		static CLASSNAME*	GetInstance( void );				\
		static unsigned int DestroyInstance( void );			

#define IMPLEMENT_SINGLETON(CLASSNAME)							\
		CLASSNAME*	CLASSNAME::m_pInstance = nullptr;			\
		CLASSNAME*	CLASSNAME::GetInstance( void )	{			\
			if(nullptr == m_pInstance) {						\
				m_pInstance = new CLASSNAME;					\
			}													\
			return m_pInstance;									\
		}														\
		unsigned int CLASSNAME::DestroyInstance( void ) {		\
			unsigned int iRefCnt = {0};							\
			if(nullptr != m_pInstance)	{						\
				iRefCnt = m_pInstance->Release();				\
				if(0 == iRefCnt)								\
					m_pInstance = nullptr;						\
			}													\
			return iRefCnt;										\
}


#endif // Engine_Macro_h__
