#include "stdafx.h"
#include "../ffmpeg.inc.h"
#include "encoder.h"
#include "enc.h"
#include "Device/CheckOptions.h"

void CEncoder::set_video_codec_option(const char *name, const char* value)
{
	av_dict_set(&video_options, name, value, 0);
}

void CEncoder::set_audio_codec_option(const char *name, const char* value)
{
	av_dict_set(&audio_options, name, value, 0);
}

static AVPixelFormat find_pixel_format(const enum AVPixelFormat *pix_fmts)
{
	static const enum AVPixelFormat pixs[] =
	{
		AV_PIX_FMT_BGRA,
		AV_PIX_FMT_BGR0,
		AV_PIX_FMT_ARGB,
		AV_PIX_FMT_RGBA,
		AV_PIX_FMT_ABGR,
		AV_PIX_FMT_0RGB,
		AV_PIX_FMT_RGB0,
		AV_PIX_FMT_0BGR,
		AV_PIX_FMT_RGB24,
		AV_PIX_FMT_BGR24,
		AV_PIX_FMT_YUV420P,
		AV_PIX_FMT_NONE,
	};
	for(int i = 0; pixs[i] != AV_PIX_FMT_NONE; i++)
	{
		for(int j = 0; pix_fmts[j] != AV_PIX_FMT_NONE; j++)
		{
			if (pix_fmts[j] == pixs[i])
			{
				return pixs[i];
			}
		}
	}
	return AV_PIX_FMT_NONE;
}

