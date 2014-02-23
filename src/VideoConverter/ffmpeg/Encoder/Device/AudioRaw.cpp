#include "stdafx.h"
#include "../../ffmpeg.inc.h"
#include "../enc.h"
#include "CodecTypes.h"

void CEncoder::audio_raw(void)
{
	audio_bit_rate = audio_sample_rate * audio_channels * 16;
}
