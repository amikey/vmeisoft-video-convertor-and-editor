#include "stdafx.h"
#include "CropFilter.h"
#include "VideoUtils.h"
#include "VideoPlayer.h"

CVideoFilterBase* CCropFilter::CreateInstance(VideoFilterTemplate *pTemplate, CVideoFilterData *pData)
{
	CCropFilter *filter = new CCropFilter(pTemplate, pData);
	return filter;
}

CCropFilter::CCropFilter(VideoFilterTemplate *pTemplate, CVideoFilterData *pData)
	:CVideoFilterBase(pTemplate, pData)
{
	m_Crop = (Rect *)pData->m_Parameter;
	m_FilterType = VIDEO_FILTER_TYPE_CROP;
}

CCropFilter::~CCropFilter(void)
{
	SAFE_FREE(m_Buffer);
}

void CCropFilter::PutSourceSize(int32_t width, int32_t height)
{
	if (
		(m_Crop->Width == 0) || 
		(m_Crop->Height == 0) ||
		((m_Crop->Width == width) && (m_Crop->Height == height))
		)
	{
		m_Crop->X = 0;
		m_Crop->Y = 0;
		m_Crop->Width = 0;
		m_Crop->Height = 0;
	}
	else
	{
		m_SourceOffset = (m_Crop->Y * width + m_Crop->X) * 4;
		m_SourceStride = width * 4;
		width = m_Crop->Width;
		height = m_Crop->Height;
		SAFE_FREE(m_Buffer);
		m_Buffer = (uint8_t *)MemoryAlloc(m_Crop->Width * m_Crop->Height * 4);
		m_DestStride = m_Crop->Width * 4;
	}
	m_next->PutSourceSize(width, height);
}

uint8_t* CCropFilter::GetBuffer()
{
	if (m_Buffer) return m_Buffer;
	return CVideoFilterBase::GetBuffer();
}

HRESULT CCropFilter::SetParameters(void *Param, int32_t length)
{
	// 不支持动态修改
	assert(0); 
	return E_FAIL;
}

void CCropFilter::Process(uint8_t *source, int64_t Pts)
{
	if (m_Buffer)
	{
		uint8_t* dest = m_next->GetBuffer();
		copy_32bpp(dest, m_DestStride, source + m_SourceOffset, m_SourceStride, m_Crop->Width, m_Crop->Height);
		m_next->Process(dest, Pts);
	}
	else
	{
		m_next->Process(source, Pts);
	}
}

void __stdcall vcVideoPlayerCrop(CVideoPlayer *player, Rect *crop)
{
	player->SetCrop(crop);
}

void __stdcall vcVideoPlayerCropStop(CVideoPlayer *player)
{
	player->RemoveCrop();
}
