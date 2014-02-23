#include "StdAfx.h"
#include <wmsdk.h>
#include "../ffmpeg.inc.h"
#include "WMEncoder.h"
#include "WMUtils.h"

static const QWORD	UNITS = 10000000ll;

#define				AUDIO_BUFFER_SIZE	2048
#define				MAX_MEDIA_TYPE_SIZE	2048

static HRESULT CheckEncoder(IWMProfileManager* pProfileManager, GUID majortype, GUID subtype)
{
	HRESULT hr;

	BYTE Bytes[MAX_MEDIA_TYPE_SIZE];
	WM_MEDIA_TYPE* pType = (WM_MEDIA_TYPE*)Bytes;

	IWMCodecInfo3* pCodecInfo = NULL;
	JIF(pProfileManager->QueryInterface(__uuidof(IWMCodecInfo3), (void**)&pCodecInfo));

	DWORD cCodecs = 0;
	JIF(pCodecInfo->GetCodecInfoCount(majortype, &cCodecs));

	for(DWORD  i = 0; i < cCodecs; i++)
	{
		DWORD cFormats = 0;
		JIF(pCodecInfo->GetCodecFormatCount(majortype, i, &cFormats ));

		for(DWORD j = 0; j < cFormats; j++)
		{
			IWMStreamConfig *pStreamConfig = NULL;
			IWMMediaProps *pMediaProps = NULL;
			DWORD cbType = sizeof(Bytes);

			JIF(pCodecInfo->GetCodecFormat( majortype, i, j, &pStreamConfig ));
			JIF(pStreamConfig->QueryInterface( __uuidof(IWMMediaProps), (void **)&pMediaProps));
			JIF(pMediaProps->GetMediaType( pType, &cbType ));

			pMediaProps->Release();
			pStreamConfig->Release();

			if (pType->subtype == subtype)
			{
				pCodecInfo->Release();
				return S_OK;
			}
		}
	}

	pCodecInfo->Release();

	return S_FALSE;
}

