#include "StdAfx.h"
#include "HSBControl.h"
#include "CommonResource.h"
#include "resource.h"

#define HSB_SELECT_WIN_CLASSNAME	L"HSB Color Select Class"

static DWORD ColorDefined[HSB_CONTROL_USER] = {
	0xFFFFFF00, 0xFF00FF00, 0xFF00FFFF,
	0xFFFF00FF, 0xFF0000FF, 0xFFFF0000,
	0xFF000080, 0xFF008080, 0xFF008000,
	0xFF800080, 0xFF800000, 0xFF808000,
	0xFF808080, 0xFFC0C0C0, 0xFF000000,
	//--------------------------------
};

#define BTN_BORDER					5
#define DROPDOWN_WIDTH				24

#define SELECT_BORDER				3
#define COLOR_SLIDER_HEIGHT			18
#define COLOR_EDIT_HEIGHT			18

#define BASE_LEFT					(120)

#define STATIC_LEFT					BASE_LEFT
#define STATIC_WIDTH				40
#define STATIC_HEIGHT				18
#define STATIC_Y_OFFSET(n)			(SLIDER_TOP + n * SLIDER_HEIGHT + 3)

#define BAR_LEFT					(STATIC_LEFT + STATIC_WIDTH + 8)
#define BAR_WIDTH					(256 + 2)
#define BAR_HEIGHT					(COLOR_BAR_HEIGHT + 2)
#define BAR_Y_OFFSET(n)				(SLIDER_TOP + n * SLIDER_HEIGHT + 0)

#define EDIT_LEFT					(BAR_LEFT + BAR_WIDTH + 16)
#define EDIT_WIDTH					30
#define EDIT_HEIGHT					18
#define EDIT_Y_OFFSET				0

#define UPDOWN_LEFT					(EDIT_LEFT + EDIT_WIDTH + 0)
#define UPDOWN_WIDTH				16
#define UPDOWN_HEIGHT				23
#define UPDOWN_Y_OFFSET				-3

#define BTN_TOP						(SELECT_BORDER)
#define COLOR_TOP					(2)
#define SLIDER_TOP					(COLOR_TOP + 256 + 2 + SELECT_BORDER + SELECT_BORDER)
#define SLIDER_HEIGHT				(25)

#define SELECT_WIDTH				(BASE_LEFT + COLOR_MAP_WIDTH + SELECT_BORDER + SELECT_BORDER + 2)
#define SELECT_HEIGHT				(SLIDER_TOP + SLIDER_HEIGHT * 3 + 2)

#define DEFINED_COLOR_WIDTH			24
#define DEFINED_COLOR_HEIGHT		18

#define TICK_LEFT					(BAR_LEFT - 6 + 1)
#define TICK_Y_OFFSET(n)			(BAR_Y_OFFSET(n) + BAR_HEIGHT)

#define B 0xFF000000
#define O 0xFFFFFFFF
static DWORD TickData[TICK_HEIGHT][TICK_WIDTH] = 
{
	{O, O, O, O, O, O, B, O, O, O, O, O, O},
	{O, O, O, O, O, B, B, B, O, O, O, O, O},
	{O, O, O, O, B, B, B, B, B, O, O, O, O},
	{O, O, O, B, B, B, B, B, B, B, O, O, O},
	{O, O, B, B, B, B, B, B, B, B, B, O, O},
	{O, B, B, B, B, B, B, B, B, B, B, B, O},
	{B, B, B, B, B, B, B, B, B, B, B, B, B}
};
#undef B 
#undef F 

CHSBControlBase::CHSBControlBase(CHSBWindow *pControl, int32_t Left, int32_t Top, int32_t Width, int32_t Height)
	:m_pControl(pControl), m_Left(Left), m_Top(Top), m_Width(Width), m_Height(Height)
{
	pControl->AddControl(this);
}

CHSBControlBase::~CHSBControlBase()
{

}
// **************************************************
// Text
// **************************************************
CHSBControlText::CHSBControlText(CHSBWindow *pControl, int32_t Left, int32_t Top, wchar_t *Text)
	:CHSBControlBase(pControl, Left, Top, 0, 0)
{
	m_pText = Text;
}

BOOL CHSBControlText::DoMessage(HWND hWnd, UINT message, int32_t xPos, int32_t yPos)
{
	return FALSE;
}

void CHSBControlText::Paint(Gdiplus::Graphics *pGraphics)
{
	PointF origin(m_Left, m_Top);
	SolidBrush blackBrush(Color(255, 0, 0, 0));
	pGraphics->DrawString(m_pText, -1, g_pFont, origin, &blackBrush);
}

void CHSBControlText::Enter(void)
{
}

void CHSBControlText::Leave(void)
{
}

