#include "stdafx.h"
#include "WatermarkFilter.h"
#include "VideoConverter.h"
#include "VideoUtils.h"

static uint8_t*		g_LogoBuffer;
static int32_t			g_LogoWidth;
static int32_t			g_LogoHeight;
static int32_t			g_LogoAlpha;
static wchar_t		g_LogoPicture[MAX_PATH];

void RecreateWatermark()
{

	if (
		(g_LogoWidth == g_opt.m_LogoRect.Width) &&
		(g_LogoHeight == g_opt.m_LogoRect.Height) &&
		(g_LogoAlpha == g_opt.m_LogoAlpha) &&
		(lstrcmp(g_LogoPicture, g_opt.m_LogoFile) == 0)
		)
	{
		return ;
	}

	SAFE_FREE(g_LogoBuffer);

	if (g_opt.m_LogoFile[0] == 0) return ;
	if (
		(g_opt.m_LogoRect.X + g_opt.m_LogoRect.Width > g_enc_opt.m_VideoWidth) ||
		(g_opt.m_LogoRect.Y + g_opt.m_LogoRect.Height > g_enc_opt.m_VideoHeight)
		)
	{
		g_opt.m_LogoFile[0] = 0;
		return;
	}
	g_LogoBuffer = LoadFileToMemory(g_opt.m_LogoFile, g_opt.m_LogoRect.Width, g_opt.m_LogoRect.Height);
	if (g_LogoBuffer == NULL)
	{
		g_opt.m_LogoFile[0] = 0;
		return;
	}
	if ((g_opt.m_LogoAlpha > 0) && (g_opt.m_LogoAlpha < 255))
	{
		ChangeAlpha(g_LogoBuffer, 255 - g_opt.m_LogoAlpha, g_opt.m_LogoRect.Width * g_opt.m_LogoRect.Height);
	}

	g_LogoWidth = g_opt.m_LogoRect.Width;
	g_LogoHeight = g_opt.m_LogoRect.Height;
	g_LogoAlpha = g_opt.m_LogoAlpha;
	wcscpy(g_LogoPicture, g_opt.m_LogoFile);
}

int32_t CheckWaterValues()
{
	if (g_opt.m_LogoFile[0] == 0) return 0;
	switch(g_opt.m_LogoHPosition)
	{
	case LogoAlignmentNear:
		g_opt.m_LogoRect.X = 0;
		break;

	case LogoAlignmentCenter:
		g_opt.m_LogoRect.X = (g_enc_opt.m_VideoWidth - g_opt.m_LogoRect.Width) / 2;
		break;

	case LogoAlignmentFar:
		g_opt.m_LogoRect.X = g_enc_opt.m_VideoWidth - g_opt.m_LogoRect.Width;
		break;
	}
	switch(g_opt.m_LogoVPosition)
	{
	case LogoAlignmentNear:
		g_opt.m_LogoRect.Y = 0;
		break;

	case LogoAlignmentCenter:
		g_opt.m_LogoRect.Y = (g_enc_opt.m_VideoHeight - g_opt.m_LogoRect.Height) / 2;
		break;

	case LogoAlignmentFar:
		g_opt.m_LogoRect.Y = g_enc_opt.m_VideoHeight - g_opt.m_LogoRect.Height;
		break;
	}
	if (
		(g_opt.m_LogoRect.X < 0) || (g_opt.m_LogoRect.X >= g_enc_opt.m_VideoWidth) ||
		(g_opt.m_LogoRect.Y < 0) || (g_opt.m_LogoRect.Y >= g_enc_opt.m_VideoHeight) ||
		(g_opt.m_LogoRect.Width < 1) || (g_opt.m_LogoRect.Width >= g_enc_opt.m_VideoWidth) ||
		(g_opt.m_LogoRect.Height < 1) || (g_opt.m_LogoRect.Height >= g_enc_opt.m_VideoHeight) ||
		(g_opt.m_LogoRect.X + g_opt.m_LogoRect.Width > g_enc_opt.m_VideoWidth) ||
		(g_opt.m_LogoRect.Y + g_opt.m_LogoRect.Height > g_enc_opt.m_VideoHeight)
		)
	{
		g_opt.m_LogoFile[0] = 0;
		SAFE_FREE(g_LogoBuffer);
		return -1;
	}
	return 0;
}

void __stdcall DrawLogoSample(BYTE *Buffer)
{
	CheckWaterValues();
	ZeroMemory(Buffer, g_enc_opt.m_VideoWidth * g_enc_opt.m_VideoHeight * 4);
	RecreateWatermark();
	if (g_LogoBuffer)
	{
		int32_t Offset = (g_opt.m_LogoRect.Y * g_enc_opt.m_VideoWidth + g_opt.m_LogoRect.X) * 4;
		bend_32bpp(Buffer + Offset, g_enc_opt.m_VideoWidth * 4, g_LogoBuffer, g_LogoWidth, g_LogoHeight);
	}
}

CVideoFilterBase* CWatermarkFilter::CreateInstance(VideoFilterTemplate *pTemplate, CVideoFilterData *pData)
{
	CWatermarkFilter *filter = new CWatermarkFilter(pTemplate, pData);
	return filter;
}

CWatermarkFilter::CWatermarkFilter(VideoFilterTemplate *pTemplate, CVideoFilterData *pData)
	:CVideoFilterBase(pTemplate, pData)
{
	m_FilterType = VIDEO_FILTER_TYPE_LOGO;
}

CWatermarkFilter::~CWatermarkFilter(void)
{
}

void CWatermarkFilter::PutSourceSize(int32_t width, int32_t height)
{
	CheckWaterValues();
	m_SourceOffset = (g_opt.m_LogoRect.Y * width + g_opt.m_LogoRect.X) * 4;
	m_SourceStride = width * 4;
	RecreateWatermark();
	CVideoFilterBase::PutSourceSize(width, height);
}

void CWatermarkFilter::Process(uint8_t *source, int64_t Pts)
{
	if (g_LogoBuffer) bend_32bpp(source + m_SourceOffset, m_SourceStride, g_LogoBuffer, g_LogoWidth, g_LogoHeight);
	m_next->Process(source, Pts);
}
