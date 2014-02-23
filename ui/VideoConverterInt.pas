{$WARN SYMBOL_PLATFORM OFF}
{$MINENUMSIZE 4}
unit VideoConverterInt;

interface

uses
    Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics, Vcl.Controls, Vcl.Forms,
    Vcl.Dialogs, Vcl.ImgList, Vcl.Menus, Vcl.StdCtrls, Vcl.ExtCtrls, Vcl.OleCtrls, SHDocVw, Vcl.ComCtrls, Vcl.Buttons;

function VCGlobal_Size(): Integer; stdcall;
function MediaInfo_Size(): Integer; stdcall;
function VCItem_Size(): Integer; stdcall;

const
    VC_MESSAGE_BASE = (WM_USER + 256);

    (* ****************************************************
      时间定义：1/1000000秒
      **************************************************** *)
const
    AV_TIME_BASE_LL: Int64 = 1000000;

type
    CPoint = Record
        X, Y: Integer;
    end;

    CSize = Record
        Width, Height: Integer;
    end;

    CRect = Record
        X, Y, Width, Height: Integer;
    end;

    CPointF = Record
        X, Y: Single;
    end;

    CSizeF = Record
        Width, Height: Single;
    end;

    CRectF = Record
        X, Y, Width, Height: Single;
    end;

    (* ****************************************************
      // Resize Method
      **************************************************** *)
const
    ResizeMethod_LetterBox = 0; // 0
    ResizeMethod_Crop      = 1; // 1
    ResizeMethod_Center    = 2; // 2
    ResizeMethod_Fill      = 3; // 3

    (* ***************************************************
      背景类型
      *************************************************** *)
const
    FillType_Solid    = 0;
    FillType_Gradient = 1;
    FillType_Picture  = 2;

    (* ****************************************************
      填充方向
      **************************************************** *)
const
    FillDirect_Top      = 0;
    FillDirect_Left     = 1;
    FillDirect_TopLeft  = 2;
    FillDirect_TopRight = 3;

    (* ****************************************************
      图片类型
      **************************************************** *)
const
    GrabFileType_PNG = 0;
    GrabFileType_JPG = 1;
    GrabFileType_BMP = 2;

    (* ****************************************************
      水印对齐方式
      **************************************************** *)
const
    LogoAlignmentNear   = 0; // 最近位置：左边，上边
    LogoAlignmentCenter = 1; // 居中
    LogoAlignmentFar    = 2; // 最远位置：右边，下边
    LogoAlignmentNone   = 3; // 任意

    (* ****************************************************
      全局的参数：系统基本信息设置
      **************************************************** *)
type
    TFilePath = Array [0 .. MAX_PATH - 1] of WideChar;

    VCGlobal = record

        // 默认输出路径
        m_OutputPath: TFilePath;

        // 并行转换的数量
        m_Parallel: Int32;

        // 拉伸方式设置
        m_ResizeMethod: Integer;

        // 背景：默认为黑色背景
        m_FillType: Integer;      // 背景填充类型
        m_FillColor1: Int32;      // 背景色1
        m_FillColor2: Int32;      // 背景色2
        m_FillDirect: Integer;    // 背景色填充方向
        m_FillPicture: TFilePath; // 图片的路径，当背景为图片时有效

        // 图形抓取时的默认格式
        m_GrabFileType: Integer;

        // (保留) 水印
        m_LogoFile: TFilePath; // 水印文件名称:如果为空下面的设置都忽略
        m_LogoFileSize: CSize; // 原始图片尺寸
        m_LogoRect: CRect;     // 水印文件的位置
        m_LogoAlpha: Int32;    // 水印文件的透明度：0-不透明，100-全透明；取值在1～99之间，其他值作为不透明处理
        m_LogoHPosition: Int32;
        m_LogoVPosition: Int32;

    end;

    PVCGlobal = ^VCGlobal;

    // 返回全局参数的指针
function vcGetGlobal(): PVCGlobal; stdcall;

(* ****************************************************
  编码参数：全局
  **************************************************** *)