// **************************************************
// Colors
// **************************************************
CHSBControlColors::CHSBControlColors(CHSBWindow *pControl, int32_t Left, int32_t Top)
	:CHSBControlBase(pControl, Left, Top, COLOR_MAP_WIDTH, 256),
	m_Bitmap(COLOR_MAP_WIDTH, 256, COLOR_MAP_WIDTH * 4, PixelFormat32bppRGB, (BYTE *)m_BitmapBuffer)
{
	for(int i = 0; i < COLOR_MAP_WIDTH; i++)
	{
		register ColorRef Center;
		Center.A = 0xFF;
		if (i < 64) // ºì->»Æ
		{
			Center.R = 0xFF;
			Center.G = i * 4;
			Center.B = 0;
		}
		else if (i < 128) // »Æ->ÂÌ
		{
			Center.R = (128 - i) * 4 - 1;
			Center.G = 0xFF;
			Center.B = 0;
		}
		else if (i < 192) // ÂÌ->Çà
		{
			Center.R = 0;
			Center.G = 0xFF;
			Center.B = (i - 128) * 4;
		}
		else if (i < 256) // Çà->À¶
		{
			Center.R = 0;
			Center.G = (256 - i) * 4 - 1;
			Center.B = 0xFF;
		}
		else if (i < 320) // À¶->×Ï
		{
			Center.R = (i - 256) * 4;
			Center.G = 0;
			Center.B = 0xFF;
		}
		else
		{
			Center.R = 0xFF;
			Center.G = 0;
			Center.B = (384 - i) * 4 - 1;
		}

		for(int j = 0; j < 128; j++)
		{
			register ColorRef v;
			v.A = 0xFF;
			v.R = (0xFF * (128 - j) + Center.R * j) / 128;
			v.G = (0xFF * (128 - j) + Center.G * j) / 128;
			v.B = (0xFF * (128 - j) + Center.B * j) / 128;
			m_BitmapBuffer[j * COLOR_MAP_WIDTH + i] = v;
		}

		for(int j = 128; j < 256; j++)
		{
			register ColorRef v;
			v.A = 0xFF;
			v.R = (Center.R * (256 - j)) / 128;
			v.G = (Center.G * (256 - j)) / 128;
			v.B = (Center.B * (256 - j)) / 128;
			m_BitmapBuffer[j * COLOR_MAP_WIDTH + i] = v;
		}
	}
}

void CHSBControlColors::Enter(void)
{
	SetCursor(hcur_colorpick);
}

void CHSBControlColors::Leave(void)
{
}

BOOL CHSBControlColors::DoMessage(HWND hWnd, UINT message, int32_t xPos, int32_t yPos)
{
	switch (message) 
	{
	case WM_MOUSEMOVE:
		{
			SetCursor(hcur_colorpick);
			if (m_pControl->m_DrageControl == 0) return 0;
			if (GetCapture() != hWnd)
			{
				m_pControl->m_DrageControl = 0;
				return 0;
			}

			if ((xPos < 0) || (xPos >= COLOR_MAP_WIDTH) ||  (yPos < 0) || (yPos > 255) )
			{
				// ×¥È¡ÆÁÄ»ÑÕÉ«
				ColorRef NewColor;
				HDC hdc = GetDC(0);
				POINT pt;
				GetCursorPos(&pt);
				NewColor.Color = GetPixel(hdc, pt.x, pt.y);
				m_pControl->OnColorChanged(NewColor.B, NewColor.G, NewColor.R);
			}
			else
			{
				ColorRef NewColor = m_BitmapBuffer[yPos * COLOR_MAP_WIDTH + xPos];
				m_pControl->OnColorChanged(NewColor.R, NewColor.G, NewColor.B);
			}
		}
		return TRUE;

	case WM_LBUTTONDOWN:
		{
			CHSBWindow *pData = (CHSBWindow *)GetWindowLongPtr(hWnd, 0);
			m_pControl->m_DrageControl = 1;
			SetCapture(hWnd);

			if (xPos < 0) return 0;
			if (xPos >= COLOR_MAP_WIDTH) return 0;
			if (yPos < 0) return 0;
			if (yPos > 255) return 0;

			ColorRef NewColor = m_BitmapBuffer[yPos * COLOR_MAP_WIDTH + xPos];
			pData->OnColorChanged(NewColor.R, NewColor.G, NewColor.B);

		}
		return TRUE;

	case WM_CANCELMODE:
		{
			CHSBWindow *pData = (CHSBWindow *)GetWindowLongPtr(hWnd, 0);
			m_pControl->m_DrageControl = 0;
		}
		return TRUE;

	case WM_LBUTTONUP:
		{
			CHSBWindow *pData = (CHSBWindow *)GetWindowLongPtr(hWnd, 0);
			if (m_pControl->m_DrageControl)
			{
				m_pControl->m_DrageControl = 0;
				ReleaseCapture();
				return 0;
			}
		}
		return TRUE;
	}

	return FALSE;
}

