#pragma once
#include "SubtitleInt.h"

class CVideoRender;

class CSubtitlePlayer
{
private:
	CVideoRender*		m_Render;
	HWND				m_hwndParent;
	HWND				m_hwndMessage;

private:
	uint8_t*			m_BkBuffer;
	Bitmap*				m_BackImage;
	Graphics*			m_BackGraphics;
	CSubtitleItem*		m_Item;
	INT					m_Width, m_Height;
	int32_t					m_FoucsIndex;

public:
	CSubtitlePlayer(HWND hwndParent, HWND hwndMessage, CSubtitleItem *item);
	~CSubtitlePlayer(void);
	HRESULT Initialize();

private:
	int64_t		m_TimerTick;
	int64_t		m_TimerStop;
	UINT_PTR	m_idTimer;
	void OnEffect(int64_t time);
	Point GetPoint(int32_t type);
	void FillBackground();

public:
	void OnTimer();

private:
	Point		m_ptIn;
	Point		m_ptSubtitle;
	Point		m_ptOut;

public:
	void Play();
	void PlayEx(int32_t index, BOOL IsExitEffect);
	void Stop();
	void Redraw();
	void SetFocus(int32_t index);
};

extern SubtitleDefault g_subtitle_default;
