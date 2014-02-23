#define _CRT_SECURE_NO_WARNINGS 1
#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>

int fseeko64 (FILE* stream, __int64 offset, int whence)
{
	return _fseeki64(stream, offset, whence);
}

__int64 ftello64 (FILE * stream)
{
	return _ftelli64(stream);
}

HMODULE __mingw_get_msvcrt_handle(void)
{
    static HMODULE msvcrt_handle;

    if(!msvcrt_handle) 
	{
		msvcrt_handle = LoadLibraryW(L"msvcrt.dll");
    }

    return msvcrt_handle;
}