void CHSBControlColors::Paint(Gdiplus::Graphics *pGraphics)
{
	Pen BorderPen(0xFF828790, 1);
	pGraphics->DrawImage(&m_Bitmap, m_Left + 1, m_Top + 1);
	pGraphics->DrawRectangle(&BorderPen, m_Left, m_Top, COLOR_MAP_WIDTH + 1, 256 + 1);
}

// **************************************************
// BAR
// **************************************************
CHSBControlBar::CHSBControlBar(CHSBWindow *pControl, int32_t Left, int32_t Top, int32_t Type)
	:CHSBControlBase(pControl, Left, Top, 256, COLOR_BAR_HEIGHT),
	m_Bitmap(256, COLOR_BAR_HEIGHT, 256 * 4, PixelFormat32bppRGB, (BYTE *)m_BitmapBuffer)
{
	m_Type = Type;
}

void CHSBControlBar::Enter(void)
{
}

void CHSBControlBar::Leave(void)
{
}

BOOL CHSBControlBar::DoMessage(HWND hWnd, UINT message, int32_t xPos, int32_t yPos)
{
	switch (message) 
	{
	case WM_MOUSEMOVE:
		{
			if (m_pControl->m_DrageControl == 0) return 0;
			if (GetCapture() != hWnd)
			{
				m_pControl->m_DrageControl = 0;
				return 0;
			}

			if (xPos < 0) return 0;
			if (xPos > 255) return 0;

			uint32_t R = m_pControl->m_Color.R;
			uint32_t G = m_pControl->m_Color.G;
			uint32_t B = m_pControl->m_Color.B;
			switch(m_Type)
			{
			case HSB_TYPE_R:
				R = xPos;
				break;
			case HSB_TYPE_G:
				G = xPos;
				break;
			case HSB_TYPE_B:
				B = xPos;
				break;
			}
			m_pControl->OnColorChanged(R, G, B);
		}
		return 0;

	case WM_LBUTTONDOWN:
		{
			m_pControl->m_DrageControl = 1;
			SetCapture(hWnd);

			uint32_t R = m_pControl->m_Color.R;
			uint32_t G = m_pControl->m_Color.G;
			uint32_t B = m_pControl->m_Color.B;
			switch(m_Type)
			{
			case HSB_TYPE_R:
				R = xPos;
				break;
			case HSB_TYPE_G:
				G = xPos;
				break;
			case HSB_TYPE_B:
				B = xPos;
				break;
			}
			m_pControl->OnColorChanged(R, G, B);
		}
		break;

	case WM_CANCELMODE:
		{
			m_pControl->m_DrageControl = 0;
		}
		break;

	case WM_LBUTTONUP:
		{
			if (m_pControl->m_DrageControl)
			{
				m_pControl->m_DrageControl = 0;
				ReleaseCapture();
				return 0;
			}
		}
		break;

	}
	return FALSE;
}

void CHSBControlBar::Paint(Gdiplus::Graphics *pGraphics)
{
	Pen BorderPen(0xFF828790, 1);
	pGraphics->DrawImage(&m_Bitmap, m_Left + 1, m_Top + 1);
	pGraphics->DrawRectangle(&BorderPen, m_Left, m_Top, 256 + 1, COLOR_BAR_HEIGHT + 1);
}

// **************************************************
// Tick
// **************************************************
CHSBControlTick::CHSBControlTick(CHSBWindow *pControl, int32_t Left, int32_t Top, int32_t Type)
	:CHSBControlBase(pControl, Left, Top, TICK_WIDTH + 256 + 2, TICK_HEIGHT + 2)
{
	m_Type = Type;
	m_xPos = 0;
}

void CHSBControlTick::Enter(void)
{
}

void CHSBControlTick::Leave(void)
{
}

