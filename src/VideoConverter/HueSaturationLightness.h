#pragma once
#include "VideoFilters.h"
#include "VideoFilter.h"

class CHueSaturationLightness :
	public CVideoFilterBase
{
public:
	CHueSaturationLightness(VideoFilterTemplate *pTemplate, CVideoFilterData *pData);
	virtual ~CHueSaturationLightness(void);
	static CVideoFilterBase* CreateInstance(VideoFilterTemplate *pTemplate, CVideoFilterData *pData);

private:
	HueSaturationLightness*		m_HueSaturationLightness;
	Gdiplus::Bitmap*			m_Bitmap;
	int32_t							m_Width, m_Height;
	void*						m_Source;

public:
	virtual HRESULT SetParameters(void *Param, int32_t length);
	virtual void PutSourceSize(int32_t width, int32_t height);
	virtual void Process(uint8_t *source, int64_t Pts);

};

