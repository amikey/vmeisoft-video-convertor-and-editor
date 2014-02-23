#include "stdafx.h"
#include "VideoConverter.h"
#include "SubtitleGoogle.h"

CSubtitleReader* CSubtitleGoogleReader::CreateInstance()
{
	CSubtitleGoogleReader *reader =  new CSubtitleGoogleReader();
	return reader;
}

BOOL CSubtitleGoogleReader::Probe(wchar_t *text)
{
	if (bProbe) return false;
	bProbe = true;
	return wcsstr(text, L"<?xml version=\"1.0\" encoding=\"utf-8\" ?><transcript>") != NULL;
}

CSubtitle* CSubtitleGoogleReader::ReadSubTitle(CSubtitleText *reader)
{
	float start, stop;
	CSubtitle *subtitle = new CSubtitle();
	for(;;)
	{
		wchar_t *line = reader->Read(L"<", L">");
		if (line == NULL) break;

		if (swscanf (line, L"<text start=\"%lf\" dur=\"%lf\"", &start, &stop) < 10)
		{
			continue;
		}

		line = reader->ReadUntil(L"<");
		if (*line)
		{
			CSubtitleItem *item = subtitle->NewItem();
			item->m_StartTime = start * AV_TIME_BASE_LL;
			item->m_Duration = (stop - start) * AV_TIME_BASE_LL;
			item->AddLine(line);
		}
	}

	if (subtitle->m_Count) return subtitle;
	delete subtitle;
	return NULL;
}
