#pragma once
#include "encoder.h"

#define ABORT_FLAG	-9
#define NO_VALUE	0x19720306

class CEncoder
{
public:
	HANDLE	m_thread;
	volatile long *m_pAbort;

public:
	struct AudioStream*	m_AStream;
	struct VideoStream*	m_VStream;
	volatile int64_t*	m_Time;
	uint8_t*			m_AudioBuffer;

public:
	AVOutputFormat *fmt;
	AVFormatContext *oc;
	AVStream *audio_st, *video_st;
	AVFrame picture_list[MAX_VIDEO_BUFFER];
	uint8_t *video_outbuf;
	int64_t video_encodeced_frames;
	int32_t video_outbuf_size;

	uint8_t *audio_outbuf;
	AVFrame audio_frame;
	int32_t audio_outbuf_size;
	int32_t audio_frame_size;
	int64_t audio_encodeced_frames;

	char target_name[64];
	char filename[1024];
	char format_name[64];
	char video_codec_name[64];
	char audio_codec_name[64];
	enum AVCodecID video_codec_id;
	enum AVCodecID audio_codec_id;
	uint32_t video_codec_tag;
	uint32_t audio_codec_tag;
	float mux_max_delay;

	enum AVPixelFormat frame_pix_fmt;
	AVRational sample_aspect_ratio;
	int32_t video_bit_rate;
	int32_t video_qscale;
	int32_t video_width, video_height;
	int32_t video_gop_size;

	enum AVSampleFormat audio_sample_fmt;
	int32_t audio_bit_rate;
	int32_t audio_sample_rate;
	int32_t audio_channels;
	uint64_t audio_channel_layout;

	int32_t packet_size;

	int32_t video_rc_max_rate;
	int32_t video_rc_min_rate;
	int32_t video_rc_buffer_size;
	int32_t video_coder_type;
	int32_t video_max_b_frames;

	int32_t video_level;
	int32_t video_sc_threshold;
	int32_t video_qmin;
	int32_t video_qmax;
	int32_t video_qdiff;
	float video_i_qfactor;
	char *video_rc_eq;
	float video_qcomp;
	int32_t video_me_range;
	int32_t thread_count;

	AVFrame *last_picture;

	AVDictionary *video_options;
	AVDictionary *audio_options;
	
public:
	CEncoder(VideoStream* vStream, struct AudioStream *aStream);
	~CEncoder();
	
	void set_video_codec_option(const char *name, const char* value);
	void set_audio_codec_option(const char *name, const char* value);
	
public:
	AVStream* add_video_stream(enum AVCodecID codec_id, char *name);
	void audio_check_samplerate(AVCodec *codec);
	void audio_check_channel_layout(AVCodec *codec);
	void audio_check_sample_fmt(AVCodec *codec);
	AVStream* add_audio_stream(enum AVCodecID codec_id, char *name);
	int32_t write_video_frame(int32_t repeat, int32_t index);
	int32_t write_audio_frame();
	void ending_video_codec();
	void ending_audio_codec();
	
public:
	int32_t enc_open(void);
	int32_t enc_loop(void);
	int32_t enc_close(void);
	
public:
	void check_format(void);
	void check_video(void);
	void check_audio(void);
	void check_options(void);
	
	void format_avi(void);
	void format_amv(void);
	void format_ra(void);
	void format_wmv(void);
	void video_x264(void);
	void video_mpeg4(void);
	void video_dv(void);
	void video_raw(void);
	void video_theora(void);
	void video_vpx(void);
	void audio_amr_nb(void);
	void audio_raw(void);
	void audio_ac3(void);
	void audio_flac(void);
	void audio_vorbis(void);
	
public:
	int32_t Initialize();
};


