#include "stdafx.h"
#include "VideoConverter.h"
#include "SubtitleMicroDVD.h"

// http://en.wikipedia.org/wiki/MicroDVD

CSubtitleReader* CSubtitleMicroDVDReader::CreateInstance()
{
	CSubtitleMicroDVDReader *reader =  new CSubtitleMicroDVDReader();
	return reader;
}

BOOL CSubtitleMicroDVDReader::Probe(wchar_t *text)
{
	__int64 v;
	return swscanf(text, L"{%lld}{%lld}", &v, &v) == 2;
}

CSubtitle* CSubtitleMicroDVDReader::ReadSubTitle(CSubtitleText *reader)
{
	wchar_t Buffer[MAX_SUBTITLE_LINE_SIZE];
	CSubtitle *subtitle = new CSubtitle();
	subtitle->m_FrameBase = 1;
	CSubtitleItem *item = NULL;
	for(;;)
	{
		wchar_t *line = reader->ReadLine();
		if (line == NULL) break;

		__int64 start = 0, stop = 0;
		if 	(swscanf(line, L"{%ld}{%ld}", &start, &stop) < 1)
		{
			continue;
		}

		int32_t a = 0, b = 0, k = 0;
		wchar_t *p = line;
		while ((*p) && (a != 2) && (b != 2))
		{
			if ((k == 0) && (*p == L'{')){a++; k = 1;};
			if ((k == 1) && (*p == L'}')){b++; k = 0;};
			p ++;
		}
		if ((a != 2) || (b != 2) || (*p == 0))
		{
			continue;
		}

		int32_t skip = 0;
		int32_t i = 0;
		if (item == NULL) item = subtitle->NewItem();
		while(*p)
		{
			if (*p == '}') 
			{
				skip = 0; p++; 
				continue;
			}
			if (*p == '{') 
			{
				skip = 1; p++; 
				continue;
			}
			if (skip) 
			{
				continue;
			}
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
