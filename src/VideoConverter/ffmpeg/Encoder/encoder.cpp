#include "stdafx.h"
#include "../ffmpeg.inc.h"
#include "encoder.h"
#include "enc.h"
#include "Device/CheckOptions.h"

#define AV_FREE(p) if (p) {av_free(p); p = NULL;};

CEncoder::CEncoder(VideoStream* vStream, struct AudioStream *aStream)
{
	m_VStream = vStream;
	m_AStream = aStream;

	audio_outbuf_size = 64 * 1024;

	video_codec_id = CODEC_ID_NONE;
	audio_codec_id = CODEC_ID_NONE;
	mux_max_delay = 0.7f;

	frame_pix_fmt = AV_PIX_FMT_NONE;
	sample_aspect_ratio.num = 1;
	sample_aspect_ratio.den = 1;

	audio_sample_fmt = AV_SAMPLE_FMT_S16;

	packet_size = NO_VALUE;

	video_rc_max_rate = NO_VALUE;
	video_rc_min_rate = NO_VALUE;
	video_rc_buffer_size = NO_VALUE;
	video_coder_type = NO_VALUE;
	video_max_b_frames = NO_VALUE;

	video_level = NO_VALUE;
	video_sc_threshold = NO_VALUE;
	video_qmin = 3;
	video_qmax = 6;
	video_qdiff = NO_VALUE;
	video_i_qfactor = NO_VALUE;
	video_qcomp = NO_VALUE;
	video_me_range = NO_VALUE;

}

int32_t CEncoder::Initialize()
{
	if (g_enc_opt.m_VideoDisable == 0)
	{
		sprintf(video_codec_name, "%ls", g_enc_opt.m_VideoCodec);
		video_bit_rate = g_enc_opt.m_VideoBitrate;
		video_width = g_enc_opt.m_VideoWidth;
		video_height = g_enc_opt.m_VideoHeight;
	}

	if (g_enc_opt.m_AudioDisable == 0)
	{
		sprintf(audio_codec_name, "%ls", g_enc_opt.m_AudioCodec);
		audio_bit_rate = g_enc_opt.m_AudioBitrate;
		audio_sample_rate = g_enc_opt.m_AudioSampleRate;
		audio_channels = g_enc_opt.m_AudioChannels;
	}

	check_options();

	if (enc_open() != 0) return -1;

	if (InterlockedCompareExchange(&g_enc_opt.m_EncInfo, 1, 0) == 0)
	{
		if (g_enc_opt.m_VideoDisable == 0)
		{
			switch(frame_pix_fmt)
			{
			case AV_PIX_FMT_BGR0:
				g_enc_opt.m_EncPixFormat = AV_PIX_FMT_BGRA;
				break;

			default:
				g_enc_opt.m_EncPixFormat = frame_pix_fmt;
				break;
			}
			
			g_enc_opt.m_EncVideoPlanes = av_pix_fmt_count_planes(frame_pix_fmt);
			av_image_fill_linesizes(g_enc_opt.m_EncVideoStride, frame_pix_fmt, g_enc_opt.m_VideoWidth);

			const AVPixFmtDescriptor *dsc = av_pix_fmt_desc_get(frame_pix_fmt);
			g_enc_opt.m_EncVideoHeight[0] = g_enc_opt.m_VideoHeight;
			g_enc_opt.m_EncVideoChromaH = dsc->log2_chroma_h;
			for(int i = 1; i < g_enc_opt.m_EncVideoPlanes; i++)
			{
				g_enc_opt.m_EncVideoHeight[i] = g_enc_opt.m_VideoHeight >> dsc->log2_chroma_h;
			}
		}
		else
		{
			g_enc_opt.m_EncPixFormat = 0;
		}
		if (g_enc_opt.m_AudioDisable == 0)
		{
			g_enc_opt.m_EncAudioFormat = audio_sample_fmt;
			g_enc_opt.m_EncAudioSampleRate = audio_sample_rate;
			g_enc_opt.m_EncAudioChannels = audio_channels;
			g_enc_opt.m_EncAudioChannelLayout = audio_channel_layout;
			if (audio_channels > 1)
			{
				g_enc_opt.m_EncAudioIsPlanar = av_sample_fmt_is_planar(audio_sample_fmt);
			}
			else
			{

				g_enc_opt.m_EncAudioIsPlanar = 0;
			}

			if (g_enc_opt.m_EncAudioIsPlanar)
			{
				g_enc_opt.m_EncAudioPacketBytes = av_get_bytes_per_sample(audio_sample_fmt);
			}
			else
			{
				g_enc_opt.m_EncAudioPacketBytes = av_get_bytes_per_sample(audio_sample_fmt) * g_enc_opt.m_EncAudioChannels;
			}
		}
		else
		{
			g_enc_opt.m_EncAudioFormat = 0;
		}
	}

	if (g_enc_opt.m_VideoDisable == 0)
	{
		for(int i = 0; i < MAX_VIDEO_BUFFER; i++)
		{
			avcodec_get_frame_defaults(&picture_list[i]);
			avpicture_alloc((AVPicture *)&picture_list[i],
				frame_pix_fmt, g_enc_opt.m_VideoWidth, g_enc_opt.m_VideoHeight);

			for(int j = 0; j < g_enc_opt.m_EncVideoPlanes; j++)
			{
				m_VStream->m_Buffers[i][j] = picture_list[i].data[j];
			}
		}
	}

	if (g_enc_opt.m_AudioDisable == 0)
	{
		if (m_AStream)
		{
			if (g_enc_opt.m_EncAudioIsPlanar)
			{
				int32_t size = (AUDIO_BUFFER_SEC + 1) * g_enc_opt.m_EncAudioSampleRate * g_enc_opt.m_EncAudioPacketBytes;
				size = (size + 63) & (~63);
				m_AudioBuffer = (uint8_t *)av_malloc(size * audio_channels);
				for(int i = 0; i < audio_channels; i++)
				{
					m_AStream->m_Buffers[i] = m_AudioBuffer + i * size;
				}
			}
			else
			{
				int32_t size = (AUDIO_BUFFER_SEC + 1) * g_enc_opt.m_EncAudioSampleRate * g_enc_opt.m_EncAudioPacketBytes;
				m_AudioBuffer = (uint8_t *)av_malloc(size);
				m_AStream->m_Buffers[0] = m_AudioBuffer;
			}
		}
		else
		{
			int32_t mutValue;
			if ((audio_sample_fmt == AV_SAMPLE_FMT_U8) || (audio_sample_fmt == AV_SAMPLE_FMT_U8P))
			{
				mutValue = 0x80;
			}
			else
			{
				mutValue = 0;
			}
			int32_t totalsize = audio_frame_size * g_enc_opt.m_EncAudioPacketBytes;
			m_AudioBuffer = (uint8_t *)av_malloc(totalsize);
			memset(m_AudioBuffer, mutValue, totalsize);
		}
	}

	return 0;
}

