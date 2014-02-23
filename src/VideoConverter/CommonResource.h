#pragma once

#define DIALOG_FONT_NAME	L"MS Shell Dlg"

extern HFONT g_hDefaultFont;
extern Gdiplus::Font* g_pFont;

extern HCURSOR hcur_arrow;
extern HCURSOR hcur_hand;
extern HCURSOR hcur_sizenwse;
extern HCURSOR hcur_sizenesw;
extern HCURSOR hcur_sizewe; // "Horizontal size"
extern HCURSOR hcur_sizens; // Vertical size"
extern HCURSOR hcur_sizeall;
extern HCURSOR hcur_crop;
extern HCURSOR hcur_select;
extern HCURSOR hcur_colorpick;

extern void InitCommonResource();
