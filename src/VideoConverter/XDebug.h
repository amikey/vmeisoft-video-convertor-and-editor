#ifndef XDebug_h_
#define XDebug_h_

#ifdef __cplusplus
#define LOG_EXPORT(type) extern "C" type
#else
#define LOG_EXPORT(type) extern type
#endif

#define ERROR_LEVEL			3  //
#define WARN_LEVEL			2  //
#define INFO_LEVEL			1  //
#define DEBUG_LEVEL			0  //

LOG_EXPORT(void) WriteLog(const char format[], ...);
LOG_EXPORT(void) WriteLog1(const int32_t level, const char format[], ...);
LOG_EXPORT(void) WriteLog2(const char file[], const int32_t line, const int32_t level, const char format[], ...);

#ifdef _DEBUG
#define ENTER_FUNCTION() WriteLog("Call:" __FUNCTION__ "\n");
#else
#define ENTER_FUNCTION()
#endif

#ifdef _DEBUG
#define DebugLog(...)		WriteLog1(DEBUG_LEVEL, __VA_ARGS__);
#define InfoLog(...)		WriteLog1(INFO_LEVEL, __VA_ARGS__);
#define WarnLog(...)		WriteLog1(WARN_LEVEL, __VA_ARGS__);
#define ErrorLog(...)		WriteLog2(__FILE__, __LINE__, ERROR_LEVEL, __VA_ARGS__);
#define PrintError(str)		WriteLog2(__FILE__, __LINE__, ERROR_LEVEL, "%s:%s\n", str, strerror(errno));
#else
#define DebugLog(...)
#define InfoLog(...)		WriteLog1(INFO_LEVEL, __VA_ARGS__);
#define WarnLog(...)		WriteLog1(WARN_LEVEL, __VA_ARGS__);
#define ErrorLog(...)		WriteLog1(ERROR_LEVEL, __VA_ARGS__);
#define PrintError(str)		WriteLog1(ERROR_LEVEL, "%s:%s\n", str, strerror(errno));
#endif

LOG_EXPORT(void) CreateLogFile();
LOG_EXPORT(HANDLE) GetLogFileHandle();

#endif