BOOL CHSBControlTick::DoMessage(HWND hWnd, UINT message, int32_t xPos, int32_t yPos)
{
	switch (message) 
	{
	case WM_MOUSEMOVE:
		{
			if (m_pControl->m_DrageControl == 0)
			{
				if ((xPos >= m_xPos) && (xPos < m_xPos + TICK_WIDTH))
				{
					SetCursor(hcur_hand);
				}
				else
				{
					SetCursor(hcur_arrow);
				}
				return 0;
			}
			SetCursor(hcur_arrow);
			if (GetCapture() != hWnd)
			{
				m_pControl->m_DrageControl = 0;
				return 0;
			}

			POINT ptCurrent;
			GetCursorPos(&ptCurrent);
			int32_t xDelta = ptCurrent.x - m_ptTickDrag.x;
			int32_t R = m_clTickDrag.R;
			int32_t G = m_clTickDrag.G;
			int32_t B = m_clTickDrag.B;
			switch(m_Type)
			{
			case HSB_TYPE_R:
				R += xDelta; if (R < 0) R = 0; if (R > 255) R = 255; 
				break;
			case HSB_TYPE_G:
				G += xDelta; if (G < 0) G = 0; if (G > 255) G = 255; 
				break;
			case HSB_TYPE_B:
				B += xDelta; if (B < 0) B = 0; if (B > 255) B = 255; 
				break;
			}

			m_pControl->OnColorChanged(R, G, B);

		}
		return TRUE;

	case WM_LBUTTONDOWN:
		{
			m_pControl->m_DrageControl = 1;
			m_clTickDrag = m_pControl->m_Color;
			GetCursorPos(&m_ptTickDrag);
			SetCapture(hWnd);
		}
		return TRUE;

	case WM_CANCELMODE:
		{
			m_pControl->m_DrageControl = 0;
		}
		return TRUE;

	case WM_LBUTTONUP:
		{
			CHSBWindow *pData = (CHSBWindow *)GetWindowLongPtr(hWnd, 0);
			if (m_pControl->m_DrageControl)
			{
				m_pControl->m_DrageControl = 0;
				ReleaseCapture();
				return 0;
			}
		}
		return TRUE;
	}
	return FALSE;
}

void CHSBControlTick::Paint(Gdiplus::Graphics *pGraphics)
{
	FillMemory(m_pControl->m_BitmapTickBuffer, sizeof(m_pControl->m_BitmapTickBuffer), 0xFF);
	for(int i = 0; i < TICK_HEIGHT; i++)
	{
		CopyMemory(m_pControl->m_BitmapTickBuffer[i] + m_xPos, TickData[i], TICK_WIDTH * 4);
	}
	pGraphics->DrawImage(&m_pControl->m_BitmapTick, m_Left, m_Top);
}

void CHSBControlTick::SetXPosition(int32_t xPos)
{
	m_xPos = xPos;
	HDC hdc = GetDC(m_pControl->m_hWnd);
	Gdiplus::Graphics g(hdc);
	Paint(&g);
	ReleaseDC(m_pControl->m_hWnd, hdc);
}

// **************************************************
// Color
// **************************************************

void CHSBControlColor::Init(CHSBWindow *pControl, int32_t index)
{
	m_index = index;
	m_pControl = pControl;
	m_Left = (index % 3) * 32 + 2;
	m_Top = (index / 3) * 27 + COLOR_TOP;
	m_Width = 24;
	m_Height = 18;
	if (index >= 15) m_Top += 18;
	pControl->AddControl(this);
}

void CHSBControlColor::Repaint()
{
	HDC hdc = GetDC(m_pControl->m_hWnd);
	Gdiplus::Graphics g(hdc);
	Paint(&g);
	ReleaseDC(m_pControl->m_hWnd, hdc);
}

void CHSBControlColor::Enter(void)
{
	if (ColorDefined[m_index] == 0) return;
	m_Fouced = TRUE;
	Repaint();
}

void CHSBControlColor::Leave(void)
{
	if (ColorDefined[m_index] == 0) return;
	m_Fouced = FALSE;
	Repaint();
}

BOOL CHSBControlColor::DoMessage(HWND hWnd, UINT message, int32_t xPos, int32_t yPos)
{
	switch (message) 
	{
	case WM_LBUTTONDOWN:
		{
			if (ColorDefined[m_index] == 0) return FALSE;
			m_pControl->ChangedColor(ColorDefined[m_index]);
		}
		return TRUE;
	}
	return FALSE;
}

void CHSBControlColor::Paint(Gdiplus::Graphics *pGraphics)
{
	SolidBrush backBrush(ColorDefined[m_index]);
	pGraphics->FillRectangle(&backBrush, m_Left + 1, m_Top + 1, 24, 18);
	if (m_Fouced)
	{
		Pen BorderPen1(0xFFF29436);
		Pen BorderPen2(0xFFFFE294);
		pGraphics->DrawRectangle(&BorderPen1, m_Left, m_Top, 24 + 1, 18 + 1);
		pGraphics->DrawRectangle(&BorderPen2, m_Left + 1, m_Top + 1, 24 - 1, 18 - 1);
	}
	else
	{
		Pen BorderPen(0xFF828790, 1);	
		pGraphics->DrawRectangle(&BorderPen, m_Left, m_Top, 24 + 1, 18 + 1);
	}
}

