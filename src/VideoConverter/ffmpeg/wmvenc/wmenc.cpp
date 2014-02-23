#include "StdAfx.h"
#include "../ffmpeg.inc.h"
#include "wmenc.h"
#include "WMEncoder.h"

static const enum AVPixelFormat wm_pixel_formats[] = {AV_PIX_FMT_BGRA, AV_PIX_FMT_BGR24, AV_PIX_FMT_YUV420P, AV_PIX_FMT_NONE};

static AVCodec ff_wmv7_encoder = 
{
	/*name*/						"wmv7",
	/*long_name*/					"Windows Media Video V7",
	/*type*/						AVMEDIA_TYPE_VIDEO,
	/*id*/							AV_CODEC_ID_WMV1,
	/*capabilities*/				CODEC_CAP_DR1,
	/*supported_framerates*/		NULL,
	/*pix_fmts*/					wm_pixel_formats,
	/*supported_samplerates*/		NULL,
	/*sample_fmts*/					NULL,					
	/*channel_layouts*/				NULL,
	/*max_lowres*/					NULL,
	/*priv_class*/					NULL,
	/*profiles*/					NULL,
	/*priv_data_size*/				NULL,
	/*next*/						NULL,
	/*init_thread_copy*/			NULL,
	/*update_thread_context*/		NULL,
	/*defaults*/					NULL,
	/*init_static_data*/			NULL,
	/*init*/						wmv_encode_init,
	/*encode*/						NULL,
	/*encode2*/						wmv_encode_frame,
	/*decode*/						NULL,
	/*close*/						wmv_encode_close,
	/*flush*/						NULL
};

static AVCodec ff_wmv8_encoder = 
{
	/*name*/						"wmv8",
	/*long_name*/					"Windows Media Video V8",
	/*type*/						AVMEDIA_TYPE_VIDEO,
	/*id*/							AV_CODEC_ID_WMV2,
	/*capabilities*/				CODEC_CAP_DR1,
	/*supported_framerates*/		NULL,
	/*pix_fmts*/					wm_pixel_formats,
	/*supported_samplerates*/		NULL,
	/*sample_fmts*/					NULL,					
	/*channel_layouts*/				NULL,
	/*max_lowres*/					NULL,
	/*priv_class*/					NULL,
	/*profiles*/					NULL,
	/*priv_data_size*/				NULL,
	/*next*/						NULL,
	/*init_thread_copy*/			NULL,
	/*update_thread_context*/		NULL,
	/*defaults*/					NULL,
	/*init_static_data*/			NULL,
	/*init*/						wmv_encode_init,
	/*encode*/						NULL,
	/*encode2*/						wmv_encode_frame,
	/*decode*/						NULL,
	/*close*/						wmv_encode_close,
	/*flush*/						NULL
};

static AVCodec ff_wmv9_encoder = 
{
	/*name*/						"wmv9",
	/*long_name*/					"Windows Media Video 9",
	/*type*/						AVMEDIA_TYPE_VIDEO,
	/*id*/							AV_CODEC_ID_WMV3,
	/*capabilities*/				CODEC_CAP_DR1,
	/*supported_framerates*/		NULL,
	/*pix_fmts*/					wm_pixel_formats,
	/*supported_samplerates*/		NULL,
	/*sample_fmts*/					NULL,					
	/*channel_layouts*/				NULL,
	/*max_lowres*/					NULL,
	/*priv_class*/					NULL,
	/*profiles*/					NULL,
	/*priv_data_size*/				NULL,
	/*next*/						NULL,
	/*init_thread_copy*/			NULL,
	/*update_thread_context*/		NULL,
	/*defaults*/					NULL,
	/*init_static_data*/			NULL,
	/*init*/						wmv_encode_init,
	/*encode*/						NULL,
	/*encode2*/						wmv_encode_frame,
	/*decode*/						NULL,
	/*close*/						wmv_encode_close,
	/*flush*/						NULL
};

