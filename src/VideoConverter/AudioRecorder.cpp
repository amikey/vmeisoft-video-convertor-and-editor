#include "StdAfx.h"
#include "VideoConverter.h"
#include "AudioRecorder.h"
#include "Initial.h"
#include "AudioSource.h"
#include <Mmdeviceapi.h>
#include <Endpointvolume.h>
#include "./ffmpeg/ffmpeg.inc.h"
#include "MediaMananger.h"

#define RIFF_TAG                    0x46464952
#define WAVE_TAG                    0x45564157
#define FMT__TAG                    0x20746D66
#define DATA_TAG                    0x61746164

// {60311288-45DF-48A7-BE29-7D8E874BA34C}
static const GUID GUID_TEMP_RECORD = 
{ 0x60311288, 0x45df, 0x48a7, { 0xbe, 0x29, 0x7d, 0x8e, 0x87, 0x4b, 0xa3, 0x4c } };

#pragma pack(1)

typedef struct _WAVE_FILE_HEADER
{
	uint32_t dwRiff;			// FIFF 头文件标志
	uint32_t dwFileSize;		// 文件尺寸
	uint32_t dwWave;			// WAVE 文件标志
	uint32_t dwFormat;			// FMT 文件标志
	uint32_t dwFormatLength;	// FMT 的长度
	WAVEFORMATEX Format;		// 音频格式
	uint32_t dwData;			//
	uint32_t dwDataLength;		//

} WAVE_FILE_HEADER;

#pragma pack()

HRESULT __stdcall SpeakerMute(int32_t bMute)
{
	if (g_OsSystemMajorVersion < 6)
	{
		static DWORD dwVolume;
		if (bMute)
		{
			if (dwVolume == 0)
			{
				waveOutGetVolume(0, &dwVolume);
			}
			waveOutSetVolume(NULL, 0);
		}
		else if (dwVolume)
		{
			waveOutSetVolume(0, dwVolume);
		}
	}
	else
	{
		HRESULT hr;
		IMMDeviceEnumerator*	pEnumerator = NULL;
		IMMDevice*				pDevice = NULL;
		IAudioEndpointVolume*	pAudioEndpointVolume = NULL;
		JIF(CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (void**)&pEnumerator));
		JIF(pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice));
		JIF(pDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (void**)&pAudioEndpointVolume));
		JIF(pAudioEndpointVolume->SetMute(bMute ? TRUE : FALSE, NULL));

		pAudioEndpointVolume->Release();
		pDevice->Release();
		pEnumerator->Release();
	}

	return S_OK;
}


#define CM_QUIT			(WM_USER + 9)

static DWORD WINAPI ThreadRecorderProc(LPVOID lpParameter)
{
	return ((CAudioRecorder *)lpParameter)->ThreadEntry();
}

