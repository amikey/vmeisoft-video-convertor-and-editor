#include "stdafx.h"
#include "VideoConverter.h"
#include "AudioSource.h"
#include "AudioRender.h"

CAudioRender::CAudioRender(CAudioSource *source)
{
	m_Source = source;
}

CAudioRender::~CAudioRender(void)
{
	if (m_pDirectSoundBuffer) m_pDirectSoundBuffer->Stop();
	SAFE_RELEASE(m_pDirectSoundBuffer);
	SAFE_RELEASE(m_pDirectSoundBufferPrimary);
	SAFE_RELEASE(m_pDirectSound8);
}

HRESULT CAudioRender::Initialize()
{
	HRESULT hr;
	DWORD				NumberOfBytesRead = 0;

	// 初始化部分
	WAVEFORMATEX wfx;
	ZeroMemory(&wfx, sizeof(wfx));
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = AUDIO_RENDER_CHANNELS;
	wfx.nSamplesPerSec = AUDIO_RENDER_RATE;
	wfx.wBitsPerSample = AUDIO_RENDER_BITS;
	wfx.nBlockAlign = AUDIO_RENDER_BYTES;
	wfx.nAvgBytesPerSec = AUDIO_RENDER_BYTES_PER_SEC;
	wfx.cbSize = 0;

	hr = DirectSoundCreate8(&DSDEVID_DefaultPlayback, &m_pDirectSound8, NULL);
	if (FAILED(hr))
	{
		ErrorLog("Faild(0x%x) to DirectSoundCreate8\n", hr);
		return hr;
	}

	DSCAPS DSCaps;
	ZeroMemory(&DSCaps, sizeof(DSCaps));
	DSCaps.dwSize = sizeof(DSCaps);
	hr = m_pDirectSound8->GetCaps(&DSCaps);
	if (DSCaps.dwFlags & DSCAPS_EMULDRIVER)
	{
		ErrorLog("DirectSound is emulated, waveOut may give better performance\n");
	}

	// emulated
	hr = m_pDirectSound8->SetCooperativeLevel(GetDesktopWindow(), DSSCL_PRIORITY);

	DSBUFFERDESC DescPrimary;
	ZeroMemory(&DescPrimary, sizeof(DescPrimary));
	DescPrimary.dwSize			= sizeof(DescPrimary);
	DescPrimary.dwFlags			= DSBCAPS_PRIMARYBUFFER;
	DescPrimary.dwBufferBytes   = 0;

	hr = m_pDirectSound8->CreateSoundBuffer(&DescPrimary, &m_pDirectSoundBufferPrimary, NULL);
	hr = m_pDirectSoundBufferPrimary->SetFormat(&wfx);

	DSBUFFERDESC dsbdesc;
	ZeroMemory(&dsbdesc, sizeof(dsbdesc));
	dsbdesc.dwSize = sizeof(DSBUFFERDESC);
	dsbdesc.dwFlags = DSBCAPS_GLOBALFOCUS  | DSBCAPS_CTRLFREQUENCY;
	dsbdesc.dwBufferBytes = SOUND_BUFFER_SIZE;
	dsbdesc.lpwfxFormat = &wfx;
	hr = m_pDirectSound8->CreateSoundBuffer(&dsbdesc, &m_pDirectSoundBuffer, NULL);

	DSBCAPS dbsCaps;
	ZeroMemory(&dbsCaps, sizeof(dbsCaps));
	dbsCaps.dwSize = sizeof(dbsCaps);
	hr = m_pDirectSoundBuffer->GetCaps(&dbsCaps);
	assert(dbsCaps.dwBufferBytes == SOUND_BUFFER_SIZE);

	return S_OK;
}

int64_t CAudioRender::Next()
{
	HRESULT hr;

	DWORD dwStatus;
	hr = m_pDirectSoundBuffer->GetStatus(&dwStatus);
	if (dwStatus == DSBSTATUS_BUFFERLOST)
	{
		m_pDirectSoundBuffer->Restore();
		m_pDirectSoundBuffer->GetStatus(&dwStatus);
	}
	if ((dwStatus & DSBSTATUS_PLAYING) != DSBSTATUS_PLAYING)
	{
		return -1;
	}

	DWORD dwCurrentPlayCursor, dwCurrentWriteCursor;
	hr = m_pDirectSoundBuffer->GetCurrentPosition(&dwCurrentPlayCursor, &dwCurrentWriteCursor);
	if (FAILED(hr)) return -1;
	if (dwCurrentPlayCursor < m_dwCurrentPlayCursorLast) m_llPosStart += SOUND_BUFFER_SIZE;
	m_dwCurrentPlayCursorLast = dwCurrentPlayCursor;
	int64_t llPosPlay = m_llPosStart + dwCurrentPlayCursor;

	if (m_llPosWrite + NOTIFY_SIZE < llPosPlay) 
	{
		void*	pvAudioPtr = NULL;
		DWORD	dwAudioBytes = 0;
		hr = m_pDirectSoundBuffer->Lock(m_dwOffset * NOTIFY_SIZE,  NOTIFY_SIZE, &pvAudioPtr, &dwAudioBytes, NULL, NULL, 0); assert(hr == S_OK);
		uint8_t *buffer = (uint8_t *)pvAudioPtr;
		m_Source->ReadSamples(&buffer);
		hr = m_pDirectSoundBuffer->Unlock(pvAudioPtr, dwAudioBytes, NULL, NULL); assert(hr == S_OK);

		m_dwOffset = (m_dwOffset + 1) % (NOTIFY_COUNT);
		m_llPosWrite += NOTIFY_SIZE;
	}

	return (m_llStartBytes + m_llPosStart + llPosPlay) * AV_TIME_BASE_LL / AUDIO_RENDER_BYTES_PER_SEC;
}

void CAudioRender::Pause()
{
	m_pDirectSoundBuffer->Stop();
}

void CAudioRender::Resume()
{
	m_pDirectSoundBuffer->Play(0, 0, DSBPLAY_LOOPING);
}

void CAudioRender::Seek(int64_t Position)
{
	HRESULT hr;
	m_Source->Seek(Position);

	void*	pvAudioPtr = NULL;
	DWORD	dwAudioBytes = 0;
	hr = m_pDirectSoundBuffer->Stop();
	hr = m_pDirectSoundBuffer->SetCurrentPosition(0);
	hr = m_pDirectSoundBuffer->Lock(0, SOUND_BUFFER_SIZE, &pvAudioPtr, &dwAudioBytes, NULL, NULL, 0); assert(hr == S_OK);
	uint8_t *buffer = (uint8_t *)pvAudioPtr;
	for(int i = 0; i < NOTIFY_COUNT; i++)
	{
		m_Source->ReadSamples(&buffer);
		buffer += NOTIFY_SIZE;
	}
	m_pDirectSoundBuffer->Unlock(pvAudioPtr, dwAudioBytes, NULL, NULL); assert(hr == S_OK);

	m_llStartBytes = Position * AUDIO_RENDER_BYTES_PER_SEC / AV_TIME_BASE_LL;
	m_dwOffset = 0;
	m_llPosStart = 0;
	m_llPosWrite = 0;
	m_dwCurrentPlayCursorLast = 0;
}

void CAudioRender::SetSpeed(int32_t t)
{
	HRESULT hr;
	DWORD dwRate = (t + 100) * AUDIO_RENDER_RATE / 100;
	hr = m_pDirectSoundBuffer->SetFrequency(dwRate);
}

