#include "stdafx.h"
#include "VideoConverter.h"
#include "Subtitle.h"
#include "VideoRender.h"

#define FRAME_RATE			10
#define FRAME_TIME			(1000 / FRAME_RATE)
#define FRAME_TIME_BASE		(AV_TIME_BASE_LL / FRAME_RATE)
#define BODER_SPACE			16

#define FONT_BORDER_WIDTH	1.0f
#define FONT_SHADOW_WIDTH	3.0f

static GUID GUID_NULL;

/*********************************************
// CSubtitleLine
*********************************************/

CSubtitleLine::CSubtitleLine()
{
	wcscpy(m_FontName, g_subtitle_default.m_FontName); // 字体名称
	m_FontSize = g_subtitle_default.m_FontSize; // 字体尺寸
	m_FontStyle = g_subtitle_default.m_FontStyle; // 字体风格
	m_FontColor = g_subtitle_default.m_FontColor; // 字体颜色：0xAARRGGBB
	m_BorderColor = g_subtitle_default.m_BorderColor; // 边框颜色：0xAARRGGBB
	m_ShadowColor = g_subtitle_default.m_ShadowColor; // 边框颜色：0xAARRGGBB

	m_Entrance = g_subtitle_default.m_Entrance; // 进入效果，如果为GUID_NULL, 则无效果
	m_EntranceParam = g_subtitle_default.m_EntranceParam; // 进入参数； SLIDE_DIRECTION_
	m_EntranceTime = g_subtitle_default.m_EntranceTime;	// 进入时长，时间越小，速度越快
	m_EntranceOnce = g_subtitle_default.m_EntranceOnce;

	m_Exit = g_subtitle_default.m_Exit; // 退出效果，如果为GUID_NULL, 则无效果
	m_ExitParam = g_subtitle_default.m_ExitParam; // 退出参数； SLIDE_DIRECTION_
	m_ExitTime = g_subtitle_default.m_ExitTime; // 退出时长，时间越小，速度越快
	m_ExitOnce = g_subtitle_default.m_ExitOnce; // 退出时长，时间越小，速度越快

    m_Alignment = g_subtitle_default.m_Alignment; // 对齐方式：左，中，右边
}

RectF CSubtitleLine::CalcBounds()
{
	Gdiplus::GraphicsPath path;
	Gdiplus::FontFamily fontFamily(m_FontName);
	Gdiplus::Rect origin;
	Gdiplus::FontStyle fontStyle = (Gdiplus::FontStyle)m_FontStyle; 
	float fontSize = m_FontSize;
	path.AddString(m_Title, -1, &fontFamily, fontStyle, fontSize, origin, NULL); 

	Gdiplus::Pen penEmpty(0xFF000000, 1.0f);
	path.GetBounds(&m_Bounds, NULL, &penEmpty);

	return m_Bounds;
}

void CSubtitleLine::CalcEffect(int64_t time, PointF &point, ColorRef &FontColor, ColorRef &BorderColor, ColorRef &ShadowColor)
{
	if (m_EntranceStop > time)
	{
		if (m_Entrance == IID_Subtitle_Fade)
		{
			DWORD MinFontColor = FontColor.A / 16;
			DWORD MinBorderColor = BorderColor.A / 16;
			FontColor.A = (int64_t)FontColor.A * (time - m_EntranceStart) / m_EntranceTime;
			BorderColor.A = (int64_t)BorderColor.A * (time - m_EntranceStart) / m_EntranceTime;
			ShadowColor.A = (int64_t)ShadowColor.A * (time - m_EntranceStart) / m_EntranceTime;
			if (FontColor.A < MinFontColor) FontColor.A = MinFontColor;
			if (BorderColor.A < MinBorderColor) BorderColor.A = MinBorderColor;
			
		}
		else if (m_Entrance == IID_Subtitle_Slide)
		{
			point.X = m_PointStart.X + (double)(m_Position.X - m_PointStart.X) * (double)(time - m_EntranceStart) / (double)m_EntranceTime;
			point.Y = m_PointStart.Y + (double)(m_Position.Y - m_PointStart.Y) * (double)(time - m_EntranceStart) / (double)m_EntranceTime;
		}
	}
	else if (m_ExitStart < time)
	{
		if (m_Exit == IID_Subtitle_Fade)
		{
			DWORD MinBorderColor = BorderColor.A / 16;
			DWORD MinFontColor = FontColor.A / 16;
			BorderColor.A = (int64_t)BorderColor.A * (m_ExitTime - (time - m_ExitStart)) / m_ExitTime;
			FontColor.A = (int64_t)FontColor.A * (m_ExitTime - (time - m_ExitStart)) / m_ExitTime;
			if (BorderColor.A < MinBorderColor) BorderColor.A = MinBorderColor;
			if (FontColor.A < MinFontColor) FontColor.A = MinFontColor;
		}
		else if (m_Exit == IID_Subtitle_Slide)
		{
			point.X = m_Position.X + (double)(m_PointStop.X - m_Position.X) * (double)(time - m_ExitStart) / (double)m_ExitTime;
			point.Y = m_Position.Y + (double)(m_PointStop.Y - m_Position.Y) * (double)(time - m_ExitStart) / (double)m_ExitTime;
		}
	}
}

