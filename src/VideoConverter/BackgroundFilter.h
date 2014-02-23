#pragma once
#include "videofilter.h"

#define BACKGROUND_NAME		L"Background"

// {26306F39-51D4-483F-BFEB-51599CD99F01}
static const GUID IID_Background = 
{ 0x26306f39, 0x51d4, 0x483f, { 0xbf, 0xeb, 0x51, 0x59, 0x9c, 0xd9, 0x9f, 0x1 } };


#define BK_TYPE_FULL		0
#define BK_TYPE_COPY		1
#define BK_TYPE_STRETCH		2

class CFastStretchBlt;

class CBackgroundFilter :
	public CVideoFilterBase
{
public:
	CBackgroundFilter(VideoFilterTemplate *pTemplate, CVideoFilterData *pData);
	virtual ~CBackgroundFilter(void);
	static CVideoFilterBase* CreateInstance(VideoFilterTemplate *pTemplate, CVideoFilterData *pData);

private:
	int32_t						m_Type;
	int32_t						m_srcWidth, m_srcHeight;
	int32_t						m_dstWidth, m_dstHeight;
	uint8_t*				m_Buffer;
	int32_t						m_srcOffset;
	int32_t						m_dstOffset;
	CFastStretchBlt*		m_Blt;
	int32_t						m_viewWidth, m_viewHeight;

public:
	virtual void PutSourceSize(int32_t width, int32_t height);
	virtual uint8_t* GetBuffer();
	virtual void Process(uint8_t *source, int64_t Pts);
};

