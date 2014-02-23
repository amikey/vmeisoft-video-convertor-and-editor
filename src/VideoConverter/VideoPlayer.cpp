#include "stdafx.h"
#include "VideoConverter.h"
#include "VideoPlayer.h"
#include "VideoFilter.h"
#include "AudioSource.h"
#include "AudioRender.h"
#include "SubtitleFilter.h"

#define PLAYER_TIMER_ID		100

#define CM_VP_FIRST				(WM_USER + 300)

#define CM_VP_PLAY				(CM_VP_FIRST + 1)
#define CM_VP_PAUSE				(CM_VP_FIRST + 2)
#define CM_VP_SEEK_TO			(CM_VP_FIRST + 3)
#define CM_VP_NEXT_FRAME 		(CM_VP_FIRST + 4)
#define CM_VP_PRE_TIME 			(CM_VP_FIRST + 5)
#define CM_VP_NEXT_TIME			(CM_VP_FIRST + 6)
#define CM_VP_CENTER			(CM_VP_FIRST + 7)
#define CM_VP_REDRAW			(CM_VP_FIRST + 8)
#define CM_VP_GRAB				(CM_VP_FIRST + 9)
#define CM_VP_SET_FILTER		(CM_VP_FIRST + 10)
#define CM_VP_GET_TIME			(CM_VP_FIRST + 11)
#define CM_VP_SET_CROP			(CM_VP_FIRST + 12)
#define CM_VP_REMOVE_CROP		(CM_VP_FIRST + 13)
#define CM_VP_SET_SUBTITLE		(CM_VP_FIRST + 14)
#define CM_VP_SET_MUSIC			(CM_VP_FIRST + 15)
#define CM_VP_SET_VOLUME		(CM_VP_FIRST + 16)
#define CM_VP_SET_START_STOP	(CM_VP_FIRST + 17)

#define CM_VP_LAST				(CM_VP_FIRST + 18)

static wchar_t WINDOW_VP_NAME[] = L"Video Player Class";

static LRESULT CALLBACK VidePlayerWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if ((message > CM_VP_FIRST) && (message < CM_VP_LAST))
	{
		return ((CVideoPlayer *)GetWindowLongPtr(hWnd, 0))->OnMessage(message, wParam, lParam);
	}
	else
	{
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}

void InitVideoPlayer()
{
	WNDCLASSEX wcex;

	ZeroMemory(&wcex, sizeof(wcex));

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style			= 0;
	wcex.lpfnWndProc	= VidePlayerWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= sizeof(CVideoRender *);
	wcex.hInstance		= g_hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= NULL;
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= WINDOW_VP_NAME;
	wcex.hIconSm		= NULL;
	RegisterClassEx(&wcex);
}

static DWORD WINAPI VideoPlayerThread(void *lpThreadParameter)
{
	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	((CVideoPlayer *)lpThreadParameter)->Start();
	((CVideoPlayer *)lpThreadParameter)->ReleaseAll();
	CoUninitialize();
	return 0;
}

inline void CVideoPlayer::ShowFrame(int64_t Pos)
{
	while ((m_VideoTime < Pos) && (m_VideoEndOfStream == 0))
	{
		m_Decoder->video_next();
	}

	if (m_VideoEndOfStream)
	{
		m_VideoTime = m_MediaInfo->m_Duration;
	}
}

CVideoPlayer::CVideoPlayer(ClipInfo *info, VideoPlayerMode mode, HWND hwndParent, HWND hwndMessage)
{
	m_Info = info;
	m_MediaInfo = m_Info->m_MediaInfo;
	m_Info->m_VideoPlayer = this;
	m_Mode = mode;
	m_hwndParent = hwndParent;
	m_hwndMessage = hwndMessage;
	
	m_hReady = CreateEvent(NULL, FALSE, FALSE, NULL);
	
	int32_t FrameSize = (m_MediaInfo->m_Width * m_MediaInfo->m_Height * 4 + 63) & (~63);
	m_Frame = (uint8_t*)_aligned_malloc(FrameSize, 64);

	m_srcSliceH = m_MediaInfo->m_CodecHeight;
	m_dstBuffers[0] = m_Frame;
	m_dstStride[0] = m_MediaInfo->m_Width * 4;

	DWORD idThread;
	m_hThread = CreateThread(NULL, 0, VideoPlayerThread, this, 0, &idThread);
	WaitForSingleObject(m_hReady, INFINITE);
	CloseHandle(m_hReady);
}


