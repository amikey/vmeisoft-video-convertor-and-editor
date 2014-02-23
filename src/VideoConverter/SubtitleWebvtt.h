#pragma once

#include "Subtitle.h"
#include "SubtitleReader.h"

class CSubtitleWebvttReader:
	public CSubtitleReader
{
private:
	BOOL bProbe;
	wchar_t Buffer[MAX_SUBTITLE_LINE_SIZE];

private:
	wchar_t* FormatText(wchar_t *t);

public:
	virtual BOOL Probe(wchar_t *text);
	virtual CSubtitle* ReadSubTitle(CSubtitleText *reader);

public:
	static CSubtitleReader * CreateInstance();
};