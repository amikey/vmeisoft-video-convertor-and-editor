#pragma once

#ifdef _DEBUG

extern int32_t CreateWaveFile(const wchar_t *file, int32_t channels, int32_t samples_rat, int32_t bits_per_sample);
extern void CloseWaveFile();
extern void PutWaveFileData(void *data, int32_t size);

#else

#define CreateWaveFile(...)
#define CloseWaveFile(...)
#define PutWaveFileData(...)

#endif
