#include "stdafx.h"
#include "../ffmpeg.inc.h"
#include "dec.h"
#include "../../VideoConverter.h"

void CDecoder::ffdec_audio_config()
{
	audio_sample_rate = audio_dec->sample_rate;
	audio_format = audio_dec->sample_fmt;
	audio_channels = audio_dec->channels;
	audio_channel_layout = audio_dec->channel_layout;
	if (audio_channel_layout == 0) audio_channel_layout = av_get_default_channel_layout(audio_dec->channels);
	audio_out_time_base = audio_stream->time_base;
	audio_bit_rate = audio_dec->bit_rate;
}

void CDecoder::ffdec_audio_put_frame(AVFrame *frame)
{
	if (audio_frame_first == 0)
	{
		if (frame->best_effort_timestamp != AV_NOPTS_VALUE)
		{
			int64_t pts = frame->best_effort_timestamp * AV_TIME_BASE_LL * (uint64_t)audio_out_time_base.num / (uint64_t)audio_out_time_base.den - stream_start_time;
			_base->OnAudioStreamStart(pts);
		}
		audio_frame_first = 1;
	}

	_base->OnAudioStream(frame);
	
}

