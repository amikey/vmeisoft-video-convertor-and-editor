#pragma once
#include ".\ffmpeg\Decoder\dec.h"
#include "VideoRender.h"

struct ClipInfo;
class CAudioSource;
class CAudioRender;
class CDecoder;
class CSubtitleFilter;
class CSubtitle;

class CVideoPlayer:
	public CBaseDecoder
{
private:
	ClipInfo*			m_Info;
	struct MediaInfo*	m_MediaInfo;
	VideoPlayerMode		m_Mode;
	CDecoder*			m_Decoder;
	CVideoRender*		m_VideoRender;
	CAudioSource*		m_AudioSource;
	CAudioRender*		m_AudioPlayer;
	CVideoFilterManager* m_pFilterManager;
	HWND				m_hwnd;
	HWND				m_hwndParent;
	HWND				m_hwndMessage;

private:
	int64_t				m_OffsetTime;
	int64_t				m_StartTime;
	int64_t				m_StopTime;

private:
	int64_t				m_TimeNum;
	int64_t				m_TimeDen;

private:
	HANDLE				m_hThread;
	HANDLE				m_hReady;

private:
	uint8_t*			m_Frame;
	int64_t				m_LastTime;
	int32_t				m_srcSliceH;
	uint8_t*			m_dstBuffers[4];
	int32_t				m_dstStride[4];

private:
	volatile long		m_abort;

private:
	BOOL				m_Playing;
	BOOL				m_Seeked;
	inline void PausePlay();

private:
	int64_t				m_VideoTime;
	int64_t				m_AudioTime;
	BOOL				m_VideoEndOfStream;
	BOOL				m_EndOfStream;

private:
	struct SwsContext*	sws_context;

public:
	virtual void OnVideoStreamStart(int64_t pts);
	virtual void OnAudioStreamStart(int64_t pts);
	virtual void OnVideoStream(int64_t pts, AVFrame *frame);
	virtual void OnAudioStream(AVFrame *frame);
	virtual int32_t OnVideoPacket();
	virtual void OnEndOfStream();

public:
	UINT_PTR m_idTimer;
	int32_t Start();
	void ReleaseAll();
	LRESULT OnMessage(UINT message, WPARAM wParam, LPARAM lParam);
	void OnTimerTick();

public:
	CVideoPlayer(ClipInfo *info, VideoPlayerMode mode, HWND hwndParent, HWND hwndMessage);
	~CVideoPlayer(void);
	HRESULT Initialize();

private:
	inline void NotifyTime(int64_t time);
	inline void OnTimeChanged();
	inline void ShowFrame(int64_t Pos);

public:
	void Play();
	void Pause();
	void SeekTo(uint64_t Pos);
	int64_t GetTime();
	void Repaint();

public:
	void NextFrame();
	void PreTime();
	void NextTime();
	
public:
	HRESULT Grab(wchar_t *szFile);
	void Resize();
	void ForceRedraw();

public:
	HRESULT SetFilterParameter(CVideoFilterData *data, void *Param);

public:
	// Clip
	void SetStartStop(int64_t start, int64_t stop);

public:
	//Crop÷ß≥÷
	void SetCrop(Rect* crop);
	void RemoveCrop();

public:
	// Subtitle
	CSubtitleFilter* m_pSubtitleFilter;
	void SetSubtitle(CSubtitle *subtitle);

public:
	// Music
	void SetMusic(int32_t index, MediaInfo *info);
	void SetVolume(int32_t index, int32_t Volume);
};

extern void InitVideoPlayer();

