#include "stdafx.h"
#include "VideoConverter.h"
#include "./ffmpeg/ffmpeg.inc.h"
#include "AudioSource.h"
#include "AudioUtils.h"

/**********************************************************
CAudioReader
**********************************************************/

CAudioReader::CAudioReader(CAudioSource *source, AudioTrack *info)
{
	m_Source = source;
	m_Info = info;
	m_MediaInfo = info->m_Media;
	m_DestSampleRate = source->m_SampleRate;
	m_DestChannels = source->m_Channels;
	m_DestPacketBytes = source->m_PacketBytes;
	
	if (m_MediaInfo->m_IsPlanar)
	{
		m_SourceChannels = m_MediaInfo->m_nChannel;
		m_SourcePacketBytes = av_get_bytes_per_sample((enum AVSampleFormat)m_MediaInfo->m_SampleFormat);
	}
	else
	{
		m_SourceChannels = 1;
		m_SourcePacketBytes = m_MediaInfo->m_nChannel * av_get_bytes_per_sample((enum AVSampleFormat)m_MediaInfo->m_SampleFormat);
	}
	m_SourceSampleRate = m_MediaInfo->m_SampleRate;
	
	for(int i = 0; i < m_SourceChannels; i++)
	{
		m_hFiles[i] = CreateFile(m_MediaInfo->m_AudioTmpFile[i],
			GENERIC_READ,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,                    
			NULL,                 
			OPEN_EXISTING,        
			FILE_ATTRIBUTE_NORMAL,
			NULL);
	}

	if (
		(m_MediaInfo->m_SampleRate != m_Source->m_SampleRate) ||
		(m_MediaInfo->m_nChannel != m_Source->m_Channels) ||
		(m_MediaInfo->m_SampleFormat != m_Source->m_SampleFormat) ||
		(m_MediaInfo->m_channel_layout != m_Source->m_Layout)
		)
	{
		swr_context =
		swr_alloc_set_opts(NULL,
						   m_Source->m_Layout, (AVSampleFormat)source->m_SampleFormat, m_Source->m_SampleRate,
						   m_MediaInfo->m_channel_layout, (AVSampleFormat)m_MediaInfo->m_SampleFormat, m_MediaInfo->m_SampleRate,
						   0, NULL);
		swr_init(swr_context);

		for(int i = 0; i < m_SourceChannels; i++)
		{
			m_ReadBuffer[i] = (uint8_t *)MemoryAlloc(m_SourceSampleRate * m_SourcePacketBytes);
		}

		for(int i = 0; i < m_DestChannels; i++)
		{
			m_ResampleBuffer[i] = (uint8_t *)MemoryAlloc(m_DestSampleRate * m_DestPacketBytes * 2);
		}
	}

	ResetStartStop();
}

CAudioReader::~CAudioReader()
{
	if (swr_context)
	{
		swr_free(&swr_context);
		swr_context = NULL;
	}
	for(int i = 0; i < m_SourceChannels; i++)
	{
		SAFE_FREE(m_ReadBuffer[i]);
	}
	for(int i = 0; i < m_SourceChannels; i++)
	{
		SAFE_FREE(m_ResampleBuffer[i]);
	}
	for(int i = 0; i < m_SourceChannels; i++)
	{
		SAFE_CLOSE(m_hFiles[i]);
	}
}

void CAudioReader::ResetStartStop()
{
	m_StartPos = m_Info->m_Start * m_DestSampleRate / AV_TIME_BASE_LL;
	if (m_Info->m_Stop)
	{
		m_StopPos = m_Info->m_Stop * m_DestSampleRate / AV_TIME_BASE_LL;
	}
	else
	{
		m_StopPos = m_StartPos + m_Info->m_Media->m_AudioLength;
	}
}