static AVCodec ff_wvc1_encoder = 
{
	/*name*/						"wvc1",
	/*long_name*/					"Windows Media Video 9 Advanced Profile",
	/*type*/						AVMEDIA_TYPE_VIDEO,
	/*id*/							AV_CODEC_ID_VC1,
	/*capabilities*/				CODEC_CAP_DR1,
	/*supported_framerates*/		NULL,
	/*pix_fmts*/					wm_pixel_formats,
	/*supported_samplerates*/		NULL,
	/*sample_fmts*/					NULL,					
	/*channel_layouts*/				NULL,
	/*max_lowres*/					NULL,
	/*priv_class*/					NULL,
	/*profiles*/					NULL,
	/*priv_data_size*/				NULL,
	/*next*/						NULL,
	/*init_thread_copy*/			NULL,
	/*update_thread_context*/		NULL,
	/*defaults*/					NULL,
	/*init_static_data*/			NULL,
	/*init*/						wmv_encode_init,
	/*encode*/						NULL,
	/*encode2*/						wmv_encode_frame,
	/*decode*/						NULL,
	/*close*/						wmv_encode_close,
	/*flush*/						NULL
};

static const enum AVSampleFormat wm_sample_fmts[] = { AV_SAMPLE_FMT_S16, AV_SAMPLE_FMT_NONE };

static AVCodec ff_wmav1_encoder = 
{
	/*name*/						"wmav1",
	/*long_name*/					"Windows Media Audio 1",
	/*type*/						AVMEDIA_TYPE_AUDIO,
	/*id*/							AV_CODEC_ID_WMAV1,
	/*capabilities*/				CODEC_CAP_DR1,
	/*supported_framerates*/		NULL,
	/*pix_fmts*/					NULL,
	/*supported_samplerates*/		NULL,
	/*sample_fmts*/					wm_sample_fmts,					
	/*channel_layouts*/				NULL,
	/*max_lowres*/					NULL,
	/*priv_class*/					NULL,
	/*profiles*/					NULL,
	/*priv_data_size*/				NULL,
	/*next*/						NULL,
	/*init_thread_copy*/			NULL,
	/*update_thread_context*/		NULL,
	/*defaults*/					NULL,
	/*init_static_data*/			NULL,
	/*init*/						wma_encode_init,
	/*encode*/						NULL,
	/*encode2*/						wma_encode_frame,
	/*decode*/						NULL,
	/*close*/						wma_encode_close,
	/*flush*/						NULL
};

static AVCodec ff_wmav2_encoder = 
{
	/*name*/						"wmav2",
	/*long_name*/					"Windows Media Audio 9.2",
	/*type*/						AVMEDIA_TYPE_AUDIO,
	/*id*/							AV_CODEC_ID_WMAV2,
	/*capabilities*/				CODEC_CAP_DR1,
	/*supported_framerates*/		NULL,
	/*pix_fmts*/					NULL,
	/*supported_samplerates*/		NULL,
	/*sample_fmts*/					wm_sample_fmts,					
	/*channel_layouts*/				NULL,
	/*max_lowres*/					NULL,
	/*priv_class*/					NULL,
	/*profiles*/					NULL,
	/*priv_data_size*/				NULL,
	/*next*/						NULL,
	/*init_thread_copy*/			NULL,
	/*update_thread_context*/		NULL,
	/*defaults*/					NULL,
	/*init_static_data*/			NULL,
	/*init*/						wma_encode_init,
	/*encode*/						NULL,
	/*encode2*/						wma_encode_frame,
	/*decode*/						NULL,
	/*close*/						wma_encode_close,
	/*flush*/						NULL
};

static AVCodec ff_wmapro_encoder = 
{
	/*name*/						"wmapro",
	/*long_name*/					"Windows Media Audio 10 Professional",
	/*type*/						AVMEDIA_TYPE_AUDIO,
	/*id*/							AV_CODEC_ID_WMAPRO,
	/*capabilities*/				CODEC_CAP_DR1,
	/*supported_framerates*/		NULL,
	/*pix_fmts*/					NULL,
	/*supported_samplerates*/		NULL,
	/*sample_fmts*/					wm_sample_fmts,					
	/*channel_layouts*/				NULL,
	/*max_lowres*/					NULL,
	/*priv_class*/					NULL,
	/*profiles*/					NULL,
	/*priv_data_size*/				NULL,
	/*next*/						NULL,
	/*init_thread_copy*/			NULL,
	/*update_thread_context*/		NULL,
	/*defaults*/					NULL,
	/*init_static_data*/			NULL,
	/*init*/						wma_encode_init,
	/*encode*/						NULL,
	/*encode2*/						wma_encode_frame,
	/*decode*/						NULL,
	/*close*/						wma_encode_close,
	/*flush*/						NULL
};

