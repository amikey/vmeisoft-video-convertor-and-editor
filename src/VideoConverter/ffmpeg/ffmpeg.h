#pragma once

extern int32_t ffmpeg_init();

struct MediaInfo;
extern const char* ffmeg_get_pix_fmt_name(MediaInfo *info);
extern const char* ffmeg_get_sample_fmt_name(MediaInfo *info);
