#pragma once

#define AUDIO_RENDER_RATE			44100
#define AUDIO_RENDER_CHANNELS		2
#define AUDIO_RENDER_BITS			16
#define AUDIO_RENDER_BYTES			(AUDIO_RENDER_CHANNELS * AUDIO_RENDER_BITS / 8)
#define AUDIO_RENDER_FMT			AV_SAMPLE_FMT_S16
#define AUDIO_RENDER_LAYOUT			AV_CH_LAYOUT_STEREO
static const int64_t AUDIO_RENDER_BYTES_PER_SEC = AUDIO_RENDER_RATE * AUDIO_RENDER_BYTES;

#define NOTIFY_COUNT			(2)
#define NOTIFY_SIZE				(AUDIO_RENDER_RATE * AUDIO_RENDER_BYTES)
#define SOUND_BUFFER_SIZE		(NOTIFY_COUNT * NOTIFY_SIZE)

class CAudioSource;
class CAudioReader
{
public:
	AudioTrack*				m_Info;
	MediaInfo*				m_MediaInfo;
	CAudioSource*			m_Source;

private:
	struct SwrContext*		swr_context;
	HANDLE					m_hFiles[8];
	uint8_t*				m_ReadBuffer[8];
	uint8_t*				m_ResampleBuffer[8];
	int32_t					m_Bufferd;
	int32_t					m_ReadPoint;

	int						m_SourceSampleRate;
	int						m_SourceChannels;
	int						m_SourcePacketBytes;

	int						m_DestSampleRate;
	int						m_DestChannels;
	int						m_DestPacketBytes;

public:
	int64_t					m_StartPos;
	int64_t					m_StopPos;


public:
	CAudioReader(CAudioSource *source, AudioTrack *info);
	~CAudioReader();

public:
	void Seek(int64_t time);
	void ResetStartStop();
	void ReadSamples(uint8_t* buffer[8], int64_t CurrentPos, int32_t samples);
};

class CAudioSource
{
	friend class CAudioReader;

public:
	uint8_t*			m_Data[3][8];
	CAudioReader*		m_Reader[3];

	int					m_SampleRate;
	int					m_PacketBytes;
	int64_t				m_Layout;
	int					m_SampleFormat;
	int					m_Channels;
	int					m_Samples;
		
public:
	int					m_MuteValue;

public:
	int64_t				m_CurrentPos;
	int64_t				m_StopPos;

public:
	BOOL				m_EndOfStream;

public:
	CAudioSource(int32_t SampleRate, int32_t Channels, int32_t PacketBytes, int IsIsPlanar, int64_t Layout, int32_t SampleFormat);
	~CAudioSource();

public:
	void AddReader(int index, AudioTrack *info);
	void RemoveReader(int index);
	void SetStop(int64_t stop);
	void Seek(int64_t time);
	int ReadSamples(uint8_t* buffer[8]);
};

class CAudioStream:
	public AudioStream
{
public:
	CAudioSource*		m_Source;
	int					m_Channels;
	int					m_PacketBytes;
	int					m_WritePoint;
	int					m_MinSamples;
	int					m_MaxBytes;
	int					m_EndOfStream;
			
public:
	void Initialize();
	~CAudioStream();

public:
	HRESULT Next();
};
