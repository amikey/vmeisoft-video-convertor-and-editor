#include "stdafx.h"
#include "../ffmpeg.inc.h"
#include "gif_lib.h"

struct GifPrivate
{
	GifFileType*	new_gif;
	ColorMapObject*	reduce_cmap;
	uint8_t*		reduce_data;
	int32_t				width, height;
	int32_t				delay;
	uint8_t*		r;
	uint8_t*		g; 
	uint8_t*		b;
};

static int32_t gif_reduce(uint8_t *src, uint8_t *dst, GifColorType *colors, int32_t width, int32_t height, uint8_t *r, uint8_t *g, uint8_t *b)
{
	uint8_t *r1 = r, *g1 = g, *b1 = b;
	for (int32_t i = 0; i < width * height; i++)
	{
		*b++ = src[0];
		*g++ = src[1];
		*r++ = src[2];
		src += 4;
	}

	int32_t size = 256;
	return GifQuantizeBuffer(width, height, &size, r1, g1, b1, dst, colors);
}

static int32_t gif_encode_init(AVCodecContext *avctx)
{
	return 0;
}

static int32_t gif_encode_frame(AVCodecContext *avctx, AVPacket *pkt, const AVFrame *pict, int32_t *got_packet)
{
	GifPrivate *data = (GifPrivate *)avctx->priv_data;

	uint8_t *slice_data = pict->data[0];
	gif_reduce(slice_data, data->reduce_data, data->reduce_cmap->Colors, data->width, data->height, data->r, data->g, data->b);

	// set up the delay control block
	char CB[4];
	CB[0] = (char)(data->delay >> 8);
	CB[1] = (char)(data->delay & 0xff);
	CB[2] = 0;
	CB[3] = 0;

	// put the control block with delay info
	EGifPutExtension(data->new_gif, 0xF9, 0x04, CB);
	// put the image description
	EGifPutImageDesc(data->new_gif, 0, 0, data->width, data->height, 0, data->reduce_cmap);
	// put the image itself
	EGifPutLine(data->new_gif, data->reduce_data, data->width * data->height);

	*got_packet = 0;
	return 0;
}

static int32_t gif_encode_close(AVCodecContext *avctx)
{
	return 0;
}

static int32_t gif_write_header(AVFormatContext *s)
{
	GifPrivate *data = (GifPrivate *)s->streams[0]->codec->priv_data;
	
	int32_t Error;
	data->width = s->streams[0]->codec->width;
	data->height = s->streams[0]->codec->height;
	data->delay = 100 * s->streams[0]->codec->time_base.num / s->streams[0]->codec->time_base.den;

	int32_t size = data->width * data->height;
	data->r = (uint8_t *)av_malloc(size);
	data->g = (uint8_t *)av_malloc(size);
	data->b = (uint8_t *)av_malloc(size);
	data->reduce_data = (uint8_t *)av_malloc(size);
	data->reduce_cmap = GifMakeMapObject(256, NULL);
	data->new_gif = EGifOpenFileName(s->filename, 0, &Error);
	EGifSetGifVersion(data->new_gif, true);
	EGifPutScreenDesc(data->new_gif, data->width, data->height, 256, 0, data->reduce_cmap);
	return 0;
}

static int32_t gif_interleave_packet(struct AVFormatContext *avctx, AVPacket *out, AVPacket *in, int32_t flush)
{
	return 0;
}

static int32_t gif_write_packet(AVFormatContext *s, AVPacket *pkt)
{
	return 0;
}

static int32_t gif_write_trailer(AVFormatContext *s)
{
	GifPrivate *data = (GifPrivate *)s->streams[0]->codec->priv_data;

	if (data->new_gif)
	{
		EGifCloseFile(data->new_gif);
		data->new_gif = NULL;
	}
	if (data->reduce_cmap != NULL)
	{
		GifFreeMapObject(data->reduce_cmap);
		data->reduce_cmap = NULL;
	}
	if (data->reduce_data)
	{
		av_free(data->reduce_data);
		data->reduce_data = NULL;
	}
	if (data->r)
	{
		av_free(data->r);
		data->r = NULL;
	}
	if (data->g)
	{
		av_free(data->g);
		data->g = NULL;
	}
	if (data->b)
	{
		av_free(data->b);
		data->b = NULL;
	}

	return 0;
}

static const enum AVPixelFormat gif_pixel_formats[] = {AV_PIX_FMT_BGRA, AV_PIX_FMT_NONE};

static AVCodec ff_libgif_encoder = 
{
	/*name*/						"libgif",
	/*long_name*/					NULL_IF_CONFIG_SMALL("GIF (Graphics Interchange Format)"),
	/*type*/						AVMEDIA_TYPE_VIDEO,
	/*id*/							AV_CODEC_ID_GIF,
	/*capabilities*/				CODEC_CAP_DR1,
	/*supported_framerates*/		NULL,
	/*pix_fmts*/					gif_pixel_formats,
	/*supported_samplerates*/		NULL,
	/*sample_fmts*/					NULL,					
	/*channel_layouts*/				NULL,
	/*max_lowres*/					NULL,
	/*priv_class*/					NULL,
	/*profiles*/					NULL,
	/*priv_data_size*/				sizeof(GifPrivate),
	/*next*/						NULL,
	/*init_thread_copy*/			NULL,
	/*update_thread_context*/		NULL,
	/*defaults*/					NULL,
	/*init_static_data*/			NULL,
	/*init*/						gif_encode_init,
	/*encode*/						NULL,
	/*encode2*/						gif_encode_frame,
	/*decode*/						NULL,
	/*close*/						gif_encode_close,
	/*flush*/						NULL
};

static AVOutputFormat ff_libgif_muxer =
{
    /*name*/						"libgif",
    /*long_name*/					NULL_IF_CONFIG_SMALL("GIF Animation"),
    /*mime_type*/					"image/gif",
    /*extensions*/					"gif",
    /*audio_codec*/					AV_CODEC_ID_NONE,
    /*video_codec*/					AV_CODEC_ID_GIF,
    /*subtitle_codec*/				AV_CODEC_ID_NONE,
	/*flags*/						AVFMT_NOFILE | AVFMT_NOTIMESTAMPS | AVFMT_NODIMENSIONS,
    /*codec_tag*/					0,
	/*priv_class*/					NULL,
    /*next*/						NULL,
    /*priv_data_size*/				0,
    /*write_header*/				gif_write_header,
    /*write_packet*/				gif_write_packet,
    /*write_trailer*/				gif_write_trailer,
    /*interleave_packet*/			gif_interleave_packet,
    /*query_codec*/					NULL,
	/*get_output_timestamp*/		NULL
};


void gif_register_all()
{
	avcodec_register(&ff_libgif_encoder);
	av_register_output_format(&ff_libgif_muxer);
}