/* add a video output stream */
AVStream* CEncoder::add_video_stream(enum AVCodecID codec_id, char *name)
{
	AVCodec *codec;
	AVStream *st;
	AVCodecContext *video_enc;

	/* find the video encoder */
	if (name)
	{
		codec = avcodec_find_encoder_by_name(name);
		if (!codec)
		{
			av_log(NULL, AV_LOG_ERROR, "codec not found video encoder:%s\n", name);
			return NULL;
		}
	}
	else
	{
		codec = avcodec_find_encoder(codec_id);
		if (!codec)
		{
			av_log(NULL, AV_LOG_ERROR, "codec not found video by ID=%d\n", codec_id);
			return NULL;
		}
	}
	if(codec->type != AVMEDIA_TYPE_VIDEO)
	{
		av_log(NULL, AV_LOG_ERROR, "Invalid video encoder type '%s'\n", name);
		return NULL;
	}

	if (frame_pix_fmt == AV_PIX_FMT_NONE)
	{
		frame_pix_fmt = find_pixel_format(codec->pix_fmts);
		if (frame_pix_fmt == AV_PIX_FMT_NONE) frame_pix_fmt = codec->pix_fmts[0];
	}

	st = avformat_new_stream(oc, NULL);
	if (!st)
	{
		av_log(NULL, AV_LOG_ERROR, "Could not alloc stream\n");
		return NULL;
	}
	st->id = 0;

	video_enc = st->codec;
	video_enc->codec_type = AVMEDIA_TYPE_VIDEO;
	if (codec_id) video_enc->codec_id = codec_id;
	else video_enc->codec_id = codec->id;
	avcodec_get_context_defaults3(video_enc, codec);

	if (video_rc_max_rate != NO_VALUE)
		if (video_bit_rate > video_rc_max_rate)
			video_bit_rate = video_rc_max_rate;

	if ((sample_aspect_ratio.num != 1) && (sample_aspect_ratio.den != 1))
	{
		sample_aspect_ratio = av_d2q((double)sample_aspect_ratio.num / (double)sample_aspect_ratio.den * (double)video_height/(double)video_width, 255);
	}

	if ((video_bit_rate == 0) && (video_qscale == 0)) video_qscale = 4;
	st->sample_aspect_ratio = sample_aspect_ratio;
	if (video_bit_rate) video_enc->bit_rate = video_bit_rate;
	video_enc->width = video_width;
	video_enc->height = video_height;
	video_enc->time_base.num = g_enc_opt.m_FrameNum;
	video_enc->time_base.den = g_enc_opt.m_FrameDen;
	if (video_gop_size) video_enc->gop_size = video_gop_size; /* emit one intra frame every twelve frames at most */
	video_enc->pix_fmt = frame_pix_fmt;
	video_enc->sample_aspect_ratio = sample_aspect_ratio;
	if (video_codec_tag) video_enc->codec_tag = video_codec_tag;
	if (video_qscale)
	{
		video_enc->flags |= CODEC_FLAG_QSCALE;
		video_enc->global_quality = FF_QP2LAMBDA * video_qscale;
	}

	if (video_rc_max_rate != NO_VALUE) video_enc->rc_max_rate = video_rc_max_rate;
	if (video_rc_min_rate != NO_VALUE) video_enc->rc_min_rate = video_rc_min_rate;
	if (video_rc_buffer_size != NO_VALUE) video_enc->rc_buffer_size = video_rc_buffer_size;
	if (video_coder_type != NO_VALUE) video_enc->coder_type = video_coder_type;
	if (video_max_b_frames != NO_VALUE) video_enc->max_b_frames = video_max_b_frames;

	if (video_level != NO_VALUE) video_enc->level = video_level;
	if (video_sc_threshold != NO_VALUE) video_enc->scenechange_threshold = video_sc_threshold;
	if (video_qmin != NO_VALUE) video_enc->qmin = video_qmin;
	if (video_qmax != NO_VALUE) video_enc->qmax = video_qmax;
	if (video_qdiff != NO_VALUE) video_enc->max_qdiff = video_qdiff;
	if (video_i_qfactor != NO_VALUE) video_enc->b_quant_factor = video_i_qfactor;
	if (video_rc_eq) video_enc->rc_eq = av_strdup(video_rc_eq);
	if (video_qcomp != NO_VALUE) video_enc->qcompress = video_qcomp;
	if (video_me_range != NO_VALUE) video_enc->me_range = video_me_range;
	if (thread_count) video_enc->thread_count = thread_count;

	switch(video_enc->codec_id)
	{
	case CODEC_ID_MPEG2VIDEO:
		video_enc->max_b_frames = 2;
		break;

	case CODEC_ID_MPEG1VIDEO:
		video_enc->mb_decision = 2;
		break;
			
	default:
		break;
	}

	// some formats want stream headers to be separate
	if(oc->oformat->flags & AVFMT_GLOBALHEADER)
		video_enc->flags |= CODEC_FLAG_GLOBAL_HEADER;

	/* open the codec */
	if (avcodec_open2(video_enc, codec, &video_options) < 0)
	{
		av_log(NULL, AV_LOG_ERROR, "could not open video codec\n");
		return NULL;
	}

	if (oc->oformat->flags & AVFMT_RAWPICTURE)
	{
		av_log(NULL, AV_LOG_ERROR, "Raw Picture ?\n");
		return NULL;
	}

	video_outbuf_size = video_width * video_height * 16;
	if (video_outbuf_size < FF_MIN_BUFFER_SIZE) video_outbuf_size = FF_MIN_BUFFER_SIZE;
	video_outbuf = (uint8_t *)av_malloc(video_outbuf_size);

	return st;
}

/*
* add an audio output stream
*/

void CEncoder::audio_check_samplerate(AVCodec *codec)
{
	int32_t i;
	if (codec->supported_samplerates == NULL) return;
	for(i = 0; codec->supported_samplerates[i]; i++)
	{
		if (codec->supported_samplerates[i] == audio_sample_rate)
		{
			return;
		}
	}

	// 找一个格式最为相近的
	int32_t error_value = INT_MAX;
	int32_t samplerate = 0;
	for(i = 0; codec->supported_samplerates[i]; i++)
	{
		int32_t e = codec->supported_samplerates[i] - audio_sample_rate;
		if (e < 0) e = -e;
		if (e < error_value)
		{
			error_value = e;
			samplerate = codec->supported_samplerates[i];
		}
	}
	
	audio_sample_rate = samplerate;

}