CVideoPlayer::~CVideoPlayer(void)
{
	m_Info->m_VideoPlayer = NULL;
	InterlockedExchange(&m_abort, 1);
	PostMessage(m_hwnd, WM_NULL, 0, 0);
	WaitForSingleObject(m_hThread, INFINITE);
	CloseHandle(m_hThread);
	_aligned_free(m_Frame);
}

void CVideoPlayer::ReleaseAll()
{
	SAFE_DELETE(m_Decoder);
	SAFE_DELETE(m_pFilterManager);
	SAFE_DELETE(m_AudioPlayer);
	SAFE_DELETE(m_VideoRender);
	if (m_hwnd) DestroyWindow(m_hwnd);
	if (sws_context)
	{
		sws_freeContext(sws_context);
		sws_context = NULL;
	}
}

void CVideoPlayer::OnVideoStreamStart(int64_t pts)
{
}

void CVideoPlayer::OnAudioStreamStart(int64_t pts)
{
	assert(0);
}

void CVideoPlayer::OnVideoStream(int64_t pts, AVFrame *frame)
{
	if (pts < m_VideoTime)
	{
		return;
	}
	if (sws_context)
	{
		sws_scale(sws_context, frame->data, frame->linesize, 0, m_srcSliceH, m_dstBuffers, m_dstStride);
	}
	else
	{
		uint8_t *p1 = m_Frame;
		uint8_t *p2 = frame->data[0];
		int32_t w1 = m_dstStride[0];
		int32_t w2 = frame->linesize[0];
		int32_t w = w1 < w2 ? w1 : w2;
		for(int i = 0; i < m_srcSliceH; i++)
		{
			memcpy(p1, p2, w);
			p1 += w1;
			p2 += w1;
		}
	}
	m_VideoTime = pts;
	m_pFilterManager->DoFilter(m_Frame, pts);
}

void CVideoPlayer::OnAudioStream(AVFrame *frame)
{
	assert(0);
}

int32_t CVideoPlayer::OnVideoPacket()
{
	assert(0);
	return 0;
}

void CVideoPlayer::OnEndOfStream()
{
	m_VideoEndOfStream = TRUE;
}

inline void CVideoPlayer::NotifyTime(int64_t time)
{
	time -= m_OffsetTime;
	PostMessage(m_hwndMessage, VC_VIDEO_RENDER_PROGRESS, (WPARAM)(time % AV_TIME_BASE_LL), (LPARAM)(time / AV_TIME_BASE_LL));
}

inline void CVideoPlayer::OnTimeChanged()
{
	int32_t t = (int32_t)(m_AudioTime / AV_TIME_BASE_LL);
	if (m_LastTime != t)
	{
		m_LastTime = t;
		NotifyTime(m_AudioTime);
	}
}