CAudioRecorder::CAudioRecorder(HWND hwndMessage, HRESULT &hr)
{
	assert(sizeof(WAVE_FILE_HEADER) == 46);

	MMRESULT mr;
	hr = E_FAIL;

	m_hwndMessage = hwndMessage;
	m_Info = new MediaInfo;
	wchar_t szGuid[128];
	CoCreateGuid(&m_Info->m_Id);
	StringFromGUID2(m_Info->m_Id, szGuid, 128);
	m_Info->m_bAudioStream = 1; // 是否存在音频
	m_Info->m_SampleRate = AUDIO_RENDER_RATE; // 音频采样
	m_Info->m_nChannel = AUDIO_RENDER_CHANNELS; // 音频通道数量
	m_Info->m_channel_layout = AUDIO_RENDER_LAYOUT; // *(内部使用) 音频通道排列方式
	m_Info->m_SampleFormat = AUDIO_RENDER_FMT; // *(内部使用)音频格式
	m_Info->m_AudioBitRate = AUDIO_RENDER_BYTES_PER_SEC * 8; // 音频比特率
	m_Info->m_IsPlanar = 0; // *(内部使用)音频是否为平面模式
	StringCchPrintf(m_Info->m_AudioTmpFile[0], MAX_PATH, AUDIO_DATA_FMT, g_szTempPath, szGuid, 99);

	m_hdFile = CreateFile(m_Info->m_AudioTmpFile[0], 
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
		NULL);
	if (m_hdFile == INVALID_HANDLE_VALUE) 
	{
		m_hdFile = NULL;
		ErrorLog("Faild(%d) to create file:%ls\n", GetLastError(), m_Info->m_AudioTmpFile[0]);
		return ;
	}

	LARGE_INTEGER liDistanceToMove;
	liDistanceToMove.QuadPart = sizeof(WAVE_FILE_HEADER);
	if (SetFilePointerEx(m_hdFile, liDistanceToMove, NULL, FILE_BEGIN) == FALSE)
	{
		ErrorLog("Faild(%d) to seek file\n", GetLastError());
		return ;
	}
	
	// 启动线程
	m_hdThread = CreateThread(NULL, 0, ThreadRecorderProc, this, 0, &m_dwThreadId);
	if (m_hdThread == NULL)
	{
		ErrorLog("FAILED(%d) to CreateThread.",  GetLastError());
		return ;
	}

	m_wfx.wFormatTag = WAVE_FORMAT_PCM;
    m_wfx.nChannels = AUDIO_RENDER_CHANNELS;
    m_wfx.wBitsPerSample = AUDIO_RENDER_BITS;
    m_wfx.nSamplesPerSec = AUDIO_RENDER_RATE;
    m_wfx.nBlockAlign = AUDIO_RENDER_BYTES;
    m_wfx.nAvgBytesPerSec = AUDIO_RENDER_BYTES_PER_SEC;
    m_wfx.cbSize = 0;
	m_nPacketSize = m_wfx.nAvgBytesPerSec / 5;

    // 打开
	mr = waveInOpen(&m_hWaveIn, WAVE_MAPPER, &m_wfx, m_dwThreadId, 0, CALLBACK_THREAD);
	if (mr)
	{
		ErrorLog("FAILED(%d) to waveInOpen.",  mr);
		return ;
	}

    for(int i = 0; i < NUM_MXIN_BUFFERS; i++)
	{
		m_WaveHeaders[i] = (WAVEHDR *)MemoryAlloc(sizeof(WAVEHDR) + m_nPacketSize);
		ZeroMemory(m_WaveHeaders[i], sizeof(WAVEHDR));
        m_WaveHeaders[i]->lpData = (LPSTR)&(m_WaveHeaders[i][1]);
        m_WaveHeaders[i]->dwBufferLength = m_nPacketSize;
		mr = waveInPrepareHeader(m_hWaveIn, m_WaveHeaders[i], sizeof(WAVEHDR)); 
		assert(mr == 0);
		mr = waveInAddBuffer(m_hWaveIn, m_WaveHeaders[i], sizeof(WAVEHDR)); 
		assert(mr == 0);
	}

	hr = S_OK;
}

CAudioRecorder::~CAudioRecorder(void)
{
	Stop();
}

void CAudioRecorder::Stop()
{
	InterlockedExchange(&m_fAbort, 1);

	if (m_hdThread)
	{
		PostThreadMessage(m_dwThreadId, CM_QUIT, 0, 0);
		WaitForSingleObject(m_hdThread, INFINITE);
		CloseHandle(m_hdThread);
	}

    if (m_hWaveIn)
	{	
		waveInReset(m_hWaveIn);
		waveInStop(m_hWaveIn);

		for(int i = 0; i < NUM_MXIN_BUFFERS; i++)
		{
			if (m_WaveHeaders[i]) 
			{
				waveInUnprepareHeader(m_hWaveIn, m_WaveHeaders[i], sizeof(WAVEHDR));
			}
		}
		waveInClose(m_hWaveIn);
	}

	for(int i = 0; i < NUM_MXIN_BUFFERS; i++)
	{
		SAFE_FREE(m_WaveHeaders[i]) 
	}

	if (m_hdFile)
	{
		WAVE_FILE_HEADER Header;
		Header.dwRiff = RIFF_TAG;
		Header.dwFileSize = m_dwTotalBytes + sizeof(Header);
		Header.dwWave = WAVE_TAG;
		Header.dwFormat = FMT__TAG;
		Header.dwFormatLength = sizeof(WAVEFORMATEX);
		Header.Format = m_wfx;
		Header.dwData = DATA_TAG;
		Header.dwDataLength = m_dwTotalBytes;

		SetFilePointer(m_hdFile, 0, NULL,FILE_BEGIN);
		DWORD nBytesWrite;
		WriteFile(m_hdFile, &Header, sizeof(WAVE_FILE_HEADER), &nBytesWrite,NULL);
		CloseHandle(m_hdFile);
	}

	m_Info->m_AudioDuration = m_dwTotalBytes * AV_TIME_BASE_LL / AUDIO_RENDER_BYTES_PER_SEC; // 音频长度
	m_Info->m_AudioLength = m_dwTotalBytes / AUDIO_RENDER_BYTES; // Samples
	m_Info->m_Duration = m_Info->m_AudioLength;
}

