#include "stdafx.h"
#include "FastStretchBlt.h"


CFastStretchBlt::CFastStretchBlt(int32_t srcWidth, int32_t srcHeight, int32_t srcStep, int32_t dstWidth, int32_t dstHeight, int32_t dstStep)
{
	m_srcWidth = srcWidth;
	m_srcHeight = srcHeight;
	m_srcStep = srcStep;
	m_dstWidth = dstWidth;
	m_dstHeight = dstHeight;
	m_dstStep = dstStep;
#ifdef USE_D3D_STRETCH
	Direct3DCreate();
#endif
}


CFastStretchBlt::~CFastStretchBlt(void)
{
#ifdef USE_D3D_STRETCH
	SAFE_RELEASE(m_surfaceSrc);
	SAFE_RELEASE(m_surfaceDst);
	SAFE_RELEASE(m_pRenderTarget);
	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pd3d);
#else
	SAFE_DELETE(m_dstGraphics);
	SAFE_DELETE(m_srcBitmap);
	SAFE_DELETE(m_dstBitmap);
#endif
}

#ifdef USE_D3D_STRETCH
HRESULT CFastStretchBlt::Direct3DCreate()
{
	HRESULT hr;

	m_pd3d = Direct3DCreate9(D3D_SDK_VERSION);
	if (m_pd3d == NULL) return E_FAIL;

	D3DPRESENT_PARAMETERS d3dPP;
	ZeroMemory(&d3dPP, sizeof(d3dPP));
	d3dPP.Windowed = TRUE;
	d3dPP.hDeviceWindow = NULL;
	d3dPP.SwapEffect = D3DSWAPEFFECT_COPY;
	d3dPP.BackBufferWidth = 1;
	d3dPP.BackBufferHeight = 1;
	d3dPP.BackBufferFormat = D3DFMT_X8R8G8B8;

	hr = m_pd3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, GetDesktopWindow(), D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dPP, &m_pDevice);
	//创建硬件顶点失败
	if (FAILED(hr))
	{		
		//创建软件顶点
		WarnLog("D3D Unable to Create Direct3D Device\n");
		hr = m_pd3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, GetDesktopWindow(), D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dPP, &m_pDevice);
	}
	if (FAILED(hr))
	{
		ErrorLog("D3D Unable to Create Direct3D Device\n");
		return hr;
	}

	hr = m_pDevice->CreateRenderTarget(m_dstWidth, m_dstHeight, D3DFMT_X8R8G8B8, D3DMULTISAMPLE_NONE, 0, FALSE, &m_pRenderTarget, NULL);  assert(hr == S_OK);
	hr = m_pDevice->CreateOffscreenPlainSurface(m_srcWidth, m_srcHeight, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &m_surfaceSrc, NULL); assert(hr == S_OK);
	hr = m_pDevice->CreateOffscreenPlainSurface(m_dstWidth, m_dstHeight, D3DFMT_X8R8G8B8, D3DPOOL_SYSTEMMEM, &m_surfaceDst, NULL); assert(hr == S_OK);

	//hr = m_pDevice->SetRenderTarget(0, m_pRenderTarget); assert(hr == S_OK);

	return S_OK;
}
#endif

void CFastStretchBlt::Proc(uint8_t *dst, uint8_t *src)
{
#ifdef USE_D3D_STRETCH
	HRESULT hr;
	D3DLOCKED_RECT LockedRect;

	hr = m_surfaceSrc->LockRect(&LockedRect, NULL, D3DLOCK_DISCARD);
	{
		LPBYTE p1 = (LPBYTE)LockedRect.pBits;
		LPBYTE p2 = src;
		int32_t w = m_srcWidth * 4;
		for(int i = 0; i < m_srcHeight; i++)
		{
			CopyMemory(p1, p2, w);
			p1 += LockedRect.Pitch;
			p2 += m_srcStep;
		}
	}
	m_surfaceSrc->UnlockRect();

	// 复制表面
	hr = m_pDevice->StretchRect(m_surfaceSrc, NULL, m_pRenderTarget, NULL, D3DTEXF_LINEAR);
	hr = m_pDevice->GetRenderTargetData(m_pRenderTarget, m_surfaceDst);
		
	hr = m_surfaceDst->LockRect(&LockedRect, NULL, D3DLOCK_DISCARD);
	{
		LPBYTE p1 = dst;
		LPBYTE p2 = (LPBYTE)LockedRect.pBits;
		int32_t w = m_dstWidth * 4;
		for(int i = 0; i < m_dstHeight; i++)
		{
			CopyMemory(p1, p2, w);
			p1 += m_dstStep;
			p2 += LockedRect.Pitch;
		}
	}
	hr = m_surfaceDst->UnlockRect();
#else
	if (m_Src != src)
	{
		SAFE_DELETE(m_srcBitmap);
		m_Src = src;
		m_srcBitmap = new Bitmap(m_srcWidth, m_srcHeight, m_srcStep, PixelFormat32bppRGB, (BYTE*)m_Src);
	}
	if (m_Dst != dst)
	{
		SAFE_DELETE(m_dstGraphics);
		SAFE_DELETE(m_dstBitmap);
		m_Dst = dst;
		m_dstBitmap = new Bitmap(m_dstWidth, m_dstHeight, m_dstStep, PixelFormat32bppRGB, (BYTE*)m_Dst);
		m_dstGraphics = new Graphics(m_dstBitmap);
	}
	m_dstGraphics->DrawImage(m_srcBitmap, 0, 0, m_dstWidth, m_dstHeight);
#endif
}

/*
void TestFastStretchBlt()
{
	uint8_t* src = (uint8_t*)malloc(1920 * 1088 * 4);
	uint8_t* dst = (uint8_t*)malloc(800 * 600 * 4);
	ZeroMemory(src, 1920 * 1088 * 4);
	ZeroMemory(dst, 800 * 600 * 4);

	CFastStretchBlt blt(1920, 1088, 800, 600);

	Bitmap bitmap1(1920, 1088, 1920 * 4, PixelFormat32bppRGB, src);
	Bitmap bitmap2(800, 600, 800 * 4, PixelFormat32bppRGB, dst);
	Graphics graphics(&bitmap2);

	LARGE_INTEGER liFrequency;
	LARGE_INTEGER liStart, liStop;
	QueryPerformanceFrequency(&liFrequency);

	for(int i = 0; i < 5; i++)
	{
		QueryPerformanceCounter(&liStart);
		blt.Proc(src, dst);
		QueryPerformanceCounter(&liStop);
		wchar_t buffer[256];
		StringCchPrintf(buffer, 256, L"F[%lld,%lld]\n", liStop.QuadPart - liStart.QuadPart, 1000ll * (liStop.QuadPart - liStart.QuadPart) / liFrequency.QuadPart);
		OutputDebugString(buffer);
	}

	for(int i = 0; i < 5; i++)
	{
		QueryPerformanceCounter(&liStart);
		graphics.DrawImage(&bitmap1, 0, 0, 800, 600);
		QueryPerformanceCounter(&liStop);
		wchar_t buffer[256];
		StringCchPrintf(buffer, 256, L"D[%lld,%lld]\n", liStop.QuadPart - liStart.QuadPart, 1000ll * (liStop.QuadPart - liStart.QuadPart) / liFrequency.QuadPart);
		OutputDebugString(buffer);
	}

	free(src);
	free(dst);
}
*/
