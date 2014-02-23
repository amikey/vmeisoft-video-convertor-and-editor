#pragma once

#ifdef _DEBUG

extern void ShowProfile(IWMProfile* pProfile);
extern void ShowMediaTypeString(char *header, const WM_MEDIA_TYPE *pmt);
extern wchar_t *GetAttributeString(WMT_ATTR_DATATYPE Type, const BYTE *pValue, WORD cbLength);

#else

#define ShowProfile(...)
#define ShowMediaTypeString(...)
#define GetAttributeString(...)

#endif
