#include "stdafx.h"
#include "VideoConverter.h"
#include "SubtitleReader.h"
#include "Subtitle.h"
#include "SubtitleMicrodvd.h"
#include "SubtitleSubrip.h"
#include "SubtitleSubviewer.h"
#include "SubtitleSami.h"
#include "SubtitleVPlayer.h"
#include "SubtitleRealText.h"
#include "SubtitleSsa.h"
#include "SubtitlePjs.h"
#include "SubtitleMpsub.h"
#include "SubtitleAqt.h"
#include "SubtitleJacosub.h"
#include "SubtitleMpl2.h"
#include "SubtitleGoogle.h"
#include "SubtitleWebvtt.h"

// http://en.wikipedia.org/wiki/Subtitle_(captioning)
// https://wiki.videolan.org/Subtitles/

#define MAX_SUBTITILE_FILE_SIZE (4 * 1024 *1024)

/*
char *sub_exts[] = {"utf", "utf8", "utf-8", "sub", "srt", "smi", "rt", "txt", "ssa", "aqt", "jss", "js", "ass", NULL};
*/

CSubtitleReader::CSubtitleReader()
{

}

CSubtitleReader::~CSubtitleReader()
{

}


CSubtitleText::CSubtitleText(wchar_t *szFile)
{
	m_hFile =  CreateFile(szFile,
			GENERIC_READ,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,                    
			NULL,                 
			OPEN_EXISTING,        
			FILE_ATTRIBUTE_NORMAL,
			NULL);
	if (m_hFile == INVALID_HANDLE_VALUE)
	{
		m_hFile = NULL;
		return ;
	}

	LARGE_INTEGER liSize;
	GetFileSizeEx(m_hFile, &liSize);

	if (liSize.QuadPart < 16) return ;
	if (liSize.QuadPart > MAX_SUBTITILE_FILE_SIZE) return ;

	int32_t fSize = liSize.LowPart;
	m_Buffer = (uint8_t *)MemoryAlloc(fSize + 8);
	DWORD NumberOfBytesRead = 0;
	ReadFile(m_hFile, m_Buffer, fSize, &NumberOfBytesRead, NULL);
	if (NumberOfBytesRead != fSize) return ;
	ZeroMemory(m_Buffer + fSize, 8);
	SAFE_CLOSE(m_hFile);

	if ((m_Buffer[0] == 0xFF) && (m_Buffer[1] == 0xFE))
	{
		m_Scrpits = (wchar_t *)MemoryAlloc(fSize + 8);
		wchar_t *s = (wchar_t *)&m_Buffer[2];
		while (*s)
		{
			if (*s != L'\r')
			{
				m_Scrpits[m_cCharacter] = *s;
				m_cCharacter ++;
			}
			s++;
		}
		ZeroMemory(&m_Scrpits[m_cCharacter], 8);
	}
	else if ((m_Buffer[0] == 0xEF) && (m_Buffer[1] == 0xBB) && (m_Buffer[2] == 0xBF))
	{
		int32_t len = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)m_Buffer + 3, fSize - 3, NULL, 0);
		if (len == 0) return ;
		m_Scrpits = (wchar_t *)MemoryAlloc(len * 2 + 8);
		m_cCharacter = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)m_Buffer + 3, fSize - 3, m_Scrpits, len * 2 + 2);
		if (m_cCharacter == 0) return ;
		ZeroMemory(&m_Scrpits[m_cCharacter], 8); 
		wchar_t *p1 = m_Scrpits;
		wchar_t *p2 = m_Scrpits;
		while(*p2)
		{
			if (*p2 != '\r')*p1++ = *p2;
			p2 ++;
		}
		ZeroMemory(p2, 8); 
	}
	else
	{
		INT unicode = IS_TEXT_UNICODE_UNICODE_MASK;
		INT unicodeReverse = IS_TEXT_UNICODE_REVERSE_MASK;
		INT notAscii = IS_TEXT_UNICODE_NOT_ASCII_MASK;
		if (((fSize & 1) == 0) && IsTextUnicode(m_Buffer, fSize, &unicode))
		{
			m_Scrpits = (wchar_t *)MemoryAlloc(fSize + 8);
			wchar_t *s = (wchar_t *)m_Buffer;
			while (*s)
			{
				if (*s != L'\r')
				{
					m_Scrpits[m_cCharacter] = *s;
					m_cCharacter ++;
				}
				s++;
			}
			ZeroMemory(&m_Scrpits[m_cCharacter], 8);
		}
		else if (((fSize & 1) == 0) && IsTextUnicode(m_Buffer, fSize, &unicodeReverse))
		{
			m_Scrpits = (wchar_t *)MemoryAlloc(fSize + 8);
			uint8_t *s = m_Buffer;
			while (*((uint16_t *)s))
			{
				wchar_t ch = (uint16_t)s[0] + ((uint16_t)s[1] << 8);
				if (ch != L'\r')
				{
					m_Scrpits[m_cCharacter] = ch;
					m_cCharacter ++;
				}
				s += 2;
			}
			ZeroMemory(&m_Scrpits[m_cCharacter], 8);
		}
		else if (IsTextUnicode(m_Buffer, fSize, &notAscii))
		{
			// 
		}
		else
		{
			int32_t len = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)m_Buffer, fSize - 3, NULL, 0);
			if (len == 0) return ;
			m_Scrpits = (wchar_t *)MemoryAlloc(len * 2 + 8);
			len = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)m_Buffer, fSize - 3, m_Scrpits, len * 2 + 8);
			if (len == 0) return ;
			if (m_cCharacter == 0) return ;
			ZeroMemory(&m_Scrpits[m_cCharacter], 8); 
			wchar_t *p1 = m_Scrpits;
			wchar_t *p2 = m_Scrpits;
			while(*p2)
			{
				if (*p2 != '\r')*p1++ = *p2;
				p2 ++;
			}
			ZeroMemory(p2, 8); 
		}
	}

	if (m_cCharacter == NULL)
	{
		return ;
	}
}

