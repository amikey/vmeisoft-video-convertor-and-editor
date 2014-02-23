#include "stdafx.h"
#include <locale.h>

HINSTANCE	g_hInstance;
HANDLE		g_hJob;
wchar_t		g_szModulePath[MAX_PATH];
wchar_t		g_szTempPath[MAX_PATH];

static void SetApplicationCodePage()
{
	char szLCData[64];
	GetLocaleInfoA(LOCALE_SYSTEM_DEFAULT, LOCALE_IDEFAULTANSICODEPAGE, szLCData + 1, sizeof(szLCData) - 1);
	szLCData[0] = '.';
	setlocale( LC_ALL, szLCData);
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		g_hInstance = hModule;
		SetApplicationCodePage();
		break;

	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