void CAudioReader::Seek(int64_t time)
{
	if (time > m_Info->m_Start)
	{
		time = time - m_Info->m_Start;
	}
	else
	{
		time = 0;
	}
	time += m_Info->m_MediaStart;

	LARGE_INTEGER liFilePointer;
	liFilePointer.QuadPart = (time * m_DestSampleRate / AV_TIME_BASE_LL) * m_SourcePacketBytes;
	for(int i = 0; i < m_SourceChannels; i++)
	{
		SetFilePointerEx(m_hFiles[i], liFilePointer, NULL, FILE_BEGIN);
	}
	m_Bufferd = 0;
}

void CAudioReader::ReadSamples(uint8_t* _buffer[8], int64_t CurrentPos, int32_t samples)
{
	uint8_t* buffer[8] = {_buffer[0]};
	for(int i = 1; i < m_SourceChannels; i++)
	{
		buffer[i] = _buffer[i];
	}

	if (CurrentPos < m_StartPos)
	{
		int l = m_StartPos - CurrentPos;
		int bytes = l * m_DestPacketBytes;
		for(int i = 0; i < m_DestChannels; i++)
		{
			FillMemory(buffer[i], bytes, m_Source->m_MuteValue);
			buffer[i] += bytes;
		}
		samples -= l;
	}
	else if (CurrentPos + samples > m_StopPos)
	{
		int len = m_StopPos - CurrentPos;
		int offset = len * m_DestPacketBytes;
		int bytes = (samples - len) * m_DestPacketBytes;
		for(int i = 0; i < m_DestChannels; i++)
		{
			FillMemory(buffer[offset], bytes, m_Source->m_MuteValue);
		}
		samples = len;
	}
	if (swr_context)
	{
		if (m_Bufferd)
		{
			int bytes;
			int offset = m_ReadPoint;
			if (m_Bufferd > samples)
			{
				bytes = samples * m_DestPacketBytes;
				samples = 0;
				m_Bufferd -= samples;
				m_ReadPoint += samples * m_DestPacketBytes;
			}
			else
			{
				bytes = m_Bufferd * m_DestPacketBytes;
				samples -= m_Bufferd;
				m_Bufferd = 0;
				m_ReadPoint = 0;
			}
			for(int i = 0; i < m_DestChannels; i++)
			{
				CopyMemory(buffer[i], m_ResampleBuffer[i] + offset, bytes);
				buffer[i] += bytes;
			}
		}

		while (samples)
		{
			DWORD nNumberOfBytesToRead = m_SourceSampleRate * m_SourcePacketBytes;
			DWORD NumberOfBytesRead;
			for(int i = 0; i < m_SourceChannels; i++)
			{
				ReadFile(m_hFiles[i], m_ReadBuffer[i], nNumberOfBytesToRead, &NumberOfBytesRead, NULL);
			}
			
			int32_t len = swr_convert(swr_context, m_ResampleBuffer, m_DestSampleRate * 2, (const uint8_t **)m_ReadBuffer, m_SourceSampleRate);
			if (len <= samples)
			{
				int32_t bytes = len * m_DestPacketBytes;
				for(int i = 0; i < m_DestChannels; i++)
				{
					CopyMemory(buffer[i], m_ResampleBuffer[i], bytes);
					buffer[i] += bytes;
				}
				samples -= len;
			}
			else
			{
				m_Bufferd = len - samples;
				int32_t bytes = samples * m_DestPacketBytes;
				int32_t leaves = m_Bufferd * m_DestPacketBytes;
				for(int i = 0; i < m_DestChannels; i++)
				{
					CopyMemory(buffer[i], m_ResampleBuffer[i], bytes);
					MoveMemory(m_ResampleBuffer[i], m_ResampleBuffer[i] + bytes, leaves);
				}
				samples = 0;
				m_ReadPoint = 0;
			}
		}
	}
	else
	{
		DWORD nNumberOfBytesToRead = samples * m_DestPacketBytes;
		DWORD NumberOfBytesRead;
		for(int i = 0; i < m_DestChannels; i++)
		{
			ReadFile(m_hFiles[i], &buffer[i], nNumberOfBytesToRead, &NumberOfBytesRead, 0);
		}
	}

}

