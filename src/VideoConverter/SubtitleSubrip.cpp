#include "stdafx.h"
#include "VideoConverter.h"
#include "SubtitleSubrip.h"

// http://en.wikipedia.org/wiki/SubRip
// .srt

CSubtitleReader* CSubtitleSubripReader::CreateInstance()
{
	CSubtitleSubripReader *reader =  new CSubtitleSubripReader();
	return reader;
}

BOOL CSubtitleSubripReader::Probe(wchar_t *text)
{
	int32_t v;
	return swscanf(text, L"%d:%d:%d%[,.:]%d --> %d:%d:%d%[,.:]%d", 
			&v, &v, &v, (char *)&v, &v, 
			&v, &v, &v, (char *)&v, &v) == 10;
}

CSubtitle* CSubtitleSubripReader::ReadSubTitle(CSubtitleText *reader)
{
	wchar_t Buffer[MAX_SUBTITLE_LINE_SIZE];
	int32_t a1, a2, a3, a4, b1, b2, b3, b4, t;
	CSubtitle *subtitle = new CSubtitle();
	CSubtitleItem *item = NULL;
	for(;;)
	{
		wchar_t *line = reader->ReadLine();
		if (line == NULL) break;

		if (swscanf (line, L"%d:%d:%d%[,.:]%d --> %d:%d:%d%[,.:]%d",
			&a1, &a2, &a3, &t, &a4,
			&b1, &b2, &b3, &t, &b4) != 10)
		{
			continue;
		}

		if (item == NULL) item = subtitle->NewItem();
		for(;;)
		{
			line = reader->ReadLine();
			if (line == NULL) break;
			if (*line == 0) break;

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
				item->AddLine(Buffer);
			}
		}

		if (item->m_Count)
		{
			item->m_StartTime = (a1*3600 + a2*60 + a3) * AV_TIME_BASE_LL + a4 * AV_TIME_BASE_LL / 1000;
			item->m_Duration = (b1*3600 + b2*60 + b3) * AV_TIME_BASE_LL + a4 * AV_TIME_BASE_LL / 1000 - item->m_StartTime;
			item = NULL;
		}
	}

	if (subtitle->m_Count) return subtitle;
	delete subtitle;
	return NULL;
}