/*********************************************
// CSubtitleItem
*********************************************/
CSubtitleItem::CSubtitleItem()
{
    m_HPosition = g_subtitle_default.m_HPosition; // 水平位置
    m_VPosition = g_subtitle_default.m_VPosition; // 垂直位置
	m_LineSpace = g_subtitle_default.m_LineSpace; // 行间距
	m_Postion = g_subtitle_default.m_Postion; // 字体位置，如果SubtitleAlignment = SubtitleAlignmentNone
}

CSubtitleItem::~CSubtitleItem()
{
	for(int i = 0; i < m_Count; i++)
	{
		delete m_Lines[i];
	}
	SAFE_FREE(m_Lines);
}

CSubtitleLine* CSubtitleItem::NewLine()
{
	CSubtitleLine *p = new CSubtitleLine();
	p->m_index = m_Count;
	m_Lines = (CSubtitleLine**)MemoryRealloc(m_Lines, (m_Count + 1) * sizeof(CSubtitleLine*));
	m_Lines[m_Count] = p;
	m_Count ++;
	return p;
}

void CSubtitleItem::AddLine(wchar_t *Text)
{
	CSubtitleLine *line = new CSubtitleLine();
	StringCchCopy(line->m_Title, MAX_SUBTITLE_LINE_SIZE, Text);
	line->m_index = m_Count;
	m_Lines = (CSubtitleLine**)MemoryRealloc(m_Lines, (m_Count + 1) * sizeof(CSubtitleLine*));
	m_Lines[m_Count] = line;
	m_Count ++;
}

void CSubtitleItem::DeleteLine(int32_t index)
{
	delete m_Lines[index];
	m_Count --;
	for(int i = index; i < m_Count; i++)
	{
		m_Lines[i] = m_Lines[i + 1];
	}
}

