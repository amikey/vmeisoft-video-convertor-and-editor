#pragma once

#include <stdint.h>

#define VC_MESSAGE_BASE (WM_USER + 256)

/*****************************************************
时间定义：1/1000000秒
*****************************************************/
#ifndef AV_TIME_BASE_LL
#define AV_TIME_BASE_LL            	1000000ll
#endif

/*
struct Point
{
	int32_t X;
	int32_t Y;
};

struct Size
{
	int32_t Width;
	int32_t Height;
};

struct Rect
{
	int32_t X;
	int32_t Y;
	int32_t Width;
	int32_t Height;
};

struct PointF
{
	float X;
	float Y;
};

struct SizeF
{
	float Width;
	float Height;
};

struct RectF
{
	float X;
	float Y;
	float Width;
	float Height;
};
*/

/*****************************************************
// Resize Method
*****************************************************/
enum ResizeMethod
{
	ResizeMethod_LetterBox, // 0
	ResizeMethod_Crop, // 1
	ResizeMethod_Center, // 2
	ResizeMethod_Fill, // 3
};

/*****************************************************
背景类型
*****************************************************/
enum FillType
{
	FillType_Solid,
	FillType_Gradient,
	FillType_Picture,
};

/*****************************************************
填充方向
*****************************************************/
enum FillDirect
{
	FillDirect_Top,
	FillDirect_Left,
	FillDirect_TopLeft,
	FillDirect_TopRight,
};


/*****************************************************
保存图像的格式
*****************************************************/
enum GrabFileType
{
	GrabFileType_PNG,
	GrabFileType_JPG,
	GrabFileType_BMP,
};

/*****************************************************
水印对齐方式
*****************************************************/
enum LogoAlignment
{
    LogoAlignmentNear = 0, // 最近位置：左边，上边
    LogoAlignmentCenter = 1, // 居中
    LogoAlignmentFar = 2, // 最远位置：右边，下边
	LogoAlignmentNone = 3 // 任意
};


/*****************************************************
全局的参数：系统基本信息设置
*****************************************************/

struct VCGlobal
{
	// 默认输出路径
	wchar_t			m_OutputPath[MAX_PATH];

	// 并行转换的数量
	int32_t			m_Parallel;

	// 拉伸方式设置
	ResizeMethod	m_ResizeMethod;

	// 背景：默认为黑色背景
	FillType		m_FillType; // 背景填充类型
	uint32_t		m_FillColor1; // 背景色1
	uint32_t		m_FillColor2; // 背景色2
	FillDirect		m_FillDirect; // 背景色填充方向
	wchar_t			m_FillPicture[MAX_PATH]; // 图片的路径，当背景为图片时有效

	// 图形抓取时的默认格式
	GrabFileType	m_GrabFileType;

	// (保留) 水印
	wchar_t			m_LogoFile[MAX_PATH]; // 水印文件名称:如果为空下面的设置都忽略
	Size			m_LogoFileSize; // 原始图片尺寸
	Rect			m_LogoRect; // 水印文件的位置，如果不是任意位置；X，Y的值会被忽略
	int32_t			m_LogoAlpha; // 水印文件的透明度：0-不透明，255-全透明；取值在0～240之间(>240没意义)，其他值作为不透明处理
	int32_t			m_LogoHPosition; // 水印的水平位置对齐方式；LogoAlignment
	int32_t			m_LogoVPosition; // 水印的垂直位置对齐方式；LogoAlignment
};

// 返回VCGlobal的字节数，你应该调用这个函数来检查应用程序定义的结构体长度和底层定义的结构体长度是否一致
extern int32_t __stdcall VCGlobal_Size();

// 返回全局参数的指针
extern VCGlobal* __stdcall vcGetGlobal();

/*****************************************************
编码参数：全局
*****************************************************/
struct CodecOptions
{
	wchar_t			m_Format[64]; // 输出文件格式: 必填参数
	wchar_t			m_Target[64]; // 类型
	wchar_t			m_ExtName[64]; // 扩展名称后缀

	wchar_t			m_VideoCodec[64]; // 视频编码器名称
	int32_t			m_VideoBitrate; // 编码后的比特率，如果为0，则为VBR
	int32_t			m_FrameNum, m_FrameDen; // 编码后的帧率
	int32_t			m_VideoWidth, m_VideoHeight; // 视频的宽和高
	int32_t			m_VideoDisable; // 不编码视频，仅生成音频

