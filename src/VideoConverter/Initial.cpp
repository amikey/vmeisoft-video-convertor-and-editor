#include "StdAfx.h"
#include "VideoConverter.h"
#include "Initial.h"

#define REG_PATH			L"SOFTWARE\\VMeisoft\\videoconverter"
#define REG_PROFILE			L"Profile"
#define REG_SETTINGS		L"Settings"

HRESULT LoadProfileSettings()
{
	HKEY hKey;
	LONG lRet = RegCreateKey(HKEY_CURRENT_USER, REG_PATH, &hKey);
	if( lRet != ERROR_SUCCESS )
	{
		ErrorLog("Faild(%d) to RegCreateKey\n", lRet);
		return E_FAIL;
	}

	DWORD cbData = sizeof(g_ProfileSettings);
	DWORD dwType = REG_BINARY;
	lRet = RegQueryValueEx(hKey, REG_PROFILE, 0, &dwType, (LPBYTE)&g_ProfileSettings, &cbData);
	if ((lRet != ERROR_SUCCESS ) || (dwType != REG_BINARY) || (cbData != sizeof(g_ProfileSettings)))
	{
		ErrorLog("Faild(%d) to RegQueryValueEx(Type=%d, cbData=%d, size=%d)\n", lRet, dwType, cbData, sizeof(g_ProfileSettings));
		RegCloseKey(hKey);
		return E_FAIL;
	}

	if (FindProfile(g_ProfileSettings.m_IdProfile) == NULL)
	{
		ErrorLog("Error Profile ID\n");
		RegCloseKey(hKey);
		return E_FAIL;
	}

	RegCloseKey(hKey);
	return S_OK;
}

void SaveProfileSettings()
{
	HKEY hKey;
	LONG lRet = RegCreateKey(HKEY_CURRENT_USER, REG_PATH, &hKey);
	if( lRet != ERROR_SUCCESS )
	{
		ErrorLog("Faild(%d) to RegCreateKey\n", lRet);
		return ;
	}

	lRet = RegSetValueEx(hKey, REG_PROFILE, 0L, REG_BINARY, (CONST BYTE *)&g_ProfileSettings, sizeof(g_ProfileSettings));
	if( lRet != ERROR_SUCCESS )
	{
		ErrorLog("Faild(%d) to RegSetValueEx\n", lRet);
	}

	RegCloseKey(hKey);
}

HRESULT LoadOptions()
{
	HKEY hKey;
	LONG lRet = RegCreateKey(HKEY_CURRENT_USER, REG_PATH, &hKey);
	if( lRet != ERROR_SUCCESS )
	{
		ErrorLog("Faild(%d) to RegCreateKey\n", lRet);
		return E_FAIL;
	}

	DWORD cbData = sizeof(g_opt);
	DWORD dwType = REG_BINARY;
	lRet = RegQueryValueEx(hKey, REG_SETTINGS, 0, &dwType, (LPBYTE)&g_opt, &cbData);
	if ((lRet != ERROR_SUCCESS ) || (dwType != REG_BINARY) || (cbData != sizeof(g_opt)))
	{
		ErrorLog("Faild(%d) to RegQueryValueEx(Type=%d, cbData=%d, size=%d)\n", lRet, dwType, cbData, sizeof(g_opt));
		RegCloseKey(hKey);
		ZeroMemory(&g_opt, sizeof(g_opt));
		return E_FAIL;
	}

	RegCloseKey(hKey);
	return S_OK;
}

void SaveOptions()
{
	HKEY hKey;
	LONG lRet = RegCreateKey(HKEY_CURRENT_USER, REG_PATH, &hKey);
	if( lRet != ERROR_SUCCESS )
	{
		ErrorLog("Faild(%d) to RegCreateKey\n", lRet);
		return ;
	}

	lRet = RegSetValueEx(hKey, REG_SETTINGS, 0L, REG_BINARY, (CONST BYTE *)&g_opt, sizeof(g_opt));
	if( lRet != ERROR_SUCCESS )
	{
		ErrorLog("Faild(%d) to RegSetValueEx\n", lRet);
	}

	RegCloseKey(hKey);
}