type
    CodecOptions = record

        m_Format: array [0 .. 63] of WideChar;  // 输出文件格式: 必填参数
        m_Target: array [0 .. 63] of WideChar;  // 类型
        m_ExtName: array [0 .. 63] of WideChar; // 扩展名称后缀

        m_VideoCodec: array [0 .. 63] of WideChar; // 视频编码器名称
        m_VideoBitrate: Int32;                     // 编码后的比特率，如果为0，则为VBR
        m_FrameNum, m_FrameDen: Integer;           // 编码后的帧率
        m_VideoWidth, m_VideoHeight: Integer;      // 视频的宽和高
        m_VideoDisable: Int32;                     // 不编码视频，仅生成音频

        m_AudioCodec: array [0 .. 63] of WideChar; // 音频编码器名称
        m_AudioBitrate: Int32;                     // 编码后的比特率，如果为0，则为VBR
        m_AudioSampleRate: Int32;                  // 编码后的采样率
        m_AudioChannels: Int32;                    // 编码后的通道数量
        m_AudioDisable: Int32;                     // 不编码视频，仅生成音频

        m_Pause: BOOL;
    end;

    PCodecOptions = ^CodecOptions;

    // 返回CodecOptions的字节数，你应该调用这个函数来检查应用程序定义的结构体长度和底层定义的结构体长度是否一致
function CodecOptions_Size(): Integer; stdcall;

// 返回全局编码参数的指针
function vcGetCodecOptions(): PCodecOptions; stdcall;

(* ****************************************************
  媒体信息定义
  **************************************************** *)
const

    MediaStat_None                = 0;  //
    MediaStat_Importing           = 10; // 正在导入媒体信息
    MediaStat_Error_File          = 11; // 导入媒体信息错误
    MediaStat_Check_Media         = 12; // 已经成功获取媒体信息，正在检查媒体文件；应当处理进度
    MediaStat_Thumbnail_Generated = 13; // 已经成功生成缩略图
    MediaStat_Complete            = 14; // 完成

type
    MediaInfo = record

        m_ID: TGUID;     // 唯一标识
        m_Status: Int32; //

        m_szFileName: TFilePath; // 文件路径

        m_Duration: Int64;                             // 长度
        m_StreamDuration: Int64;                       // 流长度
        m_FormtDesc: array [0 .. 128 - 1] of WideChar; // 文件格式

        m_bVideoStream: Int32;                          // 是否存在视频
        m_VideoCodec: array [0 .. 128 - 1] of WideChar; // 音频解码器名称
        m_VideoStartPts: Int64;                         // * 内部数据
        m_Width: Int32;                                 // 视频宽
        m_Height: Int32;                                // 视频高
        m_CodecWidth: Int32;                            // 视频宽
        m_CodecHeight: Int32;                           // 视频高
        m_FrameRateNum: Int32;                          // 帧率
        m_FrameRateDen: Int32;                          // 帧率
        m_PixelFormat: Int32;                           // * 内部数据
        m_VideoBitrate: Integer;                        // 视频比特率
        m_VideoDuration: Int64;                         // 视频长度

        m_bAudioStream: Int32;                          // 是否存在音频
        m_AudioCodec: array [0 .. 128 - 1] of WideChar; // 视频解码器名称
        m_AudioStartPts: Int64;                         // *(内部使用) 音频开始时间
        m_SampleRate: Int32;                            // 音频采样
        m_nChannel: Int32;                              // 音频通道数量
        m_channel_layout: Int64;                        // * 内部数据
        m_SampleFormat: Int32;                          // * 内部数据
        m_AudioBitrate: Integer;                        // 音频比特率
        m_AudioDuration: Int64;                         // 音频长度
        m_IsPlanar: Int32;                              // 内部数据

        // 其他
        m_Demo: TFilePath; // 示例图:BMP

        // 文件信息
        m_ftCreationTime: FILETIME;  // 创建时间
        m_liFileSize: LARGE_INTEGER; // 文件大小
        m_History: FILETIME;         // 信息导入时间

        // 内部数据
        m_AudioTmpFile: array [0 .. 7] of TFilePath;
        m_AudioLength: Int64;

    end;

    PMediaInfo = ^MediaInfo;

    (* ****************************************************
      转换项目定义
      **************************************************** *)