void CEncoder::audio_check_channel_layout(AVCodec *codec)
{
	if (codec->channel_layouts == NULL)
	{
		audio_channel_layout = av_get_default_channel_layout(audio_channels);
		return ;
	}
	
	for(int i = 0; codec->channel_layouts[i]; i++)
	{
		if (av_get_channel_layout_nb_channels(codec->channel_layouts[i]) == audio_channels)
		{
			audio_channel_layout = codec->channel_layouts[i];
			return ;
		}
	}

	for(int i = 0; codec->channel_layouts[i]; i++)
	{
		if (av_get_channel_layout_nb_channels(codec->channel_layouts[i]) > audio_channels)
		{
			audio_channels = av_get_channel_layout_nb_channels(codec->channel_layouts[i]);
			audio_channel_layout = codec->channel_layouts[i];
			return ;
		}
	}

	audio_channel_layout = codec->channel_layouts[0];
	audio_channels = av_get_channel_layout_nb_channels(audio_channel_layout);
}

void CEncoder::audio_check_sample_fmt(AVCodec *codec)
{
	static const enum AVSampleFormat sample_fmt_priority[] = 
	{
		AV_SAMPLE_FMT_S16,
		AV_SAMPLE_FMT_S16P, 
		AV_SAMPLE_FMT_FLTP, 
		AV_SAMPLE_FMT_FLT, 
		AV_SAMPLE_FMT_S32, 
		AV_SAMPLE_FMT_S32P, 
		AV_SAMPLE_FMT_NONE
	};

	if (codec->sample_fmts == NULL)
	{
		audio_sample_fmt = sample_fmt_priority[0];
		return ;
	}

	for(int i = 0; sample_fmt_priority[i] != AV_SAMPLE_FMT_NONE; i++)
	{
		for(int j = 0; codec->sample_fmts[j] != AV_SAMPLE_FMT_NONE; j++)
		{
			if (sample_fmt_priority[i] == codec->sample_fmts[j])
			{
				audio_sample_fmt = sample_fmt_priority[i];
				return ;
			}
		}
	}

	audio_sample_fmt = codec->sample_fmts[0];
}

