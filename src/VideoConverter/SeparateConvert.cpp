#include "stdafx.h"
#include "VideoConverter.h"
#include "VideoFilter.h"
#include "VideoSource.h"
#include "AudioSource.h"
#include "SeparateConvert.h"
#include ".\ffmpeg\Encoder\Encoder.h"
#include "BackgroundFilter.h"
#include "VideoUtils.h"
#include "VideoEncoder.h"
#include "Subtitle.h"
#include "SubtitleFilter.h"


/*****************************************************
// 
*****************************************************/
static DWORD WINAPI SimpleConvertThread(void *lpThreadParameter)
{
	BOOL					bSucceeded = FALSE;
	ClipInfo*				pInfo = (ClipInfo*)lpThreadParameter;
	struct MediaInfo*		pMediaInfo = pInfo->m_MediaInfo;

	CVideoStream*		VStream = NULL;
	CAudioStream*		AStream = NULL;
	if (g_enc_opt.m_VideoDisable == 0) VStream = new CVideoStream();
	if ((g_enc_opt.m_AudioDisable == 0) && (pInfo->HaveAudioStream())) AStream = new CAudioStream();

	CDecoder*				pDecoder = NULL;
	CSimpleVideoEncoder*	pRender = NULL;
	CEncoder*				pEncoder = NULL;
	CAudioSource*			pAudioSource = NULL;

	BOOL					bVideoEof = FALSE;
	BOOL					bAudioEof = FALSE;

	pEncoder = CreateEncoder(pInfo->m_OutFile, &pInfo->m_Time, &pInfo->m_AbortFlag, VStream, AStream);
	if (pEncoder == NULL)goto label_exit;
	if (VStream) VStream->Initialize();
	if (AStream) AStream->Initialize();

	if (g_enc_opt.m_VideoDisable == 0)
	{
		pRender = new CSimpleVideoEncoder(pMediaInfo, VStream);
		pDecoder = new CDecoder(pRender, 1);
		VStream->m_start_pts = pInfo->m_ClipStart;
		VStream->m_next_pts = pInfo->m_ClipStart;
		VStream->m_stop_pts = pInfo->m_ClipStop ? pInfo->m_ClipStop : pMediaInfo->m_Duration;
		pRender->m_NextTime = &VStream->m_next_pts;
		pRender->m_StopTime = VStream->m_stop_pts;

		if (pDecoder->ffdec_open(pMediaInfo->m_szFileName))	goto label_exit;
		if (pInfo->m_ClipStart)
		{
			pDecoder->ffdec_seek(pInfo->m_ClipStart);
		}
	}
	else
	{
		bVideoEof = 1;
	}

	if ((g_enc_opt.m_AudioDisable == 0) && (pInfo->HaveAudioStream()))
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

label_exit:
	if (VStream) VStream->m_Eof = 1;
	if (AStream) AStream->m_Eof = 1;
	if (pEncoder) DestroyEncoder(pEncoder);
	SAFE_DELETE(pDecoder);
	SAFE_DELETE(pRender);
	SAFE_DELETE(pAudioSource);
	SAFE_DELETE(VStream);
	SAFE_DELETE(AStream);

	if (pInfo->m_AbortFlag)
	{
		pInfo->OnStatusChanged(ClipStat_Abort);
	}
	else if (bSucceeded)
	{
		pInfo->OnStatusChanged(ClipStat_Converted);
	}
	else
	{
		pInfo->OnStatusChanged(ClipStat_Failed);
	}

	pInfo->m_Processing = 0;
	return 0;
}

/*****************************************************
// 
*****************************************************/

static DWORD WINAPI SeparateConvertThread(void *lpThreadParameter)
{
	BOOL					bSucceeded = FALSE;
	ClipInfo*				pInfo = (ClipInfo*)lpThreadParameter;
	struct MediaInfo*		pMediaInfo = pInfo->m_MediaInfo;

	CVideoStream*		VStream = NULL;
	CAudioStream*		AStream = NULL;
	if (g_enc_opt.m_VideoDisable == 0) VStream = new CVideoStream();
	if ((g_enc_opt.m_AudioDisable == 0) && (pInfo->HaveAudioStream())) AStream = new CAudioStream();

	CEncoder*				pEncoder = NULL;
	CDecoder*				pDecoder = NULL;
	CVideoFilterManager*	pFilterManager = NULL;
	CVideoEncoder*			pRender = NULL;
	CAudioSource*			pAudioSource = NULL;
	
	BOOL					bVideoEof = FALSE;
	BOOL					bAudioEof = FALSE;

	pEncoder = CreateEncoder(pInfo->m_OutFile, &pInfo->m_Time, &pInfo->m_AbortFlag, VStream, AStream);
	if (pEncoder == NULL)goto label_exit;
	if (VStream) VStream->Initialize();
	if (AStream) AStream->Initialize();

	if (g_enc_opt.m_VideoDisable == 0)
	{
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

		if (pDecoder->ffdec_open(pMediaInfo->m_szFileName))	goto label_exit;
		if (pInfo->m_ClipStart)
		{
			pDecoder->ffdec_seek(pInfo->m_ClipStart);
		}
	}
	else
	{
		bVideoEof = 1;
	}

	if ((g_enc_opt.m_AudioDisable == 0) && (pInfo->HaveAudioStream()))
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

label_exit:
	if (VStream) VStream->m_Eof = 1;
	if (AStream) AStream->m_Eof = 1;
	if (pEncoder) DestroyEncoder(pEncoder);
	SAFE_DELETE(pDecoder);
	SAFE_DELETE(pFilterManager);
	SAFE_DELETE(pRender);
	SAFE_DELETE(pAudioSource);
	SAFE_DELETE(VStream);
	SAFE_DELETE(AStream);

	if (pInfo->m_AbortFlag)
	{
		pInfo->OnStatusChanged(ClipStat_Abort);
	}
	else if (bSucceeded)
	{
		pInfo->OnStatusChanged(ClipStat_Converted);
	}
	else
	{
		pInfo->OnStatusChanged(ClipStat_Failed);
	}

	pInfo->m_Processing = 0;
	return 0;
}

/*****************************************************
// Interface
*****************************************************/

void __stdcall vcReadyForConvert()
{
	g_enc_opt.m_EncInfo = 0;
}

HRESULT __stdcall vcStartConvert(ClipInfo *item)
{
	StringCchPrintf(item->m_OutFile, MAX_PATH, L"%ls\\%ls.%ls", g_opt.m_OutputPath, item->m_Name, g_enc_opt.m_Format);
	for(int i = 1; i < 1000; i++)
	{
		if (PathFileExists(item->m_OutFile) == FALSE) break;
		StringCchPrintf(item->m_OutFile, MAX_PATH, L"%ls\\%ls(%d).%ls", g_opt.m_OutputPath, item->m_Name, i, g_enc_opt.m_Format);
	}

	BOOL bSimple = g_enc_opt.m_VideoDisable || item->SimpleConvert();
	DWORD idThread;
	item->m_AbortFlag = 0;
	item->m_Processing = 1;
	item->m_Time = 0;
	item->m_Duration = item->m_ClipStop ? item->m_ClipStop : item->m_MediaInfo->m_Duration;
	HANDLE hThread = CreateThread(NULL, 0, bSimple ? SimpleConvertThread : SeparateConvertThread, item, 0, &idThread);
	CloseHandle(hThread);
	return S_OK;
}

void __stdcall vcAbortConvert(ClipInfo *item)
{
	item->m_AbortFlag = 1;
	while (item->m_Processing)
	{
		Sleep(10);
	}
}

