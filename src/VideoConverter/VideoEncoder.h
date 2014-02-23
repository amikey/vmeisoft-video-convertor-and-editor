#pragma once
#include ".\ffmpeg\Decoder\dec.h"
class CVideoFilterManager;
class CVideoSource;

class CVideoEncoder:public CBaseDecoder
{
private:
	struct MediaInfo*		m_MediaInfo;

public:
	int64_t					m_StopTime;
	int64_t*				m_NextTime;
	struct SwsContext*		sws_context;
	CVideoFilterManager*	m_pFilterManager;
	int32_t					m_dstStride[4];
	int32_t					m_srcSliceH;

public:
	CVideoEncoder(struct MediaInfo* minfo);
	~CVideoEncoder();

public:
	virtual void OnVideoStreamStart(int64_t pts);
	virtual void OnAudioStreamStart(int64_t pts);
	virtual void OnVideoStream(int64_t pts, AVFrame *frame);
	virtual void OnAudioStream(AVFrame *frame);
	virtual int32_t OnVideoPacket();
	virtual void OnEndOfStream();
};

class CSimpleVideoEncoder:
	public CBaseDecoder
{
private:
	struct MediaInfo*		m_MediaInfo;

public:
	int64_t					m_StopTime;
	int64_t*				m_NextTime;
	struct SwsContext*		sws_context;
	CVideoStream*			m_Stream;
	int32_t					m_dstStride[4];
	int32_t					m_srcSliceH;

private:
	uint8_t*				m_DstBlank;
	int						m_Offsets[4];
	int						m_cBlank;
	int						m_FrameSize;

public:
	CSimpleVideoEncoder(struct MediaInfo* minfo, CVideoStream *stream);
	~CSimpleVideoEncoder();

public:
	virtual void OnVideoStreamStart(int64_t pts);
	virtual void OnAudioStreamStart(int64_t pts);
	virtual void OnVideoStream(int64_t pts, AVFrame *frame);
	virtual void OnAudioStream(AVFrame *frame);
	virtual int32_t OnVideoPacket();
	virtual void OnEndOfStream();
};
