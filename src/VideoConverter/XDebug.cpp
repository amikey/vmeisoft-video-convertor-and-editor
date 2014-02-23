#include "StdAfx.h"
#include "XDebug.h"

#define WRITE_LOG(buffer, l) if (logfile){DWORD NumberOfBytesWritten; WriteFile(logfile, buffer, l, &NumberOfBytesWritten, NULL);}

extern "C"
{
	extern HANDLE logfile;
};

#define MAX_MSG_BUF 4096

static const char LogHeader[4][6] =
{
	"     ", // DEBUG
	"     ", // INFO
	"  *  ", // WARN
	" *** ", // ERROR
};

void __stdcall WriteErrorLog(char *info)
{
	WRITE_LOG(info, lstrlenA(info));
}

LOG_EXPORT(void) WriteLog(const char format[], ...)
{
	char buffer[MAX_MSG_BUF];
	va_list arglist;
	int32_t l;

	va_start (arglist, format);
	l = vsprintf(buffer, format, arglist);
	WRITE_LOG(buffer, l);
	
#if defined(_DEBUG)
	OutputDebugStringA(buffer);
#endif

}

LOG_EXPORT(void) WriteLog1(const int32_t level, const char format[], ...)
{
	char buffer[MAX_MSG_BUF];
	va_list arglist;
	int32_t l = 5;

	strcpy(buffer, LogHeader[level]);
	va_start (arglist, format);
	l += vsprintf(buffer + l, format, arglist);
	WRITE_LOG(buffer, l);
	
#if defined(_DEBUG)
	OutputDebugStringA(buffer);
#endif

}

LOG_EXPORT(void) WriteLog2(const char file[], const int32_t line, const int32_t level, const char format[], ...)
{
	char buffer[MAX_MSG_BUF];
	int32_t l = sprintf(buffer, "file:%s, line:%d\n%s", file, line, LogHeader[level]);
	va_list arglist;
	va_start (arglist, format);
	l += vsprintf(buffer + l, format, arglist);
	WRITE_LOG(buffer, l);
	
#if defined(_DEBUG)
	OutputDebugStringA(buffer);
#endif

}

LOG_EXPORT(HANDLE) GetLogFileHandle()
{
	return logfile;
}

LOG_EXPORT(void) CreateLogFile()
{
	static BOOL binit;
	if (binit) return;
	binit = TRUE;

	wchar_t szDirectory[MAX_PATH];
	wchar_t szFileName[MAX_PATH];

	SECURITY_ATTRIBUTES saAttr;
	ZeroMemory(&saAttr, sizeof(saAttr));
	saAttr.nLength = sizeof(saAttr); 
	saAttr.bInheritHandle = TRUE; 

	SHGetFolderPath(NULL, CSIDL_PERSONAL | CSIDL_FLAG_CREATE, NULL, 0, szDirectory);
	StringCchCat(szDirectory, MAX_PATH, L"\\Log Files\\");
	CreateDirectory(szDirectory, NULL);

	StringCchPrintf(szFileName, MAX_PATH, L"%ls\\videoconverter.log", szDirectory);

	DeleteFile(szFileName);
	logfile = CreateFile(szFileName,
		GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		&saAttr,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
		NULL);
	if (logfile == INVALID_HANDLE_VALUE)
	{
		logfile = NULL;
		return ;
	}
	
	SetStdHandle(STD_OUTPUT_HANDLE, logfile);
	SetStdHandle(STD_ERROR_HANDLE, logfile);

	SYSTEMTIME t;
	GetLocalTime(&t);
	char buffer[1024];
	int32_t l = sprintf(buffer,
		"============================================\n"
		"%d/%d/%d %d:%d:%d.%03d\n"
		"============================================\n\n",
		t.wYear, t.wMonth + 1, t.wDay, t.wHour, t.wMinute, t.wSecond, t.wMilliseconds);
	WRITE_LOG(buffer, l);
}
