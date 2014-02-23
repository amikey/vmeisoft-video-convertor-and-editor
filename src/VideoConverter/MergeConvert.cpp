#include "stdafx.h"
#include "VideoConverter.h"
#include "VideoFilter.h"
#include "VideoSource.h"
#include "AudioSource.h"
#include "MergeConvert.h"
#include ".\ffmpeg\Encoder\Encoder.h"
#include "BackgroundFilter.h"
#include "VideoUtils.h"
#include "VideoEncoder.h"
#include "Subtitle.h"
#include "SubtitleFilter.h"

struct MergeInfo		g_merge_info;
static volatile long	g_abort;
static volatile long	g_runing;
static ClipInfo**		g_items;
static int32_t				g_count;

int32_t __stdcall MergeInfo_Size()
{
	return sizeof(g_merge_info);
}

MergeInfo* __stdcall vcGetMergeInfo()
{
	return &g_merge_info;
}

static DWORD WINAPI MergeConvertThread(void *lpThreadParameter)
{
	int error_code = 0;

	CVideoStream*		VStream = NULL;
	CAudioStream*		AStream = NULL;
	if (g_enc_opt.m_VideoDisable == 0) VStream = new CVideoStream();
	if (g_enc_opt.m_AudioDisable == 0) AStream = new CAudioStream();
	CEncoder* pEncoder = CreateEncoder(g_merge_info.m_OutFile, &g_merge_info.m_Time, &g_abort, VStream, AStream);
	if (VStream) VStream->Initialize();
	if (AStream) AStream->Initialize();

	for(int i = 0; i < g_count; i++)
	{
		if (g_abort) break;
		ClipInfo *pInfo = g_items[i];
		struct MediaInfo* pMediaInfo = pInfo->m_MediaInfo;
		BOOL bSucceeded = FALSE;

		if (g_enc_opt.m_VideoDisable || pInfo->SimpleConvert())
		{
			CDecoder*				pDecoder = NULL;
			CSimpleVideoEncoder*	pRender = NULL;
			CAudioSource*			pAudioSource = NULL;
			BOOL					bVideoEof = FALSE;
			BOOL					bAudioEof = FALSE;

			if (g_enc_opt.m_VideoDisable == 0)
			{
				pRender = new CSimpleVideoEncoder(pMediaInfo, VStream);
				pDecoder = new CDecoder(pRender, 1);
				VStream->m_start_pts = pInfo->m_ClipStart;
				VStream->m_next_pts = pInfo->m_ClipStart;
				VStream->m_stop_pts = pInfo->m_ClipStop ? pInfo->m_ClipStop : pMediaInfo->m_Duration;
				pRender->m_NextTime = &VStream->m_next_pts;
				pRender->m_StopTime = VStream->m_stop_pts;

				if (pDecoder->ffdec_open(pMediaInfo->m_szFileName))	goto label_error_1;
				if (pInfo->m_ClipStart)
				{
					pDecoder->ffdec_seek(pInfo->m_ClipStart);
				}
			}
			else
			{
				bVideoEof = 1;
			}

			if (g_enc_opt.m_AudioDisable == 0)
			{
				pAudioSource = new CAudioSource(g_enc_opt.m_EncAudioSampleRate, g_enc_opt.m_EncAudioChannels, g_enc_opt.m_EncAudioPacketBytes, g_enc_opt.m_EncAudioIsPlanar, g_enc_opt.m_EncAudioChannelLayout, g_enc_opt.m_EncAudioFormat);
				for(int i = 0; i < 3; i++)
				{
					if (pInfo->m_AudioTrack[i].m_Media)
					{
						pAudioSource->AddReader(i, &pInfo->m_AudioTrack[i]);
					}
				}
				if (pInfo->m_ClipStart)
				{
					pAudioSource->Seek(pInfo->m_ClipStart);
				}
				pAudioSource->SetStop(pInfo->m_ClipStop ? pInfo->m_ClipStop : pMediaInfo->m_Duration);
				AStream->m_Source = pAudioSource;
			}
			else
			{
				bAudioEof = 1;
			}

			pInfo->OnStatusChanged(ClipStat_Converting);
			while((bVideoEof == 0) || (bAudioEof == 0))
			{
				if (pInfo->m_AbortFlag) break;

				BOOL bSleep = TRUE;

				if ((bVideoEof == 0) && (VStream->m_Buffered < MIN_VIDEO_BUFFER) && (VStream->m_Queued < MIN_VIDEO_QUEUE))
				{
					pDecoder->video_next();
					if (VStream->m_EndOfStream) bVideoEof = 1;
					bSleep = FALSE;
				}
				if (bAudioEof == 0)
				{
					if (AStream->Next() == S_OK) bSleep = FALSE;
					if (AStream->m_EndOfStream) bAudioEof = 1;
				}

				if (bSleep) Sleep(1);
				while ((g_enc_opt.m_Pause) && (pInfo->m_AbortFlag == 0))
				{
					Sleep(100);
				}
			}
			bSucceeded = TRUE;

label_error_1:
			SAFE_DELETE(pDecoder);
			SAFE_DELETE(pRender);
			SAFE_DELETE(pAudioSource);
			if (bSucceeded == FALSE)
			{
				error_code = -1;
				break;
			}
		}
		else
		{
			CDecoder*				pDecoder = NULL;
			CVideoFilterManager*	pFilterManager = NULL;
			CVideoEncoder*			pRender = NULL;
			CAudioSource*			pAudioSource = NULL;
			BOOL					bVideoEof = FALSE;
			BOOL					bAudioEof = FALSE;

			pRender = new CVideoEncoder(pMediaInfo);
			pDecoder = new CDecoder(pRender, 1);
			VStream->m_start_pts = pInfo->m_ClipStart;
			VStream->m_next_pts = pInfo->m_ClipStart;
			VStream->m_stop_pts = pInfo->m_ClipStop ? pInfo->m_ClipStop : pMediaInfo->m_Duration;
			pRender->m_NextTime = &VStream->m_next_pts;
			pRender->m_StopTime = VStream->m_stop_pts;

			pFilterManager = new CVideoFilterManager();
			for(int i = 0; i < VIDEO_FILTER_COUNT; i++)
			{
				if (pInfo->m_Filters[i].IsDefaultValue()) continue ;
				CVideoFilterBase *base = pInfo->m_Filters[i].CreateInstance();
				pFilterManager->AddFiler(base);
			}
			if (pInfo->GetSubtitle()->m_Count)
			{
				CSubtitleFilter *pSubtitleFilter = new CSubtitleFilter(pInfo->GetSubtitle());
				pFilterManager->AddFiler(pSubtitleFilter);
			}
			if (g_enc_opt.m_EncPixFormat == AV_PIX_FMT_BGRA)
			{
				pFilterManager->AddFiler(VStream);
			}
			else
			{
				CVideoSource *pVideoSource = new CVideoSource(VStream);
				pFilterManager->AddFiler(pVideoSource);
			}

			pFilterManager->PutSourceSize(pMediaInfo->m_Width, pMediaInfo->m_Height);
			pFilterManager->NewSegment(0, pInfo->m_ClipStop ? pInfo->m_ClipStop : pMediaInfo->m_Duration);
			pRender->m_pFilterManager = pFilterManager;

			if (pDecoder->ffdec_open(pMediaInfo->m_szFileName))	goto label_error_2;
			if (pInfo->m_ClipStart)
			{
				pDecoder->ffdec_seek(pInfo->m_ClipStart);
			}

			if (g_enc_opt.m_AudioDisable == 0)
			{
				pAudioSource = new CAudioSource(g_enc_opt.m_EncAudioSampleRate, g_enc_opt.m_EncAudioChannels, g_enc_opt.m_EncAudioPacketBytes, g_enc_opt.m_EncAudioIsPlanar, g_enc_opt.m_EncAudioChannelLayout, g_enc_opt.m_EncAudioFormat);
				for(int i = 0; i < 3; i++)
				{
					if (pInfo->m_AudioTrack[i].m_Media)
					{
						pAudioSource->AddReader(i, &pInfo->m_AudioTrack[i]);
					}
				}
				if (pInfo->m_ClipStart)
				{
					pAudioSource->Seek(pInfo->m_ClipStart);
				}
				pAudioSource->SetStop(pInfo->m_ClipStop ? pInfo->m_ClipStop : pMediaInfo->m_Duration);
				AStream->m_Source = pAudioSource;
			}
			else
			{
				bAudioEof = 1;
			}

			pInfo->OnStatusChanged(ClipStat_Converting);
			while((bVideoEof == 0) || (bAudioEof == 0))
			{
				if (pInfo->m_AbortFlag) break;

				BOOL bSleep = TRUE;

				if ((bVideoEof == 0) && (VStream->m_Buffered < MIN_VIDEO_BUFFER) && (VStream->m_Queued < MIN_VIDEO_QUEUE))
				{
					pDecoder->video_next();
					if (VStream->m_EndOfStream) bVideoEof = 1;
					bSleep = FALSE;
				}
				if (bAudioEof == 0)
				{
					if (AStream->Next() == S_OK) bSleep = FALSE;
					if (AStream->m_EndOfStream) bAudioEof = 1;
				}

				if (bSleep) Sleep(1);
				while ((g_enc_opt.m_Pause) && (pInfo->m_AbortFlag == 0))
				{
					Sleep(100);
				}
			}
			bSucceeded = TRUE;

label_error_2:
			SAFE_DELETE(pDecoder);
			SAFE_DELETE(pFilterManager);
			SAFE_DELETE(pRender);
			SAFE_DELETE(pAudioSource);
			if (bSucceeded == FALSE)
			{
				error_code = -1;
				break;
			}
		}
	}

	if (VStream) VStream->m_Eof = 1;
	if (AStream) AStream->m_Eof = 1;
	if (pEncoder) DestroyEncoder(pEncoder);
	SAFE_DELETE(VStream);
	SAFE_DELETE(AStream);
	PostMessage(g_merge_info.m_hwndMessage, VC_MERGE_COMPLETE, NULL, g_abort ? -99 : error_code);
	return 0;
}

