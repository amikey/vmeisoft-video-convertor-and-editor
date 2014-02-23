#pragma once

#include "Subtitle.h"
#include "SubtitleReader.h"

class CSubtitleSubripReader:
	public CSubtitleReader
{
public:
	virtual BOOL Probe(wchar_t *text);
	virtual CSubtitle* ReadSubTitle(CSubtitleText *reader);

public:
	static CSubtitleReader * CreateInstance();
};