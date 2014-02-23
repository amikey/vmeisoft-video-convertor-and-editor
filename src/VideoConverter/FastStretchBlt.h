#pragma once

#define USE_D3D_STRETCH

class CFastStretchBlt
{
private:
#ifdef USE_D3D_STRETCH
	IDirect3D9*				m_pd3d;
	IDirect3DDevice9*		m_pDevice;
	IDirect3DSurface9*		m_pRenderTarget;
	IDirect3DSurface9*		m_surfaceSrc;
	IDirect3DSurface9*		m_surfaceDst;
#else
	Bitmap*					m_srcBitmap;
	Bitmap*					m_dstBitmap;
	Graphics*				m_dstGraphics;
	uint8_t*				m_Src;
	uint8_t*				m_Dst;
#endif
	int32_t						m_srcWidth;
	int32_t						m_srcHeight;
	int32_t						m_srcStep;
	int32_t						m_dstWidth;
	int32_t						m_dstHeight;
	int32_t						m_dstStep;

#ifdef USE_D3D_STRETCH
	HRESULT Direct3DCreate();
#endif

public:
	CFastStretchBlt(int32_t srcWidth, int32_t srcHeight, int32_t srcStep, int32_t dstWidth, int32_t dstHeight, int32_t dstStep);
	~CFastStretchBlt(void);

public:
	void Proc(uint8_t *dst, uint8_t *src);
};

//extern void TestFastStretchBlt();