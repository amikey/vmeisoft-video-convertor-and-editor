#include "stdafx.h"
#include "VideoConverter.h"
#include "SubtitleWebvtt.h"

// http://dev.w3.org/html5/webvtt/

/*
WEBVTT

00:01.000 --> 00:04.000
Never drink liquid nitrogen.

00:05.000 --> 00:09.000
¡ª It will perforate your stomach.
¡ª You could die.
*/

/*
WEBVTT

00:11.000 --> 00:13.000
<v Roger Bingham>We are in New York City

00:13.000 --> 00:16.000
<v Roger Bingham>We're actually at the Lucern Hotel, just down the street

00:16.000 --> 00:18.000
<v Roger Bingham>from the American Museum of Natural History
*/

CSubtitleReader* CSubtitleWebvttReader::CreateInstance()
{
	CSubtitleWebvttReader *reader =  new CSubtitleWebvttReader();
	return reader;
}

BOOL CSubtitleWebvttReader::Probe(wchar_t *text)
{
	if (bProbe) return FALSE;
	bProbe = TRUE;
	return _wcsicmp(text, L"WEBVTT") == 0;
}

static int64_t read_ts(const wchar_t *s)
{
    int32_t hh, mm, ss, ms;
	if (swscanf(s, L"%u:%u:%u.%u", &hh, &mm, &ss, &ms) == 4) return (hh*3600LL + mm*60LL + ss) * AV_TIME_BASE_LL + ms * AV_TIME_BASE_LL / 1000ll;
    if (swscanf(s,    L"%u:%u.%u",      &mm, &ss, &ms) == 3) return (            mm*60LL + ss) * AV_TIME_BASE_LL + ms * AV_TIME_BASE_LL / 1000ll;
    return -1;
}

wchar_t* CSubtitleWebvttReader::FormatText(wchar_t *t)
{
	int32_t skip = 0;
	int32_t i = 0;
	while (*t)
	{
		if (*t == '>')
		{
			skip = 0; t++;
			continue;
		}
		if (*t == '<')
		{
			skip = 1; t++;
			continue;
		}
		if(skip) 
		{
			continue;
		}
		Buffer[i] = *t;
		i++;
		if (i >= MAX_SUBTITLE_LINE_SIZE - 1) break;
	}
	Buffer[i] = 0;
	return Buffer;
}

CSubtitle* CSubtitleWebvttReader::ReadSubTitle(CSubtitleText *reader)
{
	int64_t start, stop;
	CSubtitle *subtitle = new CSubtitle();
	CSubtitleItem *item = NULL;
	for(;;)
	{
		wchar_t *text = reader->ReadLine();
		if (text == NULL) break;

		start = read_ts(text);
		if (start == -1) continue;

		text = wcsstr(text, L"-->");
		if (text == NULL) continue;
		stop = read_ts(text);
		if (stop == -1) continue;

		int32_t i = 0;
		if (item == NULL) item = subtitle->NewItem();
		item->m_StartTime = start;
		item->m_Duration = stop - start;
		for(;;)
		{
			text = reader->ReadLine();
			if (text == NULL) break;
			if (*text == NULL) break;
			item->AddLine(text);
		}

		if (item->m_Count)
		{
			item = NULL;
		}
	}

	if (item) subtitle->DeleteItem(item->m_index);
	if (subtitle->m_Count) return subtitle;
	delete subtitle;
	return NULL;
}
