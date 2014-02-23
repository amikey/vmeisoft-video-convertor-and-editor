#include "stdafx.h"
#include "../../ffmpeg.inc.h"
#include "../enc.h"
#include "CodecTypes.h"

void CEncoder::video_mpeg4(void)
{
	if (IS_TARGERT(IPOD))
	{
		video_rc_max_rate = 2500000;
		video_rc_buffer_size = 3000000;
		video_qmin = 3;
		video_qmax = 5;
	}
	else if (IS_TARGERT(IPHONE))
	{
		video_rc_max_rate = 2500000;
		video_rc_buffer_size = 3000000;
		video_qmin = 3;
		video_qmax = 5;
	}
	else if (IS_TARGERT(APPLE_TV))
	{
		video_rc_max_rate = 2500000;
		video_rc_buffer_size = 3000000;
		video_qmin = 3;
		video_qmax = 5;
	}
}