int32_t CVideoPlayer::Start()
{
	m_hwnd = CreateWindowEx(
		WS_EX_NOPARENTNOTIFY, 
		WINDOW_VP_NAME, NULL, 
		WS_CHILD, 
		0, 0, 0, 0, 
		HWND_MESSAGE, 
		NULL, g_hInstance, NULL);
	SetWindowLongPtr(m_hwnd, 0, (LONG_PTR)this);
	SetEvent(m_hReady);

	m_Decoder = new CDecoder(this, 1);
	if (m_Mode == VC_VIDEO_RENDER_MODE_DEST)
	{
		m_VideoRender = new CVideoRender(m_hwndParent, m_hwndMessage, g_enc_opt.m_VideoWidth, g_enc_opt.m_VideoHeight);
	}
	else
	{
		m_VideoRender = new CVideoRender(m_hwndParent, m_hwndMessage, m_MediaInfo->m_Width, m_MediaInfo->m_Height);
	}
	
	if (m_VideoRender->Init())
	{
		return -1;
	}
	
	if (m_Decoder->ffdec_open(m_MediaInfo->m_szFileName))
	{
		return -1;
	}

	if (m_Mode == VC_VIDEO_RENDER_MODE_DEST)
	{
		m_StartTime = m_Info->m_ClipStart;
		m_OffsetTime = m_StartTime;
		if (m_Info->m_ClipStop)
		{
			m_StopTime = m_Info->m_ClipStop;
		}
		else
		{
			m_StopTime = m_Info->m_MediaInfo->m_Duration;
		}
	}
	else
	{
		m_OffsetTime = 0;
		m_StartTime = 0;
		m_StopTime = m_Info->m_MediaInfo->m_Duration;
	}

	if (m_Mode == VC_VIDEO_RENDER_MODE_DEST)
	{
		m_AudioSource = new CAudioSource(AUDIO_RENDER_RATE, AUDIO_RENDER_CHANNELS, AUDIO_RENDER_BYTES, 0, AUDIO_RENDER_LAYOUT, AUDIO_RENDER_FMT);
		for(int i = 0; i < 3; i++)
		{
			if (m_Info->m_AudioTrack[i].m_Media)
			{
				m_AudioSource->AddReader(i, &m_Info->m_AudioTrack[i]);
			}
		}
		m_AudioSource->SetStop(m_StopTime);
		m_AudioPlayer = new CAudioRender(m_AudioSource);
		m_AudioPlayer->Initialize();
	}
	else
	{
		m_AudioSource = new CAudioSource(AUDIO_RENDER_RATE, AUDIO_RENDER_CHANNELS, AUDIO_RENDER_BYTES, 0, AUDIO_RENDER_LAYOUT, AUDIO_RENDER_FMT);
		if (m_MediaInfo->m_bAudioStream)
		{
			static AudioTrack track;
			track.m_Media = m_MediaInfo;
			track.m_Volume = 100;
			m_AudioSource->AddReader(0, &track);
		}
		m_AudioSource->SetStop(m_StopTime);
		m_AudioPlayer = new CAudioRender(m_AudioSource);
		m_AudioPlayer->Initialize();
	}

	m_pFilterManager = new CVideoFilterManager();
	if (m_Mode == VC_VIDEO_RENDER_MODE_DEST)
	{
		for(int i = 0; i < VIDEO_FILTER_COUNT; i++)
		{
			CVideoFilterBase *base = m_Info->m_Filters[i].CreateInstance();
			m_pFilterManager->AddFiler(base);
		}
		m_pSubtitleFilter = new CSubtitleFilter(m_Info->GetSubtitle());
		m_pFilterManager->AddFiler(m_pSubtitleFilter);
	}
	m_pFilterManager->AddFiler(m_VideoRender);

	if (
		(m_MediaInfo->m_CodecWidth != m_MediaInfo->m_Width) ||
		(m_MediaInfo->m_CodecHeight != m_MediaInfo->m_Height) ||
		(m_MediaInfo->m_PixelFormat != AV_PIX_FMT_BGRA)
		)
	{
		sws_context = sws_getContext(
			m_MediaInfo->m_CodecWidth, m_MediaInfo->m_CodecHeight, (enum AVPixelFormat)m_MediaInfo->m_PixelFormat,
			m_MediaInfo->m_Width, m_MediaInfo->m_Height, AV_PIX_FMT_BGRA,
			SWS_BICUBIC, NULL, NULL, NULL);
	}

	// First Frame
	{
		m_Decoder->ffdec_seek(m_StartTime);
		m_pFilterManager->PutSourceSize(m_MediaInfo->m_Width, m_MediaInfo->m_Height);
		m_pFilterManager->NewSegment(m_StartTime, m_StopTime);
		ShowFrame(m_StartTime);
		m_LastTime = (int32_t)(m_VideoTime / AV_TIME_BASE_LL);
		NotifyTime(m_VideoTime);
		m_AudioPlayer->Seek(m_StartTime);
	}

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (m_abort) break;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		if ((m_Playing) && (msg.message == WM_TIMER))
		{
			PeekMessage(&msg, NULL, WM_TIMER, WM_TIMER, PM_REMOVE);
			OnTimerTick();
		}
	}

	return 0;
}

inline void CVideoPlayer::PausePlay()
{
	if (m_idTimer)
	{
		KillTimer(NULL, m_idTimer);
		m_idTimer = 0;
	}
	if (m_Playing)
	{
		m_Playing = FALSE;
		m_AudioPlayer->Pause();
		NotifyTime(m_AudioTime);
	}
}

