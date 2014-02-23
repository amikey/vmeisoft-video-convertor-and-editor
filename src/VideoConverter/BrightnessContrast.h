#pragma once
#include "VideoFilters.h"
#include "VideoFilter.h"

class CBrightnessContrast :
	public CVideoFilterBase
{
public:
	CBrightnessContrast(VideoFilterTemplate *pTemplate, CVideoFilterData *pData);
	virtual ~CBrightnessContrast(void);
	static CVideoFilterBase* CreateInstance(VideoFilterTemplate *pTemplate, CVideoFilterData *pData);

private:
	BrightnessContrastParams*	m_Params;
	BrightnessContrast*			m_BrightnessContrast;
	Gdiplus::Bitmap*			m_Bitmap;
	int32_t							m_Width, m_Height;
	void*						m_Source;

public:
	virtual HRESULT SetParameters(void *Param, int32_t length);
	virtual void PutSourceSize(int32_t width, int32_t height);
	virtual void Process(uint8_t *source, int64_t Pts);
};