const
    // 状态
    ClipStat_None              = 0;  //
    ClipStat_Ready_For_Convert = 20; // 一般状态
    ClipStat_Wait_For_Convert  = 21; // 等待转换队列中
    ClipStat_Start_Convert     = 22; // 开始转换
    ClipStat_Converting        = 23; // 正在转换；应当处理进度
    ClipStat_Converted         = 24; // 转换结束；
    ClipStat_Failed            = 25; // 转换失败；
    ClipStat_Abort             = 26; // 转换被取消；

type
    AudioTrack = record
        m_Media: PMediaInfo; // 媒体信息
        m_MediaStart: Int64; // 开始时间：媒体剪切
        m_MediaStop: Int64;  // 结束时间：媒体剪切；如果为0表示直到结束
        m_Start: Int64;      // 开始时间：相对于视频
        m_Stop: Int64;       // 结束时间：相对于视频
        m_Volume: Int32;     // 音量设置：1-150; <100:减少音量； 100:原始音量； >100放大音量（可能会导致问题）
    end;

    PAudioTrack = ^AudioTrack;

    VCItem = record

        m_ID: TGUID;
        m_Name: Array [0 .. 255] of WideChar;
        m_OutFile: TFilePath; // 输出文件路径

        m_MediaInfo: PMediaInfo;
        m_AudioTrack: Array [0 .. 2] of AudioTrack;

        // Clip / Trim
        m_ClipStart: Int64; // 开始时间
        m_ClipStop: Int64;  // 结束时间，如果为0表示播放到结尾

        // Speed
        m_Speed: Int32; // 0-原始速度播放； >0: 增加1/100的速度，如100为两倍的速度； <0; 减少1/100的速度，如-50为1/2的速度，不能<=100

        // 消息处理
        m_Status: Int32;     // Item的状态
        m_hwndMessage: HWND; // 用于接收状态改变的消息的窗口
        m_Time: Int64;       // 处理进度
        m_Duration: Int64;   // 出来长度
        m_UserData: Pointer; // 用户自定义数据，可以存放任何内容

    end;

    PVCItem = ^VCItem;
    (* ****************************************************
      消息定义
      **************************************************** *)

    // 导入状态改变
    // wParam:指向VCItem的指针
    // lParam:新状态
const
    VC_ITEM_STAT_CHANGED = (VC_MESSAGE_BASE + 1);

    (* ****************************************************
      初始化
      **************************************************** *)
function vcInitialize(): HRESULT; stdcall; // 调用其他函数之前这个函数应该被调用
procedure vcUninitialize(); stdcall;       // 应用程序退出之前应该被调用

(* ****************************************************
  导入媒体
  **************************************************** *)

// 导入状态发生改变
// wParam:没有使用
// lParam:状态：MediaStat
const
    VC_IMPORT_STATUS = (VC_MESSAGE_BASE + 1);

    // 参数：
    // hwndMessage:接收消息的窗口，消息参考前面的定义
    // szFile:文件路径
    // 描述：
    // 该函数立即返回，可以多次同时导入多个文件； 后台进行导入。
    // 请参考消息VC_ITEM_STAT_CHANGED
    // 进度定义在 m_Time / m_Duration
function vcImportMediaInfo(hwndMessage: HWND; szFile: PWideChar): PMediaInfo; stdcall;

// 终止导入
procedure vcImportAbort(info: PMediaInfo); stdcall;

// 获取进度
// 返回百分比
function vcImportGetProgress(info: PMediaInfo): Int32; stdcall;

(* ****************************************************
  对ITEM的操作
  **************************************************** *)

function vcCreateItem(hwndMessage: HWND; info: PMediaInfo): PVCItem; stdcall;
procedure vcDeleteItem(item: PVCItem); stdcall; // 删除，删除后索引将改变

