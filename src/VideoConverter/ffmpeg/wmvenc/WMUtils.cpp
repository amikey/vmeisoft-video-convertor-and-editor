#define INITGUID				1
#define _CRT_SECURE_NO_WARNINGS 1
#include <stdint.h>
#include <guiddef.h>
#include <streams.h>
#include <dvdmedia.h>
#include <wmsdk.h>
#include <wmcodecdsp.h>
#include <strsafe.h>
#include "WMUtils.h"
#include "XDebug.h"

#ifdef _DEBUG

const IID GUID_NULL;

/*
#ifdef _DEBUG
#pragma comment(lib, "strmbasd.lib")
#else
#pragma comment(lib, "strmbase.lib")
#endif
*/

void ShowProfile(IWMProfile* pProfile)
{
	wchar_t szText[16 * 1024];
	DWORD dwLength = 16 * 1024;
	IWMProfileManager *pProfileManager = NULL;
	WMCreateProfileManager(&pProfileManager);
	pProfileManager->SaveProfile(pProfile, szText, &dwLength);
	WriteLog("%ls\n", szText);
}

struct GuidTyps
{
	GUID	m_guid;
	char*	m_pDec;
};

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

static GuidTyps MajorDec[] =
{
	{MEDIATYPE_Video, "Video"},
	{MEDIATYPE_Audio, "Audio"},
	{MEDIATYPE_Text, "Text"},
	{MEDIATYPE_Midi, "Midi"},
	{MEDIATYPE_Stream, "Stream"},
	{MEDIATYPE_Interleaved, "Interleaved"},
	{MEDIATYPE_File, "File"},
	{MEDIATYPE_ScriptCommand, "ScriptCommand"},
	{MEDIATYPE_AUXLine21Data, "AUXLine21Data"},
	{MEDIATYPE_DTVCCData, "DTVCCData"},
	{MEDIATYPE_MSTVCaption, "MSTVCaption"},
	{MEDIATYPE_VBI, "VBI"},
	{MEDIATYPE_Timecode, "Timecode"},
	{MEDIATYPE_LMRT, "LMRT"},
	{MEDIATYPE_URL_STREAM, "URL STREAM"},
	{WMMEDIATYPE_FileTransfer, "FileTransfer"}
};

