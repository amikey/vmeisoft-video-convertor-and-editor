#include "stdafx.h"
#include "VideoConverter.h"
#include "SubtitleSubviewer.h"

// http://en.wikipedia.org/wiki/SubViewer
// https://wiki.videolan.org/SubViewer

CSubtitleReader* CSubtitleSubviewerReader::CreateInstance()
{
	CSubtitleSubviewerReader *reader =  new CSubtitleSubviewerReader();
	return reader;
}

BOOL CSubtitleSubviewerReader::Probe(wchar_t *text)
{
	int32_t v;
	return swscanf(text, L"%d:%d:%d.%d,%d:%d:%d.%d", &v, &v, &v, &v, &v, &v, &v, &v) == 8;
}

CSubtitle* CSubtitleSubviewerReader::ReadSubTitle(CSubtitleText *reader)
{
	wchar_t Buffer[MAX_SUBTITLE_LINE_SIZE];
	int32_t a1, a2, a3, a4, b1, b2, b3, b4;
	CSubtitle *subtitle = new CSubtitle();
	CSubtitleItem *item = NULL;
	for(;;)
	{
		wchar_t *line = reader->ReadLine();
		if (line == NULL) break;

		if (swscanf (line, L"%d:%d:%d.%d,%d:%d:%d.%d",
			&a1, &a2, &a3, &a4,
			&b1, &b2, &b3, &b4) < 10)
		{
			continue;
		}

		if (item == NULL) item = subtitle->NewItem();
		int32_t i = 0;
		for(;;)
		{
			line = reader->ReadLine();
			if (line == NULL) break;
			if (*line == 0) break;

			wchar_t *p = line;
			while(*p)
			{
				if ((p[0] == '[') && (p[1] == 'b') && (p[2] == 'r') && (p[3] == ']') )
				{
					// [br]
					Buffer[i] = 0;
					i = 0;
					p += 4;
					item->AddLine(Buffer);
				}
				else
				{
					Buffer[i] = *p;
					i ++;
					p ++;
				}
			}
			if (i)
			{
				Buffer[i] = 0;
				i = 0;
				item->AddLine(Buffer);
			}
		}

		if (item->m_Count)
		{
			item->m_StartTime = (a1*3600 + a2*60 + a3) * AV_TIME_BASE_LL + a4 * AV_TIME_BASE_LL / 100;
			item->m_Duration = (b1*3600 + b2*60 + b3) * AV_TIME_BASE_LL + a4 * AV_TIME_BASE_LL / 100 - item->m_StartTime;
			item = NULL;
		}
	}

	if (subtitle->m_Count) return subtitle;
	delete subtitle;
	return NULL;
}
