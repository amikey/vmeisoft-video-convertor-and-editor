#pragma once

#define ASPECT_4x3		1
#define ASPECT_16x9		2

#define GROUP_NONE		-1
#define GROUP_HD		0
#define GROUP_VIDEO		1
#define GROUP_DEVICE	2
#define GROUP_WEB		3
#define GROUP_APP		4
#define GROUP_AUDIO		5
#define GROUP_MAX		6

#define THUMBNAIL_WIDTH		48
#define THUMBNAIL_HEIGHT	48

#define VIDEO_RES_CUSTOM	-1

// 分辨率
struct ProfileRes
{
	int					m_Width;
	int					m_Height;
};

// 帧率
struct ProfileFrameRate
{
	int					m_Num;
	int					m_Den;
};

// 编码器可选参数
struct ProfileCodec
{
	wchar_t				m_Name[32];
	wchar_t				m_Codec[32];
};

// 视频可选参数
struct ProfileVideo
{
	int					m_FixedSize;
	int					m_FixedBitRate;
	int					m_Aspect;

	int					m_cCodec;
	ProfileCodec*		m_Codec;
	int					m_DefaultCodec;

	int					m_cRes;
	ProfileRes*			m_Res;
	int					m_DefaultRes;

	int					m_cFrameRate;
	ProfileFrameRate*	m_FrameRate;
	int					m_DefaultFrameRate;

	int					m_cBitRate;
	int*				m_BitRate;
	int					m_DefaultBitRate;
};

extern int __stdcall ProfileVideo_Size();

// 音频可选参数
struct ProfileAudio
{
	int					m_FixedFormat;
	int					m_FixedBitRate;

	int					m_cCodec;
	ProfileCodec*		m_Codec;
	int					m_DefaultCodec;

	int					m_cSampleRate;
	int*				m_SampleRate;
	int					m_DefaultSampleRate;

	int					m_cChannel;
	int*				m_Channel;
	int					m_DefaultChannel;

	int					m_cBitRate;
	int*				m_BitRate;
	int					m_DefaultBitRate;
};

extern int __stdcall ProfileAudio_Size();

// 生成目标配置
// 如同样是MP4，可以是普通视频，也可以是专为iPod而定制的格式
struct ProfileTarget
{
	wchar_t				m_Name[64];
	wchar_t				m_Description[128];

	ProfileVideo		m_Video;
	ProfileAudio		m_Audio;
};

extern int __stdcall ProfileTarget_Size();

// Profile 配置结构体
struct ProfileTag
{
	GUID				m_Id;

	wchar_t				m_Name[256];
	wchar_t				m_Description[1024];
	wchar_t				m_Format[64];
	wchar_t				m_ExtName[16];

	int					m_cTarget;
	ProfileTarget*		m_Target;

	wchar_t				m_ImageFile[MAX_PATH];
	Gdiplus::Bitmap*	m_Bitmap;
};

extern int __stdcall ProfileTag_Size();

// 编码参数设置
struct ProfileSettings
{
	GUID			m_IdProfile;

	int				m_Group;
	int				m_Profile;
	int				m_Target;

	int				m_VCodec;
	int				m_Res;
	ProfileRes		m_ResCustom;
	int				m_FrameRate;
	int				m_VBitRate;

	int				m_ACodec;
	int				m_SampleRate;
	int				m_Channel;
	int				m_ABitRate;
};

extern int __stdcall ProfileSettings_Size();

// 内部使用
extern int				g_cAllProfile;
extern ProfileTag*		g_AllProfile;
extern int				g_cProfiles[GROUP_MAX];
extern ProfileTag*		g_Profiles[GROUP_MAX];
extern ProfileSettings	g_ProfileSettings;
extern HRESULT __stdcall LoadProfiles();

// 获取默认参数
extern void __stdcall GetDefaultProfileSetting1(ProfileSettings &Settings, int nGroup);
extern void __stdcall GetDefaultProfileSetting2(ProfileSettings &Settings, int nGroup, int nProfile);
extern void __stdcall GetDefaultProfileSetting3(ProfileSettings &Settings, int nGroup, int nProfile, int nTarget);

// 信息获取
extern int __stdcall GetProfofileCount(int nGroup); // Profile Count
extern ProfileTag* __stdcall GetProfile(int nGroup, int Profile); // Get Profile
extern ProfileTarget* __stdcall GetProfileTarget(int nGroup, int Profile, int Target); // Get Target

extern int __stdcall GetProfofileCountEx(ProfileSettings &Settings); // 根据设置获取当前ProfileTag的数量
extern ProfileTag* __stdcall GetProfileEx(ProfileSettings &Settings); // 根据设置获取当前ProfileTag
extern ProfileTarget* __stdcall GetProfileTargetEx(ProfileSettings &Settings); // 根据设置获取当前ProfileTarget
extern ProfileVideo* __stdcall GetProfileVideoEx(ProfileSettings &Settings); // 根据设置获取当前ProfileVideo
extern ProfileAudio* __stdcall GetProfileAudioEx(ProfileSettings &Settings); // 根据设置获取当前ProfileAudio
extern BOOL __stdcall IsFixedVideoSizeEx(ProfileSettings &Settings); // 根据设置获当前是否支持自定义尺寸(任意尺寸，宽和高必须是16的倍数)

extern ProfileTag* __stdcall FindProfile(GUID id); // 根据ID查找

extern void __stdcall DrawProfileThumbnail(ProfileTarget *Profile, HDC hdc, int x, int y); // 画缩略图

extern void __stdcall SetCustomRes(ProfileSettings &Settings, int Width, int Height); // 设置用户自定义尺寸