/**********************************************************
CAudioSource
**********************************************************/
CAudioSource::CAudioSource(int32_t SampleRate, int32_t Channels, int32_t PacketBytes, int IsIsPlanar, int64_t Layout, int32_t SampleFormat)
{
	m_SampleRate = SampleRate;
	m_PacketBytes = PacketBytes;
	m_Layout = Layout;
	m_SampleFormat = SampleFormat;
	if (IsIsPlanar)
	{
		m_Channels = Channels;
		m_Samples = 1;
	}
	else
	{
		m_Channels = 1;
		m_Samples = Channels;
	}

	switch(SampleFormat)
	{
	case AV_SAMPLE_FMT_U8:
	case AV_SAMPLE_FMT_U8P:
		m_MuteValue = 0x80;
		break;
	}
}

CAudioSource::~CAudioSource()
{
	for(int i = 0; i < 3; i++)
	{
		SAFE_DELETE(m_Reader[i]);
		for(int j = 0; j < 8; j++)
		{
			SAFE_FREE(m_Data[i][j]);
		}
	}
}

void CAudioSource::AddReader(int32_t index, AudioTrack *info)
{
	SAFE_DELETE(m_Reader[index]);
	m_Reader[index] = new CAudioReader(this, info);
}

void CAudioSource::RemoveReader(int32_t index)
{
	SAFE_DELETE(m_Reader[index]);
}

void CAudioSource::SetStop(int64_t stop)
{
	m_StopPos = stop * m_SampleRate / AV_TIME_BASE_LL;
}

void CAudioSource::Seek(int64_t time)
{
	for(int i = 0; i < 3; i++)
	{
		if (m_Reader[i])
		{
			m_Reader[i]->Seek(time);
		}
	}
	m_CurrentPos = time * m_SampleRate / AV_TIME_BASE_LL;
	m_EndOfStream = m_CurrentPos >= m_StopPos;
}