void CSubtitleItem::CalcParamters(int32_t viewWidth, int32_t viewHeight)
{
	int64_t EnterStart = m_StartTime;
	for(int i = 0; i < m_Count; i++)
	{
		m_Lines[i]->m_EntranceStart = EnterStart;
		if (m_Lines[i]->m_Entrance != GUID_NULL)
		{
			m_Lines[i]->m_EntranceStop = EnterStart + m_Lines[i]->m_EntranceTime;
			EnterStart += m_Lines[i]->m_EntranceTime;
		}
		else
		{
			m_Lines[i]->m_EntranceStop = EnterStart;
		}
	}

	int64_t ExitDuration = 0;
	for(int i = 0; i < m_Count; i++)
	{
		if (m_Lines[i]->m_Entrance != GUID_NULL)
		{
			ExitDuration += m_Lines[i]->m_ExitTime;
		}
	}

	int64_t StopBegin = 0;
	if (EnterStart + ExitDuration <= m_Duration)
	{
		StopBegin = m_StartTime + m_Duration - ExitDuration;
	}
	else
	{
		StopBegin = m_StartTime + EnterStart;
	}

	for(int i = 0; i < m_Count; i++)
	{
		m_Lines[i]->m_ExitStart = StopBegin;
		if (m_Lines[i]->m_Exit != GUID_NULL)
		{
			m_Lines[i]->m_ExitStop = StopBegin + m_Lines[i]->m_ExitTime;
			StopBegin += m_Lines[i]->m_ExitTime;
		}
		else
		{
			m_Lines[i]->m_ExitStop = StopBegin;
		}
	}

	int32_t maxWidth = 0, maxHeight = 0;
	for(int i = 0; i < m_Count; i++)
	{
		RectF bounds = m_Lines[i]->CalcBounds();
		if (bounds.Width > maxWidth) maxWidth = bounds.Width;
		maxHeight += bounds.Height + m_LineSpace;
	}

	int32_t xPos;
	switch(m_HPosition)
	{
	case SubtitleAlignmentNear:
		xPos = 0;
		break;
	case SubtitleAlignmentCenter:
		xPos = (viewWidth - maxWidth) / 2;
		break;
	case SubtitleAlignmentFar:
		xPos = viewWidth - maxWidth;
		break;
	default:
		xPos = m_Postion.X;
		break;
	}

	int32_t yPos;
	switch(m_VPosition)
	{
	case SubtitleAlignmentNear:
		yPos = viewHeight / BODER_SPACE;
		break;

	case SubtitleAlignmentCenter:
		yPos = (viewHeight - maxHeight) / 2;
		break;

	case SubtitleAlignmentFar:
		yPos = viewHeight - maxHeight - viewHeight / BODER_SPACE;
		break;

	default:
		yPos = m_Postion.Y;
		break;
	}

	int32_t yPosStart = 0;
	for(int i = 0; i < m_Count; i++)
	{
		// X
		switch(m_Lines[i]->m_Alignment)
		{
		case SubtitleAlignmentNear:
			m_Lines[i]->m_Position.X = xPos;
			break;
		case SubtitleAlignmentCenter:
			m_Lines[i]->m_Position.X = xPos + (maxWidth - m_Lines[i]->m_Bounds.Width) / 2;
			if (m_Lines[i]->m_Position.X < 0) m_Lines[i]->m_Position.X = 0;
			break;
		case SubtitleAlignmentFar:
			m_Lines[i]->m_Position.X = xPos  + (maxWidth - m_Lines[i]->m_Bounds.Width);
			break;
		}
		m_Lines[i]->m_Position.X += -m_Lines[i]->m_Bounds.X;

		// Y
		m_Lines[i]->m_Position.Y = yPos + yPosStart;
		m_Lines[i]->m_Position.Y += -m_Lines[i]->m_Bounds.Y;
		yPosStart += m_Lines[i]->m_Bounds.Height + m_LineSpace;

		if (m_Lines[i]->m_Entrance == IID_Subtitle_Slide)
		{
			m_Lines[i]->m_PointStart.X = -m_Lines[i]->m_Bounds.X;
			m_Lines[i]->m_PointStart.Y = -m_Lines[i]->m_Bounds.Y;
			switch(m_Lines[i]->m_EntranceParam)
			{
			case SLIDE_DIRECTION_LEFT:
				m_Lines[i]->m_PointStart.X += 0;
				m_Lines[i]->m_PointStart.Y += m_Lines[i]->m_Position.Y;
				break;
			case SLIDE_DIRECTION_TOP:
				m_Lines[i]->m_PointStart.X += m_Lines[i]->m_Position.X;
				m_Lines[i]->m_PointStart.Y += 0;
				break;
			case SLIDE_DIRECTION_RIGHT:
				m_Lines[i]->m_PointStart.X += viewWidth - m_Lines[i]->m_Bounds.Width;
				m_Lines[i]->m_PointStart.Y += m_Lines[i]->m_Position.Y;
				break;
			case SLIDE_DIRECTION_BOTTOM:
				m_Lines[i]->m_PointStart.X += m_Lines[i]->m_Position.X;
				m_Lines[i]->m_PointStart.Y += viewHeight - m_Lines[i]->m_Bounds.Height;
				break;
			case SLIDE_DIRECTION_LEFT_TOP:
				m_Lines[i]->m_PointStart.X += 0;
				m_Lines[i]->m_PointStart.Y += 0;
				break;
			case SLIDE_DIRECTION_RIGHT_TOP:
				m_Lines[i]->m_PointStart.X += viewWidth - m_Lines[i]->m_Bounds.Width;
				m_Lines[i]->m_PointStart.Y += 0;
				break;
			case SLIDE_DIRECTION_LEFT_BOTTOM:
				m_Lines[i]->m_PointStart.X += 0;
				m_Lines[i]->m_PointStart.Y += viewHeight - m_Lines[i]->m_Bounds.Height;
				break;
			case SLIDE_DIRECTION_RIGHT_BOTTOM:
				m_Lines[i]->m_PointStart.X += viewWidth - m_Lines[i]->m_Bounds.Width;
				m_Lines[i]->m_PointStart.Y += viewHeight - m_Lines[i]->m_Bounds.Height;
				break;
			}
		}

		if (m_Lines[i]->m_Exit == IID_Subtitle_Slide)
		{
			m_Lines[i]->m_PointStop.X = -m_Lines[i]->m_Bounds.X;
			m_Lines[i]->m_PointStop.Y = -m_Lines[i]->m_Bounds.Y;
			switch(m_Lines[i]->m_ExitParam)
			{
			case SLIDE_DIRECTION_LEFT:
				m_Lines[i]->m_PointStop.X += 0;
				m_Lines[i]->m_PointStop.Y += m_Lines[i]->m_Position.Y;
				break;
			case SLIDE_DIRECTION_TOP:
				m_Lines[i]->m_PointStop.X += m_Lines[i]->m_Position.X;
				m_Lines[i]->m_PointStop.Y += 0;
				break;
			case SLIDE_DIRECTION_RIGHT:
				m_Lines[i]->m_PointStop.X += viewWidth - m_Lines[i]->m_Bounds.Width;
				m_Lines[i]->m_PointStop.Y += m_Lines[i]->m_Position.Y;
				break;
			case SLIDE_DIRECTION_BOTTOM:
				m_Lines[i]->m_PointStop.X += m_Lines[i]->m_Position.X;
				m_Lines[i]->m_PointStop.Y += viewHeight - m_Lines[i]->m_Bounds.Height;
				break;
			case SLIDE_DIRECTION_LEFT_TOP:
				m_Lines[i]->m_PointStop.X += 0;
				m_Lines[i]->m_PointStop.Y += 0;
				break;
			case SLIDE_DIRECTION_RIGHT_TOP:
				m_Lines[i]->m_PointStop.X += viewWidth - m_Lines[i]->m_Bounds.Width;
				m_Lines[i]->m_PointStop.Y += 0;
				break;
			case SLIDE_DIRECTION_LEFT_BOTTOM:
				m_Lines[i]->m_PointStop.X += 0;
				m_Lines[i]->m_PointStop.Y += viewHeight - m_Lines[i]->m_Bounds.Height;
				break;
			case SLIDE_DIRECTION_RIGHT_BOTTOM:
				m_Lines[i]->m_PointStop.X += viewWidth - m_Lines[i]->m_Bounds.Width;
				m_Lines[i]->m_PointStop.Y += viewHeight - m_Lines[i]->m_Bounds.Height;
				break;
			}
		}
	}

}

