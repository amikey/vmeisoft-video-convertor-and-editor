#pragma once

struct ClipInfo;

/*********************************************
// 效果定义
*********************************************/

// 淡入淡出效果
// IN: 淡入，OUT：淡出
// {D7377569-F21B-4FA9-999D-B57B19B32047}
static const GUID IID_Subtitle_Fade = 
{ 0xd7377569, 0xf21b, 0x4fa9, { 0x99, 0x9d, 0xb5, 0x7b, 0x19, 0xb3, 0x20, 0x47 } };

// 幻灯片效果
// IN：进入 
// OUT：退出 
// {A0EF4D22-3DF0-4C8C-A6BB-674E194E1256}
static const GUID IID_Subtitle_Slide = 
{ 0xa0ef4d22, 0x3df0, 0x4c8c, { 0xa6, 0xbb, 0x67, 0x4e, 0x19, 0x4e, 0x12, 0x56 } };

// 幻灯片效果参数
#define SLIDE_DIRECTION_LEFT			0
#define SLIDE_DIRECTION_TOP				1
#define SLIDE_DIRECTION_RIGHT			2
#define SLIDE_DIRECTION_BOTTOM			3
#define SLIDE_DIRECTION_LEFT_TOP		4
#define SLIDE_DIRECTION_RIGHT_TOP		5
#define SLIDE_DIRECTION_LEFT_BOTTOM		6
#define SLIDE_DIRECTION_RIGHT_BOTTOM	7

/*********************************************
// 对齐方式
*********************************************/
enum SubtitleAlignment
{
    SubtitleAlignmentNear = 0,
    SubtitleAlignmentCenter = 1,
    SubtitleAlignmentFar = 2,
	SubtitleAlignmentNone = 3 // 暂时不支持
};

/*********************************************
// 颜色定义
*********************************************/
union ColorRef
{
	uint32_t Color;
	struct
	{
		uint8_t B, G, R, A;
	};
};

/*********************************************
// 字体风格定义，和GdiPlus相同
*********************************************/
#if 0
// 可以组合
enum FontStyle
{
    FontStyleRegular    = 0,
    FontStyleBold       = 1,
    FontStyleItalic     = 2,
    //FontStyleBoldItalic = 3,
    FontStyleUnderline  = 4,
    FontStyleStrikeout  = 8
};
#endif


struct SubtitleDefault
{
	//  显示效果默认参数
	wchar_t				m_FontName[32]; // 字体名称
	int32_t					m_FontSize; // 字体尺寸
	uint32_t			m_FontStyle; // 字体风格
	ColorRef			m_FontColor; // 字体颜色：0xAARRGGBB
	ColorRef			m_BorderColor; // 边框颜色：0xAARRGGBB
	ColorRef			m_ShadowColor; // 阴影颜色：0xAARRGGBB

	// 动画效果默认参数
	GUID				m_Entrance; // 进入效果，如果为GUID_NULL, 则无效果
	int32_t				m_EntranceParam; // 进入参数； SLIDE_DIRECTION_
	int64_t				m_EntranceTime;	// 进入时长，时间越小，速度越快
	BOOL				m_EntranceOnce; // 和上一个效果一起执行
	GUID				m_Exit; // 退出效果，如果为GUID_NULL, 则无效果
	int32_t					m_ExitParam; // 退出参数； SLIDE_DIRECTION_
	int64_t				m_ExitTime; // 退出时长，时间越小，速度越快
	BOOL				m_ExitOnce; // 和上一个效果一起执行

	// 对齐方式默认参数
    SubtitleAlignment	m_Alignment; // 对齐方式：左，中，右边

	// Item默认参数
    SubtitleAlignment	m_HPosition; // 水平位置
    SubtitleAlignment	m_VPosition; // 垂直位置
	int32_t				m_LineSpace; // 行间距
	Point				m_Postion; // 字体位置，如果SubtitleAlignment = SubtitleAlignmentNone

};

extern int32_t _stdcall SubtitleDefault_Size();
extern SubtitleDefault* SubtitleGetDefault();

#define MAX_SUBTITLE_LINE_SIZE		256

/*********************************************
// 字幕
*********************************************/
class CSubtitleLine
{
public:
	int32_t				m_index; // 按时间排序
	wchar_t				m_Title[MAX_SUBTITLE_LINE_SIZE]; // 文本内容

	// 参数
	wchar_t				m_FontName[32]; // 字体名称
	int32_t				m_FontSize; // 字体尺寸
	uint32_t			m_FontStyle; // 字体风格
	ColorRef			m_FontColor; // 字体颜色：0xAARRGGBB
	ColorRef			m_BorderColor; // 边框颜色：0xAARRGGBB
	ColorRef			m_ShadowColor; // 阴影颜色：0xAARRGGBB

	// 效果
	GUID				m_Entrance; // 进入效果，如果为GUID_NULL, 则无效果
	int32_t				m_EntranceParam; // 进入参数； SLIDE_DIRECTION_
	int64_t				m_EntranceTime;	// 进入时长，时间越小，速度越快
	BOOL				m_EntranceOnce; // 和上一个效果一起执行
	GUID				m_Exit; // 退出效果，如果为GUID_NULL, 则无效果
	int32_t				m_ExitParam; // 退出参数； SLIDE_DIRECTION_
	int64_t				m_ExitTime; // 退出时长，时间越小，速度越快
	BOOL				m_ExitOnce; // 和上一个效果一起执行

