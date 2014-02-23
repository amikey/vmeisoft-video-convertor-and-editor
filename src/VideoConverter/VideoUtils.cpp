#include "stdafx.h"
#include "VideoConverter.h"
#include "VideoUtils.h"

#undef malloc
#undef free
#undef realloc

#include <mmintrin.h>
#include <xmmintrin.h>
#include <emmintrin.h>

/*
 // 预乘
 C2 = (C0 * (255-A)) / 255 + C1;  * - + * / = 2 + 2
 */
static inline void blend_image_inplace(uint8_t* background, uint8_t* foreground, int32_t count)
{
	for(int i = 0; i < count; i++)
	{
		uint32_t a = foreground[3];
		switch(a)
		{
			case 0:
				break;
			case 0xFF:
				*((uint32_t *)background) = *((uint32_t *)foreground);
				break;
			default:
				background[0] = (background[0] * (255 - a)) / 255 + foreground[0];
				background[1] = (background[1] * (255 - a)) / 255 + foreground[1];
				background[2] = (background[2] * (255 - a)) / 255 + foreground[2];
				break;
		}
		background += 4;
		foreground += 4;
	}
}

static inline void blend_image_inplace_mmx(uint32_t* background, uint32_t* foreground, int32_t count)
{
	register __m64 mm0, mm1, mm2, mm3, mm4;

	mm3 = _mm_set_pi16(255, 255, 255, 255);
	mm4 = _mm_setzero_si64();

	for(int i = 0; i < count; i++, background ++, foreground ++)
	{
		register uint32_t f = *foreground;
		switch(f & 0xFF000000)
		{
			case 0x00000000:
				continue;

			case 0xFF000000:
				*background = f;
				continue;
		}

		mm0 = _mm_cvtsi32_si64(*background);
		mm1 = _mm_cvtsi32_si64(f);

		mm0 = _m_punpcklbw(mm0, mm4);  // B: 0 A 0 B 0 G 0 R
		mm1 = _m_punpcklbw(mm1, mm4);  // F: 0 A 0 B 0 G 0 R
		mm2 = mm1; // F: 0 A 0 B 0 G 0 R
		mm2 = _m_punpckhwd(mm2, mm2); // F: 0 A 0 A 0 B 0 B
		mm2 = _m_punpckhdq(mm2, mm2); // F: 0 A 0 A 0 A 0 A

		mm2 = _m_psubw(mm3, mm2); // (255 - A)
		mm2 = _m_pmullw(mm2, mm0); // (255 - A) * B
		mm2 = _m_psrlwi(mm2, 8);  // ((255 - A) * B) / 255
		mm2 = _m_paddw(mm2, mm1); // ((255 - A) * B) / 255 + F

		mm2 = _m_packuswb(mm2, mm2); // 紧缩到低32bit
		*background = _mm_cvtsi64_si32(mm2); //保存结果
	}

	_mm_empty();
}

void bend_32bpp(uint8_t *dst, int32_t dest_step, uint8_t *src, int32_t width, int32_t height)
{
	for(int i = 0; i < height; i++)
	{
#ifdef _WIN64
		blend_image_inplace((uint8_t*)dst, (uint8_t*)src, width);
#else
		blend_image_inplace_mmx((uint32_t*)dst, (uint32_t*)src, width);
#endif
		dst += dest_step;
		src += width * 4;
	}

#ifndef _WIN64
	_mm_empty();
#endif
}

static const uint32_t LINE_COLOR1		= 0xFFFFFFFF;
static const uint32_t LINE_COLOR2		= 0xFF000000;

static const int32_t R_COEF = 19595; // 65536 * 0.299 = 19595
static const int32_t G_COEF = 38469; // 65536 * 0.587 = 38469
static const int32_t B_COEF = 7471;  // 65536 * 0.114 = 7471

static void __inline GrayColor(uint8_t *p)
{
	int32_t v = p[0] * B_COEF + p[1] * G_COEF + p[2] * R_COEF;
	p[0] = p[1] = p[2] = ((v * 3) >> 18) + 0x20;
}

