{$WARN SYMBOL_PLATFORM OFF}
unit VideoEffectInt;

interface

uses
    Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics, Vcl.Controls, Vcl.Forms,
    Vcl.Dialogs, Vcl.ImgList, Vcl.Menus, Vcl.StdCtrls, Vcl.ExtCtrls, Vcl.OleCtrls, SHDocVw, Vcl.ComCtrls, Vcl.Buttons,
    VideoConverterInt;

type
    PClipInfo = PVCItem;

    (* ****************************************************
      亮度、对比度效果
      **************************************************** *)

const
    BRIGHTNESS_CONTRAST_NAME = 'Brightness Contrast';

const
    // {D6423C0E-B608-43AF-AEE0-C2056AC17FAD}
    IID_BrightnessContrast: TGUID = '{D6423C0E-B608-43AF-AEE0-C2056AC17FAD}';

type
    BrightnessContrastParams = record
        m_Brightness: Int32; // 亮度: -255~255
        m_Contrast: Int32;   // 对比度: -100~100
    end;

    (* ****************************************************
      色调、饱和度、亮度
      **************************************************** *)
    // XP 不支持

const
    H_S_L_NAME = 'Hue Saturation Lightness';

    // {D6327042-655F-4DA2-8627-E15383B20E22}
    IID_HueSaturationLightness: TGUID = '{D6327042-655F-4DA2-8627-E15383B20E22}';

type
    HueSaturationLightnessParams = record
        m_Hue: Int32;        // 色调调节：-180~180
        m_Saturation: Int32; // 饱和度调节：-100~100
        m_Lightness: Int32;  // 亮度调节：-100~100 (光照?)
    end;

    (* ****************************************************
      CROP
      **************************************************** *)

const
    CROP_NAME = 'Crop';

    // Crop 在原始视频的基础上进行CROP
    // {747E1814-D829-4CEE-9390-2B944735FB55}
    IID_Crop: TGUID = '{747E1814-D829-4CEE-9390-2B944735FB55}';

    // *****************************************************
    // 特别支持
    // *****************************************************
    // 仅仅在播放原始视频的时候才能使用下面的函数
    // 不会与效果进行关联，必须调用SetVideoEffect来设置CROP
    // 当CROP改变时
    // wParam: X | (Width << 16); X = wParam & 0xFFFF, Width = (wParam >> 16) & 0xFFFF
    // lParam: Y | (Height << 16); Y = lParam & 0xFFFF, Height = (lParam >> 16) & 0xFFFF
    VC_VIDEO_PLAYER_CROP = (VC_MESSAGE_BASE + 20);

procedure vcVideoPlayerCrop(player: Pointer; var crop: CRect); stdcall;
procedure vcVideoPlayerCropStop(player: Pointer); stdcall;

(* ****************************************************
  旋转
  **************************************************** *)
const
    ROTATE_FLIP_NAME = 'Rotate & Flip';

    // {990E4AFB-26C4-4E41-A64D-B19025C54883}
    IID_RotateFlip: TGUID = '{990E4AFB-26C4-4E41-A64D-B19025C54883}';

    RotateNone_FlipNone = 0;                   // 无
    Rotate90_FlipNone   = 1;                   // 旋转90度
    Rotate180_FlipNone  = 2;                   // 旋转180度
    Rotate270_FlipNone  = 3;                   // 旋转270度
    RotateNone_FlipX    = 4;                   // 上下翻转
    Rotate90_FlipX      = 5;                   // 旋转90度，并且上下翻转
    Rotate180_FlipX     = 6;                   // 旋转180度，并且上下翻转
    Rotate270_FlipX     = 7;                   // 旋转270度，并且上下翻转
    RotateNone_FlipY    = Rotate180_FlipX;     // 左右翻转
    Rotate90_FlipY      = Rotate270_FlipX;     // 旋转90度，并且左右翻转
    Rotate180_FlipY     = RotateNone_FlipX;    // 旋转180度，并且左右翻转
    Rotate270_FlipY     = Rotate90_FlipX;      // 旋转270度，并且左右翻转
    RotateNone_FlipXY   = Rotate180_FlipNone;  // 左右翻转，并且左右翻转
    Rotate90_FlipXY     = Rotate270_FlipNone;  // 旋转90度，并且上下翻转，然后再左右翻转
    Rotate180_FlipXY    = RotateNone_FlipNone; // 旋转180度，并且上下翻转，然后再左右翻转
    Rotate270_FlipXY    = Rotate90_FlipNone;   // 旋转270度，并且上下翻转，然后再左右翻转

    // *****************************************************
    // 特别支持
    // *****************************************************
    Rotate_Left     = 10; // 旋转90度
    Rotate_Right    = 11; // 旋转-90度
    Flip_Horizontal = 20; // 左右翻转
    Flip_Vertical   = 21; // 上下翻转

function CalcRotateFlip(value, nRotateFlip: Integer): Integer; stdcall; // 旋转视频，计算旋转方向
function RotateFlipString(nRotateFlip: Integer): PWideChar; stdcall;    // 获取描述信息

(* ****************************************************
  接口函数
  **************************************************** *)
// item:指向VCItem的指针
// id:效果ID
// value:效果参数指针
// size:参数长度
function vcGetVideoEffect(item: PClipInfo; id: TGUID; Parameters: Pointer; cbSize: Integer): HRESULT; stdcall; // 获取效果参数
function vcSetVideoEffect(item: PClipInfo; id: TGUID; Parameters: Pointer; cbSize: Integer): HRESULT; stdcall; // 设置效果参数

// 获取默认参数
function vcGetVideoEffectDefaultValue(id: TGUID; Parameters: Pointer; cbSize: Integer): HRESULT; stdcall; // 设置效果参数

implementation

uses Defines;

procedure vcVideoPlayerCrop; external DLL_NAME name 'vcVideoPlayerCrop';
procedure vcVideoPlayerCropStop; external DLL_NAME name 'vcVideoPlayerCropStop';

function CalcRotateFlip; external DLL_NAME name 'CalcRotateFlip';
function RotateFlipString; external DLL_NAME name 'RotateFlipString';

function vcGetVideoEffect; external DLL_NAME name 'vcGetVideoEffect';
function vcSetVideoEffect; external DLL_NAME name 'vcSetVideoEffect';
function vcGetVideoEffectDefaultValue; external DLL_NAME name 'vcGetVideoEffectDefaultValue';

end.
