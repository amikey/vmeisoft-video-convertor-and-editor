#pragma once

extern HRESULT LoadOptions();
extern void SaveOptions();
extern HRESULT LoadProfileSettings();
extern void SaveProfileSettings();

extern BOOL		g_System64;
extern int32_t		g_cpu_count;
extern DWORD	g_OsSystemMajorVersion;
extern BOOL GetOSDisplayString();
extern void GetSystemInformations();

