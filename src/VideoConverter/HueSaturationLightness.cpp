#include "stdafx.h"
#include "HueSaturationLightness.h"

CVideoFilterBase* CHueSaturationLightness::CreateInstance(VideoFilterTemplate *pTemplate, CVideoFilterData *pData)
{
	CHueSaturationLightness *filter = new CHueSaturationLightness(pTemplate, pData);
	return filter;
}

CHueSaturationLightness::CHueSaturationLightness(VideoFilterTemplate *pTemplate, CVideoFilterData *pData)
	:CVideoFilterBase(pTemplate, pData)
{
	if (CVideoFilterBase::IsValid())
	{
		m_HueSaturationLightness = new HueSaturationLightness();
		m_HueSaturationLightness->SetParameters((HueSaturationLightnessParams *)pData->m_Parameter);
	}
}

CHueSaturationLightness::~CHueSaturationLightness(void)
{
	SAFE_DELETE(m_HueSaturationLightness);
	SAFE_DELETE(m_Bitmap);
}

HRESULT CHueSaturationLightness::SetParameters(void *Param, int32_t length)
{
	if (CVideoFilterBase::SetParameters(Param, length)) return E_FAIL;
	if (CVideoFilterBase::IsValid())
	{
		if (m_HueSaturationLightness == NULL) m_HueSaturationLightness = new HueSaturationLightness();
		m_HueSaturationLightness->SetParameters((HueSaturationLightnessParams *)Param);
	}
	else
	{
		SAFE_DELETE(m_HueSaturationLightness);
	}
	return S_OK;
}

void CHueSaturationLightness::PutSourceSize(int32_t width, int32_t height)
{
	if ((m_Width != width) || (m_Height != height))
	{
		m_Width = width;
		m_Height = height;
		m_Source = NULL;
		SAFE_DELETE(m_Bitmap);
	}
	CVideoFilterBase::PutSourceSize(width, height);
}

void CHueSaturationLightness::Process(uint8_t *source, int64_t Pts)
{
	if (m_HueSaturationLightness)
	{
		if (source != m_Source)
		{
			SAFE_DELETE(m_Bitmap);
			m_Bitmap = new Bitmap(m_Width, m_Height, m_Width * 4, PixelFormat32bppRGB, (BYTE*)source);
			m_Source = source;
		}
		m_Bitmap->ApplyEffect(m_HueSaturationLightness, NULL);
	}
	m_next->Process(source, Pts);
}
