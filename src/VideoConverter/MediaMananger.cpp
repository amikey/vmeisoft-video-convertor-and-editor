#include "stdafx.h"
#include "VideoConverter.h"
#include "MediaMananger.h"

static int32_t					g_media_info_count;
static struct MediaInfo**	g_media_list;
static CRITICAL_SECTION		g_locker;

#define REG_PATH	L"SOFTWARE\\VMeisoft\\MediaInfo\\1.0"
#define REG_SIZE	sizeof(struct MediaInfo)

static int32_t __cdecl mediainfo_compare(const MediaInfo **a, const MediaInfo **b) 
{
	return CompareFileTime(&((*a)->m_History), &((*b)->m_History));
}

static BOOL CheckMediaInfo(MediaInfo *pMediaInfo)
{

	WIN32_FIND_DATAW wfd;
	HANDLE hFind = FindFirstFile(pMediaInfo->m_szFileName, &wfd);
	if (hFind == INVALID_HANDLE_VALUE) return FALSE;
	FindClose(hFind);

	if (pMediaInfo->m_ftCreationTime.dwLowDateTime != wfd.ftCreationTime.dwLowDateTime) return FALSE;
	if (pMediaInfo->m_ftCreationTime.dwHighDateTime != wfd.ftCreationTime.dwHighDateTime) return FALSE;
	if (pMediaInfo->m_liFileSize.LowPart != wfd.nFileSizeLow) return FALSE;
	if (pMediaInfo->m_liFileSize.HighPart != wfd.nFileSizeHigh) return FALSE;
	if (pMediaInfo->m_bVideoStream) if (PathFileExists(pMediaInfo->m_Demo) == FALSE) return FALSE;
	if (pMediaInfo->m_bAudioStream)
	{
		int32_t n = pMediaInfo->m_IsPlanar ? pMediaInfo->m_nChannel : 1;
		for(int i = 0; i < n; i++)
		{
			if (PathFileExists(pMediaInfo->m_AudioTmpFile[i]) == FALSE) return FALSE;
		}
	}
	return TRUE;
}

static GUID GetGuidFromFileName(wchar_t *FileName)
{
	//12345678901234
	//vmeisoft_v1_a_%ls_%d.tmp
	GUID id;
	CLSIDFromString(FileName + 14, &id);
	return id;
}

void InitMediaMananger()
{
	InitializeCriticalSection(&g_locker);

	HKEY hKey;
	LONG lRet;

	lRet = RegOpenKeyEx(HKEY_CURRENT_USER, REG_PATH, 0, KEY_QUERY_VALUE | KEY_SET_VALUE, &hKey );
	if (lRet == ERROR_FILE_NOT_FOUND)
	{
		return ;
	}

	if( lRet != ERROR_SUCCESS )
	{
		WarnLog("Faild(%d) to RegOpenKeyEx\n", lRet);
		return ;
	}

	for(int i = 0; ; i++)
	{
		wchar_t achGuid[256];
		DWORD cchGuid = 256;
		struct MediaInfo info;

		if (RegEnumValue(hKey, i, achGuid, &cchGuid, NULL, NULL, NULL, NULL) != ERROR_SUCCESS) break;

		DWORD cbData = REG_SIZE;
		DWORD dwType = REG_BINARY;
		lRet = RegQueryValueEx(hKey, achGuid, 0, &dwType, (LPBYTE)(&info), &cbData);
		if ((lRet != ERROR_SUCCESS ) || (dwType != REG_BINARY) || (cbData != REG_SIZE))
		{
			ErrorLog("Faild(%d) to RegQueryValueEx\n", lRet);
			RegDeleteValue(hKey, achGuid);
			continue;
		}

		if (CheckMediaInfo(&info) == FALSE)
		{
			lRet = RegDeleteValue(hKey, achGuid);
			continue;
		}

		g_media_list = (MediaInfo **)MemoryRealloc(g_media_list, (g_media_info_count + 1) * sizeof(MediaInfo*));
		g_media_list[g_media_info_count] = (MediaInfo *)MemoryAlloc(sizeof(MediaInfo));
		g_media_list[g_media_info_count][0] = info;
		g_media_info_count ++;
	}
	RegCloseKey( hKey );

	if (g_media_info_count > 1)
	{
		qsort(g_media_list, g_media_info_count, sizeof(MediaInfo *), (int32_t (__cdecl *)(const void *,const void *))mediainfo_compare);
	}

	// É¾³ý
	wchar_t szFileFormat[MAX_PATH];
	StringCchPrintf(szFileFormat, MAX_PATH, L"%ls\\vmeisoft_v1_*.*", g_szTempPath);
	WIN32_FIND_DATAW wfd;
	HANDLE hFind = FindFirstFile(szFileFormat, &wfd);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			GUID id = GetGuidFromFileName(wfd.cFileName);

			BOOL bFound = FALSE;
			for(int i = 0; i < g_media_info_count; i++)
			{
				if (g_media_list[i]->m_Id == id)
				{
					bFound = TRUE;
					break;
				}
			}
			if (bFound == FALSE)
			{
				wchar_t szFileName[MAX_PATH];
				StringCchPrintf(szFileName, MAX_PATH, L"%ls\\%ls", g_szTempPath, wfd.cFileName);
				DeleteFile(szFileName);
			}
		}
		while (FindNextFile(hFind, &wfd) != 0);
		FindClose(hFind);
	}
}

