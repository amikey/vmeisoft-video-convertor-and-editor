#include "stdafx.h"
#include "../../ffmpeg.inc.h"
#include "../enc.h"
#include "CodecTypes.h"

void CEncoder::format_amv(void)
{
	strcpy(video_codec_name, VIDEO_CODEC_AMV);
	video_bit_rate = 0;
	frame_pix_fmt = AV_PIX_FMT_YUVJ420P;
	strcpy(audio_codec_name, AUDIO_CODEC_AMV);
	audio_channels = 1;
	audio_sample_rate = 22050;
	audio_bit_rate = 0;
	video_qmin = 3;
	video_qmax = 3;
}
