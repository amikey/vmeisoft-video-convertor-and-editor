#include "stdafx.h"
#include "VideoConverter.h"
#include "SubtitleAqt.h"

// FRAME
// http://web.archive.org/web/20070210095721/http://www.volny.cz/aberka/czech/aqt.html
// https://trac.annodex.net/wiki/AQTitle
/*
-->> 00000000

-->> 00001000
This is a comment.
This is a second comment.
-->> 00002000

-->> 00003000
This is a third comment.
This is a fourth comment.
-->> 00004000

-->> 00005000
This is a fifth comment.
This is a sixth comment.
-->> 00006000
*/

CSubtitleReader* CSubtitleAqtReader::CreateInstance()
{
	CSubtitleAqtReader *reader =  new CSubtitleAqtReader();
	return reader;
}

BOOL CSubtitleAqtReader::Probe(wchar_t *text)
{
	int32_t v;
	return swscanf(text, L"-->> %ld", &v) == 3;
}

CSubtitle* CSubtitleAqtReader::ReadSubTitle(CSubtitleText *reader)
{
	int64_t start, stop;
	CSubtitle *subtitle = new CSubtitle();
	subtitle->m_FrameBase = 1;
	CSubtitleItem *item = NULL;
	for(;;)
	{
		wchar_t *line = reader->ReadLine();
		if (line == NULL) break;

		if (swscanf (line, L"-->> %lld", &start) != 1)
		{
			continue;
		}

		if (item == NULL) item = subtitle->NewItem();
		for(;;)
		{
			line = reader->ReadLine();
			if (line == NULL) break;
			if (*line == NULL) break;

			if (swscanf (line, L"-->> %lld", &stop) == 1)
			{
				item->m_Duration = stop - start;
				break;
			}
			else
			{
				item->AddLine(line);
			}
		}

		if (item->m_Count)
		{
			item->m_StartTime = start;
			item = NULL;
		}
	}

	if (item) subtitle->DeleteItem(item->m_index);
	if (subtitle->m_Count) return subtitle;
	delete subtitle;
	return NULL;
}
