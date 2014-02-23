{$WARN SYMBOL_PLATFORM OFF}
{$MINENUMSIZE 4}
unit SubtitleInt;

interface

uses
    Winapi.Windows, Winapi.Messages, System.Types, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics, Vcl.Controls, Vcl.Forms,
    Vcl.Dialogs, Vcl.ImgList, Vcl.Menus, Vcl.StdCtrls, Vcl.ExtCtrls, Vcl.OleCtrls, SHDocVw, Vcl.ComCtrls, Vcl.Buttons,
    VideoConverterInt;

const
    // *********************************************
    // 效果定义
    // *********************************************
    IID_Subtitle_None: TGUID = '{00000000-0000-0000-0000-000000000000}';
    // 淡入淡出效果
    IID_Subtitle_Fade: TGUID = '{D7377569-F21B-4FA9-999D-B57B19B32047}';
    // 幻灯片效果
    IID_Subtitle_Slide: TGUID = '{A0EF4D22-3DF0-4C8C-A6BB-674E194E1256}';

const
    SubtitleEffectIDs: Array [0 .. 2] of TGUID = (
      '{00000000-0000-0000-0000-000000000000}',
      '{D7377569-F21B-4FA9-999D-B57B19B32047}',
      '{A0EF4D22-3DF0-4C8C-A6BB-674E194E1256}'
      );

const
    // 幻灯片效果参数
    SLIDE_DIRECTION_LEFT         = 0;
    SLIDE_DIRECTION_TOP          = 1;
    SLIDE_DIRECTION_RIGHT        = 2;
    SLIDE_DIRECTION_BOTTOM       = 3;
    SLIDE_DIRECTION_LEFT_TOP     = 4;
    SLIDE_DIRECTION_RIGHT_TOP    = 5;
    SLIDE_DIRECTION_LEFT_BOTTOM  = 6;
    SLIDE_DIRECTION_RIGHT_BOTTOM = 7;

    (* ********************************************
      // 对齐方式
      ******************************************** *)
const
    SubtitleAlignmentNear   = 0;
    SubtitleAlignmentCenter = 1;
    SubtitleAlignmentFar    = 2;
    SubtitleAlignmentNone   = 3;

type
    SubtitleAlignment = Integer;

    (* ********************************************
      // 颜色定义
      ******************************************** *)
type
    ColorRef = record
        case Integer of
            0: (Color: Integer;);
            1:
                (
                  B, G, R, A: BYTE;
                );
    end;

const
    SubtitleSpeeds: array [0 .. 2] of Int64 = (500000, 1000000, 2000000);

    (* ********************************************
      // 字体风格定义，和GdiPlus相同
      ******************************************** *)
    // 可以组合
const
    FontStyleRegular   = 0;
    FontStyleBold      = 1;
    FontStyleItalic    = 2;
    FontStyleUnderline = 4;
    FontStyleStrikeout = 8;

    (* ********************************************
      // 字幕参数
      ******************************************** *)
const
    MAX_SUBTITLE_LINE_SIZE = 256;
    MAX_SUBTITLE_FONT_SIZE = 32;