	wchar_t			m_AudioCodec[64]; // 音频编码器名称
	int32_t			m_AudioBitrate; // 编码后的比特率，如果为0，则为VBR
	int32_t			m_AudioSampleRate; // 编码后的采样率
	int32_t			m_AudioChannels; // 编码后的通道数量
	int32_t			m_AudioDisable; // 不编码视频，仅生成音频

	volatile BOOL	m_Pause; // 如果为非0，会导致转换暂停，不会立即停止，可能会延时一两秒
};

// 返回CodecOptions的字节数，你应该调用这个函数来检查应用程序定义的结构体长度和底层定义的结构体长度是否一致
extern int32_t __stdcall CodecOptions_Size();

// 返回全局编码参数的指针
extern CodecOptions* __stdcall vcGetCodecOptions();

/*****************************************************
媒体信息定义
*****************************************************/
// 状态
enum MediaStat
{
	MediaStat_None = 0, // 
	MediaStat_Importing = 10, // 正在导入媒体信息
	MediaStat_Error_File = 11, // 导入媒体信息错误
	MediaStat_Check_Media = 12, // 已经成功获取媒体信息，正在检查媒体文件；应当处理进度
	MediaStat_Thumbnail_Generated = 13, // 已经成功生成缩略图
	MediaStat_Complete = 14, // 完成
};

struct MediaInfo
{
	GUID			m_Id; // 唯一标识
	MediaStat		m_Status; // 

	// 文件流信息
	wchar_t			m_szFileName[MAX_PATH]; // 文件路径

	int64_t			m_Duration; // 总长度
	int64_t			m_StreamDuration; // *(内部使用) 流长度
	wchar_t			m_FormtDesc[128]; // 文件格式

	// 视频信息
	int32_t			m_bVideoStream; // 是否存在视频
	wchar_t			m_VideoCodec[128]; // 音频格式名称
	int64_t			m_VideoStartPts; // *(内部使用) 视频开始时间
	int32_t			m_Width; // 视频宽
	int32_t			m_Height;// 视频高
	int32_t			m_CodecWidth; // 视频宽
	int32_t			m_CodecHeight;// 视频高
	int32_t			m_FrameRateNum; // 帧率
	int32_t			m_FrameRateDen; // 帧率
	int32_t			m_PixelFormat; // * 内部数据
	int32_t			m_VideoBitRate; // 视频比特率
	int64_t			m_VideoDuration; // 视频长度

	// 音频信息
	int32_t			m_bAudioStream; // 是否存在音频
	wchar_t			m_AudioCodec[128]; // 视频格式名称
	int64_t			m_AudioStartPts; // *(内部使用) 音频开始时间
	int32_t			m_SampleRate; // 音频采样
	int32_t			m_nChannel; // 音频通道数量
	uint64_t		m_channel_layout; // *(内部使用) 音频通道排列方式
	int32_t			m_SampleFormat; // *(内部使用)音频格式
	int32_t			m_AudioBitRate; // 音频比特率
	int64_t			m_AudioDuration; // 音频长度
	int32_t			m_IsPlanar; // *(内部使用)音频是否为平面模式

	// 其他
	wchar_t			m_Demo[MAX_PATH]; // 示例图:BMP

	//文件信息
	FILETIME		m_ftCreationTime; // 创建时间
	LARGE_INTEGER	m_liFileSize; // 文件大小
	FILETIME		m_History; // 信息导入时间

	// 内部数据
	wchar_t			m_AudioTmpFile[8][MAX_PATH];
	int64_t			m_AudioLength; // Samples

};

// 返回MediaInfo的字节数，你应该调用这个函数来检查应用程序定义的结构体长度和底层定义的结构体长度是否一致
extern int32_t __stdcall MediaInfo_Size();


/*****************************************************
转换项目定义
*****************************************************/

// 状态
enum VCItemStat
{
	ClipStat_None = 0, // 
	ClipStat_Ready_For_Convert = 20, // 一般状态
	ClipStat_Wait_For_Convert = 21, // 等待转换队列中
	ClipStat_Start_Convert = 22, // 开始转换
	ClipStat_Converting = 23, // 正在转换；应当处理进度
	ClipStat_Converted = 24, // 转换结束；
	ClipStat_Failed = 25, // 转换失败；
	ClipStat_Abort = 26 // 转换被取消；
};


