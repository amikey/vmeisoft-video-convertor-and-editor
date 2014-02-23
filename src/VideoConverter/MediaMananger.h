#pragma once

#define AUDIO_DATA_FMT		L"%ls\\vmeisoft_v1_a_%ls_%d.tmp"
#define VIDEO_BMP_FMT		L"%ls\\vmeisoft_v1_v_%ls.bmp"

extern void InitMediaMananger();
extern MediaInfo* FindMediaInfo(wchar_t *szFileName);
extern void SaveMediaInfo(struct MediaInfo *info);