void CSubtitleItem::DrawInto(Gdiplus::Graphics* graphics, int64_t time, int32_t FocusIndex)
{
	for(int i = 0; i < m_Count; i++)
	{
		if ((m_Lines[i]->m_EntranceStart > time) || (m_Lines[i]->m_ExitStop < time))
		{
			continue;
		}

		Gdiplus::GraphicsPath path;
		Gdiplus::FontFamily fontFamily(m_Lines[i]->m_FontName);
		Gdiplus::FontStyle fontStyle = (Gdiplus::FontStyle)m_Lines[i]->m_FontStyle; 
		float fontSize = m_Lines[i]->m_FontSize;

		Gdiplus::PointF point = m_Lines[i]->m_Position;
		ColorRef FontColor = m_Lines[i]->m_FontColor;
		ColorRef BorderColor = m_Lines[i]->m_BorderColor;
		ColorRef ShadowColor = m_Lines[i]->m_ShadowColor;
		m_Lines[i]->CalcEffect(time, point, FontColor, BorderColor, ShadowColor);

		path.AddString(m_Lines[i]->m_Title, -1, &fontFamily, fontStyle, fontSize, point, NULL); 
		Gdiplus::Pen penBorder(m_Lines[i]->m_BorderColor.Color, FONT_BORDER_WIDTH);
		Gdiplus::Pen penShadow(m_Lines[i]->m_ShadowColor.Color, FONT_SHADOW_WIDTH);
		penBorder.SetAlignment(PenAlignmentCenter);
		Gdiplus::SolidBrush FontBrush(FontColor.Color);
		graphics->DrawPath(&penShadow, &path);
		graphics->DrawPath(&penBorder, &path);
		graphics->FillPath(&FontBrush, &path);

		if (FocusIndex == i)
		{
			Gdiplus::Rect bounds;
			Gdiplus::Pen penRed(0xFFFF0000, 1.0f);
			path.GetBounds(&bounds, NULL, &penRed);
			graphics->DrawRectangle(&penRed, bounds);
		}
	}

}