type
    SubtitleDefault = record
        m_FontName: Array [0 .. MAX_SUBTITLE_FONT_SIZE - 1] of WideChar; // 字体名称
        m_FontSize: Integer;                                             // 字体尺寸
        m_FontStyle: Integer;                                            // 字体风格
        m_FontColor: ColorRef;                                           // 字体颜色：0xAARRGGBB
        m_BorderColor: ColorRef;                                         // 边框颜色：0xAARRGGBB
        m_ShadowColor: ColorRef;                                         // 阴影颜色：0xAARRGGBB

        m_Entrance: TGUID;        // 进入效果，如果为GUID_NULL, 则无效果
        m_EntranceParam: Integer; // 进入参数； SLIDE_DIRECTION_
        m_EntranceTime: Int64;    // 进入时长，时间越小，速度越快
        m_EntranceOnce: BOOL;     // 同上一个效果一起执行
        m_Exit: TGUID;            // 退出效果，如果为GUID_NULL, 则无效果
        m_ExitParam: Integer;     // 退出参数； SLIDE_DIRECTION_
        m_ExitTime: Int64;        // 退出时长，时间越小，速度越快
        m_ExitOnce: BOOL;         // 同上一个效果一起执行

        m_Alignment: SubtitleAlignment; // 对齐方式

        m_HPosition: SubtitleAlignment; // 水平位置
        m_VPosition: SubtitleAlignment; // 垂直位置
        m_LineSpace: Integer;           // 行间距
        m_Postion: TPoint;              // 字体位置，如果SubtitleAlignment = SubtitleAlignmentNone
    end;

    PSubtitleDefault = ^SubtitleDefault;

    CSubtitleLine = record
        m_index: Integer;
        m_Title: Array [0 .. MAX_SUBTITLE_LINE_SIZE - 1] of WideChar;

        m_FontName: Array [0 .. MAX_SUBTITLE_FONT_SIZE - 1] of WideChar; // 字体名称
        m_FontSize: Integer;                                             // 字体尺寸
        m_FontStyle: Integer;                                            // 字体风格
        m_FontColor: ColorRef;                                           // 字体颜色：0xAARRGGBB
        m_BorderColor: ColorRef;                                         // 边框颜色：0xAARRGGBB
        m_ShadowColor: ColorRef;                                         // 阴影颜色：0xAARRGGBB

        m_Entrance: TGUID;        // 进入效果，如果为GUID_NULL, 则无效果
        m_EntranceParam: Integer; // 进入参数； SLIDE_DIRECTION_
        m_EntranceTime: Int64;    // 进入时长，时间越小，速度越快
        m_EntranceOnce: BOOL;     // 同上一个效果一起执行
        m_Exit: TGUID;            // 退出效果，如果为GUID_NULL, 则无效果
        m_ExitParam: Integer;     // 退出参数； SLIDE_DIRECTION_
        m_ExitTime: Int64;        // 退出时长，时间越小，速度越快
        m_ExitOnce: BOOL;         // 同上一个效果一起执行
        m_Alignment: SubtitleAlignment;

        // 内部数据
        m_EntranceStart: Int64;
        m_EntranceStop: Int64;
        m_ExitStart: Int64;
        m_ExitStop: Int64;
        m_Bounds: CRectF;
        m_Position: CPointF;
        m_PointStart: CPointF;
        m_PointStop: CPointF;

    end;

    PSubtitleLine = ^CSubtitleLine;

    (* ********************************************
      // 字幕
      ******************************************** *)
    CSubtitleItem = record
        m_index: Integer;                // 按时间排序
        m_Count: Integer;                // 文本内容
        m_Lines: Array of PSubtitleLine; // Items
        m_StartTime: Int64;              // 开始时间
        m_Duration: Int64;               // 时长
        m_HPosition: SubtitleAlignment;  // 水平位置
        m_VPosition: SubtitleAlignment;  // 垂直位置
        m_LineSpace: Integer;            // 行间距
        m_Postion: TPoint;               // 字体位置，如果SubtitleAlignment = SubtitleAlignmentNone
    end;

    PSubtitleItem = ^CSubtitleItem;

    (* ********************************************
      // 字幕
      ******************************************** *)

    CSubtitle = record
        m_Count: Integer;                // 字幕个数
        m_Items: Array of PSubtitleItem; // 字幕项目
        m_FrameBase: BOOL;               // 是否基于FRAME
    end;

    PSubtitle = ^CSubtitle;

    (* ********************************************
      // 结构体相关
      ******************************************** *)
function SubtitleDefault_Size(): Integer; stdcall;
function CSubtitleLine_Size(): Integer; stdcall;
function CSubtitleItem_Size(): Integer; stdcall;
function CSubtitle_Size(): Integer; stdcall;
function SubtitleGetDefault(): PSubtitleDefault; stdcall;

(* ********************************************
  // 接口函数
  ******************************************** *)
// 导入字幕文件
// 如果失败返回0，成功返回字幕的项目数量
// 如果导入成功，会清除原先的字幕;失败将保留原来的字幕
function SubtitleImport(Info: Pointer; szFileName: PWideChar): PSubtitle; stdcall;

// 获取字幕信息
function SubtitleGet(Info: Pointer): PSubtitle; stdcall;

// 添加字幕
// 如果没有字幕会创建一个空的字幕
function SubtitleAddItem(subtitle: PSubtitle): PSubtitleItem; stdcall;

// 添加字幕：空项目
// 如果没有字幕会创建一个空的字幕
function SubtitleAddLine(item: PSubtitleItem): PSubtitleLine; stdcall;

// 删除字幕项目
procedure SubtitleDelete(subtitle: PSubtitle; index: Integer); stdcall;

// 删除字幕Line
procedure SubtitleDeleteLine(item: PSubtitleItem; index: Integer); stdcall;

// 排序：如果修改了时间，必须按时间（开始时间）排序
procedure SubtitleSort(subtitle: PSubtitle); stdcall;

(* ********************************************
  // 接口函数：播放
  ******************************************** *)
const
    // 当播放完成之后调用这个事件
    // 如果没有m_TransitionInFilter和m_TransitionOutFilter，不会触发这个事件
    // wParam:CSubtitlePlayer的指针
    // lParam:未定义
    CM_SUBTITLE_PLAYER_END = (WM_USER + 1);

    // 窗口一个Subtitle的播放窗口
function SubtitleCreatePlayer(hwndParent, hwndMessage: HWND; item: PSubtitleItem): Pointer; stdcall;

