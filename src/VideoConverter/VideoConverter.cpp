#include "stdafx.h"
#include "VideoConverter.h"
#include "VideoFilter.h"
#include "VideoPlayer.h"
#include "VideoFilter.h"
#include "MediaMananger.h"
#include "VideoUtils.h"
#include "VideoFilters.h"
#include "BackgroundFilter.h"
#include ".\ffmpeg\Decoder\InfoDecoder.h"
#include "Subtitle.h"

VCGlobal		g_opt;
CodecInfo		g_enc_opt;

extern VideoFilterTemplate VideoFilterTemplates[VIDEO_FILTER_COUNT];

int CMediaInfo::GetProgress()
{
	if (m_Status != MediaStat_Check_Media) return 0;
	int Percentage = m_ImportTime * 100 / m_ImportDuration;
	if (Percentage > 100) Percentage = 100;
	return Percentage;
}

void CMediaInfo::OnStatusChanged(MediaStat stat)
{
	m_Status = stat;
	PostMessage(m_hwndMessage, VC_IMPORT_STATUS, (WPARAM)this, stat);
}

void CMediaInfo::OnStatusChangedSync(MediaStat stat)
{
	m_Status = stat;
	SendMessage(m_hwndMessage, VC_IMPORT_STATUS, (WPARAM)this, stat);
}

CVideoFilterData::~CVideoFilterData()
{
	SAFE_FREE(m_Parameter);
}

AudioTrack::AudioTrack()
{
	m_Volume = 100;
}

ClipInfo::ClipInfo(MediaInfo *info, HWND hwndMessage)
{
	
	m_AudioTrack[0].m_Media = info;
	m_MediaInfo = info;
	m_hwndMessage = hwndMessage;

	wchar_t Name[MAX_PATH];
	StringCchCopy(Name, MAX_PATH, info->m_szFileName);
	PathStripPath(Name);
	PathRemoveExtension(Name);
	StringCchCopy(m_Name, MAX_PATH, Name);
	for(int i = 0; i < VIDEO_FILTER_COUNT; i++)
	{
		m_Filters[i].m_id = VideoFilterTemplates[i].m_ClsID;
		m_Filters[i].m_Size = VideoFilterTemplates[i].m_cbParameter;
		if (VideoFilterTemplates[i].m_cbParameter)
		{
			m_Filters[i].m_Parameter = MemoryAlloc(VideoFilterTemplates[i].m_cbParameter);
			CopyMemory(m_Filters[i].m_Parameter, VideoFilterTemplates[i].m_pParameter, VideoFilterTemplates[i].m_cbParameter);
		}
		else
		{
			m_Filters[i].m_Parameter = NULL;
		}
	}

	m_Subtitle = new CSubtitle();
}

ClipInfo::~ClipInfo()
{
	for(int i = 0; i < VIDEO_FILTER_COUNT; i++)
	{
		SAFE_FREE(m_Filters[i].m_Parameter);
	}
	delete m_Subtitle;
}

void ClipInfo::Release()
{
	SAFE_DELETE (m_VideoPlayer);
	m_AbortFlag = 1;
	while (m_Processing)
	{
		Sleep(10);
	}
	delete this;
}

void ClipInfo::OnStatusChanged(VCItemStat stat)
{
	m_Status = stat;
	PostMessage(m_hwndMessage, VC_ITEM_STAT_CHANGED, (WPARAM)this, stat);
}

void ClipInfo::OnStatusChangedSync(VCItemStat stat)
{
	m_Status = stat;
	SendMessage(m_hwndMessage, VC_ITEM_STAT_CHANGED, (WPARAM)this, stat);
}

HRESULT ClipInfo::GetVideoEffect(GUID id, void *Parameters, int32_t cbSize)
{
	for(int i = 0; i < VIDEO_FILTER_COUNT; i++)
	{
		if (m_Filters[i].m_id == id)
		{
			assert(cbSize >= m_Filters[i].m_Size);
			CopyMemory(Parameters, m_Filters[i].m_Parameter, m_Filters[i].m_Size);
			return S_OK;
		}
	}
	return vcGetVideoEffectDefaultValue(id, Parameters, cbSize);
}

HRESULT ClipInfo::SetVideoEffect(GUID id, void *Parameters, int32_t cbSize)
{
	// Crop
	if (id == IID_Crop)
	{
		if (cbSize < sizeof(Rect)) return E_FAIL;
		Rect *pRect = (Rect *)Parameters;
		if ((pRect->X == 0) && (pRect->Y == 0) && (pRect->Width == m_MediaInfo->m_Width) && (pRect->Height == m_MediaInfo->m_Height))
		{
			static Rect rectEmpty;
			Parameters = &rectEmpty;
		}
	}

	for(int i = 0; i < VIDEO_FILTER_COUNT; i++)
	{
		if (m_Filters[i].m_id == id)
		{
			if (cbSize < m_Filters[i].m_Size)
			{
				assert(0);
				return E_FAIL;
			}
			if (m_VideoPlayer)
			{
				assert(m_Filters[i].m_Instance);
				return m_VideoPlayer->SetFilterParameter(&m_Filters[i], Parameters);
			}
			else if (m_Filters[i].m_Instance)
			{
				// 不支持动态修改
				assert(0);
				// return m_Filters[i].m_Instance->SetParameters(Parameters, cbSize);
				return E_FAIL;
			}
			else
			{
				CopyMemory(m_Filters[i].m_Parameter, Parameters, m_Filters[i].m_Size);
				return S_OK;
			}
		}
	}

	assert(0);
	return E_FAIL;
}

BOOL ClipInfo::HaveAudioStream()
{
	if ((m_AudioTrack[0].m_Volume) && (m_MediaInfo->m_bAudioStream)) return TRUE;
	if ((m_AudioTrack[1].m_Media) && (m_AudioTrack[1].m_Media))  return TRUE;
	if ((m_AudioTrack[2].m_Media) && (m_AudioTrack[2].m_Media))  return TRUE;
	return FALSE;
}

BOOL ClipInfo::SimpleConvert()
{
	Rect srcRect, dstRect;
	GetResizeMethodRect(g_opt.m_ResizeMethod, g_enc_opt.m_VideoWidth, g_enc_opt.m_VideoHeight, m_MediaInfo->m_Width, m_MediaInfo->m_Height, srcRect, dstRect);
	if ((srcRect.X) || (srcRect.Y) || (srcRect.Width != m_MediaInfo->m_Width) || (srcRect.Height != m_MediaInfo->m_Height)) return FALSE;
	if (g_opt.m_LogoFile[0]) return false;
	for(int i = 0; i < VIDEO_FILTER_COUNT; i++)
	{
		if (m_Filters[i].m_id == IID_Background) continue;
		if (m_Filters[i].IsDefaultValue()) continue ;
		return FALSE;
	}
	if (m_Subtitle->m_Count) return FALSE;
	return TRUE; 
}

void ClipInfo::SetSubtitle(CSubtitle* subtitle)
{
	if (m_VideoPlayer) m_VideoPlayer->SetSubtitle(subtitle);
	delete m_Subtitle;
	m_Subtitle = subtitle;
}