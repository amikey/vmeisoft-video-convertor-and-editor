#include "stdafx.h"
#include "VideoConverter.h"
#include "SubtitleInt.h"
#include "Subtitle.h"
#include "SubtitleReader.h"

SubtitleDefault g_subtitle_default = 
{
	L"Arial", //m_FontName[32];		// 字体名称
	24, //m_FontSize; // 字体尺寸
	FontStyleBold, //m_FontStyle; // 字体风格
	0xFFFFFFFF,//m_FontColor; // 字体颜色：0xAARRGGBB
	0xFF000000,//m_BorderColor; // 边框颜色：0xAARRGGBB
	0x60808080, // 阴影颜色：0xAARRGGBB
	
	{}, //m_Entrance; // 进入效果，如果为GUID_NULL, 则无效果
	0, //m_EntranceParam; // 进入参数； SLIDE_DIRECTION_
	AV_TIME_BASE_LL, //m_EntranceTime;	// 进入时长，时间越小，速度越快
	FALSE, // 同上一个效果一起执行
	
	{}, //m_Exit; // 退出效果，如果为GUID_NULL, 则无效果
	0, //m_ExitParam; // 退出参数； SLIDE_DIRECTION_
	AV_TIME_BASE_LL, //m_ExitTime; // 退出时长，时间越小，速度越快
	FALSE, // 同上一个效果一起执行

    SubtitleAlignmentCenter,// m_Alignment; // 对齐方式：左，中，右边

	SubtitleAlignmentCenter,//m_HPosition; // 水平位置
	SubtitleAlignmentFar,//m_VPosition; // 垂直位置
	0,//m_LineSpace; // 行间距
	Point()//m_Postion; // 字体位置，如果SubtitleAlignment = SubtitleAlignmentNone
};

int32_t __stdcall CSubtitleLine_Size()
{
	return sizeof(CSubtitleLine);
}

int32_t __stdcall CSubtitleItem_Size()
{
	return sizeof(CSubtitleItem);
}

int32_t __stdcall CSubtitle_Size()
{
	return sizeof(CSubtitle);
}

int32_t _stdcall SubtitleDefault_Size()
{
	return sizeof(SubtitleDefault);
}

SubtitleDefault* SubtitleGetDefault()
{
	return &g_subtitle_default;
}

CSubtitle*__stdcall SubtitleImport(ClipInfo *Info, wchar_t *szFileName)
{
	CSubtitle *subtitle = LoadTextSubtitle(szFileName);
	if (subtitle == NULL) return NULL;
	Info->SetSubtitle(subtitle);
	return subtitle;
}

CSubtitle* __stdcall SubtitleGet(ClipInfo *Info)
{
	return Info->GetSubtitle();
}

CSubtitleItem* __stdcall SubtitleAddItem(CSubtitle *subtitle)
{
	return subtitle->NewItem();
}

CSubtitleLine* __stdcall SubtitleAddLine(CSubtitleItem *item)
{
	return item->NewLine();
}

void __stdcall SubtitleDelete(CSubtitle *subtitle, int32_t index)
{
	subtitle->DeleteItem(index);
}

void __stdcall SubtitleDeleteLine(CSubtitleItem *item, int32_t index)
{
	item->DeleteLine(index);
}

void __stdcall SubtitleSort(CSubtitle *subtitle)
{
	subtitle->SortItem();
}

CSubtitlePlayer* __stdcall SubtitleCreatePlayer(HWND hwndParent, HWND hwndMessage, CSubtitleItem *subtitle)
{
	CSubtitlePlayer *player = new CSubtitlePlayer(hwndParent, hwndMessage, subtitle);
	if (player->Initialize() != S_OK)
	{
		delete player;
		return NULL;
	}
	else
	{
		return player;
	}
}

void __stdcall SubtitlePlay(CSubtitlePlayer* player)
{
	player->Play();
}

void __stdcall SubtitlePlayEx(CSubtitlePlayer* player, int32_t index, BOOL IsExitEffect)
{
	player->PlayEx(index, IsExitEffect);
}

void __stdcall SubtitleRedraw(CSubtitlePlayer* player)
{
	player->Redraw();
}

void __stdcall SubtitleSetFocus(CSubtitlePlayer* player, int32_t index)
{
	player->SetFocus(index);
}

void __stdcall SubtitleDestroyPlayer(CSubtitlePlayer* player)
{
	delete player;
}