void CSubtitleItem::AddItem(CSubtitleItem *item)
{
	m_Lines = (CSubtitleLine **)MemoryRealloc(m_Lines, (m_Count + item->m_Count) * sizeof(CSubtitleLine *));
	for(int i = 0; i < item->m_Count; i++)
	{
		m_Lines[m_Count + i] = item->m_Lines[i];
	}
	m_Count += item->m_Count;
	item->m_Count = 0;
	SAFE_FREE(item->m_Lines);
	delete item;
}

/*********************************************
// CSubtitle
*********************************************/
CSubtitle::CSubtitle()
{

}

CSubtitle::~CSubtitle()
{
	for(int i = 0; i < m_Count; i++)
	{
		delete m_Items[i];
	}
	MemoryFree(m_Items);
}

CSubtitleItem* CSubtitle::NewItem()
{
	CSubtitleItem *p = new CSubtitleItem();
	p->m_index = m_Count;
	m_Items = (CSubtitleItem **)MemoryRealloc(m_Items, (m_Count + 1) * sizeof(CSubtitleItem*));
	m_Items[m_Count] = p;
	m_Count ++;
	return p;
}

void CSubtitle::DeleteItem(int32_t index)
{
	delete m_Items[index];
	m_Count --;
	for(int i = index; i < m_Count; i++)
	{
		m_Items[i] = m_Items[i + 1];
	}
}

void CSubtitle::SortItem()
{
	for(int i = 1; i < m_Count; i++)
	{
		BOOL b = FALSE;
		for(int j = 0; j < m_Count - i; j++)
		{
			if (m_Items[j]->m_StartTime > m_Items[j + 1]->m_StartTime)
			{
				CSubtitleItem *p = m_Items[j];
				m_Items[j] = m_Items[j + 1]; m_Items[j + 1] = p;
				b = TRUE;
			}
		}
		if (b == FALSE) break;
	}
}

void CSubtitle::CalcParamters(int32_t viewWidth, int32_t viewHeight)
{
	for(int i = 0; i < m_Count; i++)
	{
		m_Items[i]->CalcParamters(viewWidth, viewHeight);
	}
}

inline void CSubtitleItem::ConvertFrameToTime(int64_t num, int64_t den)
{
	m_StartTime = m_StartTime * num / den;
	m_Duration = m_Duration * num / den;
}

void CSubtitle::ConvertFrameToTime(int64_t num, int64_t den)
{
	for(int i = 0; i < m_Count; i++)
	{
		m_Items[i]->ConvertFrameToTime(num, den);
	}
}

void CSubtitle::CheckTime(int64_t duration)
{
	if ((m_Items[m_Count - 1]->m_Duration == 0) && (m_Items[m_Count - 1]->m_StartTime < duration))
	{
		m_Items[m_Count - 1]->m_Duration = duration - m_Items[m_Count - 1]->m_StartTime;
	}
	for(int i = 0; i < m_Count - 2; i++)
	{
		if (m_Items[i]->m_Duration == 0)
		{
			m_Items[i]->m_Duration = m_Items[i + 1]->m_StartTime - m_Items[i]->m_StartTime;
		}
	}
}

void CSubtitle::MergeTime()
{
	int32_t i = 0; 
	while (i < m_Count - 1)
	{
		if (m_Items[i]->m_StartTime == m_Items[i + 1]->m_StartTime)
		{
			m_Items[i]->AddItem(m_Items[i + 1]);
			m_Count --;
			for(int j = i + 1; j < m_Count; j++)
			{
				m_Items[j] = m_Items[j + 1];
			}
		}
		else
		{
			i++;
		}
	}
}