// 当对视频进行剪切时，应该调整开始时间和结束时间
struct AudioTrack
{
	struct MediaInfo* m_Media; // 媒体信息
	int64_t m_MediaStart; // 开始时间：相对于媒体文件的剪切
	int64_t m_MediaStop; // 结束时间：相对于媒体文件的剪切；如果为0表示直到结束
	int64_t m_Start; // 开始时间：相对于视频
	int64_t m_Stop; // 结束时间：相对于视频
	int32_t	m_Volume; // 音量设置：1-150; <100:减少音量； 100:原始音量； >100放大音量（可能会导致问题）

public:
	AudioTrack();
};

struct VCItem
{
	// 基本的
	GUID				m_ID; // 唯一标识
	wchar_t				m_Name[256]; // 名称
	wchar_t				m_OutFile[MAX_PATH]; // 输出文件路径

	// 媒体信息:视频部分
	struct MediaInfo*	m_MediaInfo;

	// 音频部分：这个在视频编辑中有效
	// !!!! 轨道一：自身，如果视频中没有音频流，该参数无效；开始时间和结束时间会被忽略（总是和视频同步）；m_Media=m_MediaInfo
	// 轨道二：背景音乐
	// 轨道三：录音
	// 当有多个音轨时应当减少每个音轨的音量
	struct AudioTrack	m_AudioTrack[3]; // 最多支持两条音轨，包含原始音量，最多可能出现三个声音

	// Clip / Trim
	int64_t				m_ClipStart; // 开始时间
	int64_t				m_ClipStop; // 结束时间，如果为0表示播放到结尾

	// Speed
	int32_t				m_Speed; // 0-原始速度播放； >0: 增加1/100的速度，如100为两倍的速度； <0; 减少1/100的速度，如-50为1/2的速度，不能<=100

	// 消息处理
	VCItemStat			m_Status; // Item的状态
	HWND				m_hwndMessage; // 用于接收状态改变的消息的窗口
	volatile int64_t	m_Time; //  当前已经完成的时长
	int64_t				m_Duration; //  转换后的时长
	void*				m_UserData; // 用户自定义数据，可以存放任何内容
};

struct ClipInfo; //ClipInfo = VCItem;

/*****************************************************
消息定义
*****************************************************/

// 返回MediaInfo的字节数，你应该调用这个函数来检查应用程序定义的结构体长度和底层定义的结构体长度是否一致
extern int32_t __stdcall VCItem_Size();

/*****************************************************
初始化
*****************************************************/
extern HRESULT __stdcall vcInitialize(); // 调用其他函数之前这个函数应该被调用
extern void __stdcall vcUninitialize(); // 应用程序退出之前应该被调用

/*****************************************************
导入媒体
*****************************************************/

// 导入状态发生改变
// wParam:没有使用
// lParam:状态：MediaStat
#define VC_IMPORT_STATUS	(VC_MESSAGE_BASE + 1)

// 参数：
//	hwndMessage:接收消息的窗口，消息参考前面的定义
//  szFile:文件路径
// 描述：
//	该函数立即返回，可以多次同时导入多个文件； 后台进行导入。
//  请参考消息VC_ITEM_STAT_CHANGED
//  进度定义在 m_Time / m_Duration
extern MediaInfo* __stdcall vcImportMediaInfo(HWND hwndMessage, wchar_t *szFile);

// 释放媒体信息
// 仅仅需要对录音信息使用
extern void __stdcall MediaInfoFree(MediaInfo* info);

// 终止导入
class CMediaInfo; // CMediaInfo = MediaInfo
extern void __stdcall vcImportAbort(CMediaInfo *info);

// 获取进度
// 返回百分比
extern int __stdcall vcImportGetProgress(CMediaInfo *info);

/*****************************************************
对ITEM的操作
*****************************************************/
extern ClipInfo* __stdcall vcCreateItem(HWND hwndMessage, MediaInfo *info); // 新建一个转换项目
extern void __stdcall vcDeleteItem(ClipInfo *item); // 删除，删除后索引将改变


/*****************************************************
视频播放
*****************************************************/