int32_t wm_write_header(AVFormatContext *avctx)
{
	HRESULT hr;
	WVPrivate*			data = (WVPrivate *)avctx->priv_data;
	IWMProfileManager*	pProfileManager = NULL;
	IWMProfile*			pProfile = NULL;
	AVStream*			video_streams = NULL;
	AVStream*			audio_streams = NULL;

	BYTE Bytes[MAX_MEDIA_TYPE_SIZE];
	WM_MEDIA_TYPE* pType = (WM_MEDIA_TYPE*)Bytes;
	data->m_afc = avctx;
	
	data->m_nVideoInputIndex = -1;
	data->m_nAudioInputIndex = -1;
	for(unsigned int i = 0; i < avctx->nb_streams; i++)
	{
		if (avctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			video_streams = avctx->streams[i];
			data->m_nVideoInputIndex = i;
		}
		else if (avctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			audio_streams = avctx->streams[i];
			data->m_nAudioInputIndex = i;
		}
	}

	JIF(WMCreateProfileManager( &pProfileManager));
	JIF(pProfileManager->CreateEmptyProfile( WMT_VER_9_0, &pProfile));

	if (video_streams)
	{
		IWMStreamConfig* pVideoStreamConfig = NULL;
		JIF(pProfile->CreateNewStream(WMMEDIATYPE_Video, &pVideoStreamConfig));

		JIF(pVideoStreamConfig->SetStreamNumber(data->m_nVideoInputIndex + 1));
		JIF(pVideoStreamConfig->SetBitrate( video_streams->codec->bit_rate ));
		JIF(pVideoStreamConfig->SetBufferWindow(3000));

		ZeroMemory(Bytes, sizeof(Bytes));
		pType->majortype = WMMEDIATYPE_Video;
		pType->formattype = WMFORMAT_VideoInfo;
		pType->pbFormat = &Bytes[sizeof(WM_MEDIA_TYPE)];
		WMVIDEOINFOHEADER* pVidHdr = (WMVIDEOINFOHEADER*)pType->pbFormat;

		switch(video_streams->codec->codec_id)
		{
		case CODEC_ID_WMV1:
			DebugLog("WMV1\n");
			pType->subtype = WMMEDIASUBTYPE_WMV1;
			break;
		case CODEC_ID_WMV2:
			DebugLog("WMV2\n");
			pType->subtype = WMMEDIASUBTYPE_WMV2;
			break;
		case CODEC_ID_WMV3:
			DebugLog("WMV3\n");
			pType->subtype = WMMEDIASUBTYPE_WMV3;
			break;
		case CODEC_ID_VC1:
			if (CheckEncoder(pProfileManager, WMMEDIATYPE_Video, WMMEDIASUBTYPE_WVC1) == S_OK)
			{
				DebugLog("VC1\n");
				pType->subtype = WMMEDIASUBTYPE_WVC1;
			}
			else
			{
				DebugLog("VC1->WMV3\n");
				pType->subtype = WMMEDIASUBTYPE_WMV3;
			}
			break;
		};

		RECT rcRect = {0, 0, video_streams->codec->width, video_streams->codec->height};
		pVidHdr->bmiHeader.biPlanes = 1;
		pVidHdr->bmiHeader.biBitCount = 24;
		pVidHdr->bmiHeader.biSizeImage = 0;
		pVidHdr->bmiHeader.biCompression = pType->subtype.Data1;
		pVidHdr->bmiHeader.biWidth = video_streams->codec->width;
		pVidHdr->bmiHeader.biHeight = video_streams->codec->height;
		pVidHdr->rcSource = rcRect;
		pVidHdr->rcTarget = rcRect;
		pVidHdr->AvgTimePerFrame = UNITS * video_streams->codec->time_base.num / video_streams->codec->time_base.den;
		pVidHdr->dwBitRate = video_streams->codec->bit_rate;

		pType->bFixedSizeSamples = FALSE;
		pType->bTemporalCompression = TRUE;
		pType->lSampleSize = 0;
		pType->pUnk = NULL;
		pType->cbFormat = sizeof(WMVIDEOINFOHEADER);

		ShowMediaTypeString("Video Format", pType);

		IWMVideoMediaProps *pMediaProps = NULL;
		JIF(pVideoStreamConfig->QueryInterface(__uuidof(IWMVideoMediaProps), (void **)&pMediaProps));
		JIF(pMediaProps->SetMediaType(pType));
		JIF(pMediaProps->SetQuality(95));
		JIF(pMediaProps->SetMaxKeyFrameSpacing(5ll * UNITS));
		pMediaProps->Release();

		IWMPropertyVault* pPropertyVault = NULL;
		JIF(pVideoStreamConfig->QueryInterface(__uuidof(IWMPropertyVault), (void **)&pPropertyVault));

		BOOL fIsVBR = TRUE;
		DWORD dwVBRQuality = 95;
		JIF(pPropertyVault->SetProperty(g_wszVBREnabled, WMT_TYPE_BOOL, (BYTE*)&fIsVBR, sizeof(fIsVBR )) );
		JIF(pPropertyVault->SetProperty(g_wszVBRQuality, WMT_TYPE_DWORD, (BYTE*)&dwVBRQuality, sizeof(DWORD)) );
		pPropertyVault->Release();

		JIF(pProfile->AddStream(pVideoStreamConfig));
		pVideoStreamConfig->Release();
	}

	if (audio_streams)
	{
		GUID subAudioType;
		switch(audio_streams->codec->codec_id)
		{
		case CODEC_ID_WMAV1: // 0x161
			DebugLog("WMAudioV7\n");
			subAudioType = WMMEDIASUBTYPE_WMAudioV7;
			break;
		case CODEC_ID_WMAV2: // 0x161
			DebugLog("WMAudioV8\n");
			subAudioType = WMMEDIASUBTYPE_WMAudioV8;
			break;
		case CODEC_ID_WMAPRO: // 0x162
			DebugLog("WMAudioV9\n");
			subAudioType = WMMEDIASUBTYPE_WMAudioV9;
			break;
		case CODEC_ID_WMALOSSLESS: // 0x163
			DebugLog("WMAudio_Lossless\n");
			subAudioType = WMMEDIASUBTYPE_WMAudio_Lossless;
			break;
		};

		IWMCodecInfo3* pCodecInfo = NULL;
		JIF(pProfileManager->QueryInterface(__uuidof(IWMCodecInfo3), (void**)&pCodecInfo));

		DWORD cCodecs = 0;
		JIF(pCodecInfo->GetCodecInfoCount(WMMEDIATYPE_Audio, &cCodecs));

		BOOL bOk = FALSE;
		BOOL bAllAudioType = FALSE;
				
		int32_t LastBitrateError = INT32_MAX;
		BYTE LastMediaType[MAX_MEDIA_TYPE_SIZE];

label_any_coder:
		for(int f = 0; f < 4; f ++)
		{
			static const int32_t ff[4][2] =
			{
				//VBR, 24
				{1, 16},
				{1, 24},
				{0, 16},
				{0, 24},
			};

			int32_t fIsVBR = ff[f][0];
			int32_t Bits = ff[f][1];
			DebugLog(">>> VBR=%d, Bits=%d <<<\n", fIsVBR, Bits);

			for(DWORD  i = 0; i < cCodecs; i++)
			{
#ifdef _DEBUG
				wchar_t szName[256];
				DWORD	cchName = 256;
				pCodecInfo->GetCodecName(WMMEDIATYPE_Audio, i, szName, &cchName);
				DebugLog("Codec%d:%ls\n", i, szName);
#endif
				hr = pCodecInfo->SetCodecEnumerationSetting( WMMEDIATYPE_Audio, i, g_wszVBREnabled, WMT_TYPE_BOOL, (BYTE*)&fIsVBR, sizeof(BOOL));
				if (hr != S_OK) continue;

				BOOL dwVBRPasses = 1;
				hr = pCodecInfo->SetCodecEnumerationSetting( WMMEDIATYPE_Audio, i, g_wszNumPasses, WMT_TYPE_DWORD, (BYTE*)&dwVBRPasses, sizeof(DWORD));
				if (hr != S_OK) continue;

				DWORD cFormats = 0;
				JIF(pCodecInfo->GetCodecFormatCount( WMMEDIATYPE_Audio, i, &cFormats ));

				for(DWORD j = 0; j < cFormats; j++)
				{
					IWMStreamConfig *pStreamConfig = NULL;
					IWMMediaProps *pMediaProps = NULL;
					DWORD cbType = sizeof(Bytes);

					JIF(pCodecInfo->GetCodecFormat( WMMEDIATYPE_Audio, i, j, &pStreamConfig ));
					JIF(pStreamConfig->QueryInterface( __uuidof(IWMMediaProps), (void **)&pMediaProps));
					JIF(pMediaProps->GetMediaType( pType, &cbType ));
#ifdef _DEBUG
					wchar_t wszFormatName[256] = L"";
					DWORD dwFormatNameLength = 256;
					pCodecInfo->GetCodecFormatDesc( WMMEDIATYPE_Audio, i, j, NULL, wszFormatName, &dwFormatNameLength );
					DebugLog("\tDesc%d:%ls;(0x%x)\n", j, wszFormatName, pType->subtype.Data1);
#endif

					WAVEFORMATEX *pWfx = (WAVEFORMATEX *) pType->pbFormat;
					if(
						((pType->subtype == subAudioType) || (bAllAudioType)) &&
						(pType->formattype == WMFORMAT_WaveFormatEx) &&
						(pWfx->nSamplesPerSec == audio_streams->codec->sample_rate) &&
						(pWfx->nChannels == audio_streams->codec->channels) &&
						(pWfx->wBitsPerSample == Bits)
						)
					{
						ShowMediaTypeString("Test This Format", pType);

						if (fIsVBR)
						{
							DWORD bVBREnabled = 0;
							WMT_ATTR_DATATYPE Type = WMT_TYPE_BOOL;
							DWORD dwSize = sizeof( bVBREnabled );
							IWMPropertyVault *pPropertyVault = NULL;
							JIF(pMediaProps->QueryInterface(IID_IWMPropertyVault, (void **)&pPropertyVault));
							JIF(pPropertyVault->GetPropertyByName(g_wszVBREnabled, &Type, (BYTE*) &bVBREnabled, &dwSize));
							if (bVBREnabled)
							{
								DWORD Q = pWfx->nAvgBytesPerSec & 0xFF;
								if (
									(Q >= 90) ||
									(bAllAudioType)
									)
								{
									bOk = TRUE;
								}
							}
							else
							{
								av_logf(NULL, AV_LOG_ERROR, "Impossible!!\n");
							}

							pPropertyVault->Release();
						}
						else
						{
							DWORD nBitrate = 0;
							JIF(pStreamConfig->GetBitrate(&nBitrate));
							int32_t BitrateError = nBitrate - audio_streams->codec->bit_rate;
							if (BitrateError < 0) BitrateError = -BitrateError + 1000;
							if (BitrateError < LastBitrateError)
							{
								LastBitrateError = BitrateError;
								CopyMemory(LastMediaType, Bytes, MAX_MEDIA_TYPE_SIZE);
							}
						}
					}

					pMediaProps->Release();
					pStreamConfig->Release();
					if (bOk) break;
				} // for j

				if (bOk) break;
			} // for i

			if (bOk)
			{
				break;
			}
			if ((bOk == FALSE) && (LastBitrateError != INT32_MAX))
			{
				bOk = TRUE;
				CopyMemory(Bytes, LastMediaType, MAX_MEDIA_TYPE_SIZE);
				break;
			}
		}

		if (bOk == FALSE)
		{
			if (bAllAudioType == FALSE)
			{
				bAllAudioType = TRUE;
				av_logf(NULL, AV_LOG_INFO, "Try All.\n");
				goto label_any_coder;
			}
			else
			{
				av_logf(NULL, AV_LOG_ERROR, "No found audio encoder.\n");
				return -1;
			}
		}
		pCodecInfo->Release();

		ShowMediaTypeString("Audio Format", pType);

		IWMStreamConfig* pAudioStreamConfig = NULL;
		JIF(pProfile->CreateNewStream(WMMEDIATYPE_Audio, &pAudioStreamConfig));
		JIF(pAudioStreamConfig->SetStreamNumber(data->m_nAudioInputIndex + 1));
		JIF(pAudioStreamConfig->SetBufferWindow( 5000 ));

		IWMMediaProps* pMediaProps = NULL;
		JIF(pAudioStreamConfig->QueryInterface( __uuidof(IWMMediaProps), (void**) &pMediaProps ));
		JIF(pMediaProps->SetMediaType( pType ));
		pMediaProps->Release();

		JIF(pProfile->AddStream(pAudioStreamConfig));
		pAudioStreamConfig->Release();
	}

	JIF(WMCreateWriter( NULL, &data->m_pWriter ));
	JIF(data->m_pWriter->SetProfile(pProfile));

	if (video_streams)
	{
		DWORD cFormats = 0;
		JIF(data->m_pWriter->GetInputFormatCount(0, &cFormats));

		GUID subVideoType;
		switch(video_streams->codec->pix_fmt)
		{
		case AV_PIX_FMT_BGRA:
			subVideoType = WMMEDIASUBTYPE_RGB32;
			break;

		case AV_PIX_FMT_BGR24:
			subVideoType = WMMEDIASUBTYPE_RGB24;
			break;

		case AV_PIX_FMT_YUV420P:
			subVideoType = WMMEDIASUBTYPE_I420;
			break;

		default:
			av_logf(NULL, AV_LOG_ERROR, "Unsupport pixel format=%d.\n", video_streams->codec->pix_fmt);
			return -1;
		}

		for(DWORD i = 0; i < cFormats; i++)
		{
			IWMInputMediaProps* pProps = NULL;
			DWORD cbType = sizeof(Bytes);
			JIF(data->m_pWriter->GetInputFormat(0, i, &pProps));
			JIF(pProps->GetMediaType(pType, &cbType));

			if (
				(pType->subtype == subVideoType) && 
				(pType->formattype == WMFORMAT_VideoInfo)
				)
			{
				WMVIDEOINFOHEADER* pVidHdr = (WMVIDEOINFOHEADER*)pType->pbFormat;
				pVidHdr->bmiHeader.biHeight = - video_streams->codec->height;
				JIF(pProps->SetMediaType(pType));
				JIF(data->m_pWriter->SetInputProps(0, pProps));
				pProps->Release();
				break;
			}
			else
			{
				pProps->Release();
			}
		}

		data->m_VideoBufferSize = video_streams->codec->width * video_streams->codec->height * 4;
		data->m_llFrameRateA = video_streams->codec->time_base.num * UNITS;
		data->m_llFrameRateB = video_streams->codec->time_base.den;
	}

	if (audio_streams)
	{
		BOOL fAudioInOk = FALSE;
		DWORD cFormats = 0;
		JIF(data->m_pWriter->GetInputFormatCount(data->m_nAudioInputIndex, &cFormats));
		for(DWORD i = 0; i < cFormats; i++)
		{
			IWMInputMediaProps* pProps = NULL;
			DWORD cbType = sizeof(Bytes);
			JIF(data->m_pWriter->GetInputFormat(data->m_nAudioInputIndex, i, &pProps));
			JIF(pProps->GetMediaType(pType, &cbType));

			ShowMediaTypeString("Audio Input Format", pType);

			WAVEFORMATEX *pWfx = (WAVEFORMATEX *) pType->pbFormat;
			if ( 
				(pType->subtype == WMMEDIASUBTYPE_PCM) &&
				(pType->formattype == WMFORMAT_WaveFormatEx) &&
				(pWfx->nSamplesPerSec == audio_streams->codec->sample_rate) &&
				(pWfx->nChannels == audio_streams->codec->channels) &&
				(pWfx->wBitsPerSample == 16)
				)
			{
				JIF(data->m_pWriter->SetInputProps(data->m_nAudioInputIndex, pProps));
				pProps->Release();
				fAudioInOk = TRUE;
				break;
			}
			else
			{
				pProps->Release();
			}
		}

		if (fAudioInOk == FALSE)
		{
			av_logf(NULL, AV_LOG_ERROR, "Faild to Audio In.\n");
			return -1;
		}

		data->m_llAudioDiv = audio_streams->codec->sample_rate * audio_streams->codec->channels * 2;
		data->m_AudioBufferSize = AUDIO_BUFFER_SIZE * audio_streams->codec->channels * 2;
	}

	wchar_t wszFile[MAX_PATH];
	StringCchPrintf(wszFile, MAX_PATH, L"%hs", avctx->filename);
	JIF(data->m_pWriter->SetOutputFilename(wszFile));
	JIF(data->m_pWriter->BeginWriting());
	pProfile->Release();
	pProfileManager->Release();

	return 0;
}

