#include "stdafx.h"
#include "VideoConverter.h"
#include "SubtitleSsa.h"

// http://en.wikipedia.org/wiki/SubStation_Alpha
// http://wiki.multimedia.cx/index.php?title=SubStation_Alpha

#define MAX_ITEM_COUNTS		256

static const wchar_t *Events = L"[Events]";

static wchar_t *RemoveSpace(wchar_t *p)
{
	while (*p == L' ') p++;
	int32_t i = wcslen(p) - 1;
	while (p[i] == L' '){p[i] = 0; i--;}
	return p;
}

static int32_t SplitFormatStrings(wchar_t *p, wchar_t *items[])
{
	int32_t count = 1;
	items[0] = p;
	while ((p = wcschr(p, L',')) != NULL)
	{
		*p = 0;
		p ++;
		items[count] = p;
		count ++;
		if (count >= MAX_ITEM_COUNTS) break;
	}
	for(int i = 0; i < count; i++)
	{
		items[i] = RemoveSpace(items[i]);
	}
	return count;
}

static BOOL SplitDialogueStrings(wchar_t *p, wchar_t *items[], int32_t count)
{
	if (_wcsnicmp(p, L"Dialogue: ", 10) != 0)
	{
		return FALSE;
	}
	p += 10;
	
	items[0] = p;
	for(int i = 1; i < count; i++)
	{
		p = wcschr(p, L',');
		if (p == NULL) return false;
		*p = 0; p++;
		items[i] = p;
	}
	return TRUE;
}

static void SplitText(CSubtitleItem *item, wchar_t *p)
{
	wchar_t szText[MAX_SUBTITLE_LINE_SIZE + 8];
	int32_t c = 0;
	while (*p)
	{
		switch(*p)
		{
		case L'{':
			while(*p)
			{
				if (*p == L'}')
				{
					p++;
					break;
				}
				p++;
			}
			break;

		case L'\\':
			{
				p ++;
				switch(*p)
				{
				case L'n':
				case L'N':
					{
						szText[c] = 0;
						item->AddLine(szText);
						c = 0;
					}
					break;

				case L'\\':
					{
						if (c < MAX_SUBTITLE_LINE_SIZE - 1)
						{
							szText[c] = L'\\';
							c++;
						}
					}
					break;
				}
				p++;
			}
			break;

		default:
			if (c < MAX_SUBTITLE_LINE_SIZE - 1)
			{
				szText[c] = *p;
				c++;
			}
			p++;
			break;
		}
	}

	if (c)
	{
		szText[c] = 0;
		item->AddLine(szText);
	};
}

CSubtitleReader * CSubtitleSsaReader::CreateInstance()
{
	CSubtitleSsaReader *reader =  new CSubtitleSsaReader();
	return reader;
}

BOOL CSubtitleSsaReader::Probe(wchar_t *text)
{
	if ((m_retry == 0) && (_wcsicmp(text, L"[Script Info]") == 0))
	{
		return TRUE;
	}
	m_retry ++;
	return FALSE;
}

static int64_t read_ts(wchar_t* p)
{
	int32_t h, m, s, ms;
	if (swscanf(p, L"%d:%d:%d.%d", &h, &m, &s, &ms) == 4) 
	{
		return (h * 3600LL + m * 60LL + s) * AV_TIME_BASE_LL + ms * AV_TIME_BASE_LL / 100ll;
	}
	return -1;
}

CSubtitle* CSubtitleSsaReader::ReadSubTitle(CSubtitleText *reader)
{
	int32_t bEvents = 0;
	for(;;)
	{
		if (_wcsicmp(reader->ReadLine(), Events) == 0)
		{
			bEvents = 1;
			break;
		}
	}
	if (bEvents == 0) return NULL;

	wchar_t *items[MAX_ITEM_COUNTS];
	int32_t FormatCount = 0;
	for(int i = 0; i < 8; i++)
	{
		wchar_t *line = reader->ReadLine();
		if (line == NULL) return NULL;
		if (_wcsnicmp(line, L"Format: ", 8) == 0)
		{
			FormatCount = SplitFormatStrings(line + 8, items);
			break;
		}
	}
	if (FormatCount < 4) return NULL;

	int32_t Start = -1, End = -1, Text = -1;
	for(int i = 1; i < FormatCount; i++)
	{
		if (_wcsicmp(items[i], L"Start") == 0)
		{
			Start = i;
		}
		else if (_wcsicmp(items[i], L"End") == 0)
		{
			End = i;
		}
		else if (_wcsicmp(items[i], L"Text") == 0)
		{
			Text = i;
		}
	}

	if ((Start == -1) || (End == -1) || (Text == -1))
	{
		return NULL;
	}

	CSubtitle *subtitle = new CSubtitle();
	for(;;)
	{
		wchar_t *line = reader->ReadLine();
		if (line == NULL) break;

		if (! SplitDialogueStrings(line, items, FormatCount)) continue;
		int64_t StartTime = read_ts(items[Start]); if (StartTime == -1) continue;
		int64_t StopTime = read_ts(items[End]); if (StopTime == -1) continue;

		CSubtitleItem *item = subtitle->NewItem();
		SplitText(item, items[Text]);
		if (item->m_Count == 0)
		{
			subtitle->DeleteItem(item->m_index);
		}
		else
		{
			item->m_StartTime = StartTime;
			item->m_Duration = StopTime - StartTime;
		}
	}

	if (subtitle->m_Count) return subtitle;
	delete subtitle;
	return NULL;
}
