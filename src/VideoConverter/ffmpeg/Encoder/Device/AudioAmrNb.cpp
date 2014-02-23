#include "stdafx.h"
#include "../../ffmpeg.inc.h"
#include "../enc.h"
#include "CodecTypes.h"

void CEncoder::audio_amr_nb(void)
{
	audio_sample_rate = 8000;
	audio_channels = 1;
}