CMediaInfo::CMediaInfo(HWND hwndMessage, wchar_t *szFileName)
{

	m_hwndMessage = hwndMessage;

	WIN32_FIND_DATAW wfd;
	HANDLE hFind = FindFirstFile(szFileName, &wfd);
	if (hFind == INVALID_HANDLE_VALUE) return;
	FindClose(hFind);

	StringCchCopy(m_szFileName, MAX_PATH, szFileName);

	wchar_t szGuid[128];
	CoCreateGuid(&m_Id);
	StringFromGUID2(m_Id, szGuid, 128);

	for(int i = 0; i < 8; i++)
	{
		StringCchPrintf(m_AudioTmpFile[i], MAX_PATH, AUDIO_DATA_FMT, g_szTempPath, szGuid, i + 1);
	}
	StringCchPrintf(m_Demo, MAX_PATH, VIDEO_BMP_FMT, g_szTempPath, szGuid);

	SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime);
	SystemTimeToFileTime(&SystemTime, &m_History);

	m_ftCreationTime = wfd.ftCreationTime;
	m_liFileSize.LowPart = wfd.nFileSizeLow;
	m_liFileSize.HighPart = wfd.nFileSizeHigh;

	EnterCriticalSection(&g_locker);
	g_media_list = (struct MediaInfo**)MemoryRealloc(g_media_list, (g_media_info_count + 1) * sizeof(void *));
	g_media_list[g_media_info_count] = this;
	g_media_info_count ++;
	LeaveCriticalSection(&g_locker);

	m_Ok = 1;
}

void SaveMediaInfo(struct MediaInfo *info)
{
	HKEY hKey;
	LONG lRet = RegCreateKey(HKEY_CURRENT_USER, REG_PATH, &hKey);
	if( lRet != ERROR_SUCCESS )
	{
		WriteLog("Faild(%d) to RegCreateKey\n", lRet);
		return ;
	}

	OLECHAR szCLSID[MAX_PATH];
	HRESULT hr = StringFromGUID2(info->m_Id, szCLSID, MAX_PATH);
	assert( SUCCEEDED(hr) );

	lRet = RegSetValueEx(hKey, szCLSID, 0L, REG_BINARY, (CONST BYTE *)info, REG_SIZE);
	if( lRet != ERROR_SUCCESS )
	{
		WriteLog("Faild(%d) to RegSetValueEx\n", lRet);
	}

	RegCloseKey(hKey);
}


struct MediaInfo* FindMediaInfo(wchar_t *szFileName)
{
	EnterCriticalSection(&g_locker);
	struct MediaInfo* Result = NULL;
	for(int i = 0; i < g_media_info_count; i++)
	{
		if (_wcsicmp(g_media_list[i]->m_szFileName, szFileName) == 0)
		{
			Result = g_media_list[i];
			break;
		}
	}
	LeaveCriticalSection(&g_locker);
	return Result;
}