void gray_32bpp(uint8_t *dst, int32_t left, int32_t top, int32_t right, int32_t bottom, int32_t width, int32_t height)
{
	uint8_t *p;

	//top
	{
		p = dst;
		for(int i = 0; i < top * width; i++)
		{
			GrayColor(p);
			p += 4;
		}
	}

	//bottom
	{
		p = dst + bottom * width * 4;
		for(int i = 0; i < (height - bottom) * width; i++)
		{
			GrayColor(p);
			p += 4;
		}
	}

	for(int i = top; i < bottom; i++)
	{
		//left
		{
			p = dst + i * width * 4;
			for(int j = 0; j < left; j++)
			{
				GrayColor(p);
				p += 4;
			}
		}

		//right
		{
			p = dst + (i * width + right) * 4;
			for(int j = right; j < width; j++)
			{
				GrayColor(p);
				p += 4;
			}
		}
	}
}

void rect_32bpp(uint8_t *dst, int32_t left, int32_t top, int32_t right, int32_t bottom, int32_t width, int32_t height)
{
	uint8_t *p;

	//top
	if (top)
	{
		p = dst;
		for(int i = 0; i < (top - 1) * width; i++)
		{
			GrayColor(p);
			p += 4;
		}

		int32_t l = 0;
		for(int i = 0; i < width; i++)
		{
			if ((i < left - 1) || (i > right))
			{
				GrayColor(p);
			}
			else if (l & 4)
			{
				*((uint32_t *)p) = LINE_COLOR1;
				l ++;
			}
			else
			{
				*((uint32_t *)p) = LINE_COLOR2;
				l ++;
			}
			p += 4;
		}
	}

	//bottom
	if (bottom < height)
	{
		p = dst + bottom * width * 4;
		int32_t l = 0;
		for(int i = 0; i < width; i++)
		{
			if ((i < left - 1) || (i > right))
			{
				GrayColor(p);
			}
			else if (l & 4)
			{
				*((uint32_t *)p) = LINE_COLOR1;
				l++;
			}
			else
			{
				*((uint32_t *)p) = LINE_COLOR2;
				l++;
			}
			p += 4;
		}

		for(int i = 0; i < (height - bottom - 1)  * width; i++)
		{
			GrayColor(p);
			p += 4;
		}
	}

	for(int i = 0; i < ((bottom - top)); i++)
	{
		//left
		if (left)
		{
			p = dst + (top + i) * width * 4;
			for(int j = 0; j < left - 1; j++)
			{
				GrayColor(p);
				p += 4;
			}

			if (i & 4)
			{
				*((uint32_t *)p) = LINE_COLOR1;
			}
			else
			{
				*((uint32_t *)p) = LINE_COLOR2;
			}
		}

		//right
		if (right < width)
		{
			p = dst + ((top + i) * width + right) * 4;
			if (i & 4)
			{
				*((uint32_t *)p) = LINE_COLOR1;
			}
			else
			{
				*((uint32_t *)p) = LINE_COLOR2;
			}
			p += 4;

			for(int j = 0; j < width - right - 1; j++)
			{
				GrayColor(p);
				p += 4;
			}
		}
	}
}

#define BORDER_COLOR 0xFF304860

void border_32bpp(uint32_t *dst, int32_t width, int32_t height)
{
	for(int i = 0; i < width; i++)
	{
		dst[i] = BORDER_COLOR;
		dst[width + i] = 0xFFFFFFFF;
		dst[(height - 2) * width + i] = 0xFFFFFFFF;
		dst[(height - 1) * width + i] = BORDER_COLOR;
	}
	for(int i = 1; i < height - 1; i++)
	{
		dst[i * width] = BORDER_COLOR;
		dst[i * width + 1] = 0xFFFFFFFF;
		dst[(i + 1) * width - 2] = 0xFFFFFFFF;
		dst[(i + 1) * width - 1] = BORDER_COLOR;
	}
}

void copy_32bpp(uint8_t *dst, int32_t dst_step, uint8_t *src, int32_t src_step, int32_t width, int32_t height)
{
	width *= 4;
	for(int i = 0; i < height; i++)
	{
		memcpy(dst, src, width);
		dst += dst_step;
		src += src_step;
	}
}

