#include "stdafx.h"
#include "../ffmpeg.inc.h"
#include "dec.h"
#include "../../VideoConverter.h"

int32_t CDecoder::ffdec_stream_component_open(AVStream *stream)
{
	AVCodecContext *avctx = stream->codec;
	AVCodec *codec;

	codec = avcodec_find_decoder(avctx->codec_id);
	if (!codec)
		return -1;

	if(codec->capabilities & CODEC_CAP_DR1)
		avctx->flags |= CODEC_FLAG_EMU_EDGE;

	if (avcodec_open2(avctx, codec, NULL) < 0)
	{
		av_log(NULL, AV_LOG_ERROR, "Failed to open codec\n");
		return -1;
	}

	stream->discard = AVDISCARD_DEFAULT;
	return 0;
}

void CDecoder::ffdec_decode_audio()
{
	AVPacket pkt_temp = pkt;
	while (pkt_temp.size > 0)
	{
		int32_t got_frame = 0;
		AVFrame frame;
		avcodec_get_frame_defaults(&frame);
		int32_t len = avcodec_decode_audio4(audio_dec, &frame, &got_frame, &pkt_temp);
		if (len < 0)
		{
			av_log(NULL, AV_LOG_ERROR, "Failed to decode audio: %d(%d) \n", len, pkt_temp.size);
			break;
		}
		if (got_frame > 0)
		{
			ffdec_audio_put_frame(&frame);
		}
		pkt_temp.data += len;
		pkt_temp.size -= len;
	}
}

void CDecoder::ffdec_decode_video()
{
	AVPacket pkt_temp = pkt;
	while (pkt_temp.size > 0)
	{
		int32_t got_picture = 0;
		AVFrame frame;
		avcodec_get_frame_defaults(&frame);
		int32_t len = avcodec_decode_video2(video_dec, &frame, &got_picture, &pkt_temp);

		if (len < 0)
		{
			av_log(NULL, AV_LOG_ERROR, "Failed to decode video: %d(%d) \n", len, pkt_temp.size);
			break;
		}
		if (got_picture)
		{
			ffdec_video_put_frame(&frame);
		}
		pkt_temp.data += len;
		pkt_temp.size -= len;
	}
}