// **************************************************
// Main
// **************************************************
CHSBWindow::CHSBWindow(HWND hwndMessage, UINT uiMesage, ColorRef Color)
	:m_hwndMessage(hwndMessage), m_Color(Color), m_uiMesage(uiMesage),
	m_BitmapTick(TICK_WIDTH + 256, TICK_HEIGHT, (TICK_WIDTH + 256) * 4, PixelFormat32bppRGB, (BYTE *)m_BitmapTickBuffer),
	m_TextR(this, STATIC_LEFT, STATIC_Y_OFFSET(0), L"Red:"),
	m_TextG(this, STATIC_LEFT, STATIC_Y_OFFSET(1), L"Green:"),
	m_TextB(this, STATIC_LEFT, STATIC_Y_OFFSET(2), L"Blue:"),
	m_Colors(this, BASE_LEFT, COLOR_TOP),
	m_BarR(this, BAR_LEFT, BAR_Y_OFFSET(0), HSB_TYPE_R),
	m_BarG(this, BAR_LEFT, BAR_Y_OFFSET(1), HSB_TYPE_G),
	m_BarB(this, BAR_LEFT, BAR_Y_OFFSET(2), HSB_TYPE_B),
	m_TickR(this, TICK_LEFT, TICK_Y_OFFSET(0), HSB_TYPE_R),
	m_TickG(this, TICK_LEFT, TICK_Y_OFFSET(1), HSB_TYPE_G),
	m_TickB(this, TICK_LEFT, TICK_Y_OFFSET(2), HSB_TYPE_B)
{
	for(int i = 0; i < HSB_CONTROL_USER; i++)
	{
		m_Buttons[i].Init(this, i);
	}
	m_FcousControl = -1;
	m_Color.A = 0xFF;
};

void CHSBWindow::AddControl(CHSBControlBase *p)
{
	m_Controls[m_ControlCount] = p;
	m_ControlCount ++;
}

CHSBWindow::~CHSBWindow(void)
{
	DestroyWindow(m_hWnd);
}

void CHSBWindow::OnColorChanged(uint32_t R, uint32_t G, uint32_t B)
{
	if (R > 255) R = 255;
	if (G > 255) G = 255;
	if (B > 255) B = 255;

	if ((R == m_Color.R) && (G == m_Color.G) && (B == m_Color.B)) return ;

	m_Locked = 1;

	m_Color.R = R;
	m_Color.G = G;
	m_Color.B = B;

	UpdateUpdownValue();
	UpdateTickValue();
	CreateBarColor();

	SendMessage(m_hwndMessage, m_uiMesage, m_Color.Color, 0);

	m_Locked = 0;
}

void CHSBWindow::ChangedColor(uint32_t Color)
{
	m_Color.Color = Color;
	m_Loop = FALSE;
	SendMessage(m_hwndMessage, m_uiMesage, Color, 0);
}

void CHSBWindow::CreateBarColor()
{
	uint32_t R = m_Color.Color & 0x00FF0000;
	uint32_t G = m_Color.Color & 0x0000FF00;
	uint32_t B = m_Color.Color & 0x000000FF;

	for(uint32_t i = 0; i < COLOR_BAR_HEIGHT; i++)
	{
		for(uint32_t j = 0; j < 256; j++)
		{
			m_BarR.m_BitmapBuffer[i * 256 + j].Color = 0xFF000000 + (j << 16) + G + B;
			m_BarG.m_BitmapBuffer[i * 256 + j].Color = 0xFF000000 + (j << 8 ) + R + B;
			m_BarB.m_BitmapBuffer[i * 256 + j].Color = 0xFF000000 + (j << 0 ) + R + G;
		}
	}

	HDC hdc = GetDC(m_hWnd);
	Gdiplus::Graphics g(hdc);
	m_BarR.Paint(&g);
	m_BarG.Paint(&g);
	m_BarB.Paint(&g);
	ReleaseDC(m_hWnd, hdc);
}

void CHSBWindow::UpdateTickValue()
{
	m_TickR.SetXPosition(m_Color.R);
	m_TickG.SetXPosition(m_Color.G);
	m_TickB.SetXPosition(m_Color.B);
}

void CHSBWindow::UpdateUpdownValue()
{
	SendMessage(m_hwndRUpdown, UDM_SETPOS32, 0, m_Color.R);
	SendMessage(m_hwndGUpdown, UDM_SETPOS32, 0, m_Color.G);
	SendMessage(m_hwndBUpdown, UDM_SETPOS32, 0, m_Color.B);
}

static int32_t GetEditValue(HWND hWnd)
{
	wchar_t t[256];
	SendMessage(hWnd, WM_GETTEXT, 100, (LPARAM)(t));
	return _wtoi(t);
}


