#pragma once

class CAudioSource;

class CAudioRender
{
private:
	IDirectSound8*			m_pDirectSound8;
	IDirectSoundBuffer*		m_pDirectSoundBufferPrimary;
	IDirectSoundBuffer*		m_pDirectSoundBuffer;

private:
	CAudioSource*			m_Source;
	int64_t					m_llStartBytes;

private:
	DWORD					m_dwOffset;
	int64_t					m_llPosStart;
	int64_t					m_llPosWrite;
	DWORD					m_dwCurrentPlayCursorLast;

public:
	CAudioRender(CAudioSource *source);
	~CAudioRender(void);
	HRESULT Initialize();

public:
	void Pause();
	void Resume();
	void Seek(int64_t Position);
	void SetSpeed(int32_t speed);

public:
	int64_t Next();
};

