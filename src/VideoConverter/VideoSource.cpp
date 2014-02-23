#include "stdafx.h"
#include "VideoConverter.h"
#include "VideoSource.h"
#include "./ffmpeg/ffmpeg.inc.h"

// {F4C0DB4D-97E6-4C69-A550-C33D6C735862}
static const GUID IID_VideoStream = 
{ 0xf4c0db4d, 0x97e6, 0x4c69, { 0xa5, 0x50, 0xc3, 0x3d, 0x6c, 0x73, 0x58, 0x62 } };
static struct VideoFilterTemplate g_video_stream_template = {L"Video Player", IID_VideoStream, NULL, VET_UTILITY, NULL, 0};

// {8A5EBD08-CCCA-4832-A0BD-D7B6DBF0BAE2}
static const GUID IID_VideoSource = 
{ 0x8a5ebd08, 0xccca, 0x4832, { 0xa0, 0xbd, 0xd7, 0xb6, 0xdb, 0xf0, 0xba, 0xe2 } };
static struct VideoFilterTemplate g_video_source_template = {L"Video Player", IID_VideoSource, NULL, VET_UTILITY, NULL, 0};

/****************************************************************
* CVideoStream
****************************************************************/
CVideoStream::CVideoStream()
	:CVideoFilterBase(&g_video_stream_template, NULL)
{
	m_FilterType = VIDEO_FILTER_TYPE_RENDER + 1;
}

void CVideoStream::Initialize()
{
	m_frame_num = AV_TIME_BASE * g_enc_opt.m_FrameNum;
	m_frame_den = g_enc_opt.m_FrameDen;
	m_frame_size = avpicture_get_size((enum AVPixelFormat)g_enc_opt.m_EncPixFormat, g_enc_opt.m_VideoWidth, g_enc_opt.m_VideoHeight);
}

void CVideoStream::Process(uint8_t *source, int64_t Pts)
{
	while (m_Buffered >= MAX_VIDEO_BUFFER)
	{
		Sleep(1);
	}
	if (source != m_Buffers[m_BufferPoint][0])
	{
		CopyMemory(m_Buffers[m_BufferPoint][0], source, m_frame_size);
	}

	while (m_next_pts <= Pts)
	{
		while (m_Queued >= MAX_VIDEO_QUEUE)
		{
			Sleep(1);
		}
		m_Queue[m_QueuePoint] = m_BufferPoint;
		m_QueuePoint = (m_QueuePoint + 1) & (MAX_VIDEO_QUEUE - 1);
		InterlockedIncrement(&m_Queued);
		m_frame_count ++;
		m_next_pts = m_start_pts + m_frame_count * m_frame_num / m_frame_den;
	}

	m_BufferPoint = (m_BufferPoint + 1) & (MAX_VIDEO_BUFFER - 1);
	InterlockedIncrement(&m_Buffered);
}

uint8_t* CVideoStream::GetBuffer()
{
	return m_Buffers[m_BufferPoint][0];
}

uint8_t **CVideoStream::GetStreamBuffer()
{
	return m_Buffers[m_BufferPoint];
}

void CVideoStream::NewSegment(int64_t tStart, int64_t tStop)
{
}

void CVideoStream::PutSourceSize(int32_t width, int32_t height)
{
	assert(width == g_enc_opt.m_VideoWidth);
	assert(height == g_enc_opt.m_VideoHeight);
}

void CVideoStream::EndOfStream()
{
	m_EndOfStream = 1;
	
	int last = (m_BufferPoint - 1) & (MAX_VIDEO_BUFFER - 1);
	while (m_next_pts < m_stop_pts)
	{
		while (m_Queued >= MAX_VIDEO_QUEUE)
		{
			Sleep(1);
		}
		m_Queue[m_QueuePoint] = last;
		m_QueuePoint = (m_QueuePoint + 1) & (MAX_VIDEO_QUEUE - 1);
		InterlockedIncrement(&m_Queued);
		m_frame_count ++;
		m_next_pts = m_start_pts + m_frame_count * m_frame_num / m_frame_den;
	}
}

void CVideoStream::FilterRelease()
{
}

/****************************************************************
* CVideoSource
****************************************************************/
CVideoSource::CVideoSource(CVideoStream *stream)
	:CVideoFilterBase(&g_video_source_template, NULL)
{
	m_FilterType = VIDEO_FILTER_TYPE_RENDER;
	m_pStream = stream;

	sws_context = sws_getContext(
		g_enc_opt.m_VideoWidth, g_enc_opt.m_VideoHeight, AV_PIX_FMT_BGRA,
		g_enc_opt.m_VideoWidth, g_enc_opt.m_VideoHeight, (enum AVPixelFormat )g_enc_opt.m_EncPixFormat,
		SWS_POINT, NULL, NULL, NULL);

	m_VideoBuffer = (uint8_t *)MemoryAlloc(g_enc_opt.m_VideoWidth * g_enc_opt.m_VideoHeight * 4);

}

CVideoSource::~CVideoSource()
{
	if (sws_context)
	{
		sws_freeContext(sws_context);
		sws_context = NULL;
	}
	SAFE_FREE(m_VideoBuffer);
}

uint8_t* CVideoSource::GetBuffer()
{
	return m_VideoBuffer;
}

void CVideoSource::Process(uint8_t *source, int64_t Pts)
{
	uint8_t *srcSlice[4] = {source};
	int32_t srcStride[4] = {g_enc_opt.m_VideoWidth * 4};
	uint8_t **dst = m_pStream->GetStreamBuffer();
	sws_scale(sws_context, srcSlice, srcStride, 0, g_enc_opt.m_VideoHeight, dst, g_enc_opt.m_EncVideoStride);
	m_pStream->Process(*dst, Pts);
}

void CVideoSource::NewSegment(int64_t tStart, int64_t tStop)
{
}

void CVideoSource::EndOfStream()
{
}

void CVideoSource::PutSourceSize(int32_t width, int32_t height)
{
	assert(width == g_enc_opt.m_VideoWidth);
	assert(height == g_enc_opt.m_VideoHeight);
}