CEncoder::~CEncoder()
{
	AV_FREE (video_outbuf);
	AV_FREE (audio_outbuf);
	AV_FREE (m_AudioBuffer);

	av_dict_free(&video_options);
	av_dict_free(&audio_options);

	for(int i = 0; i < MAX_VIDEO_BUFFER; i++)
	{
		if (picture_list[i].data[0])
		{
			avpicture_free((AVPicture *)&picture_list[i]);
		}
	}
}

static DWORD WINAPI EncThread(void *lpThreadParameter)
{
	CEncoder *p = (CEncoder *)lpThreadParameter;
	p->enc_loop();
	p->enc_close();
	return 0;
}

CEncoder* CreateEncoder(wchar_t *filename, volatile int64_t *time, volatile long *abort_addr, VideoStream* vStream, struct AudioStream *aStream)
{
	CEncoder *e = new CEncoder(vStream, aStream);

	e->m_Time = time;
	e->m_pAbort = abort_addr;
	sprintf(e->format_name, "%ls", g_enc_opt.m_Format);
	sprintf(e->filename, "%ls", filename);
	sprintf(e->target_name, "%ls",g_enc_opt.m_Target);

	if (e->Initialize())
	{
		delete e;
		return NULL;
	}

	DWORD idThread;
	e->m_thread = CreateThread(NULL, 0, EncThread, e, 0, &idThread);
	return e;
}

void DestroyEncoder(CEncoder *e)
{
	WaitForSingleObject(e->m_thread, INFINITE);
	CloseHandle(e->m_thread);
	delete e;
}
