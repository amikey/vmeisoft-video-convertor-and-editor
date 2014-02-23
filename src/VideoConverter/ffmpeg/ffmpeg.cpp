#include "stdafx.h"
#include "ffmpeg.inc.h"
#include "../../VideoConverter.h"

static int32_t ffmpeg_lockmgr_callback(void **mutex, enum AVLockOp op)
{
	if (NULL == mutex) return -1;
	switch(op)
	{
		case AV_LOCK_CREATE:
		{
			*mutex = NULL;
			break;
		}
		case AV_LOCK_OBTAIN:
		{
			while (InterlockedCompareExchange((volatile long*)mutex, 1, 0)) // != 0
			{
				Sleep(1);
			}
			break;
		}
		case AV_LOCK_RELEASE:
		{
			InterlockedExchange((volatile long*)mutex, 0);
			break;
		}
		case AV_LOCK_DESTROY:
		{
			*mutex = NULL;
			break;
		}
	}
	return 0;
}

int32_t ffmpeg_init()
{
	extern void gif_register_all();
	extern void wmv_register_all();

	avcodec_register_all();
	av_register_all();
	gif_register_all();
	wmv_register_all();
	av_lockmgr_register(ffmpeg_lockmgr_callback);

	return 0;
}

const char* ffmeg_get_pix_fmt_name(MediaInfo *info)
{
	return av_get_pix_fmt_name((enum AVPixelFormat)info->m_PixelFormat);
}

const char* ffmeg_get_sample_fmt_name(MediaInfo *info)
{
	return av_get_sample_fmt_name((enum AVSampleFormat)info->m_SampleFormat);
}

extern "C"
{
	HANDLE logfile = NULL;
	int32_t ff_rtp_chain_mux_open()
	{
		return -1;
	}
}