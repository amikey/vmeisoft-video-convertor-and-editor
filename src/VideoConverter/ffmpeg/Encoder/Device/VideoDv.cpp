#include "stdafx.h"
#include "../../ffmpeg.inc.h"
#include "../enc.h"
#include "CodecTypes.h"


void CEncoder::video_dv(void)
{
	if (IS_TARGERT(DV_PAL_HD))
	{
		frame_pix_fmt = AV_PIX_FMT_YUV422P;
		video_width = 1440;
		video_height = 1080;
		sample_aspect_ratio.den = 16;
		sample_aspect_ratio.den = 9;
	}
	else if (IS_TARGERT(DV_NTSC_HD))
	{
		frame_pix_fmt = AV_PIX_FMT_YUV422P;
		video_width = 1280;
		video_height = 1080;
		sample_aspect_ratio.den = 16;
		sample_aspect_ratio.den = 9;
	}
	else if (IS_TARGERT(DV_PAL))
	{
		frame_pix_fmt = AV_PIX_FMT_YUV420P;
		video_width = 720;
		video_height = 576;
		if (video_width == 768)
		{
			sample_aspect_ratio.den = 4;
			sample_aspect_ratio.den = 3;
		}
		else
		{
			sample_aspect_ratio.den = 16;
			sample_aspect_ratio.den = 9;
		}
	}
	else if (IS_TARGERT(DV_NTSC))
	{
		frame_pix_fmt = AV_PIX_FMT_YUV411P;
		video_width = 720;
		video_height = 480;
		if (video_width == 640)
		{
			sample_aspect_ratio.den = 4;
			sample_aspect_ratio.den = 3;
		}
		else
		{
			sample_aspect_ratio.den = 16;
			sample_aspect_ratio.den = 9;
		}
	}
	else
	{
		av_log(NULL, AV_LOG_ERROR, "Unknow Target:%hs\n", g_enc_opt.m_Target);
	}

	audio_sample_rate = 48000;
	audio_channels = 2;
}
