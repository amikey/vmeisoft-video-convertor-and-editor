#include "stdafx.h"
#include "../../ffmpeg.inc.h"
#include "../enc.h"
#include "CodecTypes.h"

void CEncoder::format_ra(void)
{
	strcpy(format_name, FORMAT_VIDEO_AVI);
	strcpy(audio_codec_name, AUDIO_CODEC_MP3);
    audio_bit_rate = 128000;
    audio_channels = 2;
    audio_sample_rate = 44100;
}