int32_t wm_write_packet(AVFormatContext *s, AVPacket *pkt)
{
	return 0;
}

int32_t wm_interleave_packet(struct AVFormatContext *avctx, AVPacket *out, AVPacket *in, int32_t flush)
{
	return 0;
}

int32_t wm_write_trailer(AVFormatContext *avctx)
{
	WVPrivate *data = (WVPrivate *)avctx->priv_data;
	if (data->m_pWriter)
	{
		data->m_pWriter->EndWriting();
		data->m_pWriter->Release();
		data->m_pWriter = NULL;
	}
	return 0;
}

/*************************************************
* Video
*************************************************/
int32_t wmv_encode_init(AVCodecContext *avctx)
{
	return 0;
}

int32_t wmv_encode_frame(AVCodecContext *avctx, AVPacket *avpkt, const AVFrame *frame, int32_t *got_packet_ptr)
{
	WVPrivate*			data = (WVPrivate *)avctx->priv_data;

	HRESULT hr;
	INSSBuffer *pBuffer = NULL;

	JIF(data->m_pWriter->AllocateSample(data->m_VideoBufferSize, &pBuffer));

	BYTE *pdwBuffer = NULL;
	JIF(pBuffer->GetBuffer(&pdwBuffer));

	CopyMemory(pdwBuffer, frame->data[0], data->m_VideoBufferSize);

	LONGLONG qwVideoTime = data->m_llFrameCount * data->m_llFrameRateA / data->m_llFrameRateB;
	data->m_llFrameCount ++;

	JIF(data->m_pWriter->WriteSample(0, qwVideoTime, 0, pBuffer));

	pBuffer->Release();

	*got_packet_ptr = 0;
	return 0;
}