static LRESULT CALLBACK HSBColorSelectControlWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
	case WM_ERASEBKGND:
		return TRUE;

	case WM_PAINT:
		{
			CHSBWindow *pData = (CHSBWindow *)GetWindowLongPtr(hWnd, 0);
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			pData->OnPaint(hdc);
			EndPaint(hWnd, &ps);
		}
		return 0;

	case WM_COMMAND:
		{
			CHSBWindow *pData = (CHSBWindow *)GetWindowLongPtr(hWnd, 0);

			switch(HIWORD(wParam))
			{
			case EN_CHANGE:
				if (pData->m_Locked == 0)
				{
					uint32_t R = pData->m_Color.R;
					uint32_t G = pData->m_Color.G;
					uint32_t B = pData->m_Color.B;

					if ((HWND)lParam == pData->m_hwndREdit)
					{
						R = GetEditValue(pData->m_hwndREdit);
					}
					else if ((HWND)lParam == pData->m_hwndGEdit)
					{
						G = GetEditValue(pData->m_hwndGEdit);
					}
					else if ((HWND)lParam == pData->m_hwndBEdit)
					{
						B = GetEditValue(pData->m_hwndBEdit);
					}
					else
					{
						return 0;
					}

					pData->OnColorChanged(R, G, B);

					return 0;
				}
				break;
			}
		}
		break;

	case WM_VSCROLL:
		if (LOWORD(wParam) == SB_THUMBPOSITION)
		{
			CHSBWindow *pData = (CHSBWindow *)GetWindowLongPtr(hWnd, 0);
			if (pData->m_Locked == 0) 
			{
				if ((HWND)lParam == pData->m_hwndRUpdown)
				{
					pData->OnColorChanged(HIWORD(wParam), pData->m_Color.G, pData->m_Color.B);
				}
				else if ((HWND)lParam == pData->m_hwndGUpdown)
				{
					pData->OnColorChanged(pData->m_Color.R, HIWORD(wParam), pData->m_Color.B);
				}
				else if ((HWND)lParam == pData->m_hwndBUpdown)
				{
					pData->OnColorChanged(pData->m_Color.R, pData->m_Color.G, HIWORD(wParam));
				}
			}
		}
		return 0;

	case WM_ACTIVATE:
		if ((LOWORD(wParam) == WA_INACTIVE) && ((HWND)lParam != hWnd)) 
		{
			CHSBWindow *pData = (CHSBWindow *)GetWindowLongPtr(hWnd, 0);
			pData->m_Loop = FALSE;
		}
		return 0;

	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
		{
			CHSBWindow *pData = (CHSBWindow *)GetWindowLongPtr(hWnd, 0);
			pData->OnMessage(message, wParam, lParam);
		}
		return 0;
		
	case WM_NCHITTEST:
		{
			CHSBWindow *pData = (CHSBWindow *)GetWindowLongPtr(hWnd, 0);
			POINT pt;
			pt.x = GET_X_LPARAM(lParam); 
			pt.y = GET_Y_LPARAM(lParam); 
			ScreenToClient(hWnd, &pt);
			if (pData->HitTest(pt.x, pt.y) == -1) 
				return HTCAPTION;
		}
		break;

	case WM_KEYDOWN:
		if (wParam == 27)
		{
			CHSBWindow *pData = (CHSBWindow *)GetWindowLongPtr(hWnd, 0);
			pData->m_Loop = FALSE;
		}
		break;

	}

	return DefWindowProc(hWnd, message, wParam, lParam);
};

void CHSBWindow::OnPaint(HDC hdc)
{
	Graphics g(hdc);
	SolidBrush backBrush(0xFFFFFFFF);
	g.FillRectangle(&backBrush, 0, 0, SELECT_WIDTH, SELECT_HEIGHT);
	for(int i = 0; i < m_ControlCount; i++)
	{
		m_Controls[i]->Paint(&g);
	}

	Gdiplus::Pen grayPen1(0xFFB0B0B0, 1);
	Gdiplus::Pen grayPen2(0xFFE7EAEA, 1);
	g.DrawLine(&grayPen1, 2, 27 * 5 + 8, 102, 27 * 5 + 8);
	g.DrawLine(&grayPen2, 3, 27 * 5 + 9, 102, 27 * 5 + 9);

	SolidBrush solidBrush(0xFF000000); 
	StringFormat stringFormat;
	stringFormat.SetAlignment(StringAlignmentCenter);
	g.DrawString(L"Recent", -1, g_pFont, RectF(2, 27 * 5 + 2, 100, 24), &stringFormat, &solidBrush);

	g.DrawLine(&grayPen1, 105, 2, 105, SELECT_HEIGHT - 5);
	g.DrawLine(&grayPen2, 106, 3, 106, SELECT_HEIGHT - 4);
}

int32_t CHSBWindow::HitTest(int32_t x, int32_t y)
{
	for(int i = 0; i < m_ControlCount; i++)
	{
		if (
			(x > m_Controls[i]->m_Left) &&
			(x <= m_Controls[i]->m_Left + m_Controls[i]->m_Width + 1) &&
			(y > m_Controls[i]->m_Top) &&
			(y <= m_Controls[i]->m_Top + m_Controls[i]->m_Height + 1)
			)
		{
			return i;
		}
	}
	return -1;
}

