#include "stdafx.h"
#include "../../ffmpeg.inc.h"
#include "../enc.h"
#include "CodecTypes.h"

void CEncoder::format_avi(void)
{
	if (g_enc_opt.m_VideoDisable == 0)
	{
		if (IS_VIDEO_CODEC(RAW))
		{
			video_codec_tag = '024I'; // I420
		}
		else if (IS_VIDEO_CODEC(MPEG4))
		{
			video_codec_tag = MKTAG('m', 'p', '4', 'v'); //
		}
		else if (IS_VIDEO_CODEC(X264))
		{
			video_codec_tag = MKTAG('X', '2', '6', '4'); //
		}
	}

	if (g_enc_opt.m_AudioDisable == 0)
	{
		if (IS_AUDIO_CODEC(PCM_S16LE))
		{
			audio_codec_tag = 1;
		}
		else if (IS_AUDIO_CODEC(MP3))
		{
			audio_codec_tag = 0x0055;
		}
		else if (IS_AUDIO_CODEC(AAC))
		{
			audio_codec_tag = 0x1600;
		}
	}

	av_log(NULL, AV_LOG_INFO, "Video Codec Tag=0x%x\n", video_codec_tag);
	av_log(NULL, AV_LOG_INFO, "Audio Codec Tag=0x%x\n", audio_codec_tag);
}
