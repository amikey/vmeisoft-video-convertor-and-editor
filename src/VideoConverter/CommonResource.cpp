#include "StdAfx.h"
#include "CommonResource.h"
#include "Resource.h"

HFONT g_hDefaultFont;
Gdiplus::Font* g_pFont;

HCURSOR hcur_arrow;
HCURSOR hcur_hand;
HCURSOR hcur_sizenwse;
HCURSOR hcur_sizenesw;
HCURSOR hcur_sizewe; // "Horizontal size"
HCURSOR hcur_sizens; // Vertical size"
HCURSOR hcur_sizeall;
HCURSOR hcur_crop;
HCURSOR hcur_select;
HCURSOR hcur_colorpick;

Gdiplus::Bitmap *LoadImageToBitmap(wchar_t *szImageFile, Gdiplus::PixelFormat format)
{
	wchar_t szImagePathFile[MAX_PATH];
	StringCchPrintf(szImagePathFile, MAX_PATH, L"%ls\\%s", g_szModulePath, szImageFile);

#if 0
	Gdiplus::Bitmap *bitmap = new Gdiplus::Bitmap(szImagePathFile);
#else
	Gdiplus::Bitmap image(szImagePathFile);
	int32_t width = image.GetWidth();
	int32_t height = image.GetHeight();
	Gdiplus::Bitmap *bitmap = new Gdiplus::Bitmap(width, height, format);
	Gdiplus::Graphics graphics(bitmap);
	graphics.DrawImage(&image, 0, 0, width, height);
#endif

	return bitmap;
}

void InitCommonResource()
{
	g_hDefaultFont = CreateFont(   
		-12,			// negative makes it use "char size"    
		0,              // logical average character width     
		0,              // angle of escapement     
		0,              // base-line orientation angle     
		FW_NORMAL,		// weight    
		0,				// italic    
		FALSE,          // underline attribute flag     
		FALSE,          // strikeout attribute flag     
		DEFAULT_CHARSET,    // character set identifier     
		OUT_DEFAULT_PRECIS, // output precision     
		CLIP_DEFAULT_PRECIS,// clipping precision     
		DEFAULT_QUALITY,    // output quality     
		DEFAULT_PITCH,  // pitch and family     
		DIALOG_FONT_NAME  // pointer to typeface name string     
		); 

	g_pFont = new Gdiplus::Font(GetDC(NULL), g_hDefaultFont);

	hcur_arrow		= LoadCursor(NULL, IDC_ARROW);
	hcur_hand		= LoadCursor(NULL, IDC_HAND); 
	hcur_sizenwse	= LoadCursor(NULL, IDC_SIZENWSE);
	hcur_sizenesw	= LoadCursor(NULL, IDC_SIZENESW);
	hcur_sizewe		= LoadCursor(NULL, IDC_SIZEWE);
	hcur_sizens		= LoadCursor(NULL, IDC_SIZENS);
	hcur_sizeall	= LoadCursor(NULL, IDC_SIZEALL); 
	hcur_crop		= LoadCursor(g_hInstance, MAKEINTRESOURCE(IDC_CUR_CROP)); 
	hcur_select		= LoadCursor(g_hInstance, MAKEINTRESOURCE(IDC_CUR_SELECT));
	hcur_colorpick	= LoadCursor(g_hInstance, MAKEINTRESOURCE(IDC_CUR_COLORPICK));
}
