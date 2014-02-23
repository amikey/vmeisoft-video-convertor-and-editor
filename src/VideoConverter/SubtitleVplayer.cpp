#include "stdafx.h"
#include "VideoConverter.h"
#include "SubtitleVPlayer.h"

CSubtitleReader* CSubtitleVPlayerReader::CreateInstance()
{
	CSubtitleVPlayerReader *reader =  new CSubtitleVPlayerReader();
	return reader;
}

BOOL CSubtitleVPlayerReader::Probe(wchar_t *text)
{
	int64_t v;
	return swscanf(text, L"%d:%d:%d ", &v, &v, &v) == 3;
}

CSubtitle* CSubtitleVPlayerReader::ReadSubTitle(CSubtitleText *reader)
{
	wchar_t Buffer[MAX_SUBTITLE_LINE_SIZE];
	int32_t a1, a2, a3, separator;
	int32_t plen;
	CSubtitle *subtitle = new CSubtitle();
	for(;;)
	{
		wchar_t *line = reader->ReadLine();
		if (line == NULL) break;

		if (swscanf (line, L"%d:%d:%d%c%n",&a1, &a2, &a3, &separator, &plen) != 4)
		{
			continue;
		}

		wchar_t *p = line + plen;
		if (*p)
		{
			CSubtitleItem *item = subtitle->NewItem();
			int32_t i = 0;
			while(*p)
			{
				if (*p == L'|')
				{
					Buffer[i] = 0;
					item->AddLine(Buffer);
					i = 0;
				}
				else
				{
					Buffer[i] = *p;
					i++;
				}
				*p ++;
			}

			if (i)
			{
				Buffer[i] = 0;
				item->AddLine(Buffer);
			}

			if (item->m_Count)
			{
				item->m_StartTime = (a1 * 3600 + a2 * 60 + a3) * 1000000ll;
			}
			else
			{
				subtitle->DeleteItem(item->m_index);
			}
		}
	}

	if (subtitle->m_Count) return subtitle;
	delete subtitle;
	return NULL;
}
