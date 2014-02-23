#include "stdafx.h"
#include "VideoConverter.h"
#include "VideoUtils.h"
#include "VideoFilter.h"
#include "VideoSource.h"
#include "VideoEncoder.h"

/*****************************************************
// CVideoEncoder
*****************************************************/
CVideoEncoder::CVideoEncoder(struct MediaInfo* minfo)
{
	m_MediaInfo = minfo;
	m_dstStride[0] = m_MediaInfo->m_Width * 4;
	m_srcSliceH = m_MediaInfo->m_CodecHeight;
	if (
		(m_MediaInfo->m_CodecWidth != m_MediaInfo->m_Width) ||
		(m_MediaInfo->m_CodecHeight != m_MediaInfo->m_Height) ||
		(m_MediaInfo->m_PixelFormat != AV_PIX_FMT_BGRA)
		)
	{
		sws_context = sws_getContext(
			m_MediaInfo->m_CodecWidth, m_MediaInfo->m_CodecHeight, (enum AVPixelFormat)m_MediaInfo->m_PixelFormat,
			m_MediaInfo->m_Width, m_MediaInfo->m_Height, AV_PIX_FMT_BGRA,
			SWS_BICUBIC, NULL, NULL, NULL);
	}
}

CVideoEncoder::~CVideoEncoder()
{
	if (sws_context)
	{
		sws_freeContext(sws_context);
		sws_context = NULL;
	}
}

void CVideoEncoder::OnVideoStreamStart(int64_t pts)
{
}

void CVideoEncoder::OnAudioStreamStart(int64_t pts)
{
	assert(0);
}

void CVideoEncoder::OnVideoStream(int64_t pts, AVFrame *frame)
{
	if (pts < *m_NextTime)
	{
		return;
	}
	if ((m_StopTime) && (pts > m_StopTime))
	{
		m_pFilterManager->EndOfStream();
		return ;
	}
	
	uint8_t *dst[4] = {m_pFilterManager->GetBuffer()};
	if (sws_context)
	{
		sws_scale(sws_context, frame->data, frame->linesize, 0, m_srcSliceH, dst, m_dstStride);
	}
	else
	{
		uint8_t *p1 = dst[0];
		uint8_t *p2 = frame->data[0];
		int32_t w1 = m_dstStride[0];
		int32_t w2 = frame->linesize[0];
		int32_t w = w1 < w2 ? w1 : w2;
		for(int i = 0; i < m_srcSliceH; i++)
		{
			memcpy(p1, p2, w);
			p1 += w1;
			p2 += w1;
		}
	}
	m_pFilterManager->DoFilter(dst[0], pts);
}

void CVideoEncoder::OnAudioStream(AVFrame *frame)
{
	assert(0);
}

int32_t CVideoEncoder::OnVideoPacket()
{
	assert(0);
	return 0;
}

void CVideoEncoder::OnEndOfStream()
{
	m_pFilterManager->EndOfStream();
}

