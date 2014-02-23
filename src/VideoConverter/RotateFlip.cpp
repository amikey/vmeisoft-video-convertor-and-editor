#include "stdafx.h"
#include "RotateFlip.h"
#include "VideoConverter.h"
#include "VideoFilter.h"

bool RotateExchanged(int32_t nRotateFlip)
{
	switch(nRotateFlip)
	{
	case Rotate90_FlipNone:
	case Rotate270_FlipNone:
	case Rotate90_FlipX:
	case Rotate270_FlipX:
		return true;

	default:
		return false;
	}
}

wchar_t * __stdcall RotateFlipString(int32_t RotateFlip)
{
	switch(RotateFlip)
	{
	case RotateNone_FlipNone: return L"";
	case Rotate90_FlipNone: return L"Rotate:90, Flip:None";
	case Rotate180_FlipNone: return L"Rotate:180, Flip:None";
	case Rotate270_FlipNone: return L"Rotate:270, Flip:None";
	case RotateNone_FlipX: return L"Rotate:None, Flip:X";
	case Rotate90_FlipX: return L"Rotate:90, Flip:X";
	case Rotate180_FlipX: return L"Rotate:180, Flip:X";
	case Rotate270_FlipX: return L"Rotate:270, Flip:X";
	default: return L"Error!!!";
	}
}


int32_t __stdcall CalcRotateFlip(int32_t Value, int32_t nRotateFlip)
{
	switch(Value)
	{
	case RotateNone_FlipNone:
		switch(nRotateFlip)
		{
		case Rotate_Left:
			return Rotate90_FlipNone;
		case Rotate_Right:
			return Rotate270_FlipNone;
		case Flip_Horizontal:
			return RotateNone_FlipY;
		case Flip_Vertical:
			return RotateNone_FlipX;
		}
		break;

	case Rotate90_FlipNone:
		switch(nRotateFlip)
		{
		case Rotate_Left:
			return Rotate180_FlipNone;
		case Rotate_Right:
			return RotateNone_FlipNone;
		case Flip_Horizontal:
			return Rotate90_FlipY;
		case Flip_Vertical:
			return Rotate90_FlipX;
		}
		break;

	case Rotate180_FlipNone:
		switch(nRotateFlip)
		{
		case Rotate_Left:
			return Rotate270_FlipNone;
		case Rotate_Right:
			return Rotate90_FlipNone;
		case Flip_Horizontal:
			return Rotate180_FlipY;
		case Flip_Vertical:
			return Rotate180_FlipX;
		}
		break;

	case Rotate270_FlipNone:
		switch(nRotateFlip)
		{
		case Rotate_Left:
			return RotateNone_FlipNone;
		case Rotate_Right:
			return Rotate180_FlipNone;
		case Flip_Horizontal:
			return Rotate270_FlipY;
		case Flip_Vertical:
			return Rotate270_FlipX;
		}
		break;

	case RotateNone_FlipX:
		switch(nRotateFlip)
		{
		case Rotate_Left:
			return Rotate90_FlipX;
		case Rotate_Right:
			return Rotate270_FlipX;
		case Flip_Horizontal:
			return RotateNone_FlipXY;
		case Flip_Vertical:
			return RotateNone_FlipNone;
		}
		break;

	case Rotate90_FlipX:
		switch(nRotateFlip)
		{
		case Rotate_Left:
			return Rotate180_FlipX;
		case Rotate_Right:
			return RotateNone_FlipX;
		case Flip_Horizontal:
			return Rotate90_FlipXY;
		case Flip_Vertical:
			return Rotate90_FlipNone;
		}
		break;

	case Rotate180_FlipX:
		switch(nRotateFlip)
		{
		case Rotate_Left:
			return Rotate270_FlipX;
		case Rotate_Right:
			return Rotate90_FlipX;
		case Flip_Horizontal:
			return Rotate180_FlipXY;
		case Flip_Vertical:
			return Rotate180_FlipNone;
		}
		break;

	case Rotate270_FlipX:
		switch(nRotateFlip)
		{
		case Rotate_Left:
			return RotateNone_FlipX;
		case Rotate_Right:
			return Rotate180_FlipX;
		case Flip_Horizontal:
			return Rotate270_FlipXY;
		case Flip_Vertical:
			return Rotate270_FlipNone;
		}
		break;
	}

	return 0;
}

