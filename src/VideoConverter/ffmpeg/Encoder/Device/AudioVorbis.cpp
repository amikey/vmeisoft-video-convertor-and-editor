#include "stdafx.h"
#include "../../ffmpeg.inc.h"
#include "../enc.h"
#include "CodecTypes.h"

void CEncoder::audio_vorbis(void)
{
	audio_sample_fmt = AV_SAMPLE_FMT_FLTP;
}