(* ****************************************************
  视频播放
  **************************************************** *)
const
    // 播放进度事件
    // wParam:Item
    // lParam:当前进度，单位为秒
    VC_VIDEO_RENDER_PROGRESS = (VC_MESSAGE_BASE + 10);

    // 播放完成事件
    // wParam:Item
    // lParam:未定义
    VC_VIDEO_RENDER_END = (VC_MESSAGE_BASE + 11);

    // 播放模式定义
type
    VideoPlayerMode =
      (
      VC_VIDEO_RENDER_MODE_SOURCE, // 播放原始的视频
      VC_VIDEO_RENDER_MODE_DEST    // 播放转换后的效果
      );

    // 创建
    // 自动居中
function vcCreateVideoPlayer(hwndParent, hwndMessage: HWND; item: Pointer; Mode: VideoPlayerMode): Pointer; stdcall;

// 保存当前图像到文件:文件格式定义在m_GrabFileType
function vcVideoPlayerGrab(player: Pointer; szFile: PWideChar): HRESULT; stdcall;

// 销毁播放窗口
procedure vcVideoPlayerDestroy(player: Pointer); stdcall;

// 重新调整播放窗口
// 异步执行
procedure vcVideoPlayerResize(player: Pointer); stdcall;

// 设置开始时间和结束时间
// 仅当播放原始视频时有效
procedure vcVideoPlayerStartStop(player: Pointer; start, stop: Int64); stdcall;

// 播放与暂停
// 如果播放到结尾处vcVideoPlayerResume将重头开始播放
// 异步执行
procedure vcVideoPlayerResume(player: Pointer); stdcall;
procedure vcVideoPlayerPause(player: Pointer); stdcall;

// Seek
// vcVideoPlayerSeekStart / vcVideoPlayerSeekStop: 用于快速连续的播放的优化，如果只是一次性的SEEK则可以直接调用vcVideoPlayerSeekTo
// Seek之后处于暂停状态
// 异步执行
procedure vcVideoPlayerSeekTo(player: Pointer; pts: Int64); stdcall;
function vcVideoPlayerCurrentTime(player: Pointer): Int64; stdcall;

// 使用下列函数应该先暂停
procedure vcVideoPlayerNextFrame(player: Pointer); stdcall; // 下一帧
procedure vcVideoPlayerPreTime(player: Pointer); stdcall;   // 上一秒
procedure vcVideoPlayerNextTime(player: Pointer); stdcall;  // 下一秒

procedure vcVideoPlayerForceRedraw(player: Pointer); stdcall;

// *****************************************************
// 音频处理部分
// 当播放器存在是必须调用下面的函数使其生效, 不可以直接修改
// 如果不在播放状态（应该不会），可以直接修改VCItem的参数
// ******************************************************

// 设置背景音乐
// index 必须等于1或者2
// 如果是录音文件，应该先设置完成后删除先前的录音文件:MediaInfoFree
// 如果info=NULL, 意味着删除背景音乐
procedure vcVideoPlayerSetMusic(player: Pointer; index: Int32; info: PMediaInfo); stdcall;

// 设置音量
procedure vcVideoPlayerSetVolume(player: Pointer; index: Int32; Volume: Int32); stdcall;

(* ****************************************************
  开始转换
  **************************************************** *)

procedure vcReadyForConvert(); stdcall;

// 开始转换
// 转换的进度在m_ConvertTime中定义
// 如果m_Parallel != 1 则多个文件将同时进行
// 转换未结束之前，不得修改任何参数
// 请参考消息VC_ITEM_STAT_CHANGED
// 进度定义在 m_Time / m_Duration
function vcStartConvert(item: PVCItem): HRESULT; stdcall;

// 取消转换，未转换完成的文件将会被删除
procedure vcAbortConvert(item: PVCItem); stdcall;

(* ****************************************************
  开始合并
  **************************************************** *)

