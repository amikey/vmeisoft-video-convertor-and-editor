#include "stdafx.h"
#include "VideoConverter.h"
#include "AudioUtils.h"
#include "./ffmpeg/ffmpeg.inc.h"

static __forceinline uint8_t ToUint8(int16_t v)
{
	if (v > 0x80) return 0xFF;
	if (v < -0x80) return 0;
	return v + 0x80;
}

static __forceinline int16_t ToInt16(int32_t v)
{
	if (v > 32767) return 32767;
	if (v < -32768) return -32768;
	return v;
}

static __forceinline int32_t ToInt32(int64_t v)
{
	if (v > 0x7fffffff) return 0x7fffffff;
	if (v < -0x7fffffff) return 0x80000000;
	return (int32_t)v;
}

static __forceinline float ToFloat(float v)
{
	if (v > 1.0f) return 1.0f;
	if (v < -1.0f) return -1.0f;
	return v;
}

static __forceinline float ToDouble(double v)
{
	if (v > 1.0) return 1.0;
	if (v < -1.0) return -1.0;
	return v;
}

void audio_procedure_volume1_8_v(uint8_t *dst, uint8_t *src, short volume, int samples)
{
	for(int i = 0; i < samples; i++)
	{
		dst[i] = ToUint8(
			((int16_t)src[i] - 0x80) * volume 
			/ 100);
	}
}

void audio_procedure_volume2_8_v(uint8_t *dst, uint8_t *src1, short volume1, uint8_t *src2, short volume2, int samples)
{
	for(int i = 0; i < samples; i++)
	{
		dst[i] = ToUint8((
			((int16_t)src1[i] - 0x80) * volume1 + 
			((int16_t)src2[i] - 0x80) * volume2
			) / 200);
	}
}

void audio_procedure_volume3_8_v(uint8_t *dst, uint8_t *src1, short volume1, uint8_t *src2, short volume2, uint8_t *src3, short volume3, int samples)
{
	for(int i = 0; i < samples; i++)
	{
		dst[i] = ToUint8((
			((int16_t)src1[i] - 0x80) * volume1 + 
			((int16_t)src2[i] - 0x80) * volume2 +
			((int16_t)src3[i] - 0x80) * volume3
			) / 300);
	}
}

void audio_procedure_volume2_8(uint8_t *dst, uint8_t *src1, uint8_t *src2, int samples)
{
	for(int i = 0; i < samples; i++)
	{
		dst[i] = ToUint8((
			((int16_t)src1[i] - 0x80) + 
			((int16_t)src2[i] - 0x80) 
			) / 2);
	}
}

void audio_procedure_volume3_8(uint8_t *dst, uint8_t *src1, uint8_t *src2, uint8_t *src3, int samples)
{
	for(int i = 0; i < samples; i++)
	{
		dst[i] = ToUint8((
			((int16_t)src1[i] - 0x80) + 
			((int16_t)src2[i] - 0x80) +
			((int16_t)src3[i] - 0x80) 
			) / 3);
	}
}

void audio_procedure_volume1_16_v(int16_t *dst, int16_t *src,  int32_t volume, int samples)
{
	for(int i = 0; i < samples; i++)
	{
		dst[i] = ToInt16((int32_t)src[i] * volume / 100);
	}
}

void audio_procedure_volume2_16_v(int16_t *dst, int16_t *src1, int32_t volume1, int16_t *src2, int32_t volume2, int samples)
{
	for(int i = 0; i < samples; i++)
	{
		dst[i] = ToInt16((
			(int32_t)src1[i] * volume1 + 
			(int32_t)src2[i] * volume2
			) / 200);
	}
}

void audio_procedure_volume3_16_v(int16_t *dst, int16_t *src1, int32_t volume1, int16_t *src2, int32_t volume2, int16_t *src3, int32_t volume3, int samples)
{
	for(int i = 0; i < samples; i++)
	{
		dst[i] = ToInt16((
			(int32_t)src1[i] * volume1 + 
			(int32_t)src2[i] * volume2 +
			(int32_t)src3[i] * volume3
			) / 300);
	}
}