static GuidTyps SubDec[] =
{
	{MEDIASUBTYPE_CLPL,"CLPL"},
	{MEDIASUBTYPE_YUYV,"YUYV"},
	{MEDIASUBTYPE_IYUV,"IYUV"},
	{MEDIASUBTYPE_YVU9,"YVU9"},
	{MEDIASUBTYPE_Y411,"Y411"},
	{MEDIASUBTYPE_Y41P,"Y41P"},
	{MEDIASUBTYPE_YUY2,"YUY2"},
	{MEDIASUBTYPE_YVYU,"YVYU"},
	{MEDIASUBTYPE_UYVY,"UYVY"},
	{MEDIASUBTYPE_Y211,"Y211"},
	{MEDIASUBTYPE_CLJR,"CLJR"},
	{MEDIASUBTYPE_IF09,"IF09"},
	{MEDIASUBTYPE_CPLA,"CPLA"},
	{MEDIASUBTYPE_MJPG,"MJPG"},
	{MEDIASUBTYPE_TVMJ,"TVMJ"},
	{MEDIASUBTYPE_WAKE,"WAKE"},
	{MEDIASUBTYPE_CFCC,"CFCC"},
	{MEDIASUBTYPE_IJPG,"IJPG"},
	{MEDIASUBTYPE_Plum,"Plum"},
	{MEDIASUBTYPE_DVCS,"DVCS"},
	{MEDIASUBTYPE_H264,"H264"},
	{MEDIASUBTYPE_DVSD,"DVSD"},
	{MEDIASUBTYPE_MDVF,"MDVF"},
	{MEDIASUBTYPE_RGB1,"RGB1"},
	{MEDIASUBTYPE_RGB4,"RGB4"},
	{MEDIASUBTYPE_RGB8,"RGB8"},
	{MEDIASUBTYPE_RGB565,"RGB565"},
	{MEDIASUBTYPE_RGB555,"RGB555"},
	{MEDIASUBTYPE_RGB24,"RGB24"},
	{MEDIASUBTYPE_RGB32,"RGB32"},
	{MEDIASUBTYPE_ARGB1555,"ARGB1555"},
	{MEDIASUBTYPE_ARGB4444,"ARGB4444"},
	{MEDIASUBTYPE_ARGB32,"ARGB32"},
	{MEDIASUBTYPE_A2R10G10B10,"A2R10G10B10"},
	{MEDIASUBTYPE_A2B10G10R10,"A2B10G10R10"},
	{MEDIASUBTYPE_AYUV,"AYUV"},
	{MEDIASUBTYPE_AI44,"AI44"},
	{MEDIASUBTYPE_IA44,"IA44"},
	{MEDIASUBTYPE_RGB32_D3D_DX7_RT,"RGB32_D3D_DX7_RT"},
	{MEDIASUBTYPE_RGB16_D3D_DX7_RT,"RGB16_D3D_DX7_RT"},
	{MEDIASUBTYPE_ARGB32_D3D_DX7_RT,"ARGB32_D3D_DX7_RT"},
	{MEDIASUBTYPE_ARGB4444_D3D_DX7_RT,"ARGB4444_D3D_DX7_RT"},
	{MEDIASUBTYPE_ARGB1555_D3D_DX7_RT,"ARGB1555_D3D_DX7_RT"},
	{MEDIASUBTYPE_RGB32_D3D_DX9_RT,"RGB32_D3D_DX9_RT"},
	{MEDIASUBTYPE_RGB16_D3D_DX9_RT,"RGB16_D3D_DX9_RT"},
	{MEDIASUBTYPE_ARGB32_D3D_DX9_RT,"ARGB32_D3D_DX9_RT"},
	{MEDIASUBTYPE_ARGB4444_D3D_DX9_RT,"ARGB4444_D3D_DX9_RT"},
	{MEDIASUBTYPE_ARGB1555_D3D_DX9_RT,"ARGB1555_D3D_DX9_RT"},
	{MEDIASUBTYPE_YV12,"YV12"},
	{MEDIASUBTYPE_NV12,"NV12"},
	{MEDIASUBTYPE_NV24,"NV24"},
	{MEDIASUBTYPE_IMC1,"IMC1"},
	{MEDIASUBTYPE_IMC2,"IMC2"},
	{MEDIASUBTYPE_IMC3,"IMC3"},
	{MEDIASUBTYPE_IMC4,"IMC4"},
	{MEDIASUBTYPE_S340,"S340"},
	{MEDIASUBTYPE_S342,"S342"},
	{MEDIASUBTYPE_Overlay,"Overlay"},
	{MEDIASUBTYPE_MPEG1Packet,"MPEG1Packet"},
	{MEDIASUBTYPE_MPEG1Payload,"MPEG1Payload"},
	{MEDIASUBTYPE_MPEG1AudioPayload,"MPEG1AudioPayload"},
	{MEDIATYPE_MPEG1SystemStream,"G1SystemStream"},
	{MEDIASUBTYPE_MPEG1System,"MPEG1System"},
	{MEDIASUBTYPE_MPEG1VideoCD,"MPEG1VideoCD"},
	{MEDIASUBTYPE_MPEG1Video,"MPEG1Video"},
	{MEDIASUBTYPE_MPEG1Audio,"MPEG1Audio"},
	{MEDIASUBTYPE_Avi,"Avi"},
	{MEDIASUBTYPE_Asf,"Asf"},
	{MEDIASUBTYPE_QTMovie,"QTMovie"},
	{MEDIASUBTYPE_QTRpza,"QTRpza"},
	{MEDIASUBTYPE_QTSmc,"QTSmc"},
	{MEDIASUBTYPE_QTRle,"QTRle"},
	{MEDIASUBTYPE_QTJpeg,"QTJpeg"},
	{MEDIASUBTYPE_PCMAudio_Obsolete,"PCMAudio_Obsolete"},
	{MEDIASUBTYPE_PCM,"PCM"},
	{MEDIASUBTYPE_WAVE,"WAVE"},
	{MEDIASUBTYPE_AU,"AU"},
	{MEDIASUBTYPE_AIFF,"AIFF"},
	{MEDIASUBTYPE_dvsd,"dvsd"},
	{MEDIASUBTYPE_dvhd,"dvhd"},
	{MEDIASUBTYPE_dvsl,"dvsl"},
	{MEDIASUBTYPE_dv25,"dv25"},
	{MEDIASUBTYPE_dv50,"dv50"},
	{MEDIASUBTYPE_dvh1,"dvh1"},
	{MEDIASUBTYPE_Line21_BytePair,"Line21_BytePair"},
	{MEDIASUBTYPE_Line21_GOPPacket,"Line21_GOPPacket"},
	{MEDIASUBTYPE_Line21_VBIRawData,"Line21_VBIRawData"},
	{MEDIASUBTYPE_708_608Data,"708_608Data"},
	{MEDIASUBTYPE_DtvCcData,"DtvCcData"},
	{MEDIASUBTYPE_TELETEXT,"TELETEXT"},
	{MEDIASUBTYPE_WSS,"WSS"},
	{MEDIASUBTYPE_VPS,"VPS"},
	{MEDIASUBTYPE_DRM_Audio,"DRM_Audio"},
	{MEDIASUBTYPE_IEEE_FLOAT,"IEEE_FLOAT"},
	{MEDIASUBTYPE_DOLBY_AC3_SPDIF,"DOLBY_AC3_SPDIF"},
	{MEDIASUBTYPE_RAW_SPORT,"RAW_SPORT"},
	{MEDIASUBTYPE_SPDIF_TAG_241h,"SPDIF_TAG_241h"},
	{MEDIASUBTYPE_DssVideo,"DssVideo"},
	{MEDIASUBTYPE_DssAudio,"DssAudio"},
	{MEDIASUBTYPE_VPVideo,"VPVideo"},
	{MEDIASUBTYPE_VPVBI,"VPVBI"},

	// WM
	// {WMMEDIASUBTYPE_RGB1, "RGB1"},
	// {WMMEDIASUBTYPE_RGB4, "RGB4"},
	// {WMMEDIASUBTYPE_RGB8, "RGB8"},
	// {WMMEDIASUBTYPE_RGB565, "RGB565"},
	// {WMMEDIASUBTYPE_RGB555, "RGB555"},
	// {WMMEDIASUBTYPE_RGB24, "RGB24"},
	// {WMMEDIASUBTYPE_RGB32, "RGB32"},
	{WMMEDIASUBTYPE_I420, "I420"},
	// {WMMEDIASUBTYPE_IYUV, "IYUV"},
	// {WMMEDIASUBTYPE_YV12, "YV12"},
	// {WMMEDIASUBTYPE_YUY2, "YUY2"},
	{WMMEDIASUBTYPE_P422, "P422"},
	// {WMMEDIASUBTYPE_UYVY, "UYVY"},
	// {WMMEDIASUBTYPE_YVYU, "YVYU"},
	// {WMMEDIASUBTYPE_YVU9, "YVU9"},
	{WMMEDIASUBTYPE_VIDEOIMAGE,  "VIDEOIMAGE"},
	{WMMEDIASUBTYPE_MP43, "MP43"},
	{WMMEDIASUBTYPE_MP4S, "MP4S"},
	{WMMEDIASUBTYPE_M4S2, "M4S2"},
	{WMMEDIASUBTYPE_WMV1, "WMV1"},
	{WMMEDIASUBTYPE_WMV2, "WMV2"},
	{WMMEDIASUBTYPE_MSS1, "MSS1"},
	{WMMEDIASUBTYPE_MPEG2_VIDEO, "MPEG2_VIDEO"},
	// {WMMEDIASUBTYPE_PCM, "PCM"},
	// {WMMEDIASUBTYPE_DRM, "DRM"},
	{WMMEDIASUBTYPE_WMAudioV9, "WMAudioV9"},
	{WMMEDIASUBTYPE_WMAudio_Lossless, "WMAudio_Lossless"},
	{WMMEDIASUBTYPE_MSS2, "MSS2"},
	{WMMEDIASUBTYPE_WMSP1, "WMSP1"},
	{WMMEDIASUBTYPE_WMSP2, "WMSP2"},
	{WMMEDIASUBTYPE_WMV3, "WMV3"},
	{WMMEDIASUBTYPE_WMVP, "WMVP"},
	{WMMEDIASUBTYPE_WVP2, "WVP2"},
	{WMMEDIASUBTYPE_WMVA, "WMVA"},
	{WMMEDIASUBTYPE_WVC1, "WVC1"},
	{WMMEDIASUBTYPE_WMAudioV8, "WMAudioV8"},
	// {WMMEDIASUBTYPE_WMAudioV7, "WMAudioV7"},
	// {WMMEDIASUBTYPE_WMAudioV2, "WMAudioV2"},
	{WMMEDIASUBTYPE_ACELPnet, "ACELPnet"},
	{WMMEDIASUBTYPE_MP3, "MP3"},
	{WMMEDIASUBTYPE_WebStream, "WebStream"}
};

