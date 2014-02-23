#pragma once

extern void audio_procedure_volume1_8_v(uint8_t *dst, uint8_t *src,  short volume, int samples);
extern void audio_procedure_volume2_8_v(uint8_t *dst, uint8_t *src1, short volume1, uint8_t *src2, short volume2, int samples);
extern void audio_procedure_volume3_8_v(uint8_t *dst, uint8_t *src1, short volume1, uint8_t *src2, short volume2, uint8_t *src3, short volume3, int samples);
extern void audio_procedure_volume2_8(uint8_t *dst, uint8_t *src1, uint8_t *src2, int samples);
extern void audio_procedure_volume3_8(uint8_t *dst, uint8_t *src1, uint8_t *src2, uint8_t *src3, int samples);

extern void audio_procedure_volume1_16_v(int16_t *dst, int16_t *src,  int32_t volume, int samples);
extern void audio_procedure_volume2_16_v(int16_t *dst, int16_t *src1, int32_t volume1, int16_t *src2, int32_t volume2, int samples);
extern void audio_procedure_volume3_16_v(int16_t *dst, int16_t *src1, int32_t volume1, int16_t *src2, int32_t volume2, int16_t *src3, int32_t volume3, int samples);
extern void audio_procedure_volume2_16(int16_t *dst, int16_t *src1, int16_t *src2, int samples);
extern void audio_procedure_volume3_16(int16_t *dst, int16_t *src1, int16_t *src2, int16_t *src3, int samples);

extern void audio_procedure_volume1_flt_v(float *dst, float *src,  float volume, int samples);
extern void audio_procedure_volume2_flt_v(float *dst, float *src1, float volume1, float *src2, float volume2, int samples);
extern void audio_procedure_volume3_flt_v(float *dst, float *src1, float volume1, float *src2, float volume2, float *src3, float volume3, int samples);
extern void audio_procedure_volume2_flt(float *dst, float *src1, float *src2, int samples);
extern void audio_procedure_volume3_flt(float *dst, float *src1, float *src2, float *src3, int samples);

extern void audio_procedure_volume1_32_v(int32_t *dst, int32_t *src,  int64_t volume, int samples);
extern void audio_procedure_volume2_32_v(int32_t *dst, int32_t *src1, int64_t volume1, int32_t *src2, int64_t volume2, int samples);
extern void audio_procedure_volume3_32_v(int32_t *dst, int32_t *src1, int64_t volume1, int32_t *src2, int64_t volume2, int32_t *src3, int64_t volume3, int samples);
extern void audio_procedure_volume2_32(int32_t *dst, int32_t *src1, int32_t *src2, int samples);
extern void audio_procedure_volume3_32(int32_t *dst, int32_t *src1, int32_t *src2, int32_t *src3, int samples);

extern void audio_procedure_volume1_dbl_v(double *dst, double *src,  double volume, int samples);
extern void audio_procedure_volume2_dbl_v(double *dst, double *src1, double volume1, double *src2, double volume2, int samples);
extern void audio_procedure_volume3_dbl_v(double *dst, double *src1, double volume1, double *src2, double volume2, double *src3, double volume3, int samples);
extern void audio_procedure_volume2_dbl(double *dst, double *src1, double *src2, int samples);
extern void audio_procedure_volume3_dbl(double *dst, double *src1, double *src2, double *src3, int samples);
