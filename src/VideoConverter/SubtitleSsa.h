#pragma once
#include "Subtitle.h"
#include "SubtitleReader.h"

class CSubtitleSsaReader:
	public CSubtitleReader
{
private:
	int32_t		m_retry;

public:
	virtual BOOL Probe(wchar_t *text);
	virtual CSubtitle* ReadSubTitle(CSubtitleText *reader);

public:
	static CSubtitleReader* CreateInstance();
};