int CAudioSource::ReadSamples(uint8_t* _buffer[8])
{
	int samples = m_SampleRate;
	uint8_t* buffer[8] = {_buffer[0]};
	for(int i = 1; i < m_Channels; i++)
	{
		buffer[i] = _buffer[i];
	}

	if (m_EndOfStream)
	{
		int32_t bytes = samples * m_PacketBytes;
		for(int i = 0; i < m_Channels; i++)
		{
			FillMemory(buffer[i], bytes, m_MuteValue);
		}
		return 0;
	}

	struct
	{
		int index;
		int volume;
	}
	items[3];

	int count = 0;
	int bVolume = 0;
	for(int i = 0; i < 3; i++)
	{
		if (
			(m_Reader[i]) && 
			(m_Reader[i]->m_Info->m_Volume) && 
			(m_Reader[i]->m_StartPos <= m_CurrentPos) &&
			(m_Reader[i]->m_StopPos > m_CurrentPos) 
			)
		{
			if (m_Reader[i]->m_Info->m_Volume != 100)
			{
				bVolume = TRUE;
			}
			items[count].index = i;
			items[count].volume = m_Reader[i]->m_Info->m_Volume;
			count ++;
		}
	}
	
	if (m_CurrentPos + samples >= m_StopPos)
	{
		int len = m_StopPos - m_CurrentPos;
		int offset = len * m_PacketBytes;
		int bytes = (samples - len) * m_PacketBytes;
		for(int i = 0; i < m_Channels; i++)
		{
			FillMemory(buffer[i] + offset, bytes, m_MuteValue);
		}
		m_EndOfStream = TRUE;
		samples = len;
	}

	if (count == 0)
	{
		int32_t bytes = samples * m_PacketBytes;
		for(int i = 0; i < m_Channels; i++)
		{
			FillMemory(buffer[i], bytes, m_MuteValue);
		}
	}
	else if ((count == 1) && (bVolume == 0))
	{
		m_Reader[items[0].index]->ReadSamples(buffer, m_CurrentPos, samples);
	}
	else
	{
		for(int i = 0; i < count; i++)
		{
			for(int j = 0; j < m_Channels; j++)
			{
				if (m_Data[i][j] == NULL)
				{
					m_Data[i][j] = (uint8_t *)MemoryAlloc(m_SampleRate * m_PacketBytes);
				}
			}
		}

		for(int i = 0; i < count; i++)
		{
			m_Reader[items[i].index]->ReadSamples(m_Data[i], m_CurrentPos, samples);
		}

		int total =  samples * m_Samples;
		for(int i = 0; i < m_Channels; i++)
		{
			switch(m_SampleFormat)
			{
			case AV_SAMPLE_FMT_U8:
			case AV_SAMPLE_FMT_U8P:
				switch(count)
				{
				case 1:
					audio_procedure_volume1_8_v(buffer[i], m_Data[0][i], items[0].volume, total);
					break;

				case 2:
					if (bVolume)
					{
						audio_procedure_volume2_8_v(buffer[i], m_Data[0][i], items[0].volume, m_Data[1][i], items[1].volume, total);
					}
					else
					{
						audio_procedure_volume2_8(buffer[i], m_Data[0][i], m_Data[1][i], total);
					}
					break;

				case 3:
					if (bVolume)
					{
						audio_procedure_volume3_8_v(buffer[i], m_Data[0][i], items[0].volume, m_Data[1][i], items[1].volume, m_Data[2][i], items[2].volume, total);
					}
					else
					{
						audio_procedure_volume3_8(buffer[i], m_Data[0][i], m_Data[1][i], m_Data[2][i], total);
					}
					break;
				}
				break;

			case AV_SAMPLE_FMT_S16:
			case AV_SAMPLE_FMT_S16P:
				switch(count)
				{
				case 1:
					audio_procedure_volume1_16_v((int16_t *)buffer[i], (int16_t *)m_Data[0][i], items[0].volume, total);
					break;

				case 2:
					if (bVolume)
					{
						audio_procedure_volume2_16_v((int16_t *)buffer[i], (int16_t *)m_Data[0][i], items[0].volume, (int16_t *)m_Data[1][i], items[1].volume, total);
					}
					else
					{
						audio_procedure_volume2_16((int16_t *)buffer[i], (int16_t *)m_Data[0][i], (int16_t *)m_Data[1][i], total);
					}
					break;

				case 3:
					if (bVolume)
					{
						audio_procedure_volume3_16_v((int16_t *)buffer[i], (int16_t *)m_Data[0][i], items[0].volume, (int16_t *)m_Data[1][i], items[1].volume, (int16_t *)m_Data[2][i], items[2].volume, total);
					}
					else
					{
						audio_procedure_volume3_16((int16_t *)buffer[i], (int16_t *)m_Data[0][i], (int16_t *)m_Data[1][i], (int16_t *)m_Data[2][i], total);
					}
					break;
				}
				break;

			case AV_SAMPLE_FMT_FLT:
			case AV_SAMPLE_FMT_FLTP:
				switch(count)
				{
				case 1:
					audio_procedure_volume1_flt_v((float *)buffer[i], (float *)m_Data[0][i], items[0].volume, total);
					break;

				case 2:
					if (bVolume)
					{
						audio_procedure_volume2_flt_v((float *)buffer[i], (float *)m_Data[0][i], items[0].volume, (float *)m_Data[1][i], items[1].volume, total);
					}
					else
					{
						audio_procedure_volume2_flt((float *)buffer[i], (float *)m_Data[0][i], (float *)m_Data[1][i], total);
					}
					break;

				case 3:
					if (bVolume)
					{
						audio_procedure_volume3_flt_v((float *)buffer[i], (float *)m_Data[0][i], items[0].volume, (float *)m_Data[1][i], items[1].volume, (float *)m_Data[2][i], items[2].volume, total);
					}
					else
					{
						audio_procedure_volume3_flt((float *)buffer[i], (float *)m_Data[0][i], (float *)m_Data[1][i], (float *)m_Data[2][i], total);
					}
					break;
				}
				break;

			case AV_SAMPLE_FMT_S32:
			case AV_SAMPLE_FMT_S32P:
				switch(count)
				{
				case 1:
					audio_procedure_volume1_32_v((int32_t *)buffer[i], (int32_t *)m_Data[0][i], items[0].volume, total);
					break;

				case 2:
					if (bVolume)
					{
						audio_procedure_volume2_32_v((int32_t *)buffer[i], (int32_t *)m_Data[0][i], items[0].volume, (int32_t *)m_Data[1][i], items[1].volume, total);
					}
					else
					{
						audio_procedure_volume2_32((int32_t *)buffer[i], (int32_t *)m_Data[0][i], (int32_t *)m_Data[1][i], total);
					}
					break;

				case 3:
					if (bVolume)
					{
						audio_procedure_volume3_32_v((int32_t *)buffer[i], (int32_t *)m_Data[0][i], items[0].volume, (int32_t *)m_Data[1][i], items[1].volume, (int32_t *)m_Data[2][i], items[2].volume, total);
					}
					else
					{
						audio_procedure_volume3_32((int32_t *)buffer[i], (int32_t *)m_Data[0][i], (int32_t *)m_Data[1][i], (int32_t *)m_Data[2][i], total);
					}
					break;
				}
				break;

			case AV_SAMPLE_FMT_DBL:
			case AV_SAMPLE_FMT_DBLP:
				switch(count)
				{
				case 1:
					audio_procedure_volume1_dbl_v((double *)buffer[i], (double *)m_Data[0][i], items[0].volume, total);
					break;

				case 2:
					if (bVolume)
					{
						audio_procedure_volume2_dbl_v((double *)buffer[i], (double *)m_Data[0][i], items[0].volume, (double *)m_Data[1][i], items[1].volume, total);
					}
					else
					{
						audio_procedure_volume2_dbl((double *)buffer[i], (double *)m_Data[0][i], (double *)m_Data[1][i], total);
					}
					break;

				case 3:
					if (bVolume)
					{
						audio_procedure_volume3_dbl_v((double *)buffer[i], (double *)m_Data[0][i], items[0].volume, (double *)m_Data[1][i], items[1].volume, (double *)m_Data[2][i], items[2].volume, total);
					}
					else
					{
						audio_procedure_volume3_dbl((double *)buffer[i], (double *)m_Data[0][i], (double *)m_Data[1][i], (double *)m_Data[2][i], total);
					}
					break;
				}
				break;
			}
		}
	}

	m_CurrentPos += samples;
	return samples;
}

