#pragma once

#define MAX_SUBTITLE_TEXT_SIZE		4096

class CSubtitleText
{
private:
	HANDLE			m_hFile;
	uint8_t*		m_Buffer;
	wchar_t*		m_Scrpits;

private:
	int32_t				m_cCharacter;
	int32_t				m_CurrentPos;
	wchar_t			m_TextBuffer[MAX_SUBTITLE_TEXT_SIZE + 8];

public:
	wchar_t* ReadLine();
	wchar_t* ReadUntil(wchar_t *end);
	wchar_t* Read(wchar_t *from, wchar_t *to);

public:
	int32_t GetCharacterCount(){return m_cCharacter;};
	void Reset(){m_CurrentPos = 0;};

public:
	CSubtitleText(wchar_t *szFile);
	~CSubtitleText();
};

class CSubtitleReader
{
public:
	virtual BOOL Probe(wchar_t *text) = 0;
	virtual CSubtitle* ReadSubTitle(CSubtitleText *reader) = 0;

public:
	CSubtitleReader();
	virtual ~CSubtitleReader();
};


extern CSubtitle *LoadTextSubtitle(wchar_t *szFile);