// 播放进度事件
// wParam:微秒 1/AV_TIME_BASE_LL
// lParam:秒数
// 总时间:lParam * AV_TIME_BASE_LL + wParam 
#define VC_VIDEO_RENDER_PROGRESS		(VC_MESSAGE_BASE + 10)

// 播放完成事件
// wParam:未定义
// lParam:未定义
#define VC_VIDEO_RENDER_END				(VC_MESSAGE_BASE + 11)

// 播放模式定义
enum VideoPlayerMode
{
	VC_VIDEO_RENDER_MODE_SOURCE, // 播放原始的视频
	VC_VIDEO_RENDER_MODE_DEST, // 播放转换后的效果
};

// 创建
// 自动居中
class CVideoPlayer;
extern CVideoPlayer* __stdcall vcCreateVideoPlayer(HWND hwndParent, HWND hwndMessage, ClipInfo* item, VideoPlayerMode Mode);

// 销毁播放窗口
extern void __stdcall vcVideoPlayerDestroy(CVideoPlayer *player);

// 保存当前图像到文件:文件格式定义在m_GrabFileType
extern HRESULT __stdcall vcVideoPlayerGrab(CVideoPlayer *player, wchar_t *szFile);

// 重新调整播放窗口
// 异步执行
extern void __stdcall vcVideoPlayerResize(CVideoPlayer *player);

// 设置开始时间和结束时间
// 仅当播放原始视频时有效
extern void __stdcall vcVideoPlayerStartStop(CVideoPlayer *player, int64_t start, int64_t stop);

// 播放与暂停
// 如果播放到结尾处vcVideoPlayerResume将重头开始播放
// 异步执行
extern void __stdcall vcVideoPlayerResume(CVideoPlayer *player);
extern void __stdcall vcVideoPlayerPause(CVideoPlayer *player);

// Seek之后处于暂停状态
// 异步执行
extern void __stdcall vcVideoPlayerSeekTo(CVideoPlayer *player, int64_t pts);

// 使用下列函数会导致暂停
extern void __stdcall vcVideoPlayerNextFrame(CVideoPlayer *player); // 下一帧, 返回当前秒数，如果返回-1表示失败
extern void __stdcall vcVideoPlayerNextTime(CVideoPlayer *player); // 下一秒, 返回当前秒数，如果返回-1表示失败
extern void __stdcall vcVideoPlayerPreTime(CVideoPlayer *player); // 上一秒, 返回当前秒数，如果返回-1表示失败

// 返回当前的时间：1/AV_TIME_BASE_LL秒
extern int64_t __stdcall vcVideoPlayerCurrentTime(CVideoPlayer *player);

// 强制重绘制
extern void __stdcall vcVideoPlayerForceRedraw(CVideoPlayer *player);


//*****************************************************
// 音频处理部分
// 当播放器存在是必须调用下面的函数使其生效, 不可以直接修改
// 如果不在播放状态（应该不会），可以直接修改VCItem的参数
//******************************************************

// 设置背景音乐
// index 必须等于1或者2
// 如果是录音文件，应该先设置完成后删除先前的录音文件:MediaInfoFree
// 如果info=NULL, 意味着删除背景音乐
extern void __stdcall vcVideoPlayerSetMusic(CVideoPlayer *player, int32_t index, MediaInfo *info);

// 设置音量
extern void __stdcall vcVideoPlayerSetVolume(CVideoPlayer *player, int32_t index, int32_t Volume);

/*****************************************************
录音支持
*****************************************************/

// 音量示例
// wParam:未使用
// lParam:音量：0～100
#define VC_RECORDING_SAMPLE		(VC_MESSAGE_BASE + 40)

// 已经录制的长度
// wParam:1/1000000秒，秒的剩余部分
// lParam:已经录制的长度(秒)
#define VC_RECORDING_DURATION	(VC_MESSAGE_BASE + 41)

