#pragma once
#include "VideoFilters.h"

#define	VET_ADJUST		0 // 视频调节类：如亮度
#define	VET_FRAME		1 // 相框
#define	VET_SCENE		2 // 场景类：2D
#define	VET_SCENE_3D	3 // 场景类：3D
#define	VET_DISTORTION	4 // 变形类
#define	VET_ANIMATION	5 // 动画效果
#define	VET_FILTER		6 // 滤镜效果
#define	VET_FACE		7 // 脸部
#define	VET_ACCESSORY	8 // 脸部装饰
#define	VET_BACKGROUND	9 // 背景
#define	VET_EMOTION		10 // 表情类
#define	VET_CUSTOM		11 // 用户自己定义
#define	VET_UTILITY		12 // 调试工具
#define	VET_NO_VISUAL	13 // 不可视类型，用于内部使用
#define	VET_MAX			14


#define VIDEO_FILTER_TYPE_SOURCE		0
#define VIDEO_FILTER_TYPE_CROP			10
#define VIDEO_FILTER_TYPE_NONE			50
#define VIDEO_FILTER_TYPE_BACKGROUND	80
#define VIDEO_FILTER_TYPE_SUBTITLE		81
#define VIDEO_FILTER_TYPE_LOGO			82
#define VIDEO_FILTER_TYPE_RENDER		100

struct VideoFilterTemplate;
class CVideoFilterBase
{
public:
	int32_t						m_FilterType;
	GUID					m_Id;
	CVideoFilterBase*		m_next;
	CVideoFilterData*		m_data;
	VideoFilterTemplate*	m_template;

public:
	int64_t m_tStart;
	int64_t m_tStop;

public:
	virtual BOOL IsValid();

public:
	virtual HRESULT SetParameters(void *Param, int32_t length);
	virtual HRESULT GetParameters(void *Param, int32_t length);

public:
	virtual void PutSourceSize(int32_t width, int32_t height);

public:
	virtual void NewSegment(int64_t tStart, int64_t tStop); 
	virtual void EndOfStream(); 

public:
	virtual uint8_t* GetBuffer();
	virtual void Process(uint8_t *source, int64_t Pts) = 0;

public:
	CVideoFilterBase(VideoFilterTemplate *pTemplate, CVideoFilterData *pData);
	virtual ~CVideoFilterBase(void);

public:
	virtual void FilterRelease();
};


class CVideoFilterManager
{
private:
	CVideoFilterBase** m_ppFilters;
	int32_t m_cFilters;
	int32_t m_cbBuffer;

public:
	CVideoFilterManager(void);
	~CVideoFilterManager(void);

public:
	void AddFiler(CVideoFilterBase* pFilter);

public:
	void PutSourceSize(int32_t width, int32_t height); 
	void DoFilter(uint8_t *source, int64_t Pts);
	void NewSegment(int64_t tStart, int64_t tStop); 
	void EndOfStream();
	uint8_t *GetBuffer(){return m_ppFilters[0]->GetBuffer();};
};


#define MAX_VF_NAME 64
typedef CVideoFilterBase* (*PCreateVideoFilterInstance)(VideoFilterTemplate *pTemplate, CVideoFilterData *pData);

struct VideoFilterTemplate 
{
	wchar_t 					m_Name[MAX_VF_NAME];		// 特效名称
	CLSID 						m_ClsID;					// 特效ID
	PCreateVideoFilterInstance	m_lpfnNew;					// 创建一个新特效
	int32_t							m_vet;						// 效果类型
	void* const					m_pParameter;				// 默认参数，创建时使用
	int32_t							m_cbParameter;				// 参数长度
};

// 获取模板
extern VideoFilterTemplate *GetVideoFilterTemplate(CLSID id);