static AVCodec ff_wma_Lossless_encoder = 
{
	/*name*/						"wmalossless",
	/*long_name*/					"Windows Media Audio 9.2 Lossless",
	/*type*/						AVMEDIA_TYPE_AUDIO,
	/*id*/							AV_CODEC_ID_WMALOSSLESS,
	/*capabilities*/				CODEC_CAP_DR1,
	/*supported_framerates*/		NULL,
	/*pix_fmts*/					NULL,
	/*supported_samplerates*/		NULL,
	/*sample_fmts*/					wm_sample_fmts,					
	/*channel_layouts*/				NULL,
	/*max_lowres*/					NULL,
	/*priv_class*/					NULL,
	/*profiles*/					NULL,
	/*priv_data_size*/				NULL,
	/*next*/						NULL,
	/*init_thread_copy*/			NULL,
	/*update_thread_context*/		NULL,
	/*defaults*/					NULL,
	/*init_static_data*/			NULL,
	/*init*/						wma_encode_init,
	/*encode*/						NULL,
	/*encode2*/						wma_encode_frame,
	/*decode*/						NULL,
	/*close*/						wma_encode_close,
	/*flush*/						NULL
};

static const AVCodecTag wm_codec_tag_list[] = 
{
	{ AV_CODEC_ID_WMV1,	MKTAG('W', 'M', 'V', '1') },
	{ AV_CODEC_ID_WMV2,	MKTAG('W', 'M', 'V', '2') },
	{ AV_CODEC_ID_WMV3,	MKTAG('W', 'M', 'V', '3') },
	{ AV_CODEC_ID_VC1,		MKTAG('W', 'V', 'C', '1') },
    { AV_CODEC_ID_WMAV1,   0x0161 },
    { AV_CODEC_ID_WMAV2,   0x0161 },
    { AV_CODEC_ID_WMAPRO,  0x0162 },
	{ AV_CODEC_ID_WMALOSSLESS, 0x0163 },
	{ AV_CODEC_ID_NONE,	0 },
};

static const AVCodecTag* const wm_codec_tag[] = 
{
	wm_codec_tag_list,
	NULL,
};

static AVOutputFormat ff_asf_muxer = 
{
    /*name*/				"wmv",
    /*long_name*/			"",
    /*mime_type*/			"",
    /*extensions*/			"asf,wmv,wma",
    /*audio_codec*/			AV_CODEC_ID_WMAPRO,
    /*video_codec*/			AV_CODEC_ID_WMV3,
    /*subtitle_codec*/		AV_CODEC_ID_NONE,
	/*flags*/				AVFMT_NOFILE | AVFMT_NOTIMESTAMPS | AVFMT_NODIMENSIONS,
    /*codec_tag*/			wm_codec_tag,
	/*priv_class*/			NULL,
    /*next*/				NULL,
    /*priv_data_size*/		sizeof(WVPrivate),
    /*write_header*/		wm_write_header,
    /*write_packet*/		wm_write_packet,
    /*write_trailer*/		wm_write_trailer,
    /*interleave_packet*/	wm_interleave_packet,
    /*query_codec*/			NULL,
	/*get_output_timestamp*/NULL
};

void wmv_register_all()
{
	avcodec_register(&ff_wmv7_encoder);
	avcodec_register(&ff_wmv8_encoder);
	avcodec_register(&ff_wmv9_encoder);
	avcodec_register(&ff_wvc1_encoder);
	avcodec_register(&ff_wmav1_encoder);
	avcodec_register(&ff_wmav2_encoder);
	avcodec_register(&ff_wmapro_encoder);
	avcodec_register(&ff_wma_Lossless_encoder);
	av_register_output_format(&ff_asf_muxer);
}
