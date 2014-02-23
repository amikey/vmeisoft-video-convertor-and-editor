#include "stdafx.h"
#include "../ffmpeg.inc.h"
#include "dec.h"
#include "../../VideoConverter.h"

static inline enum AVPixelFormat CheckFormat(enum AVPixelFormat format)
{
	switch(format)
	{
		case AV_PIX_FMT_BGR0:
			return AV_PIX_FMT_BGRA;

		default:
			return format;
	}
}

void CDecoder::ffdec_video_config()
{
	if ((video_dec->sample_aspect_ratio.num) && ((video_dec->sample_aspect_ratio.num != 1) || (video_dec->sample_aspect_ratio.den != 1)))
	{
		video_width = (video_dec->width * video_dec->sample_aspect_ratio.num / video_dec->sample_aspect_ratio.den) & (~1);
		video_height = video_dec->height;
		if (video_width < video_dec->width)
		{
			video_width = video_dec->width;
			video_height = (video_dec->height * video_dec->sample_aspect_ratio.den / video_dec->sample_aspect_ratio.num) & (~1);
		}
	}
	else
	{
		video_width = video_dec->width;
		video_height = video_dec->height;
	}

	video_codec_format = CheckFormat(video_dec->pix_fmt);
	video_codec_width = video_dec->coded_width;
	video_codec_height = video_dec->coded_height;
	video_out_time_base = video_stream->time_base;
	r_frame_rate = video_stream->r_frame_rate;
	video_bit_rate = video_dec->bit_rate;
}

int32_t CDecoder::ffdec_check_frame_info(AVFrame *frame)
{
	if (frame->best_effort_timestamp == AV_NOPTS_VALUE)
	{
		av_log(NULL, AV_LOG_ERROR, "No Video Frame Pts !\n");
		if (video_frame_first) return -1;
	}

	if (video_codec_format != CheckFormat((enum AVPixelFormat)frame->format))
	{
		av_log(NULL, AV_LOG_ERROR, "Video Format Changed !!! %d-->%d\n", video_codec_format, frame->format);
		if (video_frame_first) return -1;
		video_codec_format = CheckFormat((enum AVPixelFormat)frame->format);
	}

	if ((video_codec_width != frame->width) || (video_codec_height != frame->height))
	{
		av_log(NULL, AV_LOG_ERROR, "Video Codec Size Changed !!! %d*%d-->%d*%d\n", video_codec_width, video_codec_height, frame->width, frame->height);
		if (video_frame_first) return -1;
		video_codec_width = frame->width;
		video_codec_height = frame->height;
	}

	return 0;
}

void CDecoder::ffdec_video_put_frame(AVFrame *frame)
{
	if (ffdec_check_frame_info(frame) < 0)
	{
		return ;
	}

	uint64_t pts = frame->best_effort_timestamp * AV_TIME_BASE_LL * (uint64_t)video_out_time_base.num / (uint64_t)video_out_time_base.den - stream_start_time;
	if (video_frame_first == 0)
	{
		_base->OnVideoStreamStart(pts);
		video_frame_first = 1;
	}
	
	_base->OnVideoStream(pts, frame);
}
