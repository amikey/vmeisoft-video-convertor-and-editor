#pragma once

#define NUM_MXIN_BUFFERS	10

class CAudioRecorder
{
public:
	struct MediaInfo* m_Info;

public:
	HWAVEIN			m_hWaveIn;
	HANDLE			m_hdFile;
	HANDLE			m_hdThread;
	DWORD			m_dwThreadId;
	WAVEHDR*		m_WaveHeaders[NUM_MXIN_BUFFERS];
	WAVEFORMATEX	m_wfx;
	DWORD			m_nPacketSize;
	DWORD			m_dwTotalBytes;
	volatile LONG	m_fAbort;

public:
	volatile LONG	m_Playing;

public:
	HWND			m_hwndMessage;
	int				m_LastVolume;

public:
	CAudioRecorder(HWND hwndMessage, HRESULT &hr);
	~CAudioRecorder(void);

public:
	DWORD ThreadEntry();

public:
	void Pause();
	void Resume();

public:
	void Stop();
};

