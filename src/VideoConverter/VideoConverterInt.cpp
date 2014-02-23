#include "stdafx.h"
#include "VideoConverter.h"
#include "./ffmpeg/ffmpeg.inc.h"
#include "./ffmpeg/ffmpeg.h"
#include "./ffmpeg/Decoder/InfoDecoder.h"
#include "VideoPlayer.h"
#include "../Include/ProfileManager.h"
#include "Initial.h"
#include "MediaMananger.h"
#include "MergeConvert.h"
#include "Subtitle.h"
#include "SubtitleReader.h"
#include "CommonResource.h"
#include "HSBControl.h"

#pragma data_seg("Shared")
static volatile LONG g_lApplicationInstances = 0;
HWND g_hwndMain = NULL;
#pragma data_seg()
#pragma comment(linker, "/Section:Shared,RWS")

int32_t __stdcall GetMultipleInatance()
{
	CreateMutex(NULL, FALSE, L"VMeisoftVideoConverterMutex");
	return InterlockedIncrement(&g_lApplicationInstances);
}

HWND __stdcall GetMainWindowHandle()
{
	return g_hwndMain;
}

void __stdcall SetMainWindowHandle(HWND hwndMain)
{
	g_hwndMain = hwndMain;
}

int32_t __stdcall VCGlobal_Size()
{
	return sizeof(VCGlobal);
}

int32_t __stdcall CodecOptions_Size()
{
	return sizeof(CodecOptions);
}

int32_t __stdcall MediaInfo_Size()
{
	return sizeof(MediaInfo);
}

int32_t __stdcall VCItem_Size()
{
	return sizeof(VCItem);
}

static void InitGdiplus()
{
	static ULONG_PTR gdiplusToken;
	Gdiplus::GdiplusStartupInputEx gdiplusStartupInput;
	Gdiplus::Status s = GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	if (s == Gdiplus::Ok) return ;
}

HRESULT __stdcall vcInitialize()
{
	GetTempPath(MAX_PATH, g_szTempPath);
	PathRemoveBackslash(g_szTempPath);
	GetModuleFileName(NULL, g_szModulePath, MAX_PATH);
	PathRemoveFileSpec(g_szModulePath);

	CreateLogFile(); 
	GetOSDisplayString(); 
	GetSystemInformations(); 
	InitGdiplus();
	InitCommonResource(); 
	InitHSBControlWindow();
	ffmpeg_init(); 
	LoadProfiles();
	InitMediaMananger(); 
	InitVideoPlayer();
	InitVideoRender();
	
	StringCchCopy(g_merge_info.m_Name, 256, L"merged");

	if (LoadOptions() != S_OK)
	{
		SHGetFolderPath(NULL, CSIDL_MYVIDEO | CSIDL_FLAG_CREATE, NULL, 0, g_opt.m_OutputPath);
		g_opt.m_Parallel = g_cpu_count;
		g_opt.m_FillColor2 = 0xFFFFFFFF;
	}

	if (LoadProfileSettings() != S_OK)
	{
		ZeroMemory(&g_ProfileSettings, sizeof(g_ProfileSettings));
		GetDefaultProfileSetting1(g_ProfileSettings, GROUP_VIDEO);
	}
	CopyProfileSettings();
	
	return S_OK;
}

void __stdcall vcUninitialize()
{
	SaveOptions();
	SaveProfileSettings();
}

VCGlobal* __stdcall vcGetGlobal()
{
	return &g_opt;
}

CodecOptions* __stdcall vcGetCodecOptions()
{
	return &g_enc_opt;
}

MediaInfo* __stdcall vcImportMediaInfo(HWND hwndMessage, wchar_t *szFile)
{
	MediaInfo *info = FindMediaInfo(szFile);
	if (info)
	{
		PostMessage(hwndMessage, VC_IMPORT_STATUS, 0, MediaStat_Check_Media);
		if (info->m_bVideoStream) PostMessage(hwndMessage, VC_IMPORT_STATUS, 0, MediaStat_Thumbnail_Generated);
		PostMessage(hwndMessage, VC_IMPORT_STATUS, 0, MediaStat_Complete);
		return info;
	}
	
	CMediaInfo *info1 = new CMediaInfo(hwndMessage, szFile);
	if (info1->m_Ok == 0)
	{
		delete info1;
		return NULL;
	}
	CreateInfoDecoder(info1);
	return info1;
}

void __stdcall vcImportAbort(CMediaInfo *info)
{
	info->m_Abort = 1;
}

int __stdcall vcImportGetProgress(CMediaInfo *info)
{
	return info->GetProgress();
}

void __stdcall vcDeleteItem(ClipInfo *item)
{
	item->Release();
}

ClipInfo* __stdcall vcCreateItem(HWND hwndMessage, MediaInfo *info)
{
	return new ClipInfo(info, hwndMessage);
}

CVideoPlayer* __stdcall vcCreateVideoPlayer(HWND hwndParent, HWND hwndMessage, ClipInfo *info, VideoPlayerMode Mode)
{
	CVideoPlayer *player = new CVideoPlayer(info, Mode, hwndParent, hwndMessage);
	return player;
}

HRESULT __stdcall vcVideoPlayerGrab(CVideoPlayer *player, wchar_t *szFile)
{
	return player->Grab(szFile);
}

void __stdcall vcVideoPlayerDestroy(CVideoPlayer *player)
{
	delete player;
}

void __stdcall vcVideoPlayerResize(CVideoPlayer *player)
{
	player->Resize();
}

void __stdcall vcVideoPlayerStartStop(CVideoPlayer *player, int64_t start, int64_t stop)
{
	player->SetStartStop(start, stop);
}

