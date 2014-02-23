#include "stdafx.h"
#include "VideoConverter.h"
#include "BackgroundFilter.h"
#include "VideoUtils.h"
#include "FastStretchBlt.h"

static uint32_t*	g_FillBuffer;
static int32_t			g_BkWidth;
static int32_t			g_BkHeight;
static FillType		g_FillType; // 背景填充类型
static uint32_t		g_FillColor1; // 背景色1
static uint32_t		g_FillColor2; // 背景色2
static FillDirect	g_FillDirect; // 背景色填充方向
static wchar_t		g_FillPicture[MAX_PATH]; // 图片的路径，当背景为图片时有效

static void RecreateBackground()
{
	if (g_opt.m_FillType == FillType_Solid)
	{
		g_FillType = FillType_Solid;
		SAFE_FREE (g_FillBuffer);
		g_BkWidth = g_enc_opt.m_VideoWidth;
		g_BkHeight = g_enc_opt.m_VideoHeight;
		g_FillColor1 = g_opt.m_FillColor1;
		return ;
	}
	else if (g_opt.m_FillType == FillType_Gradient)
	{
		if (
			(g_BkWidth == g_enc_opt.m_VideoWidth) &&
			(g_BkHeight == g_enc_opt.m_VideoHeight) &&
			(g_FillType == FillType_Gradient) &&
			(g_FillColor1 == g_opt.m_FillColor1) &&
			(g_FillColor2 == g_opt.m_FillColor2) &&
			(g_FillDirect == g_opt.m_FillDirect) &&
			(g_FillBuffer != NULL)
			)
		{
			return ;
		}

		g_FillType = FillType_Gradient;
		SAFE_FREE (g_FillBuffer);
		g_BkWidth = g_enc_opt.m_VideoWidth;
		g_BkHeight = g_enc_opt.m_VideoHeight;
		g_FillColor1 = g_opt.m_FillColor1;
		g_FillColor2 = g_opt.m_FillColor2;
		g_FillDirect = g_opt.m_FillDirect; 

		int32_t width = g_enc_opt.m_VideoWidth;
		int32_t height = g_enc_opt.m_VideoHeight;
		g_FillBuffer = (uint32_t *)MemoryAlloc(width * height * 4);

		switch(g_opt.m_FillDirect)
		{
			case FillDirect_Top:
			{
				uint32_t h = height - 1;
				for(uint32_t i = 0; i <= h; i++)
				{
					uint32_t r1 = (g_opt.m_FillColor1 >> 16) & 0xFF;
					uint32_t g1 = (g_opt.m_FillColor1 >> 8) & 0xFF;
					uint32_t b1 = (g_opt.m_FillColor1 >> 0) & 0xFF;
					uint32_t r2 = (g_opt.m_FillColor2 >> 16) & 0xFF;
					uint32_t g2 = (g_opt.m_FillColor2 >> 8) & 0xFF;
					uint32_t b2 = (g_opt.m_FillColor2 >> 0) & 0xFF;
					uint32_t r = (r1 * (h - i) + r2 * i) / h;
					uint32_t g = (g1 * (h - i) + g2 * i) / h;
					uint32_t b = (b1 * (h - i) + b2 * i) / h;
					uint32_t c = 0xFF000000 + (r << 16) + (g << 8) + b;
					uint32_t *p = g_FillBuffer + i * width;
					for(uint32_t j = 0; j < width; j++)
					{
						p[j] = c;
					}
				}
			}
				break;

			case FillDirect_Left:
			{
				uint32_t w = width - 1;
				for(uint32_t i = 0; i <= w; i++)
				{
					uint32_t r1 = (g_opt.m_FillColor1 >> 16) & 0xFF;
					uint32_t g1 = (g_opt.m_FillColor1 >> 8) & 0xFF;
					uint32_t b1 = (g_opt.m_FillColor1 >> 0) & 0xFF;
					uint32_t r2 = (g_opt.m_FillColor2 >> 16) & 0xFF;
					uint32_t g2 = (g_opt.m_FillColor2 >> 8) & 0xFF;
					uint32_t b2 = (g_opt.m_FillColor2 >> 0) & 0xFF;
					uint32_t r = (r1 * (w - i) + r2 * i) / w;
					uint32_t g = (g1 * (w - i) + g2 * i) / w;
					uint32_t b = (b1 * (w - i) + b2 * i) / w;
					uint32_t c = 0xFF000000 + (r << 16) + (g << 8) + b;
					uint32_t *p = g_FillBuffer + i;
					for(uint32_t j = 0; j < height; j++)
					{
						p[j * width] = c;
					}
				}

			}
				break;

			case FillDirect_TopLeft:
			{

				uint32_t R1 = (g_opt.m_FillColor1 >> 16) & 0xFF;
				uint32_t G1 = (g_opt.m_FillColor1 >> 8) & 0xFF;
				uint32_t B1 = (g_opt.m_FillColor1 >> 0) & 0xFF;
				uint32_t R2 = (g_opt.m_FillColor2 >> 16) & 0xFF;
				uint32_t G2 = (g_opt.m_FillColor2 >> 8) & 0xFF;
				uint32_t B2 = (g_opt.m_FillColor2 >> 0) & 0xFF;
				uint32_t R0 = (R1 + R2) / 2;
				uint32_t G0 = (G1 + G2) / 2;
				uint32_t B0 = (B1 + B2) / 2;

				uint32_t w = width - 1;
				uint32_t h = height - 1;
				for(uint32_t i = 0; i <= h; i++)
				{
					uint32_t r1 = (R1 * (h - i) + R0 * i) / h;
					uint32_t g1 = (G1 * (h - i) + G0 * i) / h;
					uint32_t b1 = (B1 * (h - i) + B0 * i) / h;

					uint32_t r2 = (R0 * (h - i) + R2 * i) / h;
					uint32_t g2 = (G0 * (h - i) + G2 * i) / h;
					uint32_t b2 = (B0 * (h - i) + B2 * i) / h;

					uint32_t *p = g_FillBuffer + i * width;
					for(uint32_t j = 0; j <= w; j++)
					{
						uint32_t r = (r1 * (w - j) + r2 * j) / w;
						uint32_t g = (g1 * (w - j) + g2 * j) / w;
						uint32_t b = (b1 * (w - j) + b2 * j) / w;
						uint32_t c = 0xFF000000 + (r << 16) + (g << 8) + b;
						p[j] = c;
					}
				}

			}
				break;

			case FillDirect_TopRight:
			{

				uint32_t R1 = (g_opt.m_FillColor1 >> 16) & 0xFF;
				uint32_t G1 = (g_opt.m_FillColor1 >> 8) & 0xFF;
				uint32_t B1 = (g_opt.m_FillColor1 >> 0) & 0xFF;
				uint32_t R2 = (g_opt.m_FillColor2 >> 16) & 0xFF;
				uint32_t G2 = (g_opt.m_FillColor2 >> 8) & 0xFF;
				uint32_t B2 = (g_opt.m_FillColor2 >> 0) & 0xFF;
				uint32_t R0 = (R1 + R2) / 2;
				uint32_t G0 = (G1 + G2) / 2;
				uint32_t B0 = (B1 + B2) / 2;

				uint32_t w = width - 1;
				uint32_t h = height - 1;
				for(uint32_t i = 0; i <= h; i++)
				{
					uint32_t r1 = (R0 * (h - i) + R1 * i) / h;
					uint32_t g1 = (G0 * (h - i) + G1 * i) / h;
					uint32_t b1 = (B0 * (h - i) + B1 * i) / h;

					uint32_t r2 = (R2 * (h - i) + R0 * i) / h;
					uint32_t g2 = (G2 * (h - i) + G0 * i) / h;
					uint32_t b2 = (B2 * (h - i) + B0 * i) / h;

					uint32_t *p = g_FillBuffer + i * width;
					for(uint32_t j = 0; j <= w; j++)
					{
						uint32_t r = (r1 * (w - j) + r2 * j) / w;
						uint32_t g = (g1 * (w - j) + g2 * j) / w;
						uint32_t b = (b1 * (w - j) + b2 * j) / w;
						uint32_t c = 0xFF000000 + (r << 16) + (g << 8) + b;
						p[j] = c;
					}
				}

			}
				break;
		}
	}
	else if (g_opt.m_FillType == FillType_Picture)
	{
		if (
			(g_BkWidth == g_enc_opt.m_VideoWidth) &&
			(g_BkHeight == g_enc_opt.m_VideoHeight) &&
			(g_FillType == FillType_Picture) &&
			(lstrcmp(g_FillPicture, g_opt.m_FillPicture) == 0) &&
			(g_FillBuffer != NULL)
			)
		{
			return ;
		}

		g_FillType = FillType_Gradient;

		SAFE_FREE (g_FillBuffer);
		g_BkWidth = g_enc_opt.m_VideoWidth;
		g_BkHeight = g_enc_opt.m_VideoHeight;
		wcscpy(g_FillPicture, g_opt.m_FillPicture);

		g_FillBuffer = (uint32_t *)LoadFileToMemory(g_opt.m_FillPicture, g_enc_opt.m_VideoWidth, g_enc_opt.m_VideoHeight);
		if (g_FillBuffer == NULL)
		{
			g_FillType = g_opt.m_FillType = FillType_Solid;
		}
	}
}