static GuidTyps FormatDec[] =
{
	{FORMAT_VideoInfo, "VideoInfo"},
	{FORMAT_VideoInfo2, "VideoInfo2"},
	{FORMAT_WaveFormatEx, "WaveFormatEx"},
	{FORMAT_MPEGVideo, "MPEGVideo"},
	{FORMAT_MPEGStreams, "MPEGStreams"},
	{FORMAT_DvInfo, "DvInfo"},
	{FORMAT_525WSS, "525WSS"},
	{WMFORMAT_Script, "Script"},
	{WMFORMAT_WebStream, "WebStream"},
};

static void GetMediaTypeString(const GUID &format, DWORD cbFormat, void *pbFormat, char *szBuffer, int cbBuffer)
{
	if (format == FORMAT_VideoInfo)
	{
		if (cbFormat < sizeof(VIDEOINFOHEADER)) return;
		VIDEOINFOHEADER *p = (VIDEOINFOHEADER *)pbFormat;
		_snprintf_s_l(szBuffer, cbBuffer, cbBuffer, 
			"%d*%d, "
			"%d bits, "
			"biPlanes=%d; "
			"Compression=0x%x; "
			"biSizeImage=%d; "
			"biXPelsPerMeter=%d; "
			"biYPelsPerMeter=%d; "
			"biClrUsed=%d; "
			"biClrImportant=%d; "
			"rcSrc=(%d,%d,%d,%d), "
			"rcDst=(%d,%d,%d,%d), "
			"dwBitRate=%d, "
			"dwBitErrorRate=%d, "
			"AvgTimePerFrame=%I64d, ",
			NULL, 
			p->bmiHeader.biWidth, p->bmiHeader.biHeight,
			p->bmiHeader.biBitCount,
			p->bmiHeader.biPlanes,
			p->bmiHeader.biCompression,
			p->bmiHeader.biSizeImage,
			p->bmiHeader.biXPelsPerMeter,
			p->bmiHeader.biYPelsPerMeter,
			p->bmiHeader.biClrUsed,
			p->bmiHeader.biClrImportant,
			p->rcSource,
			p->rcTarget,
			p->dwBitRate,
			p->dwBitErrorRate,
			p->AvgTimePerFrame
			);
	}
	else if (format == FORMAT_VideoInfo2)
	{
		if (cbFormat < sizeof(VIDEOINFOHEADER2)) return;
		VIDEOINFOHEADER2 *p = (VIDEOINFOHEADER2 *)pbFormat;
		_snprintf_s_l(szBuffer, cbBuffer, cbBuffer, 
			"%d*%d, "
			"Aspect:%d*%d, "
			"%d bits, "
			"Compression=0x%x; "
			"rcSrc=(%d,%d,%d,%d), "
			"rcDst=(%d,%d,%d,%d), "
			"dwBitRate=%d, "
			"dwBitErrorRate=%d, "
			"AvgTimePerFrame=%I64d, ",
			NULL, 
			p->bmiHeader.biWidth, p->bmiHeader.biHeight,
			p->dwPictAspectRatioX,p->dwPictAspectRatioY,
			p->bmiHeader.biBitCount,
			p->bmiHeader.biCompression,
			p->rcSource,
			p->rcTarget,
			p->dwBitRate,
			p->dwBitErrorRate,
			p->AvgTimePerFrame
			);
	}
	else if (format == FORMAT_MPEGVideo)
	{
		if (cbFormat < sizeof(MPEG1VIDEOINFO)) return;
		MPEG1VIDEOINFO *p = (MPEG1VIDEOINFO *)pbFormat;
		_snprintf_s_l(szBuffer, cbBuffer, cbBuffer, 
			"%d*%d, "
			"%d bits, "
			"Compression=0x%x; "
			"rcSrc=(%d,%d,%d,%d), "
			"rcDst=(%d,%d,%d,%d), ", 
			NULL, 
			p->hdr.bmiHeader.biWidth, p->hdr.bmiHeader.biHeight,
			p->hdr.bmiHeader.biBitCount,
			p->hdr.bmiHeader.biCompression,
			p->hdr.rcSource,
			p->hdr.rcTarget
			);
	}
	else if (format == FORMAT_MPEG2Video)
	{
		if (cbFormat < sizeof(MPEG2VIDEOINFO)) return;
		MPEG2VIDEOINFO *p = (MPEG2VIDEOINFO *)pbFormat;
		_snprintf_s_l(szBuffer, cbBuffer, cbBuffer, 
			"%d*%d, "
			"Aspect:%d*%d, "
			"%d bits, "
			"Compression=0x%x; "
			"rcSrc=(%d,%d,%d,%d), "
			"rcDst=(%d,%d,%d,%d), ", 
			NULL, 
			p->hdr.bmiHeader.biWidth, p->hdr.bmiHeader.biHeight,
			p->hdr.dwPictAspectRatioX,p->hdr.dwPictAspectRatioY,
			p->hdr.bmiHeader.biBitCount,
			p->hdr.bmiHeader.biCompression,
			p->hdr.rcSource,
			p->hdr.rcTarget
			);
	}
	else if (format == FORMAT_WaveFormatEx)
	{
		if (cbFormat < sizeof(WAVEFORMATEX)) return;
		WAVEFORMATEX *p = (WAVEFORMATEX *)pbFormat;
		_snprintf_s_l(szBuffer, cbBuffer, cbBuffer, 
			"FormatTag=%d, "
			"%d Hz, "
			"%d Channels, "
			"%d Bits; "
			"AvgBytesPerSec=%d, "
			"BlockAlign=%d, "
			"Extra size=%d", 
			NULL, 
			p->wFormatTag,
			p->nSamplesPerSec,
			p->nChannels,
			p->wBitsPerSample,
			p->nAvgBytesPerSec,
			p->nBlockAlign,
			p->cbSize
			);
	}
}