#define BUFSIZE 256
BOOL	g_System64;
int32_t		g_cpu_count;
DWORD	g_OsSystemMajorVersion;

BOOL GetOSDisplayString()
{
	OSVERSIONINFOEX osvi;
	SYSTEM_INFO si;
	DWORD dwType;

	char szOS[BUFSIZE];
	char szBuf[BUFSIZE];
	szOS[0] = 0;

	ZeroMemory(&si, sizeof(SYSTEM_INFO));
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));

	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	GetVersionEx ((OSVERSIONINFO *) &osvi);
	GetNativeSystemInfo(&si);
	g_OsSystemMajorVersion = osvi.dwMajorVersion;

	if ( VER_PLATFORM_WIN32_NT != osvi.dwPlatformId)
	{
		ErrorLog("Error System Platform=%d\n", osvi.dwPlatformId);
		return FALSE;
	}

	if (osvi.dwMajorVersion < 6)
	{
		ErrorLog("Error System Version=%d.%d !\n", osvi.dwMajorVersion, osvi.dwMinorVersion);
		return FALSE;
	}

	switch(si.dwProcessorType)
	{
	case PROCESSOR_INTEL_386:
		StringCchCatA(szOS, BUFSIZE, "Processor=386 ");
		break;
	case PROCESSOR_INTEL_486:
		StringCchCatA(szOS, BUFSIZE, "Processor=486 ");
		break;
	case PROCESSOR_INTEL_PENTIUM:
		StringCchCatA(szOS, BUFSIZE, "Processor=PENTIUM ");
		break;
	case PROCESSOR_INTEL_IA64:
		StringCchCatA(szOS, BUFSIZE, "Processor=IA64 ");
		break;
	case PROCESSOR_AMD_X8664:
		StringCchCatA(szOS, BUFSIZE, "Processor=X8664 ");
		break;
	}

	switch(si.wProcessorArchitecture)
	{
	case PROCESSOR_ARCHITECTURE_AMD64:
		StringCchCatA(szOS, BUFSIZE, "Architecture=AMD64 ");
		g_System64 = TRUE;
		break;

	case PROCESSOR_ARCHITECTURE_IA64:
		StringCchCatA(szOS, BUFSIZE, "Architecture=IA64 ");
		g_System64 = TRUE;
		break;

	case PROCESSOR_ARCHITECTURE_INTEL:
		StringCchCatA(szOS, BUFSIZE, "Architecture=INTEL ");
		break;

	default:
		{
			StringCchPrintfA(szBuf, 80, "UNKNOWN(%d) ", si.wProcessorArchitecture);
			StringCchCatA(szOS, BUFSIZE, szBuf);
		}
		break;
	}

	InfoLog("%hs\n", szOS);

	StringCchCopyA(szOS, BUFSIZE, "Microsoft ");

	// Test for the specific product.
	if ( osvi.dwMajorVersion >= 6)
	{
		if( osvi.wProductType == VER_NT_WORKSTATION )
		{
			if ((osvi.dwMajorVersion == 6) && (osvi.dwMinorVersion == 1))
			{
				StringCchCatA(szOS, BUFSIZE, "Windows 7 ");
			}
			else if ((osvi.dwMajorVersion == 6) && (osvi.dwMinorVersion == 0))
			{
				StringCchCatA(szOS, BUFSIZE, "Windows Vista ");
			}
			else 
			{
				StringCchPrintfA( szBuf, 80, " Unknown System(Major Version=%d, Minor Version=%d)", osvi.dwMajorVersion, osvi.dwMinorVersion);
				StringCchCatA(szOS, BUFSIZE, szBuf);
			}
		}
		else 
		{
			StringCchCatA(szOS, BUFSIZE, "Windows Server 2008 " );
		}

		typedef BOOL (WINAPI *PGPI)(DWORD, DWORD, DWORD, DWORD, PDWORD);
		PGPI pGetProductInfo = (PGPI) GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")),  "GetProductInfo");
		pGetProductInfo(osvi.dwMajorVersion, osvi.dwMinorVersion, osvi.wServicePackMajor, osvi.wServicePackMinor, &dwType);
		switch( dwType )
		{
		case PRODUCT_ULTIMATE:
			StringCchCatA(szOS, BUFSIZE, "Ultimate Edition" );
			break;
		case PRODUCT_HOME_PREMIUM:
			StringCchCatA(szOS, BUFSIZE, "Home Premium Edition" );
			break;
		case PRODUCT_HOME_BASIC:
			StringCchCatA(szOS, BUFSIZE, "Home Basic Edition" );
			break;
		case PRODUCT_ENTERPRISE:
			StringCchCatA(szOS, BUFSIZE, "Enterprise Edition" );
			break;
		case PRODUCT_BUSINESS:
			StringCchCatA(szOS, BUFSIZE, "Business Edition" );
			break;
		case PRODUCT_STARTER:
			StringCchCatA(szOS, BUFSIZE, "Starter Edition" );
			break;
		case PRODUCT_CLUSTER_SERVER:
			StringCchCatA(szOS, BUFSIZE, "Cluster Server Edition" );
			break;
		case PRODUCT_DATACENTER_SERVER:
			StringCchCatA(szOS, BUFSIZE, "Datacenter Edition" );
			break;
		case PRODUCT_DATACENTER_SERVER_CORE:
			StringCchCatA(szOS, BUFSIZE, "Datacenter Edition (core installation)" );
			break;
		case PRODUCT_ENTERPRISE_SERVER:
			StringCchCatA(szOS, BUFSIZE, "Enterprise Edition" );
			break;
		case PRODUCT_ENTERPRISE_SERVER_CORE:
			StringCchCatA(szOS, BUFSIZE, "Enterprise Edition (core installation)" );
			break;
		case PRODUCT_ENTERPRISE_SERVER_IA64:
			StringCchCatA(szOS, BUFSIZE, "Enterprise Edition for Itanium-based Systems" );
			break;
		case PRODUCT_SMALLBUSINESS_SERVER:
			StringCchCatA(szOS, BUFSIZE, "Small Business Server" );
			break;
		case PRODUCT_SMALLBUSINESS_SERVER_PREMIUM:
			StringCchCatA(szOS, BUFSIZE, "Small Business Server Premium Edition" );
			break;
		case PRODUCT_STANDARD_SERVER:
			StringCchCatA(szOS, BUFSIZE, "Standard Edition" );
			break;
		case PRODUCT_STANDARD_SERVER_CORE:
			StringCchCatA(szOS, BUFSIZE, "Standard Edition (core installation)" );
			break;
		case PRODUCT_WEB_SERVER:
			StringCchCatA(szOS, BUFSIZE, "Web Server Edition" );
			break;
		}
		if ( si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 )
		{
			StringCchCatA(szOS, BUFSIZE,  ", 64-bit" );
		}
		else if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL )
		{
			StringCchCatA(szOS, BUFSIZE, ", 32-bit");
		}
	}
	else if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 )
	{
		if( GetSystemMetrics(SM_SERVERR2) )
		{
			StringCchCatA(szOS, BUFSIZE,  "Windows Server 2003 R2, ");
		}
		else if ( osvi.wSuiteMask==VER_SUITE_STORAGE_SERVER )
		{
			StringCchCatA(szOS, BUFSIZE,  "Windows Storage Server 2003");
		}
		else if( osvi.wProductType == VER_NT_WORKSTATION && si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
		{
			StringCchCatA(szOS, BUFSIZE,  "Windows XP Professional x64 Edition");
		}
		else
		{
			StringCchCatA(szOS, BUFSIZE, "Windows Server 2003, ");
		}

		// Test for the server type.
		if ( osvi.wProductType != VER_NT_WORKSTATION )
		{
			if ( si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64 )
			{
				if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
					StringCchCatA(szOS, BUFSIZE,  "Datacenter Edition for Itanium-based Systems" );
				else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
					StringCchCatA(szOS, BUFSIZE,  "Enterprise Edition for Itanium-based Systems" );
			}
			else if ( si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 )
			{
				if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
					StringCchCatA(szOS, BUFSIZE,  "Datacenter x64 Edition" );
				else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
					StringCchCatA(szOS, BUFSIZE,  "Enterprise x64 Edition" );
				else StringCchCatA(szOS, BUFSIZE,  "Standard x64 Edition" );
			}

			else
			{
				if ( osvi.wSuiteMask & VER_SUITE_COMPUTE_SERVER )
					StringCchCatA(szOS, BUFSIZE,  "Compute Cluster Edition" );
				else if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
					StringCchCatA(szOS, BUFSIZE,  "Datacenter Edition" );
				else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
					StringCchCatA(szOS, BUFSIZE,  "Enterprise Edition" );
				else if ( osvi.wSuiteMask & VER_SUITE_BLADE )
					StringCchCatA(szOS, BUFSIZE,  "Web Edition" );
				else StringCchCatA(szOS, BUFSIZE,  "Standard Edition" );
			}
		}
	}
	else if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
	{
		StringCchCatA(szOS, BUFSIZE, "Windows XP ");
		if( osvi.wSuiteMask & VER_SUITE_PERSONAL )
		{
			StringCchCatA(szOS, BUFSIZE,  "Home Edition" );
		}
		else
		{
			StringCchCatA(szOS, BUFSIZE,  "Professional" );
		}
	}

	// Include service pack (if any) and build number.
	if( _tcslen(osvi.szCSDVersion) > 0 )
	{
		StringCchPrintfA( szBuf, 80, " %ls", osvi.szCSDVersion);
		StringCchCatA(szOS, BUFSIZE, szBuf);
	}

	{
		StringCchPrintfA( szBuf, 80, " (build %d)", osvi.dwBuildNumber);
		StringCchCatA(szOS, BUFSIZE, szBuf);
	}

	InfoLog("%hs\n", szOS);
	return TRUE; 

}