AVStream* CEncoder::add_audio_stream(enum AVCodecID codec_id, char *name)
{
	AVCodec *codec;
	AVStream *st;
	AVCodecContext *audio_enc;

	/* find the audio encoder */
	if (name)
	{
		codec = avcodec_find_encoder_by_name(name);
		if (!codec)
		{
			av_log(NULL, AV_LOG_ERROR, "codec not found audio encoder, name=%s\n", name);
			return NULL;
		}
	}
	else
	{
		codec = avcodec_find_encoder(codec_id);
		if (!codec)
		{
			av_log(NULL, AV_LOG_ERROR, "codec not found audio encoder, id=%d\n", codec_id);
			return NULL;
		}
	}
	if(codec->type != AVMEDIA_TYPE_AUDIO)
	{
		av_log(NULL, AV_LOG_ERROR, "Invalid audio encoder type '%s'\n", name);
		return NULL;
	}

	st = avformat_new_stream(oc, NULL);
	if (!st)
	{
		av_log(NULL, AV_LOG_ERROR, "Could not alloc stream\n");
		return NULL;
	}
	st->id = g_enc_opt.m_VideoDisable ? 0 : 1;

	audio_enc = st->codec;
	audio_enc->codec_type = AVMEDIA_TYPE_AUDIO;

	if (codec_id) audio_enc->codec_id = codec_id;
	else audio_enc->codec_id = codec->id;

	avcodec_get_context_defaults3(audio_enc, codec);

	// 检查格式是否支持
	audio_check_channel_layout(codec);
	audio_check_samplerate(codec);
	audio_check_sample_fmt(codec);
	
	//
	audio_enc->sample_rate = audio_sample_rate;
	audio_enc->sample_fmt = audio_sample_fmt;
	audio_enc->channels = audio_channels;
	audio_enc->channel_layout = audio_channel_layout;

	audio_enc->bit_rate = audio_bit_rate;
	audio_enc->time_base.num = 1;
	audio_enc->time_base.den = audio_sample_rate;
	if (audio_codec_tag) audio_enc->codec_tag = audio_codec_tag;

	if (audio_enc->codec_id == CODEC_ID_FLAC)
	{
		audio_enc->frame_size = 256 << 3;
		audio_enc->compression_level = 5;
	}

	// some formats want stream headers to be separate
	if(oc->oformat->flags & AVFMT_GLOBALHEADER)
		audio_enc->flags |= CODEC_FLAG_GLOBAL_HEADER;

	/* open it */
	if (avcodec_open2(audio_enc, codec, &audio_options) < 0)
	{
		av_log(NULL, AV_LOG_ERROR, "could not open audio codec\n");
		return NULL;
	}

	/* ugly hack for PCM codecs (will be removed ASAP with new PCM support to compute the input frame size in samples */
	switch(st->codec->codec_id)
	{
	case CODEC_ID_PCM_S16LE:
		{
			if (g_enc_opt.m_VideoDisable)
			{
				audio_frame_size = audio_sample_rate;
			}
			else
			{
				audio_frame_size = audio_sample_rate * g_enc_opt.m_FrameNum / g_enc_opt.m_FrameDen;
			}
			audio_outbuf_size = audio_frame_size * audio_channels * 2;
		}
		break;

	default:
		{
			if (audio_enc->frame_size <= 1)
			{
				av_log(NULL, AV_LOG_ERROR, "Unsupported raw audio output(id=%d) !\n", st->codec->codec_id);
				return NULL;
			}
			audio_frame_size = audio_enc->frame_size;
		}
		break;
	}


	audio_outbuf = (uint8_t *)av_malloc(audio_outbuf_size);

	avcodec_get_frame_defaults(&audio_frame);
	return st;
}

int32_t CEncoder::write_video_frame(int32_t repeat, int32_t index)
{
	int32_t ret;
	AVCodecContext *enc = video_st->codec;
	AVPacket pkt;
	int32_t got_packet = 0;

	if (repeat == 0)
	{
		last_picture = &picture_list[index];
	}

	last_picture->repeat_pict = repeat;
	last_picture->quality = enc->global_quality;
	last_picture->pict_type = AV_PICTURE_TYPE_NONE;
	last_picture->pts = video_encodeced_frames;

	av_init_packet(&pkt);
	pkt.data = video_outbuf;
	pkt.size = video_outbuf_size;

	/* encode the image */
	ret = avcodec_encode_video2(enc, &pkt, last_picture, &got_packet);
	if (ret < 0)
	{
		av_log(NULL, AV_LOG_FATAL, "Video encoding failed\n");
		return ret;
    }

	if (got_packet)
	{
		if ((pkt.pts == AV_NOPTS_VALUE) && !(enc->codec->capabilities & CODEC_CAP_DELAY))
			pkt.pts = video_encodeced_frames;

		if (pkt.pts != AV_NOPTS_VALUE) pkt.pts = av_rescale_q(pkt.pts, enc->time_base, video_st->time_base);
		if (pkt.dts != AV_NOPTS_VALUE) pkt.dts = av_rescale_q(pkt.dts, enc->time_base, video_st->time_base);

		/* write the compressed frame in the media file */
		pkt.stream_index = video_st->index;
		ret = av_interleaved_write_frame(oc, &pkt);
		if (ret != 0)
		{
			av_log(NULL, AV_LOG_ERROR, "Error while writing video frame: got_packet=%d\n", got_packet);
			return -1;
		}
	}

	video_encodeced_frames ++;

	return 0;
}