static void CopyBackground(uint32_t *buffer)
{
	if (g_FillType == FillType_Solid)
	{
		for(int i = 0; i < g_BkWidth * g_BkHeight; i++)
		{
			buffer[i] = g_FillColor1;
		}
	}
	else
	{
		memcpy(buffer, g_FillBuffer, g_BkWidth * g_BkHeight * 4);
	}
	
}

void __stdcall DraweBackgroundSample(uint8_t *buffer)
{
	RecreateBackground();
	CopyBackground((uint32_t*)buffer);
}

void __stdcall DrawResizeSample(uint8_t *buffer, ClipInfo *info)
{
	ZeroMemory(buffer, g_enc_opt.m_VideoWidth * g_enc_opt.m_VideoHeight * 4);

	Rect crop;
	vcGetVideoEffect(info, IID_Crop, &crop, sizeof(crop)); 
	if (crop.Width == 0) crop.Width = info->m_MediaInfo->m_Width;
	if (crop.Height == 0) crop.Height = info->m_MediaInfo->m_Height;
	Rect srcRect, dstRect;
	GetResizeMethodRect(g_opt.m_ResizeMethod, g_enc_opt.m_VideoWidth, g_enc_opt.m_VideoHeight, crop.Width, crop.Height, srcRect, dstRect);

	Bitmap bitmap(info->m_MediaInfo->m_Demo);
	Bitmap BkBitmap(g_enc_opt.m_VideoWidth, g_enc_opt.m_VideoHeight, g_enc_opt.m_VideoWidth * 4, PixelFormat32bppRGB, buffer);
	Graphics graphics(&BkBitmap);
	RectF source(srcRect.X, srcRect.Y, srcRect.Width, srcRect.Height);
	RectF destination(dstRect.X, dstRect.Y, dstRect.Width, dstRect.Height);

	graphics.DrawImage(&bitmap, destination, source, Gdiplus::UnitPixel, NULL);
}

