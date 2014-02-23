#include "stdafx.h"
#include "../../ffmpeg.inc.h"
#include "../enc.h"
#include "CodecTypes.h"

static char g_video_rc_eq[] = "blurCplx^(1-qComp)";

void CEncoder::video_x264(void)
{
	thread_count = 4;
	if (IS_TARGERT(IPOD))
	{
		video_coder_type = 0;
		video_rc_buffer_size = 3000000;
		video_gop_size = 250;  
		video_qscale = 2;
		video_rc_max_rate = 1500000;
		video_level = 30;
		video_sc_threshold = 40;
		video_qmax = 51;
		video_qmin = 10;
		video_qdiff = 4;
		video_i_qfactor = 0.71f;
		video_rc_eq = g_video_rc_eq;
		video_qcomp = 0.6f;	
		set_video_codec_option("qp", "30"); //video_cqp = 30;
		set_video_codec_option("8x8dct", "0"); //video_flags2_and &= ~CODEC_FLAG2_8X8DCT;
		set_video_codec_option("weightb", "0"); //video_flags2_and &= ~CODEC_FLAG2_WPRED;
		set_video_codec_option("weightp", "0"); //video_weighted_p_pred = 0;
		video_max_b_frames = 0;
		if (IS_TARGERT(IPOD_VIDEO))
		{
			video_level = 13;
		}
		else if (IS_TARGERT(IPOD_NANO))
		{
			video_level = 13;
			video_bit_rate = 100000;
			video_rc_max_rate = 150000;
		}
	}
	else if (IS_TARGERT(IPHONE))
	{
		video_coder_type = 0;
		video_rc_buffer_size = 3000000;
		video_gop_size = 250;
		video_qscale = 2;
		video_rc_max_rate = 1500000;
		video_level = 30;
		video_sc_threshold = 40;
		video_qmax = 51;
		video_qmin = 10;
		video_qdiff = 4;
		video_i_qfactor = 0.71f;
		video_rc_eq = g_video_rc_eq;
		video_qcomp = 0.6f;
		set_video_codec_option("qp", "30"); //video_cqp = 30;
		set_video_codec_option("8x8dct", "0"); //video_flags2_and &= ~CODEC_FLAG2_8X8DCT;
		set_video_codec_option("weightb", "0"); //video_flags2_and &= ~CODEC_FLAG2_WPRED;
		set_video_codec_option("weightp", "0"); //video_weighted_p_pred = 0;
		video_max_b_frames = 0;
	}
	else if (IS_TARGERT(APPLE_TV))
	{
		video_coder_type = 0;
		video_rc_buffer_size = 16777216;
		video_gop_size = 250;  
		video_qscale = 2;
		video_rc_max_rate = 1500000;
		video_level = 30;
		video_sc_threshold = 40;
		video_qmax = 51;
		video_qmin = 10;
		video_qdiff = 4;
		video_i_qfactor = 0.71f;
		video_rc_eq = g_video_rc_eq;
		video_qcomp = 0.6f;
		set_video_codec_option("qp", "30"); //video_cqp = 30;
	}
	else if (IS_TARGERT(BLACKBERRY))
	{
		video_qmin = 10;
		video_qmax = 51;
		video_qdiff = 4;
		video_qcomp = 0.6f;
		video_me_range = 16;
		set_video_codec_option("qp", "30"); //video_cqp = 30;
	}
	else if (IS_TARGERT(GPHONE))
	{
		video_coder_type = 0;
		video_rc_buffer_size = 16777216;
		video_gop_size = 250;
		video_qscale = 2;
		video_rc_max_rate = 1500000;
		video_level = 30;
		video_sc_threshold = 40;
		video_qmax = 51;
		video_qmin = 10;
		video_qdiff = 4;
		video_i_qfactor = 0.71f;
		video_rc_eq = g_video_rc_eq;
		video_qcomp = 0.6f;
		set_video_codec_option("qp", "30"); //video_cqp = 30;
	}
	else if (IS_TARGERT(PALM))
	{
		video_qmin = 10;
		video_qmax = 51;
		video_qdiff = 4;
		video_qcomp = 0.6f;
		video_me_range = 16;
		set_video_codec_option("qp", "30"); //video_cqp = 30;
	}
	else if (IS_TARGERT(PSP))
	{
		video_coder_type = 1;
		video_rc_buffer_size = 12800000;
		video_gop_size = 250;
		video_qscale = 10;
		video_rc_max_rate = 1500000;
		video_level = 21;	
		video_qmin = 10;
		video_qmax = 51;
		video_qdiff = 4;
		video_qcomp = 0.6f;
		video_me_range = 16;
		set_video_codec_option("qp", "30"); //video_cqp = 30;
	}
	else if (IS_TARGERT(SANSA))
	{
		video_qmin = 10;
		video_qmax = 51;
		video_qdiff = 4;
		video_qcomp = 0.6f;
		video_me_range = 16;
		set_video_codec_option("qp", "30"); //video_cqp = 30;
	}
	else if (IS_TARGERT(WALKMAN))
	{
		video_qmin = 10;
		video_qmax = 51;
		video_qdiff = 4;
		video_qcomp = 0.6f;
		video_me_range = 16;
		set_video_codec_option("qp", "30"); //video_cqp = 30;
	}
	else if (IS_TARGERT(ZUNE))
	{
		video_coder_type = 0;
		video_rc_buffer_size = 16777216;
		video_gop_size = 250;
		video_qscale = 2;
		video_rc_max_rate = 1500000;
		video_level = 30;
		video_sc_threshold = 40;
		video_qmax = 51;
		video_qmin = 10;
		video_qdiff = 4;
		video_i_qfactor = 0.71f;
		video_rc_eq = g_video_rc_eq;
		video_qcomp = 0.6f;
		set_video_codec_option("qp", "30"); //video_cqp = 30;
	}
	else if (IS_VIDEO_FORMAT(MOV))
	{
		video_coder_type = 0;
		video_rc_buffer_size = 16777216;
		video_gop_size = 250;
		video_qscale = 2;
		video_rc_max_rate = 1500000;
		video_sc_threshold = 40;
		video_qmax = 50;
		video_qmin = 10;
		video_qdiff = 4;
		video_i_qfactor = 0.71f;
		video_rc_eq = g_video_rc_eq;
		video_qcomp = 0.6f;
		set_video_codec_option("qp", "30"); //video_cqp = 30;
	}
	else
	{
		video_qmin = 10;
		video_qmax = 51;
		video_qdiff = 4;
		video_qcomp = 0.6f;
		video_me_range = 16;
		set_video_codec_option("qp", "30"); //video_cqp = 30;
	}
}