int32_t CEncoder::write_audio_frame()
{
	int32_t ret;
	AVPacket pkt;
	AVCodecContext *enc = audio_st->codec;
	int32_t got_packet = 0;

	av_init_packet(&pkt);
	pkt.data = audio_outbuf;
	pkt.size = audio_outbuf_size;

	audio_frame.format = audio_sample_fmt;
	audio_frame.nb_samples = audio_frame_size;
	audio_frame.pts = audio_encodeced_frames;
	audio_encodeced_frames += audio_frame_size;

	ret = avcodec_encode_audio2(enc, &pkt, &audio_frame, &got_packet);
	if (ret < 0)
	{
		av_log(NULL, AV_LOG_FATAL, "Audio encoding failed\n");
		return ret;
    }

	if (got_packet)
	{
		if (pkt.pts != AV_NOPTS_VALUE) pkt.pts = av_rescale_q(pkt.pts, enc->time_base, audio_st->time_base);
		if (pkt.dts != AV_NOPTS_VALUE) pkt.dts = av_rescale_q(pkt.dts, enc->time_base, audio_st->time_base);
		if (pkt.duration > 0) pkt.duration = (int32_t)av_rescale_q(pkt.duration, enc->time_base, audio_st->time_base);

		/* write the compressed frame in the media file */
		pkt.stream_index = audio_st->index;
		if (av_interleaved_write_frame(oc, &pkt) != 0)
		{
			av_log(NULL, AV_LOG_ERROR, "Error while writing audio frame\n");
			return -1;
		}
	}

	return 0;
}

void CEncoder::ending_video_codec()
{
	AVPacket pkt;
	int32_t got_packet = 0;
	AVCodecContext *enc = video_st->codec;

	av_log(NULL, AV_LOG_ERROR, "Video: End of Stream\n");

	if (oc->oformat->flags & AVFMT_RAWPICTURE) return;
	if (oc->oformat->flags & AVFMT_NOFILE) return;
	if (enc->codec_id == CODEC_ID_RAWVIDEO) return;

	av_init_packet(&pkt);
	pkt.data = video_outbuf;
	pkt.size = video_outbuf_size;

	avcodec_encode_video2(enc, &pkt, NULL, &got_packet);
	if (got_packet == 0) return ;

	if (pkt.pts != AV_NOPTS_VALUE) pkt.pts = av_rescale_q(pkt.pts, enc->time_base, video_st->time_base);
	if (pkt.dts != AV_NOPTS_VALUE) pkt.dts = av_rescale_q(pkt.dts, enc->time_base, video_st->time_base);

	pkt.stream_index = video_st->index;
	if (av_interleaved_write_frame(oc, &pkt) != 0)
	{
		av_log(NULL, AV_LOG_ERROR, "Error while writing video ending frame\n");
	}
}

void CEncoder::ending_audio_codec()
{
	AVPacket pkt;
	int32_t got_packet = 0;
	AVCodecContext *enc = audio_st->codec;

	av_log(NULL, AV_LOG_ERROR, "Audio: End of Stream\n");

	if(enc->frame_size <= 1) return;

	av_init_packet(&pkt);
	pkt.data = audio_outbuf;
	pkt.size = audio_outbuf_size;
	avcodec_encode_audio2(enc, &pkt, NULL, &got_packet);

	if (got_packet == 0) return ;

	if (pkt.pts != AV_NOPTS_VALUE) pkt.pts = av_rescale_q(pkt.pts, enc->time_base, audio_st->time_base);
	if (pkt.dts != AV_NOPTS_VALUE) pkt.dts = av_rescale_q(pkt.dts, enc->time_base, audio_st->time_base);
	if (pkt.duration > 0) pkt.duration = (int32_t)av_rescale_q(pkt.duration, enc->time_base, audio_st->time_base);

	pkt.stream_index = audio_st->index;
	if (av_interleaved_write_frame(oc, &pkt) != 0)
	{
		av_log(NULL, AV_LOG_ERROR, "Error while writing audio ending frame\n");
	}
}

