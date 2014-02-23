#include "stdafx.h"
#include "SubtitleUtils.h"
#include "VideoConverterInt.h"

static const wchar_t* SUBTITLE_FILE_FILTER[] =
{
	L"utf",
	L"utf8",
	L"utf-8",
	L"sub",
	L"srt",
	L"smi",
	L"rt",
	L"txt",
	L"ssa",
	L"aqt",
	L"jss",
	L"js",
	L"ass",
	L"vtt",
	NULL
};

static BOOL IsSubtitleFile(wchar_t *szFile)
{
	wchar_t *szExt = PathFindExtension(szFile);
	if (szExt == NULL) return FALSE;
	for(int i = 0; SUBTITLE_FILE_FILTER[i]; i++)
	{
		if (_wcsicmp(szExt, SUBTITLE_FILE_FILTER[i]) == 0)
		{
			return TRUE;
		}
	}
	return FALSE;
}

static int SubtitleCount;
static wchar_t *SubtileFiles[32];
static wchar_t *SubtileNames[32];

int __stdcall SubtitleFind(wchar_t *szVideoFile)
{
	wchar_t szTemp[MAX_PATH];
	wchar_t szName[MAX_PATH];
	wchar_t szPath[MAX_PATH];
	StringCchCopy(szTemp, MAX_PATH, szVideoFile);
	StringCchCopy(szName, MAX_PATH, szVideoFile);
	StringCchCopy(szPath, MAX_PATH, szVideoFile);
	PathRemoveExtension(szTemp);
	PathStripPath(szName);
	PathRemoveFileSpec(szPath);
	StringCchCat(szTemp, MAX_PATH, L"*.*");

	WIN32_FIND_DATAW wfd;
	HANDLE hFind = FindFirstFile(szTemp, &wfd);
	if (hFind == INVALID_HANDLE_VALUE) return 0;
	SubtitleCount = 0;
	do
	{
		if (
			((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) &&
			(_wcsicmp(wfd.cFileName, szName) != 0) &&
			(IsSubtitleFile(wfd.cFileName))
			)
		{
			SubtileFiles[SubtitleCount] = (wchar_t *)MemoryAlloc(MAX_PATH * sizeof(wchar_t));
			SubtileNames[SubtitleCount] = (wchar_t *)MemoryAlloc(MAX_PATH * sizeof(wchar_t));
			StringCchPrintf(SubtileFiles[SubtitleCount], MAX_PATH, L"%ls\\%ls", szPath, wfd.cFileName);
			StringCchCopy(SubtileNames[SubtitleCount], MAX_PATH, wfd.cFileName);
			SubtitleCount ++;
		}
	}
	while (FindNextFile(hFind, &wfd) != 0);
	FindClose(hFind);

	return SubtitleCount;
}

wchar_t * __stdcall SubtitleFindGetPath(int index)
{
	return SubtileFiles[index];
}

wchar_t * __stdcall SubtitleFindGetName(int index)
{
	return SubtileNames[index];
}

void __stdcall SubtitleFindFree()
{
	for(int i = 0; i < SubtitleCount; i++)
	{
		MemoryFree(SubtileFiles[i]);
		MemoryFree(SubtileNames[i]);
	}
	SubtitleCount = 0;
}