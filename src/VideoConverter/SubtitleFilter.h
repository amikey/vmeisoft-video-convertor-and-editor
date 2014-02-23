#pragma once
#include "videofilter.h"

class CSubtitle;
class CSubtitleFilter:
	public CVideoFilterBase
{
private:
	CSubtitle* m_Subtitle;
	int32_t	m_Width, m_Height;
	Gdiplus::Bitmap* m_Bitmap;
	Gdiplus::Graphics* m_Graphics;
	uint8_t* m_Source;

public:
	CSubtitleFilter(CSubtitle *pSubtitle);
	virtual ~CSubtitleFilter(void);

public:
	void SetSubtitle(CSubtitle* subtitle);

public:
	virtual void PutSourceSize(int32_t width, int32_t height);
	virtual void Process(uint8_t *source, int64_t Pts);
};