void RotateFlipRect(Rect &r, int32_t Width, int32_t Height, int32_t nRotateFlip)
{
	int32_t x = r.X, y = r.Y, w = r.Width, h = r.Height;
	int32_t x1 = x, y1 = y, w1 = w, h1 = h;
	switch(nRotateFlip)
	{
	case Rotate_Left:
		{
			x = y1;
			y = Width - (x1 + w1);
			w = h1;
			h = w1;
		}
		break;

	case Rotate_Right:
		{
			x = Height - (y1 + h1);
			y = x1;
			w = h1;
			h = w1;
		}
		break;

	case Flip_Horizontal:
		{
			x = Width - (x1 + w1);
			y = y1;
			w = w1;
			h = h1;
		}
		break;

	case Flip_Vertical:
		{
			x = x1;
			y = Height - (y1 + h1);
			w = w1;
			h = h1;
		}
		break;

	default:
		ErrorLog("Error RotateFlip=%d\n", nRotateFlip);
		return ;
	}

	r.X = x; r.Y = y; r.Width = w; r.Height = h;
}

void RotateFlip32(uint32_t *d, uint32_t *s, int32_t w, int32_t h, int32_t nRotateFlip)
{
	switch(nRotateFlip)
	{
	case RotateNone_FlipNone:
		memcpy(d, s, w * h * 4);
		break;

	case RotateNone_FlipX:
		{
			for(int i = 0; i < h; i++)
			{
				uint32_t *p = d + (h - i - 1) * w;
				for(int j = 0; j < w; j++)
				{
					*p++ = *s ++;
				}
			}
		}
		break;

	case RotateNone_FlipY:
		{
			for(int i = 0; i < h; i++)
			{
				uint32_t *p = d + (i + 1) * w - 1;
				for(int j = 0; j < w; j++)
				{
					*p-- = *s ++;
				}
			}
		}
		break;

	case RotateNone_FlipXY:
		{
			for(int i = 0; i < h; i++)
			{
				uint32_t *p = d + (h - i) * w - 1;
				for(int j = 0; j < w; j++)
				{
					*p-- = *s ++;
				}
			}
		}
		break;

	case Rotate90_FlipNone:
		{
			for(int i = 0; i < h; i++)
			{
				uint32_t *p = d + h * (w - 1) + i;
				for(int j = 0; j < w; j++)
				{
					*p = *s++;
					p -= h;
				}
			}
		}
		break;

	case Rotate90_FlipX: // Rotate270_FlipY
		{
			for(int i = 0; i < h; i++)
			{
				uint32_t *p = d + i;
				for(int j = 0; j < w; j++)
				{
					*p = *s++;
					p += h;
				}
			}
		}
		break;

	case Rotate90_FlipY: // Rotate270_FlipX
		{
			for(int i = 0; i < h; i++)
			{
				uint32_t *p = d + h * w - 1 - i ;
				for(int j = 0; j < w; j++)
				{
					*p = *s++;
					p -= h;
				}
			}
		}
		break;

	case Rotate90_FlipXY: // Rotate270_FlipNone
		{
			for(int i = 0; i < h; i++)
			{
				uint32_t *p = d + h - i - 1;
				for(int j = 0; j < w; j++)
				{
					*p = *s++;
					p += h;
				}
			}
		}
		break;
	}
}

