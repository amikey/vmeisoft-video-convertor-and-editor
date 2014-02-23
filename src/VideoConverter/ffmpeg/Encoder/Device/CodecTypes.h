#ifndef _CODEC_TYPES_H_
#define _CODEC_TYPES_H_

extern int32_t StringCmpA(const char *a, const char *b);
extern int32_t StringCmpB(const char *a, const char *b);

#define TARGERT_IPOD			"iPod"
#define TARGERT_IPOD_TOUCH		"iPod Touch"
#define TARGERT_IPOD_VIDEO		"iPod video"
#define TARGERT_IPOD_NANO		"iPod Nano"
#define TARGERT_APPLE_TV		"Apple TV"
#define TARGERT_IPHONE			"iPhone"
#define TARGERT_BLACKBERRY		"Blackberry"
#define TARGERT_GPHONE			"GPhone"
#define TARGERT_PALM			"Palm"
#define TARGERT_PSP				"Psp"
#define TARGERT_SANSA			"Sansa"
#define TARGERT_WALKMAN			"Walkman"
#define TARGERT_ZUNE			"Zune"

#define TARGERT_DV_NTSC			"DV-NTSC"
#define TARGERT_DV_PAL			"DV-PAL"
#define TARGERT_DV_NTSC_HD		"DV-NTSC-HD"
#define TARGERT_DV_PAL_HD		"DV-PAL-HD"

#define IS_TARGERT(type)		StringCmpB(target_name, TARGERT_##type)

/*************************************************************
 * 文件格式定义
**************************************************************/
#define FORMAT_AUDIO_MP3		"mp3"
#define FORMAT_AUDIO_WAV		"wav"
#define FORMAT_AUDIO_RA			"ra"
#define FORMAT_AUDIO_AAC		"adts"
#define FORMAT_AUDIO_AC3		"ac3"
#define FORMAT_AUDIO_AMR		"amr"
#define FORMAT_AUDIO_MP2		"mp2"
#define FORMAT_AUDIO_FLAC		"flac"
#define FORMAT_AUDIO_AIFF		"aiff"
#define FORMAT_AUDIO_M4A		"mp4"
#define FORMAT_AUDIO_WMA		"wma"
#define FORMAT_AUDIO_MKA		"matroska"
#define FORMAT_AUDIO_OGG		"ogg"

#define IS_AUDIO_FORMAT(type) StringCmpB(format_name, FORMAT_AUDIO_##type)

#define FORMAT_VIDEO_3GP		"3gp"
#define FORMAT_VIDEO_3G2		"3g2"
#define FORMAT_VIDEO_MKV		"matroska"
#define FORMAT_VIDEO_MATROSKA	"matroska"
#define FORMAT_VIDEO_DV			"dv"
#define FORMAT_VIDEO_MPEG1		"mpeg"
#define FORMAT_VIDEO_MPEG2		"vob"
#define FORMAT_VIDEO_MP4		"mp4"
#define FORMAT_VIDEO_WMV		"wmv"
#define FORMAT_VIDEO_AVI		"avi"
#define FORMAT_VIDEO_RM			"rm"
#define FORMAT_VIDEO_TOD		"mpegts"
#define FORMAT_VIDEO_MOD		"mpeg"
#define FORMAT_VIDEO_M4V		"m4v"
#define FORMAT_VIDEO_AMV		"amv"
#define FORMAT_VIDEO_MTV		"amv"
#define FORMAT_VIDEO_FLV		"flv"
#define FORMAT_VIDEO_SWF		"swf"
#define FORMAT_VIDEO_MOV		"mov"
#define FORMAT_VIDEO_PSP		"psp"
#define FORMAT_VIDEO_SMV		"smv"
#define FORMAT_VIDEO_GIF		"gif"

#define IS_VIDEO_FORMAT(type) StringCmpA(format_name, FORMAT_VIDEO_##type)

/*************************************************************
 * 视频编码器定义
**************************************************************/
#define VIDEO_CODEC_H263		"h263"
#define VIDEO_CODEC_X264		"libx264"
#define VIDEO_CODEC_MPEG1		"mpeg1video"
#define VIDEO_CODEC_MPEG2		"mpeg2video"
#define VIDEO_CODEC_MPEG4		"mpeg4"
#define VIDEO_CODEC_FLV			"flv"
#define VIDEO_CODEC_DV			"dvvideo"
#define VIDEO_CODEC_AMV			"amv"
#define VIDEO_CODEC_FFV1		"ffv1"
#define VIDEO_CODEC_RAW			"rawvideo"
#define VIDEO_CODEC_THEORA		"libtheora"
#define VIDEO_CODEC_VPX			"libvpx"

#define IS_VIDEO_CODEC(type) StringCmpA(video_codec_name, VIDEO_CODEC_##type)

/*************************************************************
 * 音频编码器定义
**************************************************************/
#define AUDIO_CODEC_AAC			"libfaac"
#define AUDIO_CODEC_AMR_NB		"libopencore_amrnb"
#define AUDIO_CODEC_MP3			"libmp3lame"
#define AUDIO_CODEC_AC3			"ac3"
#define AUDIO_CODEC_MP2			"mp2"
#define AUDIO_CODEC_PCM_S16LE	"pcm_s16le"
#define AUDIO_CODEC_VORBIS		"libvorbis"
#define AUDIO_CODEC_FLAC		"flac"
#define AUDIO_CODEC_COOKER		"cooker"
#define AUDIO_CODEC_AMV			"adpcm_ima_amv"

#define IS_AUDIO_CODEC(type)	StringCmpA(audio_codec_name, AUDIO_CODEC_##type)

/*************************************************************
 * 参数处理
**************************************************************/

#endif