int32_t wmv_encode_close(AVCodecContext *avctx)
{
	return 0;
}

/*************************************************
* Audio
*************************************************/
int32_t wma_encode_init(AVCodecContext *avctx)
{
	avctx->frame_size = AUDIO_BUFFER_SIZE;
	return 0;
}

int32_t wma_encode_frame(AVCodecContext *avctx, AVPacket *avpkt, const AVFrame *frame, int32_t *got_packet_ptr)
{
	WVPrivate*			data = (WVPrivate *)avctx->priv_data;
	HRESULT hr;
	INSSBuffer *pBuffer = NULL;

	JIF(data->m_pWriter->AllocateSample(data->m_AudioBufferSize, &pBuffer));

	BYTE *pdwBuffer = NULL;
	JIF(pBuffer->GetBuffer(&pdwBuffer));

	CopyMemory(pdwBuffer, frame->data[0], data->m_AudioBufferSize);

	LONGLONG qwAudioTime = data->m_llAudioWriten * UNITS / data->m_llAudioDiv;
	data->m_llAudioWriten += data->m_AudioBufferSize;

	JIF(data->m_pWriter->WriteSample(data->m_nAudioInputIndex, qwAudioTime, 0, pBuffer));

	pBuffer->Release();

	*got_packet_ptr = 0;
	return 0;
}

int32_t wma_encode_close(AVCodecContext *avctx)
{
	return 0;
}