void audio_procedure_volume2_16(int16_t *dst, int16_t *src1, int16_t *src2, int samples)
{
	for(int i = 0; i < samples; i++)
	{
		dst[i] = ToInt16((
			(int32_t)src1[i]  + 
			(int32_t)src2[i] 
			) / 2);
	}
}

void audio_procedure_volume3_16(int16_t *dst, int16_t *src1, int16_t *src2, int16_t *src3, int samples)
{
	for(int i = 0; i < samples; i++)
	{
		dst[i] = ToInt16((
			(int32_t)src1[i] + 
			(int32_t)src2[i] +
			(int32_t)src3[i] 
			) / 3);
	}
}

void audio_procedure_volume1_flt_v(float *dst, float *src,  float volume, int samples)
{
	for(int i = 0; i < samples; i++)
	{
		dst[i] = ToFloat(src[i] * volume / 100.0f);
	}
}

void audio_procedure_volume2_flt_v(float *dst, float *src1, float volume1, float *src2, float volume2, int samples)
{
	for(int i = 0; i < samples; i++)
	{
		dst[i] = ToFloat((
			src1[i] * volume1 + 
			src2[i] * volume2
			) / 200.0f);
	}
}

void audio_procedure_volume3_flt_v(float *dst, float *src1, float volume1, float *src2, float volume2, float *src3, float volume3, int samples)
{
	for(int i = 0; i < samples; i++)
	{
		dst[i] = ToFloat((
			src1[i] * volume1 + 
			src2[i] * volume2 +
			src3[i] * volume3
			) / 300.0f);
	}
}

void audio_procedure_volume2_flt(float *dst, float *src1, float *src2, int samples)
{
	for(int i = 0; i < samples; i++)
	{
		dst[i] = ToFloat((
			src1[i] + 
			src2[i]
			) / 2.0f);
	}
}

void audio_procedure_volume3_flt(float *dst, float *src1, float *src2, float *src3, int samples)
{
	for(int i = 0; i < samples; i++)
	{
		dst[i] = ToFloat((
			src1[i] + 
			src2[i] +
			src3[i] 
			) / 3.0f);
	}
}

void audio_procedure_volume1_32_v(int32_t *dst, int32_t *src,  int64_t volume, int samples)
{
	for(int i = 0; i < samples; i++)
	{
		dst[i] = ToInt32((int64_t)src[i] * volume / 100ll);
	}
}

void audio_procedure_volume2_32_v(int32_t *dst, int32_t *src1, int64_t volume1, int32_t *src2, int64_t volume2, int samples)
{
	for(int i = 0; i < samples; i++)
	{
		dst[i] = ToInt32((
			(int64_t)src1[i] * volume1 + 
			(int64_t)src2[i] * volume2
			) / 200ll);
	}
}

void audio_procedure_volume3_32_v(int32_t *dst, int32_t *src1, int64_t volume1, int32_t *src2, int64_t volume2, int32_t *src3, int64_t volume3, int samples)
{
	for(int i = 0; i < samples; i++)
	{
		dst[i] = ToInt32((
			(int64_t)src1[i] * volume1 + 
			(int64_t)src2[i] * volume2 +
			(int64_t)src3[i] * volume3
			) / 300ll);
	}
}

void audio_procedure_volume2_32(int32_t *dst, int32_t *src1, int32_t *src2, int samples)
{
	for(int i = 0; i < samples; i++)
	{
		dst[i] = ToInt32((
			(int64_t)src1[i]  + 
			(int64_t)src2[i] 
			) / 2ll);
	}
}