void RotateFlip8(uint8_t *d, uint8_t *s, int32_t w, int32_t h, int32_t nRotateFlip)
{
	switch(nRotateFlip)
	{
	case RotateNone_FlipNone:
		memcpy(d, s, w * h);
		break;

	case RotateNone_FlipX:
		{
			for(int i = 0; i < h; i++)
			{
				uint8_t *p = d + (h - i - 1) * w;
				for(int j = 0; j < w; j++)
				{
					*p++ = *s ++;
				}
			}
		}
		break;

	case RotateNone_FlipY:
		{
			for(int i = 0; i < h; i++)
			{
				uint8_t *p = d + (i + 1) * w - 1;
				for(int j = 0; j < w; j++)
				{
					*p-- = *s ++;
				}
			}
		}
		break;

	case RotateNone_FlipXY:
		{
			for(int i = 0; i < h; i++)
			{
				uint8_t *p = d + (h - i) * w - 1;
				for(int j = 0; j < w; j++)
				{
					*p-- = *s ++;
				}
			}
		}
		break;

	case Rotate90_FlipNone:
		{
			for(int i = 0; i < h; i++)
			{
				uint8_t *p = d + h * (w - 1) + i;
				for(int j = 0; j < w; j++)
				{
					*p = *s++;
					p -= h;
				}
			}
		}
		break;

	case Rotate90_FlipX: // Rotate270_FlipY
		{
			for(int i = 0; i < h; i++)
			{
				uint8_t *p = d + i;
				for(int j = 0; j < w; j++)
				{
					*p = *s++;
					p += h;
				}
			}
		}
		break;

	case Rotate90_FlipY: // Rotate270_FlipX
		{
			for(int i = 0; i < h; i++)
			{
				uint8_t *p = d + h * w - 1 - i ;
				for(int j = 0; j < w; j++)
				{
					*p = *s++;
					p -= h;
				}
			}
		}
		break;

	case Rotate90_FlipXY: // Rotate270_FlipNone
		{
			for(int i = 0; i < h; i++)
			{
				uint8_t *p = d + h - i - 1;
				for(int j = 0; j < w; j++)
				{
					*p = *s++;
					p += h;
				}
			}
		}
		break;
	}
}

void RotateFlipYUV420(uint8_t *d[3], uint8_t *s[3], int32_t w, int32_t h, int32_t nRotateFlip)
{
	RotateFlip8(d[0], s[0], w, h, nRotateFlip);
	RotateFlip8(d[1], s[1], w / 2, h / 2, nRotateFlip);
	RotateFlip8(d[2], s[2], w / 2, h / 2, nRotateFlip);
}

CVideoFilterBase* CRotateFlipFilter::CreateInstance(VideoFilterTemplate *pTemplate, CVideoFilterData *pData)
{
	CRotateFlipFilter *filter = new CRotateFlipFilter(pTemplate, pData);
	return filter;
}

CRotateFlipFilter::CRotateFlipFilter(VideoFilterTemplate *pTemplate, CVideoFilterData *pData)
	:CVideoFilterBase(pTemplate, pData)
{
	m_Value = (int32_t *)pData->m_Parameter;
}

CRotateFlipFilter::~CRotateFlipFilter(void)
{
	SAFE_FREE(m_Buffer);
}

uint8_t* CRotateFlipFilter::GetBuffer()
{
	if (m_Buffer) return m_Buffer;
	return CVideoFilterBase::GetBuffer();
}

void CRotateFlipFilter::PutSourceSize(int32_t width, int32_t height)
{
	m_Width = width;
	m_Height = height;
	if (RotateExchanged(*m_Value))
	{
		int32_t t = width; width = height; height = t;
	}
	SAFE_FREE(m_Buffer);
	if (*m_Value) m_Buffer = (uint8_t *)MemoryAlloc(m_Width * m_Height * 4);
	CVideoFilterBase::PutSourceSize(width, height);
}

void CRotateFlipFilter::Process(uint8_t *source, int64_t Pts)
{
	if (m_Buffer)
	{
		uint8_t* dest = m_next->GetBuffer();
		RotateFlip32((uint32_t *)dest, (uint32_t *)source, m_Width, m_Height, *m_Value);
		m_next->Process(dest, Pts);
	}
	else
	{
		m_next->Process(source, Pts);
	}
}
