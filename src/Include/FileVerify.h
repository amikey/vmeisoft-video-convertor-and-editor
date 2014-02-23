#pragma once

#define VI_DATA_SEG	0x12345678
#define VI_END_SEG	0x87654321

#pragma pack(1)
struct VerifyInfo
{
	uint32_t	m_Header;
	uint32_t	m_Length;
	uint32_t	m_FileSize;
	uint64_t	m_CheckSum;
	wchar_t		m_FileName[1];
};
#pragma pack()

extern int (__cdecl *PWriteLine)(const char * _Format, ...);
extern HRESULT GenerateVerify(wchar_t *szPath, wchar_t *szOutFile, uint64_t Key);
extern HRESULT VerifyFiles(wchar_t *szPath, wchar_t *szOutFile, uint64_t Key);
extern BOOL __stdcall HaveProductInfo();
