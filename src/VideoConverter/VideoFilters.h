#pragma once
#include "VideoConverter.h"

/* ****************************************
ClipInfo = VCItem
**************************************** */
#ifndef VIDEOCONVERTER_EXPORTS
typedef VCItem ClipInfo;
#endif

/*****************************************************
 亮度、对比度效果
*****************************************************/

#define BRIGHTNESS_CONTRAST_NAME		L"Brightness Contrast"

// {D6423C0E-B608-43AF-AEE0-C2056AC17FAD}
static const GUID IID_BrightnessContrast = 
{ 0xd6423c0e, 0xb608, 0x43af, { 0xae, 0xe0, 0xc2, 0x5, 0x6a, 0xc1, 0x7f, 0xad } };

#ifndef VIDEOCONVERTER_EXPORTS
struct BrightnessContrastParams
{
	int32_t	m_Brightness;	// 亮度: -255~255
	int32_t	m_Contrast;		// 对比度: -100~100
};
#endif

/*****************************************************
 色调、饱和度、亮度
*****************************************************/
// XP 不支持

#define H_S_L_NAME		L"Hue Saturation Lightness"

// {D6327042-655F-4DA2-8627-E15383B20E22}
static const GUID IID_HueSaturationLightness = 
{ 0xd6327042, 0x655f, 0x4da2, { 0x86, 0x27, 0xe1, 0x53, 0x83, 0xb2, 0xe, 0x22 } };

#ifndef VIDEOCONVERTER_EXPORTS
struct HueSaturationLightnessParams
{
	int32_t m_Hue;			// 色调调节：-180~180
	int32_t	m_Saturation;	// 饱和度调节：-100~100
	int32_t	m_Lightness;	// 亮度调节：-100~100 (光照?)
};
#endif

/*****************************************************
 CROP
*****************************************************/

#define CROP_NAME		L"Crop"

// Crop 在原始视频的基础上进行CROP
// {747E1814-D829-4CEE-9390-2B944735FB55}
static const GUID IID_Crop = 
{ 0x747e1814, 0xd829, 0x4cee, { 0x93, 0x90, 0x2b, 0x94, 0x47, 0x35, 0xfb, 0x55 } };

#ifndef VIDEOCONVERTER_EXPORTS
struct Crop
{
	Rect m_Crop; // Crop的范围，如果范围和原始视频相同，则该效果无效。如果宽度或者高度为0，则该效果无效
};
#endif

//*****************************************************
// 特别支持
//*****************************************************
// 仅仅在播放原始视频的时候才能使用下面的函数
// 不会与效果进行关联，必须调用SetVideoEffect来设置CROP

// 当CROP改变时,该消息会发生到创建CVideoPlayer时的消息接收窗口
// wParam: X | (Width << 16); X = wParam & 0xFFFF, Width = (wParam >> 16) & 0xFFFF
// lParam: Y | (Height << 16); Y = lParam & 0xFFFF, Height = (lParam >> 16) & 0xFFFF
#define VC_VIDEO_PLAYER_CROP		(VC_MESSAGE_BASE + 20)

// 开启Crop模式
// hwndMessage:用于接收Crop改变的消息；如果为NULL，则不改变消息窗口
// crop:Crop的位置
extern void __stdcall vcVideoPlayerCrop(CVideoPlayer *player, Rect* crop); 

// 停止Crop
extern void __stdcall vcVideoPlayerCropStop(CVideoPlayer *player); 

/*****************************************************
 旋转
 *****************************************************/

#define ROTATE_FLIP_NAME		L"Rotate & Flip"

// {990E4AFB-26C4-4E41-A64D-B19025C54883}
static const GUID IID_RotateFlip = 
{ 0x990e4afb, 0x26c4, 0x4e41, { 0xa6, 0x4d, 0xb1, 0x90, 0x25, 0xc5, 0x48, 0x83 } };

#define RotateNone_FlipNone			0 // 无
#define Rotate90_FlipNone			1 // 旋转90度
#define Rotate180_FlipNone			2 // 旋转180度
#define Rotate270_FlipNone			3 // 旋转270度
#define RotateNone_FlipX			4 // 上下翻转
#define Rotate90_FlipX				5 // 旋转90度，并且上下翻转
#define Rotate180_FlipX				6 // 旋转180度，并且上下翻转
#define Rotate270_FlipX				7 // 旋转270度，并且上下翻转
#define RotateNone_FlipY			Rotate180_FlipX        // 左右翻转
#define Rotate90_FlipY				Rotate270_FlipX		   // 旋转90度，并且左右翻转
#define Rotate180_FlipY				RotateNone_FlipX	   // 旋转180度，并且左右翻转
#define Rotate270_FlipY				Rotate90_FlipX		   // 旋转270度，并且左右翻转
#define RotateNone_FlipXY			Rotate180_FlipNone	   // 左右翻转，并且左右翻转
#define Rotate90_FlipXY				Rotate270_FlipNone	   // 旋转90度，并且上下翻转，然后再左右翻转
#define Rotate180_FlipXY			RotateNone_FlipNone	   // 旋转180度，并且上下翻转，然后再左右翻转
#define Rotate270_FlipXY			Rotate90_FlipNone	   // 旋转270度，并且上下翻转，然后再左右翻转

#ifndef VIDEOCONVERTER_EXPORTS
struct RotateFlip
{
	int32_t m_RotateFlip; // 旋转角度定义，如果为0，则该效果无效
};
#endif

//*****************************************************
// 特别支持
//*****************************************************
#define Rotate_Left					10 // 旋转90度
#define Rotate_Right				11 // 旋转-90度
#define Flip_Horizontal 			20 // 左右翻转
#define Flip_Vertical				21 // 上下翻转

extern int32_t __stdcall CalcRotateFlip(int32_t Value, int32_t nRotateFlip); // 旋转视频，计算旋转方向
extern wchar_t *__stdcall RotateFlipString(int32_t RotateFlip); // 获取描述信息

/*****************************************************
  接口函数
*****************************************************/
// item:指向VCItem的指针
// id:效果ID
// value:效果参数指针
// size:参数长度
extern HRESULT __stdcall vcGetVideoEffect(ClipInfo *item, GUID id, void *Parameters, int32_t cbSize); // 获取效果参数
extern HRESULT __stdcall vcSetVideoEffect(ClipInfo *item, GUID id, void *Parameters, int32_t cbSize); // 设置效果参数

// 获取默认参数
extern HRESULT __stdcall vcGetVideoEffectDefaultValue(GUID id, void *Parameters, int32_t cbSize); // 设置效果参数

