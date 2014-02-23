#pragma once
#include "../ffmpeg.inc.h"
#include "../../VideoConverter.h"

class CBaseDecoder
{
public:
	virtual void OnAudioStreamStart(int64_t pts) = 0;
	virtual void OnAudioStream(AVFrame *frame) = 0;
	virtual void OnVideoStreamStart(int64_t pts) = 0;
	virtual void OnVideoStream(int64_t pts, AVFrame *frame) = 0;
	virtual int32_t OnVideoPacket() = 0;
	virtual void OnEndOfStream() = 0;
};

class CDecoder
{
public:
	CBaseDecoder*		_base;

public:

	AVFormatContext*	in;
	AVStream*			video_stream;
	AVStream*			audio_stream;
	AVCodecContext*		video_dec;
	AVCodecContext*		audio_dec;

	AVPacket	pkt;

	int32_t					audio_disable;

	int32_t st_video_index;
	int32_t st_audio_index;
	
	int64_t stream_start_time;
	int64_t stream_duration;

public:
	int32_t ffdec_stream_component_open(AVStream *stream);
	void ffdec_decode_audio();
	void ffdec_decode_video();
	void ffdec_seek(int64_t amount);
	int32_t ffdec_open(wchar_t *wszFile);
	void audio_eof();
	void video_eof();
	void video_next();
	void audio_next();
		
public:
	enum AVPixelFormat	video_codec_format;
	int32_t					video_codec_width, video_codec_height;
	int32_t					video_width, video_height;
	AVRational			video_out_time_base;
	AVRational			r_frame_rate;
	int32_t					video_bit_rate;
	
	int32_t video_frame_first;
	
public:
	void ffdec_video_config();
	int32_t ffdec_check_frame_info(AVFrame *frame);
	void ffdec_video_put_frame(AVFrame *frame);
	
public:
	enum AVSampleFormat	audio_format;
	AVRational			audio_out_time_base;
	int32_t					audio_channels;
	int32_t 				audio_sample_rate;
	int64_t 			audio_channel_layout;
	int32_t					audio_bit_rate;

	int32_t					audio_frame_first;
	
public:
	void ffdec_audio_config();
	void ffdec_audio_put_frame(AVFrame *frame);

public:
	CDecoder(CBaseDecoder* base, int32_t AudioDisable);
	virtual ~CDecoder();
};