int32_t CEncoder::enc_open(void)
{
	avformat_alloc_output_context2(&oc, NULL, format_name, filename);
	if (oc == NULL)
	{
		av_log(NULL, AV_LOG_ERROR, "Could not find suitable output format:%s(%s)\n", format_name, filename);
		return -1;
	}

	fmt = oc->oformat;
	oc->max_delay = (int32_t)(mux_max_delay * AV_TIME_BASE);
	if (packet_size != NO_VALUE) oc->packet_size = packet_size;
	strcpy(oc->filename, filename);

	if (g_enc_opt.m_VideoDisable == 0)
	{
		if ((video_codec_id == CODEC_ID_NONE) && (video_codec_name == NULL))
		{
			video_codec_id = fmt->video_codec;
		}
		video_st = add_video_stream(video_codec_id, video_codec_name);
		if (video_st == NULL) return -1;
	}
	if (g_enc_opt.m_AudioDisable == 0)
	{
		if ((audio_codec_id == CODEC_ID_NONE) && (audio_codec_name == NULL))
		{
			audio_codec_id = fmt->audio_codec;
		}
		audio_st = add_audio_stream(audio_codec_id, audio_codec_name);
		if (audio_st == NULL) return -1;
	}

	if (!(oc->oformat->flags & AVFMT_NOSTREAMS))
	{
		av_dump_format(oc, 0, filename, 1);
	}

	/* open the output file, if needed */
	if (!(oc->oformat->flags & AVFMT_NOFILE))
	{
		if (avio_open(&oc->pb, filename, AVIO_FLAG_WRITE) < 0)
		{
			av_log(NULL, AV_LOG_ERROR, "Could not open '%s'\n", filename);
			return -8;
		}
	}

	/* write the stream header, if any */
	if (avformat_write_header(oc, NULL) != 0)
	{
		av_log(NULL, AV_LOG_ERROR, "Failed to av_write_header !\n");
		return -9;
	}

	return 0;
}

int32_t CEncoder::enc_close()
{
	av_log(NULL, AV_LOG_WARNING, "End of stream, Write header.\n");
	if (oc)
	{
		av_write_trailer(oc);
		if (oc->pb)
		{
			avio_close(oc->pb);
			oc->pb = NULL;
		}
	}
	if (video_st)
	{
		if (video_st->codec) avcodec_close(video_st->codec);
		video_st = NULL;
	}
	if (audio_st)
	{
		if (audio_st->codec) avcodec_close(audio_st->codec);
		audio_st = NULL;
	}
	if (oc)
	{
		avformat_free_context(oc);
		oc = NULL;
	}
	av_log(NULL, AV_LOG_WARNING, "OK.\n");
	return 0;
}