/*********************************************
// Player
*********************************************/

static CSubtitlePlayer *g_SubtitlePlayer;

static void CALLBACK SubtitleTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	if (g_SubtitlePlayer == NULL) return ;
	g_SubtitlePlayer->OnTimer();
}


CSubtitlePlayer::CSubtitlePlayer(HWND hwndParent, HWND hwndMessage, CSubtitleItem *item)
{
	g_SubtitlePlayer = this;
	m_hwndParent = hwndParent;
	m_hwndMessage = hwndMessage;
	m_Item = item;

	RECT rc;
	GetClientRect(m_hwndParent, &rc);
	m_Width = rc.right - 2;
	m_Height = rc.bottom - 2;
}

CSubtitlePlayer::~CSubtitlePlayer(void)
{
	if (m_idTimer) KillTimer(NULL, m_idTimer);
	g_SubtitlePlayer = NULL;
	SAFE_DELETE(m_Render);
	SAFE_DELETE(m_BackImage);
	SAFE_DELETE(m_BackGraphics);
}

HRESULT CSubtitlePlayer::Initialize()
{
	m_Render = new CVideoRender(m_hwndParent, m_hwndMessage, m_Width, m_Height);
	if (m_Render->Init() != S_OK) return E_FAIL;
	m_BkBuffer = m_Render->GetBuffer();
	m_BackImage = new Bitmap(m_Width, m_Height, m_Width * 4, PixelFormat32bppRGB, m_BkBuffer);
	m_BackGraphics = new Graphics(m_BackImage);
	m_BackGraphics->SetPageUnit(Gdiplus::UnitPixel);
	m_BackGraphics->SetCompositingMode(Gdiplus::CompositingModeSourceOver);
	m_BackGraphics->SetInterpolationMode(Gdiplus::InterpolationModeHighQuality);
	m_BackGraphics->SetCompositingQuality(Gdiplus::CompositingQualityHighQuality);
	m_BackGraphics->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
	m_BackGraphics->SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
	m_BackGraphics->SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
	Redraw();
	return S_OK;
}

void CSubtitlePlayer::OnTimer()
{
	if (m_idTimer == 0) return ;
	m_TimerTick += FRAME_TIME_BASE;
	if (m_TimerTick > m_TimerStop)
	{
		Redraw();
		PostMessage(m_hwndMessage, CM_SUBTITLE_PLAYER_END, (WPARAM)this, 0);
	}
	else
	{
		OnEffect(m_TimerTick);
	}
}

void CSubtitlePlayer::Play()
{
	Stop();
	m_TimerTick = m_Item->m_StartTime;
	m_TimerStop = m_Item->m_StartTime + m_Item->m_Duration;
	m_Item->CalcParamters(m_Width, m_Height);
	m_idTimer = SetTimer(NULL, (UINT_PTR)this, FRAME_TIME, SubtitleTimerProc);
	OnEffect(m_TimerTick);
}

void CSubtitlePlayer::PlayEx(int32_t index, BOOL IsExitEffect)
{
	Stop();
	m_Item->CalcParamters(m_Width, m_Height);
	if (IsExitEffect)
	{
		m_TimerTick = m_Item->m_Lines[index]->m_ExitStart;
		m_TimerStop = m_Item->m_Lines[index]->m_ExitStop;
	}
	else
	{
		m_TimerTick = m_Item->m_Lines[index]->m_EntranceStart;
		m_TimerStop = m_Item->m_Lines[index]->m_EntranceStop;
	}
	m_idTimer = SetTimer(NULL, (UINT_PTR)this, FRAME_TIME, SubtitleTimerProc);
	OnEffect(m_TimerTick);
}

void CSubtitlePlayer::Stop()
{
	if (m_idTimer)
	{
		KillTimer(NULL, m_idTimer);
		m_idTimer = 0;
	}
}