// 播放
procedure SubtitlePlay(player: Pointer); stdcall;
procedure SubtitlePlayEx(player: Pointer; index: Integer; IsExitEffect: BOOL); stdcall;

// 重绘
procedure SubtitleRedraw(player: Pointer); stdcall;

// 设置焦点
procedure SubtitleSetFocus(player: Pointer; index: Integer); stdcall;

// 释放播放器
procedure SubtitleDestroyPlayer(player: Pointer); stdcall;

function GetSubtitleEffectIndex(id: TGUID): Integer;
function GetSubtitleSpeedIndex(speed: Int64): Integer;

function FontStyleFromMask(mask: Integer): TFontStyles;
function FontStyleToMask(FontStyles: TFontStyles): Integer;

function SubtitleFind(szVideoFile: PWideChar): Integer; stdcall;
function SubtitleFindGetPath(index:Integer): PWideChar; stdcall;
function SubtitleFindGetName(index:Integer): PWideChar; stdcall;
procedure SubtitleFindFree(); stdcall;

implementation

uses Defines;

function SubtitleDefault_Size; external DLL_NAME name 'SubtitleDefault_Size';
function CSubtitleLine_Size; external DLL_NAME name 'CSubtitleLine_Size';
function CSubtitleItem_Size; external DLL_NAME name 'CSubtitleItem_Size';
function CSubtitle_Size; external DLL_NAME name 'CSubtitle_Size';
function SubtitleGetDefault; external DLL_NAME name 'SubtitleGetDefault';

function SubtitleImport; external DLL_NAME name 'SubtitleImport';
function SubtitleGet; external DLL_NAME name 'SubtitleGet';
function SubtitleAddItem; external DLL_NAME name 'SubtitleAddItem';
function SubtitleAddLine; external DLL_NAME name 'SubtitleAddLine';
procedure SubtitleDelete; external DLL_NAME name 'SubtitleDelete';
procedure SubtitleDeleteLine; external DLL_NAME name 'SubtitleDeleteLine';
procedure SubtitleSort; external DLL_NAME name 'SubtitleSort';

function SubtitleCreatePlayer; external DLL_NAME name 'SubtitleCreatePlayer';
procedure SubtitlePlay; external DLL_NAME name 'SubtitlePlay';
procedure SubtitlePlayEx; external DLL_NAME name 'SubtitlePlayEx';
procedure SubtitleRedraw; external DLL_NAME name 'SubtitleRedraw';
procedure SubtitleSetFocus; external DLL_NAME name 'SubtitleSetFocus';
procedure SubtitleDestroyPlayer; external DLL_NAME name 'SubtitleDestroyPlayer';
procedure SubtitleDestroyPlayerEx; external DLL_NAME name 'SubtitleDestroyPlayerEx';

function SubtitleFind; external DLL_NAME name 'SubtitleFind';
function SubtitleFindGetPath; external DLL_NAME name 'SubtitleFindGetPath';
function SubtitleFindGetName; external DLL_NAME name 'SubtitleFindGetName';
procedure SubtitleFindFree; external DLL_NAME name 'SubtitleFindFree';

function GetSubtitleEffectIndex(id: TGUID): Integer;
var
    i: Integer;
begin
    Result := -1;
    for i := 0 to 2 do
    begin
        if SubtitleEffectIDs[i] = id then
        begin
            Result := i;
            exit;
        end;
    end;
end;

function GetSubtitleSpeedIndex(speed: Int64): Integer;
var
    i: Integer;
begin
    Result := 1;
    if speed = 0 then exit;
    for i := 0 to 2 do
    begin
        if SubtitleSpeeds[i] = speed then
        begin
            Result := i;
            exit;
        end;
    end;
    if speed < SubtitleSpeeds[0] then
    begin
        Result := 0;
        exit;
    end;
    if speed > SubtitleSpeeds[2] then
    begin
        Result := 2;
        exit;
    end;
end;

function FontStyleFromMask(mask: Integer): TFontStyles;
begin
    Result := [];
    if ((mask and FontStyleBold) <> 0) then Result := Result + [fsBold];
    if ((mask and FontStyleItalic) <> 0) then Result := Result + [fsItalic];
    if ((mask and FontStyleUnderline) <> 0) then Result := Result + [fsUnderline];
    if ((mask and FontStyleStrikeout) <> 0) then Result := Result + [fsStrikeout];
end;

function FontStyleToMask(FontStyles: TFontStyles): Integer;
begin
    Result := 0;
    if fsBold in FontStyles then Result := Result or FontStyleBold;
    if fsItalic in FontStyles then Result := Result or FontStyleItalic;
    if fsUnderline in FontStyles then Result := Result or FontStyleUnderline;
    if fsStrikeout in FontStyles then Result := Result or FontStyleStrikeout;
end;

end.
