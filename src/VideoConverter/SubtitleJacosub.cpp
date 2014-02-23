#include "stdafx.h"
#include "VideoConverter.h"
#include "SubtitleJacosub.h"

// http://unicorn.us.com/jacosub/jscripts.html
/*
0:00:10.11 0:00:12.00 D {fudo-ikiteru} It's alive!
0:00:10.11 0:00:12.00 D {fudo-ikiteru}It's alive!
0:00:10.11 0:00:12.00 D It's alive!{line doesn't start with a comment}
0:00:10.11 0:00:12.00 {fudo-ikiteru} It's alive!{starts with a comment}
0:00:10.11 0:00:12.00 [default] {fudo-ikiteru} It's alive!\
               {this assumes the D directive was named to "default"}*/

CSubtitleReader* CSubtitleJacosubReader::CreateInstance()
{
	CSubtitleJacosubReader *reader =  new CSubtitleJacosubReader();
	return reader;
}

BOOL CSubtitleJacosubReader::Probe(wchar_t *text)
{
	int32_t v;
	wchar_t t[4096];
	return swscanf(text, L"%u:%u:%u.%u %u:%u:%u.%u %s", &v, &v, &v, &v, &v, &v, &v, &v, t) == 9;
}


CSubtitle* CSubtitleJacosubReader::ReadSubTitle(CSubtitleText *reader)
{
	int32_t l = 0;
	int32_t a1, a2, a3, a4, b1, b2, b3, b4;
	wchar_t s[4096];
	wchar_t Buffer[MAX_SUBTITLE_LINE_SIZE];
	CSubtitle *subtitle = new CSubtitle();
	CSubtitleItem *item = NULL;
	for(;;)
	{
		wchar_t *line = reader->ReadLine();
		if (line == NULL) break;

		if (swscanf (line, L"%u:%u:%u.%u %u:%u:%u.%u %s %n",
			&a1, &a2, &a3, &a4,
			&b1, &b2, &b3, &b4,
			s, &l) < 9)
		{
			continue;
		}

		int32_t i = 0;
		if (item == NULL) item = subtitle->NewItem();
		for(;;)
		{
			int32_t skip = 0;
			wchar_t *p = &line[l];
			while (*p)
			{
				if ((*p == '}') || (*p == ']'))
				{
					skip = 0; p++;
					continue;
				}
				if ((*p == '{') || (*p == '['))
				{
					skip = 1; p++;
					continue;
				}
				if(skip) 
				{
					continue;
				}

				if (*p == L'\\')
				{
					p++;
					switch(*p)
					{
					case 'n':
						{
							Buffer[i] = 0;
							i = 0;
							item->AddLine(Buffer);
						}
						break;
					}
					p++;
				}
				else
				{
					if (i < MAX_SUBTITLE_LINE_SIZE - 1)
					{
						Buffer[i] = *p;
						i++;
					}
					p++;
				}
			}

			if (p[-1] != '\\') break;
			line = reader->ReadLine();
			if (line == NULL) break;
			l = 0;
			while (line[l] == L' ') l++;
		}

		if (i)
		{
			Buffer[i] = 0;
			i = 0;
			item->AddLine(Buffer);
		}

		if (item->m_Count)
		{
			item->m_StartTime = (a1*3600 + a2*60 + a3) * AV_TIME_BASE_LL + a4 * AV_TIME_BASE_LL / 100;
			item->m_Duration = (b1*3600 + b2*60 + b3) * AV_TIME_BASE_LL + a4 * AV_TIME_BASE_LL / 100 - item->m_StartTime;
			item = NULL;
		}
	}

	if (item) subtitle->DeleteItem(item->m_index);
	if (subtitle->m_Count) return subtitle;
	delete subtitle;
	return NULL;
}