	// 对齐
    SubtitleAlignment	m_Alignment; // 对齐方式：左，中，右边

	// 内部数据
	int64_t				m_EntranceStart;
	int64_t				m_EntranceStop;
	int64_t				m_ExitStart;
	int64_t				m_ExitStop;
	RectF				m_Bounds;
	PointF				m_Position;
	PointF				m_PointStart;
	PointF				m_PointStop;

public:
	CSubtitleLine();

public:
	RectF CalcBounds();
	void CalcEffect(int64_t time, PointF &point, ColorRef &FontColor, ColorRef &BorderColor, ColorRef &ShadowColor);
};

extern int32_t __stdcall CSubtitleLine_Size();

/*********************************************
// 字幕
*********************************************/
class CSubtitleItem
{
public:
	int32_t				m_index; // 按时间排序
	int32_t				m_Count; // 行数
	CSubtitleLine**		m_Lines; // 文本内容
	int64_t				m_StartTime; // 开始时间
	int64_t				m_Duration; // 时长

public:
    SubtitleAlignment	m_HPosition; // 水平位置
    SubtitleAlignment	m_VPosition; // 垂直位置
	int32_t				m_LineSpace; // 行间距
	Point				m_Postion; // 字体位置，如果SubtitleAlignment = SubtitleAlignmentNone

public:
	CSubtitleItem();
	~CSubtitleItem();

public:
	void CalcParamters(int32_t viewWidth, int32_t viewHeight);

public:
	CSubtitleLine* NewLine();
	void AddItem(CSubtitleItem *item);
	void AddLine(wchar_t *Text); // Copy Text
	void DeleteLine(int32_t index);
	void DrawInto(Gdiplus::Graphics* graphics, int64_t pts, int32_t FocusIndex);
	inline void ConvertFrameToTime(int64_t num, int64_t den);
};

extern int32_t __stdcall CSubtitleItem_Size();

/*********************************************
// 字幕
*********************************************/

class CSubtitle
{
public:
	int32_t					m_Count; // 字幕个数
	CSubtitleItem**			m_Items; // 字幕项目

public:
	// 内部数据
	BOOL					m_FrameBase; // 是否基于FRAME

public:
	CSubtitle();
	~CSubtitle();

public:
	CSubtitleItem* NewItem();
	void DeleteItem(int32_t index);
	void ConvertFrmaeToTime(int32_t num, int32_t den);

public:
	void SortItem();
	void CalcParamters(int32_t viewWidth, int32_t viewHeight);
	int32_t FirstItem(int64_t pte);
	void ConvertFrameToTime(int64_t num, int64_t den);
	void CheckTime(int64_t duration);
	void MergeTime();
};

extern int32_t __stdcall CSubtitle_Size();

/*********************************************
// 接口函数
*********************************************/
// 导入字幕文件
// 如果导入成功，会清除原先的字幕;失败将保留原来的字幕
// 返回导入的字幕，失败返回NULL
extern CSubtitle* __stdcall SubtitleImport(ClipInfo *Info, wchar_t *szFileName);

// 获取字幕信息
extern CSubtitle* __stdcall SubtitleGet(ClipInfo *Info);

// 添加字幕：空项目
//  如果没有字幕会创建一个空的字幕
extern CSubtitleItem* __stdcall SubtitleAddItem(CSubtitle *subtitle);

// 添加字幕：空项目
//  如果没有字幕会创建一个空的字幕
extern CSubtitleLine* __stdcall SubtitleAddLine(CSubtitleItem *item);

// 删除字幕项目
extern void __stdcall SubtitleDelete(CSubtitle *subtitle, int32_t index);

// 删除字幕Line
extern void __stdcall SubtitleDeleteLine(CSubtitleItem *item, int32_t index);

// 排序：如果修改了时间，必须按时间（开始时间）排序
extern void __stdcall SubtitleSort(CSubtitle *subtitle);

/*********************************************
// 接口函数：播放
*********************************************/

// 当播放完成之后调用这个事件
// 如果没有m_TransitionInFilter和m_TransitionOutFilter，不会触发这个事件
// wParam:CSubtitlePlayer的指针
// lParam:未定义
#define CM_SUBTITLE_PLAYER_END	(WM_USER + 1)

class CSubtitlePlayer;

// 窗口一个Subtitle的播放窗口
extern CSubtitlePlayer* __stdcall SubtitleCreatePlayer(HWND hwndParent, HWND hwndMessage, CSubtitleItem *item);

// 播放
extern void __stdcall SubtitlePlay(CSubtitlePlayer* player);

// 播放
extern void __stdcall SubtitlePlayEx(CSubtitlePlayer* player, int32_t index, BOOL IsExitEffect);

// 重绘
extern void __stdcall SubtitleRedraw(CSubtitlePlayer* player);

// 设置焦点
extern void __stdcall SubtitleSetFocus(CSubtitlePlayer* player, int32_t index);

// 释放播放器
extern void __stdcall SubtitleDestroyPlayer(CSubtitlePlayer* player);
