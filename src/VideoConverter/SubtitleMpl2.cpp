#include "stdafx.h"
#include "VideoConverter.h"
#include "SubtitleMpl2.h"

CSubtitleReader* CSubtitleMpl2Reader::CreateInstance()
{
	CSubtitleMpl2Reader *reader =  new CSubtitleMpl2Reader();
	return reader;
}


BOOL CSubtitleMpl2Reader::Probe(wchar_t *text)
{
	__int64 v;
	return swscanf(text, L"[%lld][%lld]", &v, &v) == 2;
}

CSubtitle* CSubtitleMpl2Reader::ReadSubTitle(CSubtitleText *reader)
{
	__int64 start, stop;
	int32_t l;
	CSubtitle *subtitle = new CSubtitle();
	wchar_t Buffer[MAX_SUBTITLE_LINE_SIZE];
	CSubtitleItem *item = NULL;
	for(;;)
	{
		wchar_t *line = reader->ReadLine();
		if (line == NULL) break;

		if 	(swscanf(line, L"[%lld][%lld]%n",&start, &stop, &l) < 1)
		{
			continue;
		}

		wchar_t *p = line + l;
		int32_t i = 0;
		if (item == NULL) item = subtitle->NewItem();

		while(*p)
		{
			if (*p == L'|')
			{
				Buffer[i] = 0;
				i = 0;
				item->AddLine(Buffer);
			}
			else
			{
				if (i < MAX_SUBTITLE_LINE_SIZE - 1)
				{
					Buffer[i] = *p; i++;
				}
			}
		}

		if (i)
		{
			Buffer[i] = 0;
			i = 0;
			item->AddLine(Buffer);
		}

		if (item->m_Count)
		{
			item->m_StartTime = start;
			item->m_Duration = stop;
			item = NULL;
		}
	}

	if (subtitle->m_Count) return subtitle;
	delete subtitle;
	return NULL;
}
