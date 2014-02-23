#include "stdafx.h"
#include "VideoConverter.h"
#include "VideoFilters.h"
#include "VideoFilter.h"

#include "BrightnessContrast.h"
#include "HueSaturationLightness.h"
#include "CropFilter.h"
#include "RotateFlip.h"
#include "BackgroundFilter.h"
#include "WatermarkFilter.h"

static BrightnessContrastParams g_Default_BrightnessContrast;
static HueSaturationLightnessParams g_Default_HueSaturationLightness;
static int32_t g_Default_RotateFlip;
static Rect g_Default_Crop;

VideoFilterTemplate VideoFilterTemplates[VIDEO_FILTER_COUNT] = 
{
	// NAME							ClsID							CreateInstance										VET					Parameter							cbParameter
	{BRIGHTNESS_CONTRAST_NAME,		IID_BrightnessContrast,			CBrightnessContrast::CreateInstance,				VET_ADJUST,			&g_Default_BrightnessContrast,		sizeof(g_Default_BrightnessContrast)},
	{H_S_L_NAME,					IID_HueSaturationLightness,		CHueSaturationLightness::CreateInstance,			VET_ADJUST,			&g_Default_HueSaturationLightness,	sizeof(g_Default_HueSaturationLightness)},
	{ROTATE_FLIP_NAME,				IID_RotateFlip,					CRotateFlipFilter::CreateInstance,					VET_ADJUST,			&g_Default_RotateFlip,				sizeof(g_Default_RotateFlip)},
	{CROP_NAME,						IID_Crop,						CCropFilter::CreateInstance,						VET_ADJUST,			&g_Default_Crop,					sizeof(g_Default_Crop)},
	{BACKGROUND_NAME,				IID_Background,					CBackgroundFilter::CreateInstance,					VET_BACKGROUND,		NULL,								0},
	{WATERMARK_NAME,				IID_Watermark,					CWatermarkFilter::CreateInstance,					VET_BACKGROUND,		NULL,								0},
};


VideoFilterTemplate *GetVideoFilterTemplate(CLSID id)
{
	for(int i = 0; i < VIDEO_FILTER_COUNT; i++)
	{
		if (VideoFilterTemplates[i].m_ClsID == id)
		{
			return &VideoFilterTemplates[i];
		}
	}
	return NULL;
}

HRESULT __stdcall vcGetVideoEffectDefaultValue(GUID id, void *Parameters, int32_t cbSize)
{
	VideoFilterTemplate *p = GetVideoFilterTemplate(id);
	if (p == NULL) return E_FAIL_RESULT();
	if (p->m_cbParameter > cbSize)
	{
		return E_FAIL_RESULT();
	}
	CopyMemory(Parameters, p->m_pParameter, p->m_cbParameter);
	return S_OK;
}

BOOL CVideoFilterData::IsDefaultValue()
{
	if (m_id == IID_Watermark) return g_opt.m_LogoFile[0] == 0;
	if (m_Size == 0) return FALSE;
	VideoFilterTemplate *p = GetVideoFilterTemplate(m_id);
	return memcmp(p->m_pParameter, m_Parameter, m_Size) == 0;
}

CVideoFilterBase* CVideoFilterData::CreateInstance()
{
	VideoFilterTemplate *p = GetVideoFilterTemplate(m_id);
	m_Instance = p->m_lpfnNew(p, this);
	return m_Instance;
}

HRESULT __stdcall vcGetVideoEffect(ClipInfo *item, GUID id, void *value, int32_t size)
{
	return item->GetVideoEffect(id, value, size);
}

HRESULT __stdcall vcSetVideoEffect(ClipInfo *item, GUID id, void *value, int32_t size)
{
	return item->SetVideoEffect(id, value, size);
}
