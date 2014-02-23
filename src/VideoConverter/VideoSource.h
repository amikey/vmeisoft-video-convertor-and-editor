#pragma once
#include "VideoFilters.h"
#include "VideoFilter.h"

class CVideoStream: 
	public VideoStream,
	public CVideoFilterBase
{
private:
	int64_t		m_frame_num;
	int64_t		m_frame_den;
	int64_t		m_frame_count;
	int64_t		m_frame_size;
	int			m_BufferPoint;
	int			m_QueuePoint;

public:
	int64_t		m_start_pts;
	int64_t		m_stop_pts;
	int64_t		m_next_pts;
	int			m_EndOfStream;

public:
	CVideoStream();
	void Initialize();

public:
	virtual void NewSegment(int64_t tStart, int64_t tStop); 
	virtual void EndOfStream(); 
	virtual void PutSourceSize(int32_t width, int32_t height);
	virtual uint8_t* GetBuffer();
	virtual void Process(uint8_t *source, int64_t Pts);
	virtual void FilterRelease();

public:
	uint8_t **GetStreamBuffer();
};

class CVideoSource:
	public CVideoFilterBase
{
private:
	uint8_t*				m_VideoBuffer;
	struct SwsContext*		sws_context;
	CVideoStream*			m_pStream;

public:
	CVideoSource(CVideoStream *stream);
	~CVideoSource();

public:
	virtual void NewSegment(int64_t tStart, int64_t tStop); 
	virtual void EndOfStream(); 
	virtual void PutSourceSize(int32_t width, int32_t height);
	virtual uint8_t* GetBuffer();
	virtual void Process(uint8_t *source, int64_t Pts);
};