/**********************************************************
CAudioSource
**********************************************************/
void CAudioStream::Initialize()
{
	m_MinSamples = AUDIO_BUFFER_SEC * g_enc_opt.m_EncAudioSampleRate / 2;
	m_MaxBytes = AUDIO_BUFFER_SEC * g_enc_opt.m_EncAudioSampleRate * g_enc_opt.m_EncAudioPacketBytes;
	m_Channels = g_enc_opt.m_EncAudioIsPlanar ? g_enc_opt.m_EncAudioChannels : 1;
	m_PacketBytes = g_enc_opt.m_EncAudioPacketBytes;
}

CAudioStream::~CAudioStream()
{
}

HRESULT CAudioStream::Next()
{
	if (m_Source->m_EndOfStream)
	{
		m_EndOfStream = 1;
		return S_FALSE;
	}

	if (m_Buffered < m_MinSamples)
	{
		uint8_t* buffer[8];
		for(int i = 0; i < m_Channels; i++)
		{
			buffer[i] = m_Buffers[i] + m_WritePoint;
		}
		int samples = m_Source->ReadSamples(buffer);
		m_WritePoint += samples * m_PacketBytes;
		if (m_WritePoint >= m_MaxBytes)
		{
			int len = m_WritePoint - m_MaxBytes; 
			for(int i = 0; i < m_Channels; i++)
			{
				CopyMemory(m_Buffers[i], m_Buffers[i] + m_MaxBytes, len);
			}
			m_WritePoint = len;
		}
		InterlockedExchangeAdd(&m_Buffered, samples);
	}
	else
	{
		return S_FALSE;
	}

	return S_OK;
}