static char* GetMediaTypeString(const WM_MEDIA_TYPE *pmt, char *szBuffer, int cbBuffer)
{
	if (pmt == NULL)
	{
		StringCchCopyA(szBuffer, cbBuffer, "Nil Pointer");
		goto label_exit;
	}

	if (pmt->majortype == GUID_NULL)
	{
		StringCchCopyA(szBuffer, cbBuffer, "任何类型");
		goto label_exit;
	}

	char *pBuffer = szBuffer;
	int l;

	for(int i = 0; i < ARRAY_SIZE(MajorDec); i++)
	{
		if (pmt->majortype == MajorDec[i].m_guid)
		{
			l = _snprintf_s_l(pBuffer, cbBuffer, cbBuffer, "Major Type:%s; ", NULL, MajorDec[i].m_pDec);
			cbBuffer -= l;
			pBuffer += l;
			goto label_sub;
		}
	}
	l = _snprintf_s_l(pBuffer, cbBuffer, cbBuffer, "Major Type:Unknow(%4.4s); ", NULL, &pmt->majortype);
	cbBuffer -= l;
	pBuffer += l;

label_sub:
	l = _snprintf_s_l(pBuffer, cbBuffer, cbBuffer, "bFixedSizeSamples=%d; bTemporalCompression=%d; lSampleSize=%d; cbFormat=%d; ", NULL, pmt->bFixedSizeSamples, pmt->bTemporalCompression, pmt->lSampleSize, pmt->cbFormat);
	cbBuffer -= l;
	pBuffer += l;

	if (pmt->subtype == GUID_NULL) goto label_format;
	for(int i = 0; i < ARRAY_SIZE(SubDec); i++)
	{
		if (pmt->subtype == SubDec[i].m_guid)
		{
			l = _snprintf_s_l(pBuffer, cbBuffer, cbBuffer, "Sub Type:%s; ", NULL, SubDec[i].m_pDec);
			cbBuffer -= l;
			pBuffer += l;
			goto label_format;
		}
	}

	l = _snprintf_s_l(pBuffer, cbBuffer, cbBuffer, "Sub Type:Unknow(%4.4s); ", NULL, &pmt->subtype);
	cbBuffer -= l;
	pBuffer += l;

label_format:
	if (pmt->formattype == GUID_NULL) goto label_exit;
	for(int i = 0; i < ARRAY_SIZE(FormatDec); i++)
	{
		if (pmt->formattype == FormatDec[i].m_guid)
		{
			l = _snprintf_s_l(pBuffer, cbBuffer, cbBuffer, "Format:%s; ", NULL, FormatDec[i].m_pDec);
			cbBuffer -= l;
			pBuffer += l;
			goto label_format_detail;
		}
	}

	l = _snprintf_s_l(pBuffer, cbBuffer, cbBuffer, "Format:Unknow(%4.4s); ", NULL, &pmt->formattype);
	cbBuffer -= l;
	pBuffer += l;

label_format_detail:
	if ((pmt->formattype != GUID_NULL) && (pmt->cbFormat))
	{
		l = _snprintf_s_l(pBuffer, cbBuffer, cbBuffer, "\n\t", NULL);
		cbBuffer -= l;
		pBuffer += l;

		GetMediaTypeString(pmt->formattype, pmt->cbFormat, pmt->pbFormat, pBuffer, cbBuffer);
	}

label_exit:
	return szBuffer;
}

