#pragma once
#include "VideoFilters.h"
#include "VideoFilter.h"

class CCropFilter :
	public CVideoFilterBase
{
public:
	CCropFilter(VideoFilterTemplate *pTemplate, CVideoFilterData *pData);
	virtual ~CCropFilter(void);
	static CVideoFilterBase* CreateInstance(VideoFilterTemplate *pTemplate, CVideoFilterData *pData);

private:
	Rect*		m_Crop;
	int32_t			m_SourceOffset;
	int32_t			m_SourceStride;
	int32_t			m_DestStride;
	uint8_t*	m_Buffer;

public:
	virtual uint8_t* GetBuffer();
	virtual HRESULT SetParameters(void *Param, int32_t length);
	virtual void PutSourceSize(int32_t width, int32_t height);
	virtual void Process(uint8_t *source, int64_t Pts);
};