Point CSubtitlePlayer::GetPoint(int32_t type)
{
	Point pt = m_ptSubtitle;
	switch(type)
	{
	case SLIDE_DIRECTION_LEFT:
		pt.X = 0;
		break;
	case SLIDE_DIRECTION_TOP:
		pt.Y = 0;
		break;
	case SLIDE_DIRECTION_RIGHT:
		pt.X = g_enc_opt.m_VideoWidth - 1;
		break;
	case SLIDE_DIRECTION_BOTTOM:
		pt.Y = g_enc_opt.m_VideoHeight - 1;
		break;
	case SLIDE_DIRECTION_LEFT_TOP:
		pt.X = 0;
		pt.Y = 0;
		break;
	case SLIDE_DIRECTION_RIGHT_TOP:
		pt.X = g_enc_opt.m_VideoWidth - 1;
		pt.Y = 0;
		break;
	case SLIDE_DIRECTION_LEFT_BOTTOM:
		pt.X = 0;
		pt.Y = g_enc_opt.m_VideoHeight - 1;
		break;
	case SLIDE_DIRECTION_RIGHT_BOTTOM:
		pt.X = g_enc_opt.m_VideoWidth - 1;
		pt.Y = g_enc_opt.m_VideoHeight - 1;
		break;
	}
	return pt;
}

void CSubtitlePlayer::OnEffect(int64_t time)
{
	FillBackground();
	m_Item->DrawInto(m_BackGraphics, time, m_FoucsIndex);
	m_Render->Process(m_BkBuffer, 0);
}

void CSubtitlePlayer::Redraw()
{
	Stop();
	FillBackground();
	m_Item->CalcParamters(m_Width, m_Height);
	for(int i = 0; i < m_Item->m_Count; i++)
	{
		Gdiplus::GraphicsPath path;
		Gdiplus::FontFamily fontFamily(m_Item->m_Lines[i]->m_FontName);
		Gdiplus::PointF point(m_Item->m_Lines[i]->m_Position.X, m_Item->m_Lines[i]->m_Position.Y);
		Gdiplus::FontStyle fontStyle = (Gdiplus::FontStyle)m_Item->m_Lines[i]->m_FontStyle; 
		float fontSize = m_Item->m_Lines[i]->m_FontSize;

		path.AddString(m_Item->m_Lines[i]->m_Title, -1, &fontFamily, fontStyle, fontSize, point, NULL); 

		Gdiplus::Pen penShadow(m_Item->m_Lines[i]->m_ShadowColor.Color, FONT_SHADOW_WIDTH);
		Gdiplus::Pen penBorder(m_Item->m_Lines[i]->m_BorderColor.Color, FONT_BORDER_WIDTH);
		penBorder.SetAlignment(PenAlignmentCenter);
		Gdiplus::SolidBrush FontBrush(m_Item->m_Lines[i]->m_FontColor.Color);

		m_BackGraphics->DrawPath(&penShadow, &path);
		m_BackGraphics->DrawPath(&penBorder, &path);
		m_BackGraphics->FillPath(&FontBrush, &path);

		if (m_FoucsIndex == i)
		{
			Gdiplus::Rect bounds;
			Gdiplus::Pen penRed(0xFFFF0000, 1.0f);
			path.GetBounds(&bounds, NULL, &penRed);
			m_BackGraphics->DrawRectangle(&penRed, bounds);
		}
	}
	m_Render->Process(m_BkBuffer, 0);
}

void CSubtitlePlayer::SetFocus(int32_t index)
{
	m_FoucsIndex = index;
	Redraw();
}

#define BK_COLOR1 0xFFD0D0D0
#define BK_COLOR2 0xFF808080

static uint32_t inline GetBackColor(int32_t i, int32_t j)
{
	if ((i & 15) < 8)
	{
		if ((j & 15) < 8)
		{
			return BK_COLOR1;
		}
		else
		{
			return BK_COLOR2;
		}
	}
	else
	{
		if ((j & 15) < 8)
		{
			return BK_COLOR2;
		}
		else
		{
			return BK_COLOR1;
		}
	}
}

void CSubtitlePlayer::FillBackground()
{
	uint32_t *p = (uint32_t *)m_BkBuffer;
	for(int i = 0; i < m_Height; i++)
	{
		for(int j = 0; j < m_Width; j++)
		{
			p[i * m_Width + j] = GetBackColor(i, j);
		}
	}
}
