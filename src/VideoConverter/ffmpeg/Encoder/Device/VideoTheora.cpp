#include "stdafx.h"
#include "../../ffmpeg.inc.h"
#include "../enc.h"
#include "CodecTypes.h"

void CEncoder::video_theora(void)
{
	if (video_bit_rate == 0) 
		video_qscale = 5;
}
