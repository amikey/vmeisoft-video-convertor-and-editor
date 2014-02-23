#include "stdafx.h"
#include "VideoConverter.h"
#include "SubtitleRealText.h"

// RealText
// http://service.real.com/help/library/guides/realone/ProductionGuide/HTML/htmfiles/realtext.htm

CSubtitleReader* CSubtitleRealTextReader::CreateInstance()
{
	CSubtitleRealTextReader *reader =  new CSubtitleRealTextReader();
	return reader;
}

BOOL CSubtitleRealTextReader::Probe(wchar_t *text)
{
	if (bProbe) return FALSE;
	bProbe = TRUE;
	return _wcsnicmp(text, L"<window", 7) == 0;
}

static __int64 read_ts(const wchar_t *s)
{
    int32_t hh, mm, ss, ms;

	if (swscanf(s, L"%u:%u:%u.%u", &hh, &mm, &ss, &ms) == 4) return (hh*3600 + mm*60 + ss) * AV_TIME_BASE_LL + ms * AV_TIME_BASE_LL / 100;
    if (swscanf(s, L"%u:%u:%u"   , &hh, &mm, &ss     ) == 3) return (hh*3600 + mm*60 + ss) * AV_TIME_BASE_LL;
    if (swscanf(s,    L"%u:%u.%u",      &mm, &ss, &ms) == 3) return (          mm*60 + ss) * AV_TIME_BASE_LL + ms * AV_TIME_BASE_LL / 100;
    if (swscanf(s,    L"%u:%u"   ,      &mm, &ss     ) == 2) return (          mm*60 + ss) * AV_TIME_BASE_LL;
    if (swscanf(s,       L"%u.%u",           &ss, &ms) == 2) return (                  ss) * AV_TIME_BASE_LL + ms * AV_TIME_BASE_LL / 100;
    return _wtol(s) * AV_TIME_BASE_LL / 100;
}

wchar_t *StrIStr(wchar_t *str, const wchar_t *strSearch)
{
	int32_t l = wcslen(strSearch);
	while (*str)
	{
		if (_wcsnicmp(str, strSearch, l) == 0)
		{
			return str + l;
		}
		str ++;
	}
	return NULL;
}

CSubtitle* CSubtitleRealTextReader::ReadSubTitle(CSubtitleText *reader)
{
	wchar_t Buffer[MAX_SUBTITLE_LINE_SIZE];
	CSubtitle *subtitle = new CSubtitle();
	
	for(;;)
	{
		wchar_t *line = reader->ReadLine();
		if (line == NULL) break;
		
		line = StrIStr(line, L"<time ");
		if (line == NULL) continue;
		line += 6;

		wchar_t *begin = StrIStr(line, L"Begin=\"");
		if (begin == NULL) continue;
		int64_t start = read_ts(begin);

		wchar_t *end = StrIStr(line, L"end=\"");
		int64_t stop = end ? read_ts(end) : 0;

		line = StrIStr(line, L">");

		int32_t skip = 0;
		int32_t i = 0;
		
		for(wchar_t *p = line; *p; p++)
		{
			if(*p == '>') 
			{
				skip = 0; 
				continue;
			}
			if(*p == '<') 
			{
				skip = 1; 
				continue;
			}
			if(skip) 
			{
				continue;
			}

			if (i < MAX_SUBTITLE_LINE_SIZE - 1)
			{
				Buffer[i] = *p;
				i++;
			}
		}
		
		if (i)
		{
			Buffer[i] = 0;
			CSubtitleItem *item = subtitle->NewItem();
			item->AddLine(Buffer);
			item->m_StartTime = start;
			item->m_Duration = stop;
		}
	}

	if (subtitle->m_Count) return subtitle;
	delete subtitle;
	return NULL;
}