void GetResizeMethodRect(ResizeMethod ResizeMethod, int32_t DstWidth, int32_t DstHeight, int32_t SrcWidth, int32_t SrcHeight, Rect &srcRect, Rect &dstRect)
{
	if (ResizeMethod == ResizeMethod_Center)
	{
		if (SrcWidth > DstWidth)
		{
			srcRect.X = (SrcWidth - DstWidth) / 2;
			dstRect.X = 0;
			srcRect.Width = DstWidth;
			dstRect.Width = DstWidth;
		}
		else
		{
			srcRect.X = 0;
			dstRect.X = (DstWidth - SrcWidth) / 2;
			srcRect.Width = SrcWidth;
			dstRect.Width = SrcWidth;
		}

		if (SrcHeight > DstHeight)
		{
			srcRect.Y = (SrcHeight - DstHeight) / 2;
			dstRect.Y = 0;
			srcRect.Height = DstHeight;
			dstRect.Height = DstHeight;
		}
		else
		{
			srcRect.Y = 0;
			dstRect.Y = (DstHeight - SrcHeight) / 2;
			srcRect.Height = SrcHeight;
			dstRect.Height = SrcHeight;
		}
	}
	else if (ResizeMethod == ResizeMethod_Fill)
	{
		srcRect.X = 0;
		srcRect.Y = 0;
		srcRect.Width = SrcWidth;
		srcRect.Height = SrcHeight;
		dstRect.X = 0;
		dstRect.Y = 0;
		dstRect.Width = DstWidth;
		dstRect.Height = DstHeight;
	}
	else if (ResizeMethod == ResizeMethod_LetterBox)
	{
		srcRect.X = 0;
		srcRect.Y = 0;
		srcRect.Width = SrcWidth;
		srcRect.Height = SrcHeight;

		// 源比较宽
		if (SrcWidth * DstHeight > DstWidth * SrcHeight)
		{
			dstRect.Width = DstWidth;
			dstRect.Height = (DstWidth * SrcHeight) / SrcWidth;
			dstRect.X = 0;
			dstRect.Y = (DstHeight - dstRect.Height) / 2;
		}
		else
		{
			dstRect.Width = (DstHeight * SrcWidth) / SrcHeight;
			dstRect.Height = DstHeight;
			dstRect.X = (DstWidth - dstRect.Width) / 2;
			dstRect.Y = 0;
		}
	}
	else
	{
		dstRect.X = 0;
		dstRect.Y = 0;
		dstRect.Width = DstWidth;
		dstRect.Height = DstHeight;

		// 源比较宽
		if (SrcWidth * DstHeight > DstWidth * SrcHeight)
		{
			srcRect.Y = 0;
			srcRect.Height = SrcHeight;
			srcRect.Width = (SrcHeight * DstWidth) / DstHeight;
			srcRect.X = (SrcWidth - srcRect.Width) / 2;
		}
		else
		{
			srcRect.X = 0;
			srcRect.Width = SrcWidth;
			srcRect.Height = (SrcWidth * DstHeight) / DstWidth;
			srcRect.Y = (SrcHeight - srcRect.Height) / 2;
		}
	}
}

uint8_t* LoadFileToMemory(const wchar_t *file, int32_t width, int32_t height)
{
	Gdiplus::Image image(file);
	if ((image.GetLastStatus() != Gdiplus::Ok) || (image.GetType() == Gdiplus::ImageTypeUnknown))
	{
		ErrorLog("Faild to Open:%ls\n", file);
		return NULL;
	}

	uint8_t *Result = (uint8_t *)MemoryAlloc(width * height * 4);
	ZeroMemory(Result, width * height * 4);
	Gdiplus::Bitmap bitmap(width, height, width * 4, PixelFormat32bppARGB, Result);

	Gdiplus::Graphics graphics(&bitmap);
	graphics.DrawImage(&image, 0, 0, width, height);

	return Result;
}

void ChangeAlpha(uint8_t *p, uint32_t a, int32_t pixels)
{
	for(int i = 0; i < pixels; i++, p += 4)
	{
		switch(p[3])
		{
			case 0:
				*((uint32_t *)p) = 0;
				break;
			default:
				p[0] = (p[0] * a) / 255;
				p[1] = (p[1] * a) / 255;
				p[2] = (p[2] * a) / 255;
				p[3] = (p[3] * a) / 255;
				break;
		}
	}

}
