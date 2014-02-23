#pragma once
#include "VideoFilters.h"
#include "VideoFilter.h"

extern wchar_t *__stdcall RotateFlipString(int32_t RotateFlip);
extern int32_t __stdcall CalcRotateFlip(int32_t Value, int32_t nRotateFlip);
extern bool RotateExchanged(int32_t nRotateFlip);
extern void RotateFlipRect(Rect &r, int32_t Width, int32_t Height, int32_t nRotateFlip);
extern void RotateFlip32(uint32_t *d, uint32_t *s, int32_t w, int32_t h, int32_t nRotateFlip);
extern void RotateFlipYUV420(uint8_t *d[3], uint8_t *s[3], int32_t w, int32_t h, int32_t nRotateFlip);

class CRotateFlipFilter :
	public CVideoFilterBase
{
public:
	CRotateFlipFilter(VideoFilterTemplate *pTemplate, CVideoFilterData *pData);
	virtual ~CRotateFlipFilter(void);
	static CVideoFilterBase* CreateInstance(VideoFilterTemplate *pTemplate, CVideoFilterData *pData);

private:
	int32_t*		m_Value;
	int32_t			m_Width;
	int32_t			m_Height;
	uint8_t*	m_Buffer;

public:
	virtual uint8_t* GetBuffer();
	virtual void PutSourceSize(int32_t width, int32_t height);
	virtual void Process(uint8_t *source, int64_t Pts);
};