void __stdcall vcVideoPlayerResume(CVideoPlayer *player)
{
	player->Play();
}

void __stdcall vcVideoPlayerPause(CVideoPlayer *player)
{
	player->Pause();
}

void __stdcall vcVideoPlayerSeekTo(CVideoPlayer *player, int64_t pts)
{
	player->SeekTo(pts);
}

void __stdcall vcVideoPlayerNextFrame(CVideoPlayer *player)
{
	return player->NextFrame();
}

void __stdcall vcVideoPlayerPreTime(CVideoPlayer *player)
{
	return player->PreTime();
}

void __stdcall vcVideoPlayerNextTime(CVideoPlayer *player)
{
	return player->NextTime();
}

int64_t __stdcall vcVideoPlayerCurrentTime(CVideoPlayer *player)
{
	return player->GetTime();
}

void __stdcall vcVideoPlayerForceRedraw(CVideoPlayer *player)
{
	player->ForceRedraw();
}

void __stdcall vcVideoPlayerSetMusic(CVideoPlayer *player, int32_t index, MediaInfo *info)
{
	return player->SetMusic(index, info);
}

void __stdcall vcVideoPlayerSetVolume(CVideoPlayer *player, int32_t index, int32_t Volume)
{
	return player->SetVolume(index, Volume);
}

extern "C" void av_get_channel_layout_string(char *buf, int32_t buf_size, int32_t nb_channels, uint64_t channel_layout);

wchar_t * __stdcall MakeMediaInfoString(MediaInfo* info, wchar_t *buffer)
{
	buffer[0] = 0;
    if (info->m_bVideoStream)
    {
		StringCchPrintf(buffer + wcslen(buffer), 128, L"Video:%dx%d; ", info->m_Width, info->m_Height);
    }
	else
	{
        StringCchCat(buffer + wcslen(buffer), 128, L"Video:None; ");
    };

    if (info->m_bAudioStream)
    {
        StringCchCat(buffer + wcslen(buffer), 128, L"Audio:");
        switch (info->m_SampleRate)
		{
            case 8000:
				StringCchCat(buffer + wcslen(buffer), 128, L"Phone Quality,");
				break;
            case 22050:
				StringCchCat(buffer + wcslen(buffer), 128, L"Low Quality,");
				break;
            case 44100:
				StringCchCat(buffer + wcslen(buffer), 128, L"CD Quality,");
				break;
            case 48000:
				StringCchCat(buffer + wcslen(buffer), 128, L"DVD Quality,");
				break;
			default:
				StringCchPrintf(buffer + wcslen(buffer), 128, L"%dHz,", info->m_SampleRate);
				break;
        };

		char buf[1024] = {};
		av_get_channel_layout_string(buf, 1024, info->m_nChannel, info->m_channel_layout);
       	StringCchPrintf(buffer + wcslen(buffer), 128, L"%hs", buf);

    }
	else
	{
        StringCchCat(buffer + wcslen(buffer), 128, L"Audio:None");
    };

	{
		int32_t t = (int32_t)(info->m_Duration / AV_TIME_BASE_LL);
		int32_t h = t / 3600;
		t %= 3600;
		int32_t m = t / 60;
		int32_t s = t % 60;
		StringCchPrintf(buffer + wcslen(buffer), 128, L"; Duration:%d:%02d:%02d", h, m ,s);
	}

	return buffer;
};


void __stdcall CopyProfileSettings()
{
	ProfileTag* p = GetProfile(g_ProfileSettings.m_Group, g_ProfileSettings.m_Profile);
	ProfileTarget* t = GetProfileTarget(g_ProfileSettings.m_Group, g_ProfileSettings.m_Profile, g_ProfileSettings.m_Target);

	StringCchCopy(g_enc_opt.m_Format, 64, p->m_Format);
	StringCchCopy(g_enc_opt.m_Target, 64, t->m_Name);
	
	g_enc_opt.m_VideoDisable = g_ProfileSettings.m_VCodec == -1;
	g_enc_opt.m_AudioDisable = g_ProfileSettings.m_ACodec == -1;

	if (g_enc_opt.m_VideoDisable == 0)
	{
		StringCchCopy(g_enc_opt.m_VideoCodec, 64, t->m_Video.m_Codec[g_ProfileSettings.m_VCodec].m_Codec);
		g_enc_opt.m_VideoBitrate = t->m_Video.m_BitRate[g_ProfileSettings.m_VBitRate];
		g_enc_opt.m_FrameNum = t->m_Video.m_FrameRate[g_ProfileSettings.m_FrameRate].m_Num;
		g_enc_opt.m_FrameDen = t->m_Video.m_FrameRate[g_ProfileSettings.m_FrameRate].m_Den;
		g_enc_opt.m_VideoWidth = g_ProfileSettings.m_ResCustom.m_Width;
		g_enc_opt.m_VideoHeight = g_ProfileSettings.m_ResCustom.m_Height;
	}
	
	if (g_enc_opt.m_AudioDisable == 0)
	{
		StringCchCopy(g_enc_opt.m_AudioCodec, 64, t->m_Audio.m_Codec[g_ProfileSettings.m_ACodec].m_Codec);
		g_enc_opt.m_AudioBitrate = t->m_Audio.m_BitRate[g_ProfileSettings.m_ABitRate];
		g_enc_opt.m_AudioSampleRate = t->m_Audio.m_SampleRate[g_ProfileSettings.m_SampleRate];
		g_enc_opt.m_AudioChannels = t->m_Audio.m_Channel[g_ProfileSettings.m_Channel];
	}

}