#define MB			(1024 * 1024)

void GetSystemInformations()
{
	SYSTEM_INFO sysinfo;
	ZeroMemory(&sysinfo, sizeof(sysinfo));

	char *lpProcessor = NULL;
	GetSystemInfo(&sysinfo);
	switch(sysinfo.wProcessorArchitecture)
	{
	case PROCESSOR_ARCHITECTURE_UNKNOWN:
		lpProcessor = "Unknow";
		break;
	case PROCESSOR_ARCHITECTURE_INTEL:
		lpProcessor = "Intel";
		break;
	case PROCESSOR_ARCHITECTURE_IA64:
		lpProcessor = "IA64";
		break;
	case PROCESSOR_ARCHITECTURE_AMD64:
		lpProcessor = "AMD64";
		break;
	}
	InfoLog("CPU TYPE:%s\n", lpProcessor);
	InfoLog("NumberOfProcessors=%d\n", sysinfo.dwNumberOfProcessors);
	g_cpu_count = sysinfo.dwNumberOfProcessors;

	// 取内存信息
	MEMORYSTATUS stat;
	GlobalMemoryStatus(&stat);

	InfoLog("MemoryStatus:\n");
	InfoLog("%d percent of memory is in use.\n", stat.dwMemoryLoad);
	InfoLog("%dM bytes total of physical memory.\n", (DWORD)(stat.dwTotalPhys / MB));
	InfoLog("%dM bytes free of physical memory.\n", (DWORD)(stat.dwAvailPhys / MB));
	InfoLog("%dM bytes total of virtual memory.\n",(DWORD)(stat.dwTotalVirtual / MB));
	InfoLog("%dM bytes free  of virtual memory.\n",(DWORD)(stat.dwAvailVirtual / MB));
}

int32_t __stdcall GetCpuCoreCount()
{
	return g_cpu_count;	
}
