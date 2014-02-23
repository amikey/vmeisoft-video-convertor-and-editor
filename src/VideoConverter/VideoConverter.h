#pragma once
#include "VideoConverterInt.h"

class CInfoDecoder;
class CMediaInfo: public MediaInfo
{
public:
	CInfoDecoder*	m_Decoder;
	int64_t			m_ImportTime;
	int64_t			m_ImportDuration;
	HWND			m_hwndMessage;
	volatile long	m_Abort;
	int				m_Ok;

public:
	CMediaInfo(HWND hwndMessage, wchar_t *szFileName);

public:
	int GetProgress();
	void OnStatusChanged(MediaStat stat);
	void OnStatusChangedSync(MediaStat stat);
};

/*****************************************************
编码参数：全局
*****************************************************/
struct AVPixFmtDescriptor;
struct CodecInfo:
	public CodecOptions
{
	// 以下部分内部使用数据
	int32_t				m_EncPixFormat;
	int32_t				m_EncVideoPlanes;
	int32_t				m_EncVideoHeight[4];
	int32_t				m_EncVideoStride[4];
	int32_t				m_EncVideoChromaH;

	int32_t				m_EncAudioFormat;
	int32_t				m_EncAudioSampleRate;
	int32_t				m_EncAudioChannels;
	uint64_t			m_EncAudioChannelLayout;
	int32_t				m_EncAudioIsPlanar;
	int32_t				m_EncAudioPacketBytes;

	volatile long m_EncInfo;
};


/*****************************************************
 Structures
 *****************************************************/
struct VideoStreamPacket
{
	int32_t		m_IsKey;
	int64_t		m_Postion;
	int64_t		m_Pts;
};

#define AUDIO_BUFFER_SEC			4

struct AudioStream
{
	volatile long	m_Buffered;  // 缓冲中音频的长度
	volatile long	m_Eof;       // End of Stream
	uint8_t*		m_Buffers[8];
};

#define MAX_VIDEO_BUFFER			32
#define MIN_VIDEO_BUFFER			30
#define MAX_VIDEO_QUEUE				1024
#define MIN_VIDEO_QUEUE				1000

struct VideoStream
{
	volatile long	m_Buffered; // 被使用的缓冲数量
	volatile long	m_Queued; // 被使用的缓冲数量
	volatile long	m_Eof;	   // End of Stream
	volatile long	m_Queue[MAX_VIDEO_QUEUE]; // 队列中的个数，仅仅用于编码
	uint8_t*		m_Buffers[MAX_VIDEO_BUFFER][4];
};

#define VIDEO_FILTER_COUNT		6

class CVideoFilterBase;
class CVideoFilterData
{
public:
	GUID	m_id;
	int32_t		m_Size;
	void*	m_Parameter;
	CVideoFilterBase* m_Instance;

public:
	CVideoFilterData(){};
	~CVideoFilterData();

public:
	BOOL IsDefaultValue();
	CVideoFilterBase* CreateInstance();
};

class CInfoDecoder;
class CVideoPlayer;
class CSubtitle;

struct ClipInfo:
	public VCItem
{
public:
	CVideoPlayer*		m_VideoPlayer;

public:
	volatile long		m_AbortFlag;
	volatile long		m_Processing;

private:
	CSubtitle*			m_Subtitle;

public:
	void SetSubtitle(CSubtitle* subtitle);
	inline CSubtitle* GetSubtitle(){return m_Subtitle;};
	
public:
	CVideoFilterData	m_Filters[VIDEO_FILTER_COUNT];
	HRESULT GetVideoEffect(GUID id, void *Parameters, int32_t cbSize);
	HRESULT SetVideoEffect(GUID id, void *Parameters, int32_t cbSize);

public:
	void OnStatusChanged(VCItemStat stat);
	void OnStatusChangedSync(VCItemStat stat);

public:
	ClipInfo(MediaInfo *info, HWND hwndMessage);
	~ClipInfo();

public:
	BOOL HaveAudioStream();
	BOOL SimpleConvert();

public:
	void Release();
};

extern VCGlobal g_opt;
extern CodecInfo g_enc_opt;

extern void CalculateClipRects(ClipInfo *p, Rect &srcRect, Rect &dstRect);
extern Rect IntersectRect(Rect rectDest, Rect rectSource, Point &point);
extern void GetClipRotateSize(ClipInfo *p, int32_t &width, int32_t &height);

#define CM_MESSAGE_BASE		(WM_USER + 200)
#define CM_FORCE_DRAW	(CM_MESSAGE_BASE + 1)
