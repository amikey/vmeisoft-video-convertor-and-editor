#pragma once
#include "VideoConverter.h"
#include "Subtitle.h"

#define COLOR_BAR_HEIGHT	10
#define COLOR_MAP_WIDTH		(64 * 6)
#define COLOR_WIDTH			32 
#define COLOR_HEIGHT		18
#define TICK_WIDTH			13
#define TICK_HEIGHT			7

#define HSB_TYPE_R	1
#define HSB_TYPE_G	2
#define HSB_TYPE_B	3

class CHSBWindow;

class CHSBControlBase
{
public:
	CHSBWindow*	m_pControl;
	int32_t			m_Left;
	int32_t			m_Top;
	int32_t			m_Width;
	int32_t			m_Height;

public:
	CHSBControlBase(CHSBWindow *pControl, int32_t Left, int32_t Top, int32_t Width, int32_t Height);
	CHSBControlBase(){};
	virtual ~CHSBControlBase();

public:
	virtual BOOL DoMessage(HWND hWnd, UINT message, int32_t xPos, int32_t yPos) = 0;
	virtual void Paint(Gdiplus::Graphics *pGraphics) = 0;
	virtual void Enter(void) = 0;
	virtual void Leave(void) = 0;
};

class CHSBControlText:public CHSBControlBase
{
private:
	wchar_t*		m_pText;

public:
	CHSBControlText(CHSBWindow *pControl, int32_t Left, int32_t Top, wchar_t *Text);
	virtual BOOL DoMessage(HWND hWnd, UINT message, int32_t xPos, int32_t yPos);
	virtual void Paint(Gdiplus::Graphics *pGraphics);
	virtual void Enter(void);
	virtual void Leave(void);
};

class CHSBControlColors:
	public CHSBControlBase
{
public:
	ColorRef			m_BitmapBuffer[COLOR_MAP_WIDTH * 256];
	Gdiplus::Bitmap		m_Bitmap;

public:
	CHSBControlColors(CHSBWindow *pControl, int32_t Left, int32_t Top);
	virtual BOOL DoMessage(HWND hWnd, UINT message, int32_t xPos, int32_t yPos);
	virtual void Paint(Gdiplus::Graphics *pGraphics);
	virtual void Enter(void);
	virtual void Leave(void);
};

class CHSBControlBar:
	public CHSBControlBase
{
public:
	int32_t					m_Type;
	ColorRef			m_BitmapBuffer[256 * COLOR_BAR_HEIGHT];
	Gdiplus::Bitmap		m_Bitmap;
		
public:
	CHSBControlBar(CHSBWindow *pControl, int32_t Left, int32_t Top, int32_t Type);
	virtual BOOL DoMessage(HWND hWnd, UINT message, int32_t xPos, int32_t yPos);
	virtual void Paint(Gdiplus::Graphics *pGraphics);
	virtual void Enter(void);
	virtual void Leave(void);
};

class CHSBControlTick:
	public CHSBControlBase
{
private:
	int32_t					m_Type;
	int32_t					m_xPos;
	POINT				m_ptTickDrag;
	ColorRef			m_clTickDrag;
	
public:
	CHSBControlTick(CHSBWindow *pControl, int32_t Left, int32_t Top, int32_t Type);
	virtual BOOL DoMessage(HWND hWnd, UINT message, int32_t xPos, int32_t yPos);
	virtual void Paint(Gdiplus::Graphics *pGraphics);
	void SetXPosition(int32_t xPos);
	virtual void Enter(void);
	virtual void Leave(void);
};

class CHSBControlColor:
	public CHSBControlBase
{
private:
	int32_t			m_index;
	BOOL		m_Fouced;

public:
	void Init(CHSBWindow *pControl, int32_t index);
	virtual BOOL DoMessage(HWND hWnd, UINT message, int32_t xPos, int32_t yPos);
	virtual void Paint(Gdiplus::Graphics *pGraphics);
	virtual void Enter(void);
	virtual void Leave(void);
	void Repaint();
};

#define HSB_CONTROL_USER	36

class CHSBWindow
{
public:
	HWND				m_hwndMessage;
	UINT				m_uiMesage;
	HWND				m_hWnd;

	ColorRef			m_Color;
	int32_t				m_ControlCount;
	CHSBControlBase*	m_Controls[64];

	CHSBControlColors	m_Colors;
	CHSBControlText		m_TextR, m_TextG, m_TextB;
	CHSBControlBar		m_BarR, m_BarG, m_BarB;
	CHSBControlTick		m_TickR, m_TickG, m_TickB;
	CHSBControlColor	m_Buttons[HSB_CONTROL_USER];

	Gdiplus::Bitmap		m_BitmapTick;
	DWORD				m_BitmapTickBuffer[TICK_HEIGHT][TICK_WIDTH + 256];
		
	HWND				m_hwndREdit;
	HWND				m_hwndGEdit;
	HWND				m_hwndBEdit;
	HWND				m_hwndRUpdown;
	HWND				m_hwndGUpdown;
	HWND				m_hwndBUpdown;

	int32_t				m_DrageControl;
	int32_t				m_FcousControl;
	int32_t				m_Locked;
	BOOL				m_Loop;

public:
	CHSBWindow(HWND hwndMessage, UINT uiMesage, ColorRef Color);
	~CHSBWindow(void);
	void AddControl(CHSBControlBase *p);

public:
	void OnColorChanged(uint32_t R, uint32_t G, uint32_t B);
	void ChangedColor(uint32_t Color);
	int32_t HitTest(int32_t x, int32_t y);

public:
	void OnPaint(HDC hdc);
	void CreateBarColor();
	void UpdateTickValue();
	void UpdateUpdownValue();
	void Run(HWND hwndParent, int32_t xPos, int32_t yPos);
	void OnMessage(UINT message, WPARAM wParam, LPARAM lParam);
};

extern void InitHSBControlWindow();
extern void LoadCustomColor();
extern void SaveCustomColor();