int32_t CDecoder::ffdec_open(wchar_t *wszFile)
{
	int32_t err;
	int32_t i;
	char file[MAX_PATH]; 
	sprintf(file, "%ls", wszFile);

	err = avformat_open_input(&in, file, NULL, NULL);
	if (err < 0)
	{
		av_log(NULL, AV_LOG_ERROR, "Can't open video input:%s, error=%d\n", file, err);
		return -1;
	}

	err = avformat_find_stream_info(in, NULL);
	if (err < 0)
	{
		av_log(NULL, AV_LOG_ERROR, "%s: could not find codec parameters\n", file);
		return -1;
	}

	for (i = 0; i < in->nb_streams; i++) in->streams[i]->discard = AVDISCARD_ALL;

	// if (video_disable == 0)
	{
		st_video_index = av_find_best_stream(in, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
		if (st_video_index >= 0)
		{
			video_stream = in->streams[st_video_index];
			err = ffdec_stream_component_open(video_stream);
			if (err < 0)
			{
				st_video_index = -1;
				video_stream = NULL;
				av_log(NULL, AV_LOG_ERROR, "Could not open video codec\n");
			}
			else
			{
				video_dec = video_stream->codec;
				ffdec_video_config();
			}
		}
		else
		{
			st_video_index = -1;
			av_log(NULL, AV_LOG_ERROR, "No Video Stream:%s\n", file);
		}
	}

	if (audio_disable == 0)
	{
		st_audio_index = av_find_best_stream(in, AVMEDIA_TYPE_AUDIO, -1, st_video_index, NULL, 0);
		if (st_audio_index >= 0)
		{
			audio_stream = in->streams[st_audio_index];
			err = ffdec_stream_component_open(audio_stream);
			if (err < 0)
			{
				st_audio_index = -1;
				audio_stream = NULL;
				av_log(NULL, AV_LOG_ERROR, "Could not open audio codec\n");
			}
			else
			{
				audio_dec = audio_stream->codec;
				ffdec_audio_config();
			}
		}
		else
		{
			st_audio_index = -1;
			av_log(NULL, AV_LOG_ERROR, "No Audio Stream:%s\n", file);
		}
	}

	if (in->start_time != AV_NOPTS_VALUE) stream_start_time = in->start_time;
	if ((in->duration) && (in->duration != AV_NOPTS_VALUE))
	{
		stream_duration = in->duration;
	}

	return 0;
}

void CDecoder::audio_eof()
{
	for(;;)
	{
		int32_t len;
		int32_t got_frame = 0;
		av_init_packet(&pkt);
		pkt.data = NULL;
		pkt.size = 0;
		AVFrame frame;
		avcodec_get_frame_defaults(&frame);
		len = avcodec_decode_audio4(audio_dec, &frame, &got_frame, &pkt);
		if ((len == 0) && (got_frame > 0))
		{
			ffdec_audio_put_frame(&frame);
		}
		else
		{
			break;
		}
	}
}

void CDecoder::video_eof()
{
	for(;;)
	{
		int32_t len;
		AVPacket pkt;
		int32_t got_picture = 0;
		av_init_packet(&pkt);
		pkt.data = NULL;
		pkt.size = 0;
		AVFrame frame;
		avcodec_get_frame_defaults(&frame);
		len = avcodec_decode_video2(video_dec, &frame, &got_picture, &pkt);
		if ((len == 0) && (got_picture))
		{
			ffdec_video_put_frame(&frame);
		}
		else
		{
			break;
		}
	}
}

void CDecoder::ffdec_seek(int64_t amount)
{
	int64_t seek_pos = amount + stream_start_time;

	int32_t err = avformat_seek_file(in, -1, INT64_MIN, seek_pos, INT64_MAX, 0);
	if (err < 0)
	{
		err = avformat_seek_file(in, -1, INT64_MIN, seek_pos, INT64_MAX, 0);
		if (err < 0)
			av_log(NULL, AV_LOG_ERROR, "Failed(%d) to Seek Audio, amount=%lld\n", err, seek_pos);
	}
	avcodec_flush_buffers(video_dec);
	video_frame_first = 0;
	audio_frame_first = 0;
}


void CDecoder::video_next()
{

	int32_t err = av_read_frame(in, &pkt);
	if(err == AVERROR(EAGAIN))
	{
		return;
	}

	if (err < 0)
	{
		av_log(NULL, AV_LOG_ERROR, "End of Stream.\n");
		video_eof();
		_base->OnEndOfStream();
		return;
	}

	if (pkt.stream_index == st_video_index)
	{
		ffdec_decode_video();
	}
	av_free_packet(&pkt);
}

void CDecoder::audio_next()
{
	int32_t err = av_read_frame(in, &pkt);
	if(err == AVERROR(EAGAIN))
	{
		return ;
	}
	if (err < 0)
	{
		av_log(NULL, AV_LOG_ERROR, "End of Stream.\n");
		audio_eof();
		_base->OnEndOfStream();
		return ;
	}

	if ((st_video_index >= 0) && (pkt.stream_index == st_video_index))
	{
		if (_base->OnVideoPacket() == 0)
		{
			ffdec_decode_video();
		}
	}
	else if ((st_audio_index >= 0) && (pkt.stream_index == st_audio_index))
	{
		ffdec_decode_audio();
	}
	av_free_packet(&pkt);
	
}

CDecoder::CDecoder(CBaseDecoder* base, int32_t AudioDisable)
{
	_base = base;
	st_video_index = -1;
	st_audio_index = -1;
	audio_disable = AudioDisable;
}

CDecoder::~CDecoder()
{
	if (video_dec) avcodec_close(video_dec);
	if (audio_dec) avcodec_close(audio_dec);
	if (in) avformat_close_input(&in);
}

