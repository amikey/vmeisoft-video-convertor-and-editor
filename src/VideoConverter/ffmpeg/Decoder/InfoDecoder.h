#pragma once
#include "dec.h"

class CInfoDecoder:
	public CBaseDecoder,
	public CDecoder
{
public:
	volatile long*		m_pAbort;
	CMediaInfo*			m_MediaInfo;

public:
	CInfoDecoder(CMediaInfo* info);
	virtual ~CInfoDecoder();

private:
	BOOL		m_EndOfStream;

private:
	int32_t		m_AudioFiles;
	int32_t		m_AudioSampleBytes;
	HANDLE		m_AudioTmpFile[8];
	HANDLE		m_AudioData;

private:
	int64_t		m_AudioTotalSampes;
	int64_t		max_video_pts;
	int64_t		demo_video_pts;
	int64_t		frame_num;

public:
	void CreateAudioFiles();

public:
	void CopyMediaInfo1();
	void CopyMediaInfo2();

public:
	virtual void OnVideoStreamStart(int64_t pts);
	virtual void OnAudioStreamStart(int64_t pts);
	virtual void OnVideoStream(int64_t pts, AVFrame *frame);
	virtual void OnAudioStream(AVFrame *frame);
	virtual int32_t OnVideoPacket();
	virtual void OnEndOfStream();

public:
	void Start();
};

extern CInfoDecoder* CreateInfoDecoder(CMediaInfo* info);