void audio_procedure_volume3_32(int32_t *dst, int32_t *src1, int32_t *src2, int32_t *src3, int samples)
{
	for(int i = 0; i < samples; i++)
	{
		dst[i] = ToInt32((
			(int64_t)src1[i] + 
			(int64_t)src2[i] +
			(int64_t)src3[i] 
			) / 3ll);
	}
}

void audio_procedure_volume1_dbl_v(double *dst, double *src,  double volume, int samples)
{
	for(int i = 0; i < samples; i++)
	{
		dst[i] = ToDouble(src[i] * volume / 100.0);
	}
}

void audio_procedure_volume2_dbl_v(double *dst, double *src1, double volume1, double *src2, double volume2, int samples)
{
	for(int i = 0; i < samples; i++)
	{
		dst[i] = ToDouble((
			src1[i] * volume1 + 
			src2[i] * volume2
			) / 200.0);
	}
}

void audio_procedure_volume3_dbl_v(double *dst, double *src1, double volume1, double *src2, double volume2, double *src3, double volume3, int samples)
{
	for(int i = 0; i < samples; i++)
	{
		dst[i] = ToDouble((
			src1[i] * volume1 + 
			src2[i] * volume2 +
			src3[i] * volume3
			) / 300.0);
	}
}

void audio_procedure_volume2_dbl(double *dst, double *src1, double *src2, int samples)
{
	for(int i = 0; i < samples; i++)
	{
		dst[i] = ToDouble((
			src1[i] + 
			src2[i]
			) / 2.0);
	}
}

void audio_procedure_volume3_dbl(double *dst, double *src1, double *src2, double *src3, int samples)
{
	for(int i = 0; i < samples; i++)
	{
		dst[i] = ToDouble((
			src1[i] + 
			src2[i] +
			src3[i] 
			) / 3.0);
	}
}


/*
void test_volume_format()
{
	short src[10] = {0, -32768, 0, 32767, 0, -32768, 0, 32767};
	int8_t dest8[10] = {};
	int32_t dest32[10] = {};
	float destFlt[10] = {};
	struct SwrContext*		swr_context8;
	struct SwrContext*		swr_context32;
	struct SwrContext*		swr_contextFloat;

	swr_context8 =     swr_alloc_set_opts(NULL, AV_CH_LAYOUT_MONO, AV_SAMPLE_FMT_U8,  44100, AV_CH_LAYOUT_MONO, AV_SAMPLE_FMT_S16, 44100, 0, NULL);
	swr_context32 =    swr_alloc_set_opts(NULL, AV_CH_LAYOUT_MONO, AV_SAMPLE_FMT_S32, 44100, AV_CH_LAYOUT_MONO, AV_SAMPLE_FMT_S16, 44100, 0, NULL);
	swr_contextFloat = swr_alloc_set_opts(NULL, AV_CH_LAYOUT_MONO, AV_SAMPLE_FMT_FLT, 44100, AV_CH_LAYOUT_MONO, AV_SAMPLE_FMT_S16, 44100, 0, NULL);
	swr_init(swr_context8);
	swr_init(swr_context32);
	swr_init(swr_contextFloat);
	
	const uint8_t *s[8] = {(uint8_t *)src};
	uint8_t *d1[8] = {(uint8_t *)dest8};
	uint8_t *d2[8] = {(uint8_t *)dest32};
	uint8_t *d3[8] = {(uint8_t *)destFlt};

	int32_t len1 = swr_convert(swr_context8,	d1, 10, s, 8);
	int32_t len2 = swr_convert(swr_context32,	d2, 10, s, 8);
	int32_t len3 = swr_convert(swr_contextFloat,d3, 10, s, 8);

	printf("%x, %x, %x, %x\n", dest8[0],   dest8[1],   dest8[2],   dest8[3]);
	printf("%x, %x, %x, %x\n", dest32[0],  dest32[1],  dest32[2],  dest32[3]);
	printf("%f, %f, %f, %f\n", destFlt[0], destFlt[1], destFlt[2], destFlt[3]);
}
*/