void CVideoPlayer::OnTimerTick()
{
	m_AudioTime = m_AudioPlayer->Next();

	if (m_AudioTime > m_StopTime) m_AudioTime = m_StopTime;

	if ((m_VideoTime < m_AudioTime) && (m_VideoEndOfStream == 0))
	{
		ShowFrame(m_AudioTime);
	}

	OnTimeChanged();
	if (m_AudioTime >= m_StopTime)
	{
		m_Playing = FALSE;
		m_EndOfStream = TRUE;
		m_AudioPlayer->Pause();
		PostMessage(m_hwndMessage, VC_VIDEO_RENDER_END, 0, 0);
	}
}

LRESULT CVideoPlayer::OnMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case CM_VP_PLAY:
		if (m_Playing == FALSE)
		{
			if (m_Seeked)
			{
				m_Seeked = FALSE;
				m_EndOfStream = FALSE;
				m_VideoEndOfStream = FALSE;
				m_AudioPlayer->Seek(m_VideoTime);
			}
			else if ((m_EndOfStream) || (m_VideoTime >= m_StopTime))
			{
				m_EndOfStream = FALSE;
				m_VideoEndOfStream = FALSE;
				m_VideoTime = -1;
				m_Decoder->ffdec_seek(m_StartTime);
				m_AudioPlayer->Seek(m_StartTime);
				ShowFrame(m_StartTime);
			}
			
			m_AudioPlayer->Resume();
			m_idTimer = SetTimer(NULL, PLAYER_TIMER_ID, 10, NULL);
			m_Playing = TRUE;
		}
		return 0;

	case CM_VP_PAUSE:
		{
			PausePlay();
		}
		return 0;

	case CM_VP_SEEK_TO:
		{
			PausePlay();
			m_Seeked = TRUE;

			MSG msg;
			while (PeekMessage(&msg, m_hwnd, CM_VP_SEEK_TO, CM_VP_SEEK_TO, PM_REMOVE))
			{
				wParam = msg.wParam;
				lParam = msg.lParam;
			}
			int64_t Pos = (uint64_t)wParam | ((uint64_t)lParam << 32);
			Pos += m_OffsetTime;
			m_Decoder->ffdec_seek(Pos);
			m_VideoEndOfStream = FALSE;
			m_VideoTime = -1;
			ShowFrame(Pos);
			NotifyTime(m_VideoTime);
		}
		return 0;

	case CM_VP_NEXT_FRAME:
		{
			PausePlay();
			if (m_VideoEndOfStream) return -1;
			m_Seeked = TRUE;

			m_VideoTime = -1;
			ShowFrame(0);
			NotifyTime(m_VideoTime);
		}
		return 0;

	case CM_VP_PRE_TIME:
		{
			PausePlay();
			m_Seeked = TRUE;

			m_Decoder->ffdec_seek(m_VideoTime > AV_TIME_BASE_LL ? m_VideoTime - AV_TIME_BASE_LL : 0);
			m_VideoEndOfStream = FALSE;
			m_VideoTime = -1;
			ShowFrame(0);
			NotifyTime(m_VideoTime);
		}
		return 0;

	case CM_VP_NEXT_TIME:
		{
			PausePlay();
			if (m_VideoEndOfStream) return -1;
			m_Seeked = TRUE;

			m_VideoTime = -1;
			ShowFrame(m_VideoTime + AV_TIME_BASE_LL);
			NotifyTime(m_VideoTime);
		}
		return 0;

	case CM_VP_CENTER:
		{
			m_VideoRender->Resize();
		}
		return 0;

	case CM_VP_FIRST:
		{
			Repaint();
		}
		return 0;

	case CM_VP_GRAB:
		return m_VideoRender->SaveToFile((wchar_t *)lParam);

	case CM_VP_SET_FILTER:
		{
			CVideoFilterData *pData = (CVideoFilterData *)wParam;
			void* Param = (CVideoFilterData *)lParam;
			pData->m_Instance->SetParameters(Param, pData->m_Size);
			Repaint();
		}
		return 0;

	case CM_VP_GET_TIME:
		{
			int64_t *time = (int64_t *)wParam;
			*time = m_VideoTime;
		}
		return 0;

	case CM_VP_SET_CROP:
		{
			m_VideoRender->SetSelectionRect((Rect *)lParam);
		}
		return 0;

	case CM_VP_REMOVE_CROP:
		{
			m_VideoRender->RemoveSelectionRect();
		}
		return 0;

	case CM_VP_SET_SUBTITLE:
		{
			PausePlay();
			m_pSubtitleFilter->SetSubtitle((CSubtitle *)lParam);
			Repaint();
		}
		return 0;

	case CM_VP_SET_MUSIC:
		{
			assert(m_Mode == VC_VIDEO_RENDER_MODE_DEST);
			m_Info->m_AudioTrack[wParam].m_Media = (MediaInfo *)lParam;
			if (lParam)
			{
				m_AudioSource->AddReader(wParam, &m_Info->m_AudioTrack[wParam]);
			}
			else
			{
				m_AudioSource->RemoveReader(wParam);
			}
		}
		return 0;

	case CM_VP_SET_VOLUME:
		{
			assert(m_Mode == VC_VIDEO_RENDER_MODE_DEST);
			m_Info->m_AudioTrack[wParam].m_Volume = lParam;
		}
		return 0;

	case CM_VP_SET_START_STOP:
		{
			m_StartTime = *((int64_t *)wParam);
			m_StopTime = *((int64_t *)lParam);
			if (m_StopTime == 0)
			{
				m_StopTime = m_MediaInfo->m_Duration;
			}
		}
		return 0;
	}

	return 0;
}

