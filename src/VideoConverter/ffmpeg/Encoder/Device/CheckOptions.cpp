#include "stdafx.h"
#include "../../ffmpeg.inc.h"
#include "../enc.h"
#include "CodecTypes.h"
#include "CheckOptions.h"

int32_t StringCmpA(const char *a, const char *b)
{
	while (*b)
	{
		if ((*a != *b) && (_toupper(*a) != _toupper(*b))) return 0;
		a ++; b++;
	}
	return *a == *b;
}

int32_t StringCmpB(const char *a, const char *b)
{
	while (*b)
	{
		if ((*a != *b) && (_toupper(*a) != _toupper(*b))) return 0;
		a ++; b++;
	}
	return 1;
}

void CEncoder::check_format(void)
{
	if (IS_VIDEO_FORMAT(AMV))
	{
		format_amv();
	}
	else if (IS_AUDIO_FORMAT(RA))
	{
		format_ra();
	}
	else if (IS_VIDEO_FORMAT(AVI))
	{
		format_avi();
	}
#ifndef _WIN32
	else if (IS_VIDEO_FORMAT(WMV))
	{
		format_wmv();
	}
	else if (IS_AUDIO_FORMAT(WMA))
	{
		format_wmv();
	}
#endif
}

void CEncoder::check_video(void)
{
	if (IS_VIDEO_CODEC(X264))
	{
		video_x264();
	}
	else if (IS_VIDEO_CODEC(MPEG4))
	{
		video_mpeg4();
	}
	else if (IS_VIDEO_CODEC(DV))
	{
		video_dv();
	}
	else if (IS_VIDEO_CODEC(RAW))
	{
		video_raw();
	}
	else if (IS_VIDEO_CODEC(VPX))
	{
		video_vpx();
	}
	else if (IS_VIDEO_CODEC(THEORA))
	{
		video_theora();
	}
}

void CEncoder::check_audio(void)
{
	if (IS_AUDIO_CODEC(AMR_NB))
	{
		audio_amr_nb();
	}
	else if (IS_AUDIO_CODEC(PCM_S16LE))
	{
		audio_raw();
	}
	else if (IS_AUDIO_CODEC(AC3))
	{
		audio_ac3();
	}
	else if (IS_AUDIO_CODEC(FLAC))
	{
		audio_flac();
	}
	else if (IS_AUDIO_CODEC(VORBIS))
	{
		audio_vorbis();
	}
}

void CEncoder::check_options(void)
{
	check_format();
	if (g_enc_opt.m_VideoDisable == 0)
	{
		check_video();
	}
	if (g_enc_opt.m_AudioDisable == 0)
	{
		check_audio();
	}
}
