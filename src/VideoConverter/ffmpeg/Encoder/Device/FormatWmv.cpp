#include "stdafx.h"
#include "../../ffmpeg.inc.h"
#include "../enc.h"
#include "CodecTypes.h"

void CEncoder::format_wmv(void)
{
#ifndef _WIN32

	if (strcmp(video_codec_name, "wmv7") == 0)
	{
		strcpy(video_codec_name, "wmv1");
	}
	else if (strcmp(video_codec_name, "wmv8") == 0)
	{
		strcpy(video_codec_name, "wmv2");
	}
	else if (strcmp(video_codec_name, "wmv9") == 0)
	{
		strcpy(video_codec_name, "msmpeg4");
	}
	else if (strcmp(video_codec_name, "wvc1") == 0)
	{
		strcpy(video_codec_name, "msmpeg4");
		video_codec_id = AV_CODEC_ID_MSMPEG4V3;
	}

	if (strcmp(audio_codec_name, "wmapro") == 0)
	{
		strcpy(audio_codec_name, "wmav2");
	}

	strcpy(format_name, "asf");

#endif
}