CVideoFilterBase* CBackgroundFilter::CreateInstance(VideoFilterTemplate *pTemplate, CVideoFilterData *pData)
{
	CBackgroundFilter *filter = new CBackgroundFilter(pTemplate, pData);
	return filter;
}

CBackgroundFilter::CBackgroundFilter(VideoFilterTemplate *pTemplate, CVideoFilterData *pData)
	:CVideoFilterBase(pTemplate, pData)
{
	m_FilterType = VIDEO_FILTER_TYPE_BACKGROUND;
}

CBackgroundFilter::~CBackgroundFilter(void)
{
	SAFE_FREE(m_Buffer);
}

uint8_t* CBackgroundFilter::GetBuffer()
{
	if (m_Type) return m_Buffer;
	return CVideoFilterBase::GetBuffer();
}

void CBackgroundFilter::PutSourceSize(int32_t width, int32_t height)
{
	if (
		(m_srcWidth != width) ||
		(m_srcHeight != height) ||
		(m_dstWidth != g_enc_opt.m_VideoWidth) ||
		(m_dstHeight != g_enc_opt.m_VideoHeight) 
		)
	{
		SAFE_FREE(m_Buffer)
		SAFE_DELETE(m_Blt);
		m_srcWidth = width;
		m_srcHeight = height;
		m_dstWidth = g_enc_opt.m_VideoWidth;
		m_dstHeight = g_enc_opt.m_VideoHeight;
		
		Rect srcRect, dstRect;
		GetResizeMethodRect(g_opt.m_ResizeMethod, g_enc_opt.m_VideoWidth, g_enc_opt.m_VideoHeight, width, height, srcRect, dstRect);
		if (srcRect.Equals(dstRect))
		{
			m_Type = BK_TYPE_FULL;
		}
		else if ((srcRect.Width == dstRect.Width) && (srcRect.Height == dstRect.Height))
		{
			m_Type = BK_TYPE_COPY;
			m_srcOffset = (srcRect.Y * m_srcWidth + srcRect.X) * 4;
			m_dstOffset = (dstRect.Y * m_dstWidth + dstRect.X) * 4;
			m_viewWidth = dstRect.Width;
			m_viewHeight = dstRect.Height;
		}
		else
		{
			m_Type = BK_TYPE_STRETCH;
			m_srcOffset = (srcRect.Y * m_srcWidth + srcRect.X) * 4;
			m_dstOffset = (dstRect.Y * m_dstWidth + dstRect.X) * 4;
			m_Blt = new CFastStretchBlt(srcRect.Width, srcRect.Height, m_srcWidth * 4, dstRect.Width, dstRect.Height, m_dstWidth * 4);
		}

		if (m_Type != BK_TYPE_FULL)
		{
			m_Buffer = (uint8_t *)MemoryAlloc(m_srcWidth * m_srcHeight * 4);
		}
	}

	RecreateBackground();
	CVideoFilterBase::PutSourceSize(g_enc_opt.m_VideoWidth, g_enc_opt.m_VideoHeight);
}

void CBackgroundFilter::Process(uint8_t *source, int64_t Pts)
{
	uint8_t* dest = m_next->GetBuffer();
	switch(m_Type)
	{
	case BK_TYPE_FULL:
		assert(dest == dest);
		m_next->Process(source, Pts);
		return ; 

	case BK_TYPE_COPY:
		{
			CopyBackground((uint32_t *)dest);
			copy_32bpp(dest + m_dstOffset, m_dstWidth * 4, source + m_srcOffset, m_srcWidth * 4, m_viewWidth, m_viewHeight);
		}
		break;

	case BK_TYPE_STRETCH:
		{
			CopyBackground((uint32_t *)dest);
			m_Blt->Proc(dest + m_dstOffset, source  + m_srcOffset);
		}
		break;
	}
	return m_next->Process(dest, Pts);;
}
