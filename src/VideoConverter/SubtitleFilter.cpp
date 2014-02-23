#include "stdafx.h"
#include "VideoConverterInt.h"
#include "SubtitleFilter.h"
#include "Subtitle.h"

// {047D9E9D-FE20-4FC8-A1E3-34E3F4B228D8}
static const GUID IID_Subtitle = 
{ 0x47d9e9d, 0xfe20, 0x4fc8, { 0xa1, 0xe3, 0x34, 0xe3, 0xf4, 0xb2, 0x28, 0xd8 } };
static struct VideoFilterTemplate g_subtitle_template = {L"Video Player", IID_Subtitle, NULL, VET_UTILITY, NULL, 0};

CSubtitleFilter::CSubtitleFilter(CSubtitle *pSubtitle)
	:CVideoFilterBase(&g_subtitle_template, NULL)
{
	m_FilterType = VIDEO_FILTER_TYPE_SUBTITLE;
	m_Subtitle = pSubtitle;
}

CSubtitleFilter::~CSubtitleFilter(void)
{
}

void CSubtitleFilter::PutSourceSize(int32_t width, int32_t height)
{
	m_Width = width; m_Height = height;
	m_Subtitle->CalcParamters(m_Width, m_Height);
	SAFE_DELETE(m_Bitmap);
	SAFE_DELETE(m_Graphics);
	CVideoFilterBase::PutSourceSize(width, height);
}

void CSubtitleFilter::Process(uint8_t *source, int64_t Pts)
{
	for(int i = 0; i < m_Subtitle->m_Count; i++)
	{
		CSubtitleItem *item = m_Subtitle->m_Items[i];
		if ((item->m_StartTime <= Pts) && (item->m_StartTime + item->m_Duration >= Pts))
		{
			if ((m_Bitmap == NULL) || (m_Source != source))
			{
				SAFE_DELETE(m_Bitmap);
				SAFE_DELETE(m_Graphics);
				m_Source = source;
				m_Bitmap = new Bitmap(m_Width, m_Height, m_Width * 4, PixelFormat32bppRGB, (BYTE*)m_Source);
				m_Graphics = new Graphics(m_Bitmap);
				m_Graphics->SetPageUnit(Gdiplus::UnitPixel);
				m_Graphics->SetCompositingMode(Gdiplus::CompositingModeSourceOver);
				m_Graphics->SetInterpolationMode(Gdiplus::InterpolationModeHighQuality);
				m_Graphics->SetCompositingQuality(Gdiplus::CompositingQualityHighQuality);
				m_Graphics->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
				m_Graphics->SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
				m_Graphics->SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
			}
			item->DrawInto(m_Graphics, Pts, -1);
		}
	}
	m_next->Process(source, Pts);
}

void CSubtitleFilter::SetSubtitle(CSubtitle* subtitle)
{
	m_Subtitle = subtitle;
}
