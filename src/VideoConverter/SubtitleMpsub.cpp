#include "stdafx.h"
#include "VideoConverter.h"
#include "SubtitleMpsub.h"

// http://www.mplayerhq.hu/DOCS/tech/mpsub.sub

CSubtitleReader* CSubtitleMpsubReader::CreateInstance()
{
	CSubtitleMpsubReader *reader =  new CSubtitleMpsubReader();
	return reader;
}

BOOL CSubtitleMpsubReader::Probe(wchar_t *text)
{
	__int64 v;
	return (swscanf(text, L"FORMAT=%d", &v) == 1) || (_wcsicmp(text, L"FORMAT=TIME") == 0);
}

CSubtitle* CSubtitleMpsubReader::ReadSubTitle(CSubtitleText *reader)
{
	wchar_t Buffer[MAX_SUBTITLE_LINE_SIZE];
	float start = 0, stop = 0;
	CSubtitle *subtitle = new CSubtitle();
	CSubtitleItem *item = NULL;
	for(;;)
	{
		wchar_t *line = reader->ReadLine();
		if (line == NULL) break;

		if (swscanf(line, L"%f %f", &start, &stop) != 2)
		{
			continue;
		}

		int32_t i = 0;
		if (item == NULL) item = subtitle->NewItem();
		item->m_StartTime = start;
		for(;;)
		{
			line = reader->ReadLine();
			if (line == NULL) break;
			if (*line == NULL) break;

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

			Buffer[i] = 0;
			item->AddLine(Buffer);
		}

		if (item->m_Count)
		{
			item = NULL;
		}
	}

	if (subtitle->m_Count) return subtitle;
	delete subtitle;
	return NULL;
}