void ShowMediaTypeString(char *header, const WM_MEDIA_TYPE *pmt)
{
	char szBuffer[2048];
	GetMediaTypeString(pmt, szBuffer, 1024);
	WriteLog("%hs:%hs\n", header, szBuffer);
}

wchar_t *GetAttributeString(WMT_ATTR_DATATYPE Type, const BYTE *pValue, WORD cbLength)
{
	static wchar_t szText[1024];
	switch(Type)
	{
	case WMT_TYPE_DWORD:
		StringCchPrintf(szText, 1024, L"%d", *((DWORD *)pValue));
		return szText;

	case WMT_TYPE_STRING:
		return (wchar_t *)pValue;

	case WMT_TYPE_BINARY:
		szText[0] = 0;
		for(int i = 0; i < cbLength; i++)
		{
			StringCchPrintf(szText + wcslen(szText), 8, L"%x ", pValue[i]);
		}
		return szText;

	case WMT_TYPE_BOOL:
		return *((BOOL *)pValue) ? L"TRUE" : L"FALSE";

	case WMT_TYPE_QWORD:
		StringCchPrintf(szText, 1024, L"%lld", *((QWORD *)pValue));
		return szText;

	case WMT_TYPE_WORD:
		StringCchPrintf(szText, 1024, L"%d", *((WORD *)pValue));
		return szText;

	case WMT_TYPE_GUID:
		StringFromGUID2(*((GUID *)pValue), szText, 128);
		return szText;

	default:
		return L"?";
	}
}

#endif