wchar_t* CSubtitleText::ReadLine()
{
	while (m_CurrentPos < m_cCharacter)
	{
		if (m_Scrpits[m_CurrentPos] != L' ') break;
		m_CurrentPos ++;
	}

	int32_t i = 0;
	while (m_CurrentPos < m_cCharacter)
	{
		switch(m_Scrpits[m_CurrentPos])
		{
		case L'\n':
			m_CurrentPos ++;
			m_TextBuffer[i] = 0;
			i --;
			while ((i >= 0) && (m_TextBuffer[i] == L' ')) {m_TextBuffer[i] = 0; i--;};
			ZeroMemory(&m_TextBuffer[i + 1], 8);
			return m_TextBuffer;

		default:
			if (i < MAX_SUBTITLE_TEXT_SIZE)
			{
				m_TextBuffer[i] = m_Scrpits[m_CurrentPos];
				i ++;
			}
			m_CurrentPos ++;
			break;
		}
	}

	if (i)
	{
		m_TextBuffer[i] = 0;
		i --;
		while ((i >= 0) && (m_TextBuffer[i] == L' ')) {m_TextBuffer[i] = 0; i--;};
		ZeroMemory(&m_TextBuffer[i + 1], 8);
		return NULL;
	}
	else
	{
		return NULL;
	}
}

wchar_t* CSubtitleText::ReadUntil(wchar_t *end)
{
	while (m_CurrentPos < m_cCharacter)
	{
		if (m_Scrpits[m_CurrentPos] != L' ') break;
		m_CurrentPos ++;
	}

	int32_t i = 0;
	int32_t l = wcslen(end);
	while (m_CurrentPos < m_cCharacter)
	{
		if (_wcsnicmp(&m_Scrpits[m_CurrentPos], end, l) == 0)
		{
			m_CurrentPos += l;
			m_TextBuffer[i] = 0;
			i --;
			while ((i >= 0) && (m_TextBuffer[i] == L' ')) {m_TextBuffer[i] = 0; i--;};
			ZeroMemory(&m_TextBuffer[i + 1], 8);
			return m_TextBuffer;
		}

		if (i < MAX_SUBTITLE_TEXT_SIZE)
		{
			m_TextBuffer[i] = m_Scrpits[m_CurrentPos];
			i ++;
		}
		m_CurrentPos ++;
	}

	return NULL;
}

