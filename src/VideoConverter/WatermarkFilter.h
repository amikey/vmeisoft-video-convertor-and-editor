#pragma once
#include "VideoFilters.h"
#include "VideoFilter.h"

#define WATERMARK_NAME		L"Watermark"

// {D55AD9BA-DEF4-4283-B6B6-04C41C6FD506}
static const GUID IID_Watermark = 
{ 0xd55ad9ba, 0xdef4, 0x4283, { 0xb6, 0xb6, 0x4, 0xc4, 0x1c, 0x6f, 0xd5, 0x6 } };


class CWatermarkFilter :
	public CVideoFilterBase
{
public:
	CWatermarkFilter(VideoFilterTemplate *pTemplate, CVideoFilterData *pData);
	virtual ~CWatermarkFilter(void);
	static CVideoFilterBase* CreateInstance(VideoFilterTemplate *pTemplate, CVideoFilterData *pData);

private:
	int32_t			m_SourceOffset;
	int32_t			m_SourceStride;

public:
	virtual void PutSourceSize(int32_t width, int32_t height);
	virtual void Process(uint8_t *source, int64_t Pts);
};

