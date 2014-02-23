#include "stdafx.h"
#include "../../ffmpeg.inc.h"
#include "../enc.h"
#include "CodecTypes.h"

void CEncoder::video_raw(void)
{
	frame_pix_fmt = AV_PIX_FMT_YUV420P;
	video_bit_rate = video_width * video_height * 12 * g_enc_opt.m_FrameDen / g_enc_opt.m_FrameNum;
}