void CVideoPlayer::Repaint()
{
	m_pFilterManager->PutSourceSize(m_MediaInfo->m_Width, m_MediaInfo->m_Height);
	m_pFilterManager->NewSegment(m_StartTime, m_StopTime);
	m_pFilterManager->DoFilter(m_Frame, m_VideoTime);
}

void CVideoPlayer::Play()
{
	PostMessage(m_hwnd, CM_VP_PLAY, 0, 0);
}

void CVideoPlayer::Pause()
{
	SendMessage(m_hwnd, CM_VP_PAUSE, 0, 0);
}

void CVideoPlayer::SeekTo(uint64_t Pos)
{
	PostMessage(m_hwnd, CM_VP_SEEK_TO, (WPARAM)(Pos & 0xFFFFFFFFll), (LPARAM)((Pos >> 32)& 0xFFFFFFFFll));
}

int64_t CVideoPlayer::GetTime()
{
	int64_t time;
	SendMessage(m_hwnd, CM_VP_GET_TIME, (WPARAM)&time, 0);
	return time;
}

void CVideoPlayer::NextFrame()
{
	PostMessage(m_hwnd, CM_VP_NEXT_FRAME, 0, 0);
}

void CVideoPlayer::PreTime()
{
	PostMessage(m_hwnd, CM_VP_PRE_TIME, 0, 0);
}

void CVideoPlayer::NextTime()
{
	PostMessage(m_hwnd, CM_VP_NEXT_TIME, 0, 0);
}

HRESULT CVideoPlayer::Grab(wchar_t *szFile)
{
	return SendMessage(m_hwnd, CM_VP_GRAB, 0, (LPARAM)szFile);
}

void CVideoPlayer::Resize()
{
	PostMessage(m_hwnd, CM_VP_CENTER, 0, 0);
}

void CVideoPlayer::ForceRedraw()
{
	PostMessage(m_hwnd, CM_VP_REDRAW, 0, 0);
}

HRESULT CVideoPlayer::SetFilterParameter(CVideoFilterData *data, void *Param)
{
	return SendMessage(m_hwnd, CM_VP_SET_FILTER, (WPARAM)data, (LPARAM)Param);
}

void CVideoPlayer::SetCrop(Rect* crop)
{
	SendMessage(m_hwnd, CM_VP_SET_CROP, 0, (LPARAM)crop);
}

void CVideoPlayer::RemoveCrop()
{
	SendMessage(m_hwnd, CM_VP_REMOVE_CROP, 0, 0);
}

void CVideoPlayer::SetSubtitle(CSubtitle *subtitle)
{
	SendMessage(m_hwnd, CM_VP_SET_SUBTITLE, 0, (LPARAM)subtitle);
}

void CVideoPlayer::SetMusic(int32_t index, MediaInfo *info)
{
	SendMessage(m_hwnd, CM_VP_SET_MUSIC, index, (LPARAM)info);
}

void CVideoPlayer::SetVolume(int32_t index, int32_t Volume)
{
	PostMessage(m_hwnd, CM_VP_SET_VOLUME, index, (LPARAM)Volume);
}

void CVideoPlayer::SetStartStop(int64_t start, int64_t stop)
{
	SendMessage(m_hwnd, CM_VP_SET_START_STOP, (WPARAM)&start, (LPARAM)&stop);
}