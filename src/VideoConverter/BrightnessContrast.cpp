#include "stdafx.h"
#include "BrightnessContrast.h"

CVideoFilterBase* CBrightnessContrast::CreateInstance(VideoFilterTemplate *pTemplate, CVideoFilterData *pData)
{
	CBrightnessContrast *filter = new CBrightnessContrast(pTemplate, pData);
	return filter;
}

CBrightnessContrast::CBrightnessContrast(VideoFilterTemplate *pTemplate, CVideoFilterData *pData)
	:CVideoFilterBase(pTemplate, pData)
{
	m_Params = (BrightnessContrastParams *)pData->m_Parameter;
	if (CVideoFilterBase::IsValid())
	{
		m_BrightnessContrast = new BrightnessContrast();
		m_BrightnessContrast->SetParameters(m_Params);
	}
}

CBrightnessContrast::~CBrightnessContrast(void)
{
	SAFE_DELETE(m_BrightnessContrast);
	SAFE_DELETE(m_Bitmap);
}

HRESULT CBrightnessContrast::SetParameters(void *Param, int32_t length)
{
	if (CVideoFilterBase::SetParameters(Param, length)) return E_FAIL;
	if (CVideoFilterBase::IsValid())
	{
		if (m_BrightnessContrast == NULL) m_BrightnessContrast = new BrightnessContrast();
		m_BrightnessContrast->SetParameters(m_Params);
	}
	else
	{
		SAFE_DELETE(m_BrightnessContrast);
	}
	return S_OK;
}

void CBrightnessContrast::PutSourceSize(int32_t width, int32_t height)
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

void CBrightnessContrast::Process(uint8_t *source, int64_t Pts)
{
	if (m_BrightnessContrast )
	{
		if (source != m_Source)
		{
			SAFE_DELETE(m_Bitmap);
			m_Bitmap = new Bitmap(m_Width, m_Height, m_Width * 4, PixelFormat32bppRGB, (BYTE*)source);
			m_Source = source;
		}
		m_Bitmap->ApplyEffect(m_BrightnessContrast, NULL);
	}
	m_next->Process(source, Pts);
}