int32_t CEncoder::enc_loop(void)
{
	int video_frames = 0;

	int video_index = 0;
	int video_queue_index = 0;
	int last_video_index = -1;
	uint64_t audio_total_write = 0;
	
	uint64_t allow_audio_samples = g_enc_opt.m_VideoDisable ? INT64_MAX : 0;
	int audio_buffer_point = 0;
	int audio_buffer_channels = g_enc_opt.m_EncAudioIsPlanar ? g_enc_opt.m_EncAudioChannels : 1;
	int audio_buffer_max_bytes = AUDIO_BUFFER_SEC * g_enc_opt.m_EncAudioSampleRate * g_enc_opt.m_EncAudioPacketBytes;
	int audio_buffer_frame_bytes = audio_frame_size * g_enc_opt.m_EncAudioPacketBytes;
	uint64_t audio_samples_per_sec = audio_sample_rate;

	int32_t video_eof = g_enc_opt.m_VideoDisable;
	int32_t audio_eof = g_enc_opt.m_AudioDisable;

	while ((video_eof == 0) || (audio_eof == 0))
	{
		if (*m_pAbort) return ABORT_FLAG;
		if (video_eof == 0)
		{
			while ((m_VStream->m_Eof == 0) && (m_VStream->m_Queued == 0))
			{
				if (*m_pAbort) return ABORT_FLAG;
				Sleep(1);
			}

			if (m_VStream->m_Queued == 0)
			{
				assert(m_VStream->m_Eof);
				video_eof = 1;
				ending_video_codec();
				if (m_AStream)
				{
					allow_audio_samples = INT64_MAX;
				}
				else if (audio_eof == 0)
				{
					// 结束静音
					audio_eof = 1;
					ending_audio_codec();
				}
				continue;
			}

			video_index = m_VStream->m_Queue[video_queue_index];
			video_queue_index = (video_queue_index + 1) & (MAX_VIDEO_QUEUE - 1);
			if (write_video_frame(last_video_index == video_index, video_index) < 0)
			{
				return -1;
			}
			if (last_video_index != video_index)
			{
				if (last_video_index != -1)
					InterlockedDecrement(&m_VStream->m_Buffered);
				last_video_index = video_index;
			}
			InterlockedDecrement(&m_VStream->m_Queued);
			video_frames ++;
			allow_audio_samples = (uint64_t)video_frames * audio_samples_per_sec * (uint64_t)g_enc_opt.m_FrameNum / (uint64_t)g_enc_opt.m_FrameDen;
		}

		if (*m_pAbort) return ABORT_FLAG;
		if ((audio_eof == 0) && (m_AStream))
		{
			while (audio_total_write < allow_audio_samples)
			{

				while ((m_AStream->m_Eof == 0) && (m_AStream->m_Buffered < audio_frame_size))
				{
					if (*m_pAbort) return ABORT_FLAG;
					Sleep(1);
				}
				
				if (m_AStream->m_Buffered < audio_frame_size)
				{
					assert(m_AStream->m_Eof);
					m_AStream->m_Buffered = 0;
					audio_eof = 1;
					ending_audio_codec();
					break;
				}

				for(int i = 0; i < audio_buffer_channels; i++)
				{
					audio_frame.data[i] = &m_AStream->m_Buffers[i][audio_buffer_point];
					audio_frame.linesize[i] = audio_buffer_frame_bytes;
				}
				
				if ((audio_buffer_point + audio_buffer_frame_bytes) < audio_buffer_max_bytes)
				{
					audio_buffer_point += audio_buffer_frame_bytes;
				}
				else
				{
					int32_t l = audio_buffer_point + audio_buffer_frame_bytes - audio_buffer_max_bytes;
					for(int i = 0; i < audio_buffer_channels; i++)
					{
						memcpy(&m_AStream->m_Buffers[i][audio_buffer_max_bytes], m_AStream->m_Buffers[i], l);
					}
					audio_buffer_point = l;
				}
				
				if (write_audio_frame() < 0)
				{
					return -1;
				}

				InterlockedExchangeAdd((volatile long *)&m_AStream->m_Buffered, -audio_frame_size);
				audio_total_write += audio_frame_size;
			}
		}
		else if (audio_eof == 0)
		{
			// 写入静音
			while (audio_total_write < allow_audio_samples)
			{
				for(int i = 0; i < audio_buffer_channels; i++)
				{
					audio_frame.data[i] = m_AudioBuffer;
					audio_frame.linesize[i] = audio_buffer_frame_bytes;
				}
				if (write_audio_frame() < 0)
				{
					return -1;
				}
				audio_total_write += audio_frame_size;
			}
		}
				
		if (video_eof == 0)
		{
			uint64_t t = (uint64_t)video_frames * AV_TIME_BASE_LL * (uint64_t)g_enc_opt.m_FrameNum / (uint64_t)g_enc_opt.m_FrameDen;
			InterlockedExchange64(m_Time, t);
		}
		else if (audio_eof == 0)
		{
			int64_t t = audio_total_write * AV_TIME_BASE_LL / audio_samples_per_sec;
			InterlockedExchange64(m_Time,  t);
		}
	}

	return 0;
}

