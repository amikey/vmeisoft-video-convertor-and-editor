#include "stdafx.h"
#include "VideoConverter.h"
#include "SubtitleSami.h"

// http://en.wikipedia.org/wiki/SAMI

CSubtitleReader* CSubtitleSamiReader::CreateInstance()
{
	CSubtitleSamiReader *reader =  new CSubtitleSamiReader();
	return reader;
}

BOOL CSubtitleSamiReader::Probe(wchar_t *text)
{
	if (bProbe) return false;
	bProbe = true;
	return _wcsicmp(text, L"<SAMI>") == 0;
}

CSubtitle* CSubtitleSamiReader::ReadSubTitle(CSubtitleText *reader)
{
	wchar_t Buffer[MAX_SUBTITLE_LINE_SIZE];
	CSubtitle *subtitle = new CSubtitle();
	for(;;)
	{
		wchar_t *line = reader->Read(L"<SYNC Start=", L"</SYNC>");
		if (line == NULL) break;
		int64_t start = _wtoi64(line);
		while (*line != L'>') line ++;

		int32_t i = 0;
		int32_t skip = 0;
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
			item->m_StartTime = start * AV_TIME_BASE_LL;
		}

	}

	if (subtitle->m_Count) return subtitle;
	delete subtitle;
	return NULL;
}