HRESULT __stdcall vcStartMerge(ClipInfo *items[], int32_t count)
{
	g_enc_opt.m_EncInfo = 0;
	StringCchPrintf(g_merge_info.m_OutFile, MAX_PATH, L"%ls\\%ls.%ls", g_opt.m_OutputPath, g_merge_info.m_Name, g_enc_opt.m_Format);
	for(int i = 1; i < 1000; i++)
	{
		if (PathFileExists(g_merge_info.m_OutFile) == FALSE) break;
		StringCchPrintf(g_merge_info.m_OutFile, MAX_PATH, L"%ls\\%ls(%d).%ls", g_opt.m_OutputPath, g_merge_info.m_Name, i, g_enc_opt.m_Format);
	}

	g_merge_info.m_Duration = 0;
	for(int i = 0; i < count; i++)
	{
		g_merge_info.m_Duration += (items[i]->m_ClipStop ? items[i]->m_ClipStop : items[i]->m_MediaInfo->m_Duration) - items[i]->m_ClipStart;
	}

	g_count = count;
	g_items = (ClipInfo **)MemoryAlloc(g_count * sizeof(ClipInfo *));
	CopyMemory(g_items, items, g_count * sizeof(ClipInfo *));

	g_abort = 0;
	g_runing = 1;
	DWORD idThread;
	HANDLE hThread = CreateThread(NULL, 0, MergeConvertThread, NULL, 0, &idThread);
	CloseHandle(hThread);
	return S_OK;
}

void __stdcall vcAbortMerge()
{
	if (g_runing)
	{
		g_abort = 1;
		while(g_runing)
		{
			Sleep(5);
		}
	}
}
