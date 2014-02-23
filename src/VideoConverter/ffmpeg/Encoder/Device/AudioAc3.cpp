#include "stdafx.h"
#include "../../ffmpeg.inc.h"
#include "../enc.h"
#include "CodecTypes.h"

void CEncoder::audio_ac3(void)
{
	strcpy(audio_codec_name, "ac3_fixed");
	audio_channels = 2;
}