class CAudioRecorder;
// 开始录制
// 如果StartTime!=0, 会在前面填充静音
extern CAudioRecorder* __stdcall AudioRecorderCreate(HWND hwndMessage); 
// 获取媒体信息
// 如果要废弃录音，应用程序应该删除：MediaInfoFree
extern MediaInfo* __stdcall AudioRecorderGetInfo(CAudioRecorder *recorder);
// 暂停录制
extern void __stdcall AudioRecorderPause(CAudioRecorder *recorder);
// 继续录制
// 如果和视频同步录制，暂停会导致音频和视频不同步，应该重新Seek视频
extern void __stdcall AudioRecorderResume(CAudioRecorder *recorder);
// 销毁
extern void __stdcall AudioRecorderDestroy(CAudioRecorder *recorder);

// 辅助
// 喇叭静音
// bMute=0不静音，否则静音，在录制过程中应该静音，否则会干扰麦克风
extern HRESULT __stdcall SpeakerMute(int32_t bMute);

/*****************************************************
开始转换
*****************************************************/

// 当状态改变是发送次消息
// wParam:指向VCItem的指针
// lParam:新状态
#define VC_ITEM_STAT_CHANGED		(VC_MESSAGE_BASE + 1)

//***************************************************
// 在转换未结束/未取消之前不得进行任何形式的修改（可以进行删除）
// 否则会导致未知结果
//***************************************************

// 在开始转换之前必须调用这个函数
extern void __stdcall vcReadyForConvert(); 

// 开始转换
// 转换的进度在m_Time中定义
// 如果m_Parallel != 1 则应当多个文件将同时进行
//  请参考消息VC_ITEM_STAT_CHANGED
//  进度定义在 m_Time / m_Duration
extern HRESULT __stdcall vcStartConvert(ClipInfo *item); 

// 取消转换，未转换完成的文件将会被删除
extern void __stdcall vcAbortConvert(ClipInfo *item); 

/*****************************************************
开始合并
*****************************************************/

//***************************************************
// 在合并未结束/未取消之前不得进行任何形式的修改和删除，
// 否则会导致未知结果
//***************************************************

struct MergeInfo
{
	wchar_t				m_Name[256]; // 生成的文件名称
	wchar_t				m_OutFile[MAX_PATH]; // 输出文件全路径
	HWND				m_hwndMessage; // 用于接收完成消息的窗口
	volatile int64_t	m_Time; //  当前已经完成的时长
	int64_t				m_Duration; //  转换后的时长
};

// 返回CodecOptions的字节数，你应该调用这个函数来检查应用程序定义的结构体长度和底层定义的结构体长度是否一致
extern int32_t __stdcall MergeInfo_Size();

// 返回全局编码参数的指针
extern MergeInfo* __stdcall vcGetMergeInfo();

// 合并完成
// wParam:没有使用
// lParam:如果为非0表示失败; -99表示被取消
#define VC_MERGE_COMPLETE	(VC_MESSAGE_BASE + 30)

// 开始合并
// items:指向item的数组； ClipInfo **
// count:合并的个数，不应该小于2
// szOutputFile: 输出文件名称
// hwndMessage：接收消息的窗口
// Time, Duration:用于处理进度, Time-当前已经编码的长度， Duration-总长度
extern HRESULT __stdcall vcStartMerge(ClipInfo *items[], int32_t count); 

// 取消合并，未转换完成的文件将会被删除
extern void __stdcall vcAbortMerge(); 

/*****************************************************
其他，辅助函数
*****************************************************/
// 返回媒体文件的描述信息
// buffer: 字符串缓冲，不少于256字符
extern wchar_t * __stdcall MakeMediaInfoString(MediaInfo* info, wchar_t *buffer);

// 将选择的Profile的信息填充到CodecOptions中
// 请参考Profile的部分
extern void __stdcall CopyProfileSettings();

// 生成一张背景图片
// buffer: CodecOptions.m_VideoWidth * CodecOptions.m_VideoHeight * 4字节
extern void __stdcall DraweBackgroundSample(uint8_t *buffer);

// 绘制水印效果
extern void __stdcall DrawLogoSample(BYTE *Buffer);

// CPU核心数
extern int32_t __stdcall GetCpuCoreCount();

/*****************************************************
其他，辅助函数
*****************************************************/
extern int32_t __stdcall GetMultipleInatance(); // 返回当前运行的实例个数
extern HWND __stdcall GetMainWindowHandle(); // 获取上一个实例的主窗口
extern void __stdcall SetMainWindowHandle(HWND hwndMain); // 设置这次实例的主窗口
extern void __stdcall WriteErrorLog(char *info); // 写日志

