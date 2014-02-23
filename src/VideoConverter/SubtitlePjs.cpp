#include "stdafx.h"
#include "VideoConverter.h"
#include "SubtitlePjs.h"

// http://devel.aegisub.org/wiki/SubtitleFormats/PJS
/*
2447,   2513, "You should come to the Drama Club, too."
2513,   2594, "Yeah. The Drama Club is worried|that you haven't been coming."
2603,   2675, "I see. Sorry, I'll drop by next time."
*/

CSubtitleReader* CSubtitlePjsReader::CreateInstance()
{
	CSubtitlePjsReader *reader =  new CSubtitlePjsReader();
	return reader;
}

BOOL CSubtitlePjsReader::Probe(wchar_t *text)
{
	int32_t v;
	return swscanf(text, L"%d,%d,\"%c", &v, &v, &v) == 3;
}

CSubtitle* CSubtitlePjsReader::ReadSubTitle(CSubtitleText *reader)
{
	wchar_t Buffer[MAX_SUBTITLE_LINE_SIZE];
	int64_t start, stop;
	int32_t plen;
	CSubtitle *subtitle = new CSubtitle();
	subtitle->m_FrameBase = 1;
	CSubtitleItem *item = NULL;
	for(;;)
	{
		wchar_t *line = reader->ReadLine();
		if (line == NULL) break;

		if (swscanf (line, L"%ld,%ld, \"%n", &start, &stop, &plen) < 10)
		{
			continue;
		}

		wchar_t *p = line + plen;

		int32_t skip = 0;
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