/*****************************************************
// CSimpleVideoEncoder
*****************************************************/
CSimpleVideoEncoder::CSimpleVideoEncoder(struct MediaInfo* minfo, CVideoStream *stream)
{
	m_MediaInfo = minfo;
	m_Stream = stream;

	Rect srcRect, dstRect;
	GetResizeMethodRect(g_opt.m_ResizeMethod, g_enc_opt.m_VideoWidth, g_enc_opt.m_VideoHeight, m_MediaInfo->m_Width, m_MediaInfo->m_Height, srcRect, dstRect);
	dstRect.Width &= ~3;
	dstRect.Height &= ~3;
	dstRect.X = (g_enc_opt.m_VideoWidth - dstRect.Width) / 2;
	dstRect.Y = (g_enc_opt.m_VideoHeight - dstRect.Height) / 2;
	dstRect.X &= ~1;
	dstRect.Y &= ~1;

	if ((dstRect.X != 0) || (dstRect.Y != 0) || (dstRect.Width != g_enc_opt.m_VideoWidth) || (dstRect.Height != g_enc_opt.m_VideoHeight))
	{
		uint8_t *buffer = (uint8_t *)MemoryAlloc(g_enc_opt.m_VideoWidth * g_enc_opt.m_VideoHeight * 4);
		m_FrameSize = avpicture_get_size((enum AVPixelFormat)g_enc_opt.m_EncPixFormat, g_enc_opt.m_VideoWidth, g_enc_opt.m_VideoHeight);
		m_DstBlank = (uint8_t *)MemoryAlloc(m_FrameSize);
		
		DraweBackgroundSample(buffer);

		struct SwsContext* swsContext = sws_getContext(
			g_enc_opt.m_VideoWidth, g_enc_opt.m_VideoHeight, AV_PIX_FMT_BGRA,
			g_enc_opt.m_VideoWidth, g_enc_opt.m_VideoHeight, (enum AVPixelFormat)g_enc_opt.m_EncPixFormat,
			SWS_BICUBIC, NULL, NULL, NULL);

		uint8_t *srcSlice[4] = {buffer};
		const int32_t srcStride[4] = {g_enc_opt.m_VideoWidth * 4};
		AVPicture dstBlank;
		avpicture_fill(&dstBlank, buffer, (enum AVPixelFormat)g_enc_opt.m_EncPixFormat, g_enc_opt.m_VideoWidth, g_enc_opt.m_VideoHeight);
		sws_scale(swsContext, srcSlice, srcStride, 0, g_enc_opt.m_VideoHeight, dstBlank.data, dstBlank.linesize);
		sws_freeContext(swsContext);
		av_free(buffer);
	}
	
	if ((dstRect.X != 0) || (dstRect.Y != 0))
	{
		for(int i = 0; i < g_enc_opt.m_EncVideoPlanes; i++)
		{
			if (i == 0)
			{
				m_Offsets[i] = dstRect.Y * g_enc_opt.m_EncVideoStride[i];
			}
			else
			{
				m_Offsets[i] = (dstRect.Y >> g_enc_opt.m_EncVideoChromaH) * g_enc_opt.m_EncVideoStride[i];
			}
			m_Offsets[i] += dstRect.X * g_enc_opt.m_EncVideoStride[i] / g_enc_opt.m_VideoWidth;
		}
	}

	m_srcSliceH = m_MediaInfo->m_CodecHeight;
	sws_context = sws_getContext(
		m_MediaInfo->m_CodecWidth, m_MediaInfo->m_CodecHeight, (enum AVPixelFormat)m_MediaInfo->m_PixelFormat,
		dstRect.Width, dstRect.Height, (enum AVPixelFormat)g_enc_opt.m_EncPixFormat,
		SWS_BICUBIC, NULL, NULL, NULL);
}

CSimpleVideoEncoder::~CSimpleVideoEncoder()
{
	if (sws_context)
	{
		sws_freeContext(sws_context);
		sws_context = NULL;
	}
	SAFE_FREE(m_DstBlank);
}

void CSimpleVideoEncoder::OnVideoStreamStart(int64_t pts)
{
}

void CSimpleVideoEncoder::OnAudioStreamStart(int64_t pts)
{
	assert(0);
}

void CSimpleVideoEncoder::OnVideoStream(int64_t pts, AVFrame *frame)
{
	if (pts < *m_NextTime)
	{
		return;
	}
	if ((m_StopTime) && (pts > m_StopTime))
	{
		OnEndOfStream();
		return ;
	}

	uint8_t *dstBuffer[4] = {};
	uint8_t **dst = m_Stream->GetStreamBuffer();
	for(int i = 0; i < g_enc_opt.m_EncVideoPlanes; i++)
	{
		dstBuffer[i] = dst[i] + m_Offsets[i];
	}
	if (m_cBlank < MAX_VIDEO_BUFFER)
	{
		CopyMemory(dst[0], m_DstBlank, m_FrameSize);
		m_cBlank ++;
	}

	sws_scale(sws_context, frame->data, frame->linesize, 0, m_srcSliceH, dstBuffer, g_enc_opt.m_EncVideoStride);
	m_Stream->Process(*dst, pts);
}

void CSimpleVideoEncoder::OnAudioStream(AVFrame *frame)
{
	assert(0);
}

int32_t CSimpleVideoEncoder::OnVideoPacket()
{
	assert(0);
	return 0;
}

void CSimpleVideoEncoder::OnEndOfStream()
{
	m_Stream->EndOfStream();
}
