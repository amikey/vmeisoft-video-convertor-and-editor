#include "stdafx.h"
#include "VideoConverter.h"
#include "../ffmpeg.inc.h"
#include "InfoDecoder.h"
#include "SaveBitmapFile.h"
#include "MediaMananger.h"

#define RESAMPLE_SIZE		(2 * 1024 * 1024)

void CInfoDecoder::CopyMediaInfo1()
{
	m_MediaInfo->m_ImportDuration = stream_duration ? stream_duration : 1;

	m_MediaInfo->m_Duration = stream_duration ? stream_duration : 1;
	m_MediaInfo->m_StreamDuration = stream_duration;

	m_MediaInfo->m_bVideoStream = video_stream != NULL; // 是否存在视频
	m_MediaInfo->m_Width = video_width; // 视频宽
	m_MediaInfo->m_Height = video_height;// 视频高
	m_MediaInfo->m_CodecWidth = video_codec_width;
	m_MediaInfo->m_CodecHeight = video_codec_height;
	m_MediaInfo->m_FrameRateNum = r_frame_rate.num; // 帧率
	m_MediaInfo->m_FrameRateDen = r_frame_rate.den; // 帧率
	m_MediaInfo->m_VideoBitRate = video_bit_rate;
	
	m_MediaInfo->m_bAudioStream = audio_stream != NULL; // 是否存在音频
	m_MediaInfo->m_SampleRate = audio_sample_rate; // 音频采样
	m_MediaInfo->m_nChannel = audio_channels; // 音频通道数量
	m_MediaInfo->m_channel_layout = audio_channel_layout;
	m_MediaInfo->m_AudioBitRate = audio_bit_rate;

	if (video_dec)
	{
		const AVCodecDescriptor *desc = avcodec_descriptor_get(video_dec->codec_id);
		if (desc)
		{
			if (desc->long_name) StringCchPrintf(m_MediaInfo->m_VideoCodec, 128, L"%hs", desc->long_name);
			else if (desc->name) StringCchPrintf(m_MediaInfo->m_VideoCodec, 128, L"%hs", desc->name);
		}
		m_MediaInfo->m_PixelFormat = video_dec->pix_fmt;
	}

	if (audio_dec)
	{
		const AVCodecDescriptor *desc = avcodec_descriptor_get(audio_dec->codec_id);
		if (desc)
		{
			if (desc->long_name) StringCchPrintf(m_MediaInfo->m_AudioCodec, 128, L"%hs", desc->long_name);
			else if (desc->name) StringCchPrintf(m_MediaInfo->m_AudioCodec, 128, L"%hs", desc->name);
		}
		m_MediaInfo->m_SampleFormat = audio_dec->sample_fmt;
		m_MediaInfo->m_IsPlanar = av_sample_fmt_is_planar(audio_format);
	}
}

void CInfoDecoder::CopyMediaInfo2()
{
	if (m_MediaInfo->m_bAudioStream)
	{
		int64_t need_samples = max_video_pts * audio_sample_rate / AV_TIME_BASE_LL;
		int64_t max_samples = (m_AudioTotalSampes > need_samples ? m_AudioTotalSampes : need_samples) + 1 * 1024 * 1024;
		LARGE_INTEGER liFileSize;
		liFileSize.QuadPart = (max_samples * m_AudioSampleBytes) & (~(64 * 1024 - 1));;
		for(int i = 0; i < m_AudioFiles; i++)
		{
			SetFilePointerEx(m_AudioTmpFile[i], liFileSize, NULL, FILE_BEGIN);
			SetEndOfFile(m_AudioTmpFile[i]);
			CloseHandle(m_AudioTmpFile[i]); m_AudioTmpFile[i] = NULL;
		}

		if (m_AudioData)
		{
			liFileSize.QuadPart = (max_samples * 4) & (~(64 * 1024 - 1));
			SetFilePointerEx(m_AudioData, liFileSize, NULL, FILE_BEGIN);
			SetEndOfFile(m_AudioData);
			CloseHandle(m_AudioData); m_AudioData = NULL;
		}

		m_MediaInfo->m_AudioLength = m_AudioTotalSampes * m_AudioSampleBytes;
		m_MediaInfo->m_AudioDuration = m_AudioTotalSampes * AV_TIME_BASE_LL / audio_sample_rate;
	}

	m_MediaInfo->m_VideoDuration = max_video_pts;
	m_MediaInfo->m_Duration = max(m_MediaInfo->m_VideoDuration, m_MediaInfo->m_AudioDuration);
	SaveMediaInfo(m_MediaInfo);
}

CInfoDecoder::CInfoDecoder(CMediaInfo* info)
	:CDecoder(this, 0)
{
	m_MediaInfo = info;
	demo_video_pts = -1ll;
	m_pAbort = &info->m_Abort;
}

CInfoDecoder::~CInfoDecoder()
{
	for(int i = 0; i < 8; i++)
	{
		SAFE_CLOSE(m_AudioTmpFile[i]);
	}
	SAFE_CLOSE (m_AudioData);
}

void CInfoDecoder::OnVideoStreamStart(int64_t pts)
{
	m_MediaInfo->m_VideoStartPts = pts;
}