type
    MergeInfo = record
        m_Name: Array [0 .. 255] of WideChar; // 生成的文件名称
        m_OutFile: TFilePath;                 // 输出文件全路径
        m_hwndMessage: HWND;                  // 用于接收状态改变的消息的窗口
        m_Time: Int64;                        // 当前已经完成的时长
        m_Duration: Int64;                    // 转换后的时长
    end;

    PMergeInfo = ^MergeInfo;

const
    // 合并完成
    // wParam:指向VCItem的指针
    // lParam:如果为非0表示失败
    VC_MERGE_COMPLETE = (VC_MESSAGE_BASE + 30);

function MergeInfo_Size(): Integer; stdcall;
function vcGetMergeInfo(): PMergeInfo; stdcall;

// 开始合并
function vcStartMerge(var item: PVCItem; Count: Integer): HRESULT; stdcall;

// 取消合并，未转换完成的文件将会被删除
procedure vcAbortMerge(); stdcall;

// 辅助函数
function MakeMediaInfoString(info: PMediaInfo; var buffer: array of WideChar): PWideChar; stdcall;
function GetMediaString(info: PMediaInfo): String;
procedure CopyProfileSettings(); stdcall;
function GetCpuCoreCount(): Integer; stdcall;
procedure DraweBackgroundSample(buffer: PBYTE); stdcall;
procedure DrawLogoSample(buffer: PBYTE); stdcall;
procedure DrawResizeSample(buffer: PBYTE; item: PVCItem); stdcall;

procedure CreateHSBControl(hwndParent, hwndMessage: HWND; uiMessage: UINT; Color: DWORD); stdcall;
procedure CreateHSBControlEx(hwndParent, hwndMessage: HWND; uiMessage: UINT; Color: DWORD; xPos, yPos: Integer); stdcall;

(* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  录音支持
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *)
const
    // 音量示例
    // wParam:未使用
    // lParam:音量：0～100
    VC_RECORDING_SAMPLE = (VC_MESSAGE_BASE + 40);

    // 已经录制的长度
    // wParam:1/1000000秒，秒的剩余部分
    // lParam:已经录制的长度(秒)
    VC_RECORDING_DURATION = (VC_MESSAGE_BASE + 41);

    // 开始录制
    // 如果StartTime!=0, 会在前面填充静音
function AudioRecorderCreate(hwndMessage: HWND): Pointer; stdcall;

// 获取媒体信息
// 如果要废弃录音，应用程序应该删除：MediaInfoFree
function AudioRecorderGetInfo(recorder: Pointer): PMediaInfo; stdcall;

// 暂停录制
procedure AudioRecorderPause(recorder: Pointer); stdcall;

// 继续录制
// 如果和视频同步录制，暂停会导致音频和视频不同步，应该重新Seek视频
procedure AudioRecorderResume(recorder: Pointer); stdcall;

// 销毁
procedure AudioRecorderDestroy(recorder: Pointer); stdcall;

// 辅助
// 喇叭静音
// bMute=0不静音，否则静音，在录制过程中应该静音，否则会干扰麦克风
function SpeakerMute(bMute: Int32): HRESULT; stdcall;

implementation

uses Defines;

function VCGlobal_Size; external DLL_NAME name 'VCGlobal_Size';
function CodecOptions_Size; external DLL_NAME name 'CodecOptions_Size';
function MediaInfo_Size; external DLL_NAME name 'MediaInfo_Size';
function VCItem_Size; external DLL_NAME name 'VCItem_Size';

function vcGetGlobal; external DLL_NAME name 'vcGetGlobal';
function vcGetCodecOptions; external DLL_NAME name 'vcGetCodecOptions';

function vcInitialize; external DLL_NAME name 'vcInitialize';
procedure vcUninitialize; external DLL_NAME name 'vcUninitialize';

function vcImportMediaInfo; external DLL_NAME name 'vcImportMediaInfo';
procedure vcImportAbort; external DLL_NAME name 'vcImportAbort';
function vcImportGetProgress; external DLL_NAME name 'vcImportGetProgress';