void CHSBWindow::OnMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	int32_t xPos = GET_X_LPARAM(lParam); 
	int32_t yPos = GET_Y_LPARAM(lParam); 
	if (m_DrageControl == 0)
	{
		int32_t FcousControl = HitTest(xPos, yPos);
		if (m_FcousControl != FcousControl)
		{
			if (m_FcousControl != -1) m_Controls[m_FcousControl]->Leave();
			m_FcousControl = FcousControl;
			if (FcousControl != -1)m_Controls[m_FcousControl]->Enter();
		}
	}
	if (m_FcousControl != -1)
	{
		m_Controls[m_FcousControl]->DoMessage(m_hWnd, message, xPos - m_Controls[m_FcousControl]->m_Left - 1, yPos - m_Controls[m_FcousControl]->m_Top - 1);
	}
	else
	{
		SetCursor(hcur_arrow);
	}
}

void CHSBWindow::Run(HWND hwndParent, int32_t xPos, int32_t yPos)
{
	m_Locked = 1;

	if (xPos < 0) xPos = 0;
	if (yPos < 0) yPos = 0;
	if ((xPos + SELECT_WIDTH) > GetSystemMetrics(SM_CXSCREEN))
	{
		xPos = GetSystemMetrics(SM_CXSCREEN) - SELECT_WIDTH;
	}
	if ((yPos + SELECT_HEIGHT + 25) > GetSystemMetrics(SM_CYSCREEN))
	{
		yPos = GetSystemMetrics(SM_CYSCREEN) - SELECT_HEIGHT - 25;
	}
	int32_t Width = SELECT_WIDTH;
	int32_t Height = SELECT_HEIGHT;
	m_hWnd = CreateWindowEx(
		WS_EX_TOOLWINDOW | WS_EX_TOPMOST, 
		HSB_SELECT_WIN_CLASSNAME, 
		NULL, 
		WS_POPUP | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 
		xPos, yPos, Width, Height, 
		hwndParent, NULL, g_hInstance, NULL
		);
	SetWindowLongPtr(m_hWnd, 0, (LONG_PTR)this);
		
	// R
	int32_t y = SLIDER_TOP;

	m_hwndREdit = CreateWindowEx(
		WS_EX_NOPARENTNOTIFY | WS_EX_NOPARENTNOTIFY, 
		WC_EDIT,
		NULL, 
		WS_VISIBLE | WS_CHILD | WS_BORDER | ES_CENTER | ES_NUMBER ,
		EDIT_LEFT, y + EDIT_Y_OFFSET, EDIT_WIDTH, EDIT_HEIGHT, 
		m_hWnd, NULL, g_hInstance, NULL);

	m_hwndRUpdown = CreateWindowEx(WS_EX_NOPARENTNOTIFY, 
		UPDOWN_CLASS,
		NULL, 
		WS_VISIBLE | WS_CHILD | UDS_SETBUDDYINT | UDS_AUTOBUDDY | UDS_ARROWKEYS,
		UPDOWN_LEFT, y + UPDOWN_Y_OFFSET, UPDOWN_WIDTH, UPDOWN_HEIGHT, 
		m_hWnd, NULL, g_hInstance, NULL);

	// G
	y += SLIDER_HEIGHT;

	m_hwndGEdit = CreateWindowEx(WS_EX_NOPARENTNOTIFY, 
		WC_EDIT,
		NULL, 
		WS_VISIBLE | WS_CHILD | WS_BORDER | ES_CENTER | ES_NUMBER ,
		EDIT_LEFT, y + EDIT_Y_OFFSET, EDIT_WIDTH, EDIT_HEIGHT, 
		m_hWnd, NULL, g_hInstance, NULL);

	m_hwndGUpdown = CreateWindowEx(WS_EX_NOPARENTNOTIFY, 
		UPDOWN_CLASS,
		NULL, 
		WS_VISIBLE | WS_CHILD | UDS_SETBUDDYINT | UDS_AUTOBUDDY | UDS_ARROWKEYS,
		UPDOWN_LEFT, y + UPDOWN_Y_OFFSET, UPDOWN_WIDTH, UPDOWN_HEIGHT, 
		m_hWnd, NULL, g_hInstance, NULL);

	// B
	y += SLIDER_HEIGHT;

	m_hwndBEdit = CreateWindowEx(WS_EX_NOPARENTNOTIFY, 
		WC_EDIT,
		NULL, 
		WS_VISIBLE | WS_CHILD | WS_BORDER | ES_CENTER | ES_NUMBER ,
		EDIT_LEFT, y + EDIT_Y_OFFSET, EDIT_WIDTH, EDIT_HEIGHT, 
		m_hWnd, NULL, g_hInstance, NULL);

	m_hwndBUpdown = CreateWindowEx(WS_EX_NOPARENTNOTIFY, 
		UPDOWN_CLASS,
		NULL, 
		WS_VISIBLE | WS_CHILD | UDS_SETBUDDYINT | UDS_AUTOBUDDY | UDS_ARROWKEYS,
		UPDOWN_LEFT, y + UPDOWN_Y_OFFSET, UPDOWN_WIDTH, UPDOWN_HEIGHT, 
		m_hWnd, NULL, g_hInstance, NULL);

	SendMessage(m_hwndREdit, WM_SETFONT, (LPARAM)g_hDefaultFont, FALSE);
	SendMessage(m_hwndGEdit, WM_SETFONT, (LPARAM)g_hDefaultFont, FALSE);
	SendMessage(m_hwndBEdit, WM_SETFONT, (LPARAM)g_hDefaultFont, FALSE);

	SendMessage(m_hwndRUpdown, WM_SETFONT, (LPARAM)g_hDefaultFont, FALSE);
	SendMessage(m_hwndGUpdown, WM_SETFONT, (LPARAM)g_hDefaultFont, FALSE);
	SendMessage(m_hwndBUpdown, WM_SETFONT, (LPARAM)g_hDefaultFont, FALSE);

	SendMessage(m_hwndRUpdown, UDM_SETRANGE32, 0, 255); 
	SendMessage(m_hwndGUpdown, UDM_SETRANGE32, 0, 255); 
	SendMessage(m_hwndBUpdown, UDM_SETRANGE32, 0, 255); 

	CreateBarColor();
	UpdateUpdownValue();
	UpdateTickValue();

	m_Locked = 0;
	m_Loop = TRUE;
	HWND hwndFoucs = GetFocus();
	ShowWindow(m_hWnd, SW_SHOW);
	MSG msg;
	while ((m_Loop) && GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	SetFocus(hwndFoucs);

	for(int i = 0; i < HSB_CONTROL_USER; i++)
	{
		if (ColorDefined[i] == m_Color.Color) return ;
	}

	MoveMemory(&ColorDefined[16], &ColorDefined[15], (HSB_CONTROL_USER - 16) * 4);
	ColorDefined[15] = m_Color.Color;
	SaveCustomColor();
}

void __stdcall CreateHSBControl(HWND hwndParent, HWND hwndMessage, UINT uiMessage, ColorRef clColor)
{
	RECT rc;
	GetWindowRect(hwndMessage, &rc);
	int32_t xPos = rc.left;
	int32_t yPos = rc.bottom;

	CHSBWindow *pData = new CHSBWindow(hwndMessage, uiMessage, clColor);
	pData->Run(hwndParent, xPos, yPos);
	delete pData;
}

void __stdcall CreateHSBControlEx(HWND hwndParent, HWND hwndMessage, UINT uiMessage, ColorRef clColor, int32_t xPos, int32_t yPos)
{
	CHSBWindow *pData = new CHSBWindow(hwndMessage, uiMessage, clColor);
	pData->Run(hwndParent, xPos, yPos);
	delete pData;
}

#define COLOR_PATH			L"SOFTWARE\\VMeisoft\\Colors"
#define COLOR_NAME			L"Costom"

void LoadCustomColor()
{
	HKEY hKey;
	if( RegCreateKey(HKEY_CURRENT_USER, COLOR_PATH, &hKey) != ERROR_SUCCESS )
	{
		ErrorLog("Faild to RegCreateKey\n");
		return ;
	}

	DWORD cbData = (HSB_CONTROL_USER - 15) * 4;
	DWORD dwType = REG_BINARY;
	RegQueryValueEx(hKey, COLOR_NAME, 0, &dwType, (LPBYTE)&ColorDefined[15], &cbData);
	RegCloseKey(hKey);
}

void SaveCustomColor()
{
	HKEY hKey;
	if( RegCreateKey(HKEY_CURRENT_USER, COLOR_PATH, &hKey) != ERROR_SUCCESS )
	{
		ErrorLog("Faild to RegCreateKey\n");
		return ;
	}
	RegSetValueEx(hKey, COLOR_NAME, 0L, REG_BINARY, (CONST BYTE *)&ColorDefined[15], (HSB_CONTROL_USER - 15) * 4);
	RegCloseKey(hKey);
}

void InitHSBControlWindow()
{
	WNDCLASSEX wcex;

	// Selector
	ZeroMemory(&wcex, sizeof(wcex));
	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_DROPSHADOW;
	wcex.lpfnWndProc	= HSBColorSelectControlWindowProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= sizeof(void *);
	wcex.hInstance		= g_hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= NULL;
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= HSB_SELECT_WIN_CLASSNAME;
	wcex.hIconSm		= NULL;

	RegisterClassEx(&wcex);
	LoadCustomColor();
}