void CInfoDecoder::OnAudioStreamStart(int64_t pts)
{
	m_MediaInfo->m_AudioStartPts = pts;
	if (pts)
	{
		m_AudioTotalSampes = pts * audio_sample_rate / AV_TIME_BASE_LL;
		int32_t n = (int32_t)(m_AudioTotalSampes * m_AudioSampleBytes);
		int32_t mutValue;
		if ((audio_format == AV_SAMPLE_FMT_U8) || (audio_format == AV_SAMPLE_FMT_U8P))
		{
			mutValue = 0x80;
		}
		else
		{
			mutValue = 0;
		}
		char *data = (char *)MemoryAlloc(n);
		memset(data, mutValue, n);
		for(int i = 0; i < m_AudioFiles; i++)
		{
			DWORD dwWriten;
			WriteFile(m_AudioTmpFile[i], data, n, &dwWriten, NULL);
		}
		MemoryFree(data);
	}
}

int32_t CInfoDecoder::OnVideoPacket()
{
	int64_t t;
	if (pkt.pts != AV_NOPTS_VALUE)
	{
		t = pkt.pts * AV_TIME_BASE_LL * (uint64_t)video_out_time_base.num / (uint64_t)video_out_time_base.den - stream_start_time;
	}
	else if (pkt.dts != AV_NOPTS_VALUE)
	{
		t = pkt.dts * AV_TIME_BASE_LL * (uint64_t)video_out_time_base.num / (uint64_t)video_out_time_base.den - stream_start_time;
	}
	else
	{
		in->streams[st_video_index]->discard = AVDISCARD_DEFAULT;
		frame_num ++;
		t = AV_TIME_BASE_LL * frame_num * r_frame_rate.den / r_frame_rate.num;
	}
	
	if (t > max_video_pts)
	{
		max_video_pts = t;
		if (audio_stream == NULL)
		{
			m_MediaInfo->m_ImportTime = t;
		}
	}
	if ((demo_video_pts < 10ll * AV_TIME_BASE_LL) && (pkt.flags & AV_PKT_FLAG_KEY))
	{
		return 0;
	}
	else
	{
		return -1;
	}
}

void CInfoDecoder::OnVideoStream(int64_t pts, AVFrame *frame)
{
	// 原始图像
	uint8_t *Bits = (uint8_t *)av_malloc(video_width * video_height * 4);
	uint8_t *buffer[3] = {Bits};
	int32_t dstStride[3] = {video_width * 4};
	struct SwsContext *context = sws_getContext(
		video_codec_width, video_codec_height, video_codec_format,
		video_width, video_height, AV_PIX_FMT_BGRA,
		SWS_BICUBIC, NULL, NULL, NULL);
	sws_scale(context, frame->data, frame->linesize, 0, video_codec_height, buffer, dstStride);
	sws_freeContext(context);

	SaveTempBitmapFile(m_MediaInfo->m_Demo, Bits, video_width, video_height);
	av_free(Bits);

	demo_video_pts = pts;
	m_MediaInfo->OnStatusChangedSync(MediaStat_Thumbnail_Generated);
}

void CInfoDecoder::OnAudioStream(AVFrame *frame)
{
	int32_t n = frame->nb_samples * m_AudioSampleBytes;
	for(int i = 0; i < m_AudioFiles; i++)
	{
		DWORD dwWriten;
		WriteFile(m_AudioTmpFile[i], frame->data[i], n, &dwWriten, NULL);
	}
	
	m_AudioTotalSampes += frame->nb_samples;
	m_MediaInfo->m_ImportTime = m_AudioTotalSampes * AV_TIME_BASE_LL / audio_sample_rate;
}

void CInfoDecoder::OnEndOfStream()
{
	m_EndOfStream = TRUE;
}

void CInfoDecoder::CreateAudioFiles()
{
	if (av_sample_fmt_is_planar(audio_format))
	{
		m_AudioFiles = audio_channels;
		m_AudioSampleBytes = av_get_bytes_per_sample(audio_format);
	}
	else
	{
		m_AudioFiles = 1;
		m_AudioSampleBytes = av_get_bytes_per_sample(audio_format) * audio_channels;
	}
	for(int i = 0; i < m_AudioFiles; i++)
	{
		m_AudioTmpFile[i] = CreateFile(m_MediaInfo->m_AudioTmpFile[i],
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,                    
			NULL,                 
			CREATE_ALWAYS,        
			FILE_ATTRIBUTE_NORMAL,
			NULL);               
	}
}

void CInfoDecoder::Start()
{
	if (ffdec_open(m_MediaInfo->m_szFileName))
	{
		m_MediaInfo->OnStatusChanged(MediaStat_Error_File);
		return ;
	}

	if (*m_pAbort) return ;
	CopyMediaInfo1();
	m_MediaInfo->OnStatusChanged(MediaStat_Check_Media);

	CreateAudioFiles();
	in->streams[st_video_index]->discard = AVDISCARD_NONKEY;
	while ((*m_pAbort == 0) && (m_EndOfStream == 0))
	{
		audio_next();
	}
	if (*m_pAbort) return ;

	CopyMediaInfo2();
	m_MediaInfo->OnStatusChanged(MediaStat_Complete);
}

static DWORD WINAPI MediaInfoThread(void *lpThreadParameter)
{
	CInfoDecoder *info = (CInfoDecoder *)lpThreadParameter;
	info->Start();
	delete info;
	return 0;
}


/***********************************************
 Interface
 ***********************************************/

CInfoDecoder* CreateInfoDecoder(CMediaInfo* info)
{
	CInfoDecoder *d = new CInfoDecoder(info);
	DWORD idThread;
	HANDLE hThread = CreateThread(NULL, 0, MediaInfoThread, d, 0, &idThread);
	CloseHandle(hThread);
	return d;
}