wchar_t* CSubtitleText::Read(wchar_t *from, wchar_t *to)
{
	int32_t l = wcslen(from);
	while (m_CurrentPos < m_cCharacter)
	{
		if (_wcsnicmp(&m_Scrpits[m_CurrentPos], from, l) == 0)
		{
			m_CurrentPos += l;
			break;
		}
		m_CurrentPos ++;
	}

	int32_t i = 0;
	l = wcslen(to);
	while (m_CurrentPos < m_cCharacter)
	{
		if (_wcsnicmp(&m_Scrpits[m_CurrentPos], to, l) == 0)
		{
			m_CurrentPos += l;
			m_TextBuffer[i] = 0;
			i --;
			while ((i >= 0) && (m_TextBuffer[i] == L' ')) {m_TextBuffer[i] = 0; i--;};
			ZeroMemory(&m_TextBuffer[i + 1], 8);
			return m_TextBuffer;
		}

		if (i < MAX_SUBTITLE_TEXT_SIZE)
		{
			m_TextBuffer[i] = m_Scrpits[m_CurrentPos];
			i ++;
		}
		m_CurrentPos ++;
	}

	return NULL;
}


CSubtitleText::~CSubtitleText()
{
	SAFE_CLOSE(m_hFile);
	SAFE_FREE(m_Buffer);
	SAFE_FREE(m_Scrpits);
}

#define SUBTITLE_READER_FORMATS 14

typedef CSubtitleReader* (*PCreateSubtitleReaderInstance)();
static PCreateSubtitleReaderInstance g_subtitle_readers[SUBTITLE_READER_FORMATS] =
{
	// 特殊标志
	CSubtitleWebvttReader::CreateInstance,
	CSubtitleSamiReader::CreateInstance,
	CSubtitleRealTextReader::CreateInstance,
	CSubtitleSsaReader::CreateInstance,
	CSubtitleMpsubReader::CreateInstance,
	CSubtitleGoogleReader::CreateInstance,

	// 关键字
	CSubtitleMicroDVDReader::CreateInstance,
	CSubtitleSubripReader::CreateInstance,
	CSubtitleSubviewerReader::CreateInstance,
	CSubtitleVPlayerReader::CreateInstance,
	CSubtitlePjsReader::CreateInstance,
	CSubtitleAqtReader::CreateInstance,
	CSubtitleJacosubReader::CreateInstance,
	CSubtitleMpl2Reader::CreateInstance,
};

CSubtitle *LoadTextSubtitle(wchar_t *szFile)
{
	CSubtitleText* text = new CSubtitleText(szFile);
	if (text->GetCharacterCount() == 0)
	{
		delete text;
		return NULL;
	}

	CSubtitleReader *readers[SUBTITLE_READER_FORMATS];
	for(int i = 0; i < SUBTITLE_READER_FORMATS; i++)
	{
		readers[i] = g_subtitle_readers[i]();
	}

	CSubtitleReader *reader = NULL;
	for(int i = 0; i < 100; i++)
	{
		wchar_t *line = text->ReadLine();
		if (line == NULL) break;
		readers[i] = g_subtitle_readers[i]();
		for(int j = 0; j < SUBTITLE_READER_FORMATS; j++)
		{
			if (readers[j]->Probe(line))
			{
				reader = readers[j];
				break;
			}
		}
		if (reader) break;
	}

	CSubtitle *subtitle = NULL;
	if (reader) 
	{
		text->Reset();
		subtitle = reader->ReadSubTitle(text);
	}

	for(int i = 0; i < SUBTITLE_READER_FORMATS; i++)
	{
		delete readers[i];
	}
	delete text;

	if (subtitle)
	{
		subtitle->SortItem();
		subtitle->MergeTime();
	}
	return subtitle;
}