void CAudioRecorder::Pause()
{
	waveInReset(m_hWaveIn);
}

void CAudioRecorder::Resume()
{
	waveInStart(m_hWaveIn);
}

DWORD CAudioRecorder::ThreadEntry()
{
	MSG msg;
	msg.message = 0;
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		switch(msg.message)
		{
		case MM_WIM_OPEN:
			DebugLog("New Recorder Handle:%d\n", msg.wParam);
			break;

		case MM_WIM_DATA:
			{
				LPWAVEHDR pHdr = (LPWAVEHDR)msg.lParam; 
				if (pHdr->dwBytesRecorded == 0) break;

				if (m_Playing)
				{
					DWORD nNumberOfBytesWritten;
					BOOL b = WriteFile(m_hdFile, pHdr->lpData, pHdr->dwBytesRecorded, &nNumberOfBytesWritten, NULL);
					if ((b == FALSE) || (nNumberOfBytesWritten != pHdr->dwBytesRecorded))
					{
						ErrorLog("FAILD(%d) to WriteFile (%d, %d) !!!\n", GetLastError(), nNumberOfBytesWritten, pHdr->dwBytesRecorded);
						break;
					}
					m_dwTotalBytes += pHdr->dwBytesRecorded;
					int time = m_dwTotalBytes * AV_TIME_BASE_LL / AUDIO_RENDER_BYTES_PER_SEC;
					PostMessage(m_hwndMessage, VC_RECORDING_SAMPLE, time % AV_TIME_BASE_LL, time / AV_TIME_BASE_LL);
				}

				if (m_hwndMessage)
				{
					short *p = (short *)pHdr->lpData;
					int n = m_nPacketSize >> 1;
					int l = 0; 
					for(int i = 0; i < n; i++)
					{
						int v = *p;
						if (v < 0) v = -v;
						if (l < v) l = v;
					}
					m_LastVolume = (m_LastVolume + l) >> 1; 
					int Vol = m_LastVolume * 200 / 32768;
					//int Vol = l * 200 / 32768;
					if (Vol > 100) Vol = 100;
					PostMessage(m_hwndMessage, VC_RECORDING_SAMPLE, 0, Vol);
				}

				if (m_fAbort == 0)
				{
					waveInAddBuffer(m_hWaveIn, pHdr, sizeof(WAVEHDR));
				}
			}
			break;

		case MM_WIM_CLOSE:
			DebugLog("Close Recorder Handle:%d\n", msg.wParam);
			return 0;

		case CM_QUIT:
			return 0;
		}
		msg.message = 0;
	}
	return 0;
}

CAudioRecorder* __stdcall AudioRecorderCreate(HWND hwndMessage)
{
	HRESULT hr;
	CAudioRecorder *recorder = new CAudioRecorder(hwndMessage, hr);
	if (hr != S_OK) 
	{
		delete recorder;
		return NULL;
	}
	return recorder;
}

void __stdcall AudioRecorderPause(CAudioRecorder *recorder)
{
	InterlockedExchange(&recorder->m_Playing, 0);
}

void __stdcall AudioRecorderResume(CAudioRecorder *recorder)
{
	InterlockedExchange(&recorder->m_Playing, 1);
}

MediaInfo* __stdcall AudioRecorderGetInfo(CAudioRecorder *recorder)
{
	return recorder->m_Info;
}

void __stdcall AudioRecorderDestroy(CAudioRecorder *recorder)
{
	delete recorder;
}