function vcCreateItem; external DLL_NAME name 'vcCreateItem';
procedure vcDeleteItem; external DLL_NAME name 'vcDeleteItem';

function vcCreateVideoPlayer; external DLL_NAME name 'vcCreateVideoPlayer';
function vcVideoPlayerGrab; external DLL_NAME name 'vcVideoPlayerGrab';
procedure vcVideoPlayerDestroy; external DLL_NAME name 'vcVideoPlayerDestroy';
procedure vcVideoPlayerResize; external DLL_NAME name 'vcVideoPlayerResize';
procedure vcVideoPlayerStartStop; external DLL_NAME name 'vcVideoPlayerStartStop';
procedure vcVideoPlayerResume; external DLL_NAME name 'vcVideoPlayerResume';
procedure vcVideoPlayerPause; external DLL_NAME name 'vcVideoPlayerPause';
procedure vcVideoPlayerSeekStart; external DLL_NAME name 'vcVideoPlayerSeekStart';
procedure vcVideoPlayerSeekStop; external DLL_NAME name 'vcVideoPlayerSeekStop';
procedure vcVideoPlayerSeekTo; external DLL_NAME name 'vcVideoPlayerSeekTo';
procedure vcVideoPlayerNextFrame; external DLL_NAME name 'vcVideoPlayerNextFrame';
procedure vcVideoPlayerPreTime; external DLL_NAME name 'vcVideoPlayerPreTime';
procedure vcVideoPlayerNextTime; external DLL_NAME name 'vcVideoPlayerNextTime';
function vcVideoPlayerCurrentTime; external DLL_NAME name 'vcVideoPlayerCurrentTime';
procedure vcVideoPlayerForceRedraw; external DLL_NAME name 'vcVideoPlayerForceRedraw';

procedure vcVideoPlayerSetMusic; external DLL_NAME name 'vcVideoPlayerSetMusic';
procedure vcVideoPlayerSetVolume; external DLL_NAME name 'vcVideoPlayerSetVolume';

procedure vcReadyForConvert; external DLL_NAME name 'vcReadyForConvert';
function vcStartConvert; external DLL_NAME name 'vcStartConvert';
procedure vcAbortConvert; external DLL_NAME name 'vcAbortConvert';

function MergeInfo_Size; external DLL_NAME name 'MergeInfo_Size';
function vcGetMergeInfo; external DLL_NAME name 'vcGetMergeInfo';
function vcStartMerge; external DLL_NAME name 'vcStartMerge';
procedure vcAbortMerge; external DLL_NAME name 'vcAbortMerge';

function MakeMediaInfoString; external DLL_NAME name 'MakeMediaInfoString';
procedure CopyProfileSettings; external DLL_NAME name 'CopyProfileSettings';
function GetCpuCoreCount; external DLL_NAME name 'GetCpuCoreCount';
procedure DraweBackgroundSample; external DLL_NAME name 'DraweBackgroundSample';
procedure DrawLogoSample; external DLL_NAME name 'DrawLogoSample';
procedure DrawResizeSample; external DLL_NAME name 'DrawResizeSample';

procedure CreateHSBControl; external DLL_NAME name 'CreateHSBControl';
procedure CreateHSBControlEx; external DLL_NAME name 'CreateHSBControlEx';

function AudioRecorderCreate; external DLL_NAME name 'AudioRecorderCreate';
function AudioRecorderGetInfo; external DLL_NAME name 'AudioRecorderGetInfo';
procedure AudioRecorderPause; external DLL_NAME name 'AudioRecorderPause';
procedure AudioRecorderResume; external DLL_NAME name 'AudioRecorderResume';
procedure AudioRecorderDestroy; external DLL_NAME name 'AudioRecorderDestroy';
function SpeakerMute; external DLL_NAME name 'SpeakerMute';

function GetMediaString(info: PMediaInfo): String;
var
    buffer: Array [0 .. 1024 - 1] of WideChar;
begin
    Result := MakeMediaInfoString(info, buffer);
end;

end.
