#include "stdafx.h"
#include "VideoConverter.h"
#include "VideoRender.h"
#include "VideoUtils.h"
#include "SaveBitmapFile.h"
#include "CommonResource.h"

#define CORNER_WIDTH 5
#define BORDER_COLOR 0xFF304860

// {A83D0045-A1EA-4D2F-A9F3-2AF075CF1632}
static const GUID IID_VideoPlayer = 
{ 0xa83d0045, 0xa1ea, 0x4d2f, { 0xa9, 0xf3, 0x2a, 0xf0, 0x75, 0xcf, 0x16, 0x32 } };
static struct VideoFilterTemplate g_video_player_template = {L"Video Player", IID_VideoPlayer, NULL, VET_UTILITY, NULL, 0};

static DWORD BoxColors[] = 
{
	0xFFFF0000, 0xFFF08000, 0xFF80F000, 0xFF0000FF
};

static wchar_t WINDOW_RENDER_NAME[] = L"Video Renderer Class";

static LRESULT CALLBACK RenderWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_ERASEBKGND:
		return TRUE;

	case WM_PAINT:
		{
			ValidateRect(hWnd, NULL);
			((CVideoRender *)GetWindowLongPtr(hWnd, 0))->RenderWindowPaint();
		}
		return 0;

	case WM_TIMER:
		{
			((CVideoRender *)GetWindowLongPtr(hWnd, 0))->OnBoxTimer();
		}
		return 0;

	case WM_CAPTURECHANGED:
	case WM_CANCELMODE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_MOUSEMOVE:
		{
			((CVideoRender *)GetWindowLongPtr(hWnd, 0))->OnMouseEvent(hWnd, message, wParam, lParam);
		}
		return 0;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

void InitVideoRender()
{
	WNDCLASSEX wcex;

	ZeroMemory(&wcex, sizeof(wcex));

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style			= CS_VREDRAW | CS_HREDRAW;
	wcex.lpfnWndProc	= RenderWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= sizeof(CVideoRender *);
	wcex.hInstance		= g_hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= NULL;
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= WINDOW_RENDER_NAME;
	wcex.hIconSm		= NULL;
	RegisterClassEx(&wcex);

	ZeroMemory(&wcex, sizeof(wcex));

}

CVideoRender::CVideoRender(HWND hwndParent, HWND hwndMessage, int32_t width, int32_t height)
	:CVideoFilterBase(&g_video_player_template, NULL)
{
	m_FilterType = VIDEO_FILTER_TYPE_RENDER;
	m_SrcRect.left = 0;
	m_SrcRect.top = 0;
	m_SrcRect.right = width;
	m_SrcRect.bottom = height;
	m_hwndMessage = hwndMessage;
	m_hwndParent = hwndParent;
	m_VideoBuffer = (uint8_t *)MemoryAlloc(m_SrcRect.right * m_SrcRect.bottom * 4);
}

CVideoRender::~CVideoRender(void)
{
	SAFE_RELEASE(m_Surface);
	SAFE_RELEASE(m_pRenderTarget);
	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pd3d);

	if (m_hwnd)	DestroyWindow(m_hwnd);
	SAFE_FREE(m_VideoBuffer);
	SAFE_FREE(m_SelectionBuffer);
}

void CVideoRender::FilterRelease()
{
	// 
}

void CVideoRender::RenderWindowPaint()
{
	HRESULT hr = m_pDevice->TestCooperativeLevel();
	switch(hr)
	{
	case S_OK:
		Repaint();
		break;

	case D3DERR_DEVICENOTRESET:
		{
			hr = m_pDevice->Reset(&m_d3dPP);
			if (hr == S_OK) return ;
			Resize();
		}
		break;

	default:
		break;
	}
}

void CVideoRender::CalcWindowPos()
{
	RECT rect;

	GetClientRect(m_hwndParent, &rect);
	int32_t w = rect.right - rect.left - 2;
	int32_t h = rect.bottom - rect.top - 2;
	int32_t w1 = m_SrcRect.right;
	int32_t h1 = m_SrcRect.bottom;

	int32_t width;
	int32_t height;
	if ((w >= w1) && (h >= h1))
	{
		width = w1;
		height = h1; 
	}
	else if (w1 * h >= w * h1)
	{
		width = w;
		height = w * h1 / w1; 
	}
	else
	{
		width = h * w1 / h1;
		height = h; 
	}

	m_DstRect.left = 1;
	m_DstRect.top = 1;
	m_DstRect.right = width + 1;
	m_DstRect.bottom = height + 1;

	m_ViewRect.left = 0;
	m_ViewRect.top = 0;
	m_ViewRect.right = width + 2;
	m_ViewRect.bottom = height + 2;

	m_WindowPoint.x = (rect.right - rect.left - width - 2) / 2;
	m_WindowPoint.y = (rect.bottom - rect.top - height - 2) / 2;
}

void CVideoRender::Resize()
{
	CalcWindowPos();
	SetWindowPos(m_hwnd, HWND_TOP, m_WindowPoint.x, m_WindowPoint.y, m_ViewRect.right, m_ViewRect.bottom, SWP_SHOWWINDOW);
	RecreateDevice();
	CopyToSurface();
	if (m_SelectionBuffer) MapBoxVertexCoord();
	Repaint();
}

HRESULT CVideoRender::RecreateDevice()
{
	HRESULT hr;

	SAFE_RELEASE(m_Surface);
	SAFE_RELEASE(m_pRenderTarget);
	SAFE_RELEASE(m_pDevice);

	ZeroMemory(&m_d3dPP, sizeof(m_d3dPP));
	m_d3dPP.Windowed = TRUE;
	m_d3dPP.hDeviceWindow = m_hwnd;
	m_d3dPP.SwapEffect = D3DSWAPEFFECT_COPY;
	m_d3dPP.BackBufferCount = 0;
	m_d3dPP.BackBufferWidth = m_ViewRect.right;
	m_d3dPP.BackBufferHeight = m_ViewRect.bottom;
	m_d3dPP.BackBufferFormat = D3DFMT_X8R8G8B8;
	m_d3dPP.MultiSampleType = D3DMULTISAMPLE_NONE;
	m_d3dPP.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	m_d3dPP.Flags = D3DPRESENTFLAG_VIDEO;

	// 创建硬件顶点
	hr = m_pd3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, NULL, D3DCREATE_HARDWARE_VERTEXPROCESSING, &m_d3dPP, &m_pDevice);	
	if (FAILED(hr))
	{		
		// 创建软件顶点
		WriteLog("D3D Unable to Create Direct3D Device\n");
		hr = m_pd3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, NULL, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &m_d3dPP, &m_pDevice);
	}
	if (FAILED(hr))
	{		
		// 软件模拟
		WriteLog("D3D Unable to Create Direct3D Device\n");
		hr = m_pd3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, NULL, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &m_d3dPP, &m_pDevice);
	}
	if (FAILED(hr))
	{
		WriteLog("D3D Unable to Create Direct3D Device\n");
		return hr;
	}

	m_pDevice->GetRenderTarget(0, &m_pRenderTarget);

	m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	m_pDevice->SetRenderState(D3DRS_ALPHAREF, (DWORD)0x0);
	m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

	hr = m_pDevice->CreateOffscreenPlainSurface(m_SrcRect.right, m_SrcRect.bottom, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &m_Surface, NULL);
	if (FAILED(hr))
	{
		WriteLog("Unsupported this D3D FORMAT.\n", hr);
		return hr;
	}

	D3DVIEWPORT9 vp = {0, 0, m_ViewRect.right, m_ViewRect.bottom, 0, 1};
	m_pDevice->SetViewport(&vp);

	return S_OK;

}

HRESULT CVideoRender::Init(void)
{
	CalcWindowPos();

	m_hwnd = CreateWindowEx(
		WS_EX_NOPARENTNOTIFY, 
		WINDOW_RENDER_NAME, NULL, 
		WS_CHILD, 
		m_WindowPoint.x, m_WindowPoint.y, m_ViewRect.right, m_ViewRect.bottom, 
		m_hwndParent, 
		NULL, g_hInstance, NULL);

	SetWindowLongPtr(m_hwnd, 0, (LONG_PTR)this);

	m_pd3d = Direct3DCreate9(D3D_SDK_VERSION);
	if (m_pd3d == NULL)
	{
		WriteLog("Faild to Direct3DCreate9\n");
		return E_FAIL;
	}

	RecreateDevice();
	ShowWindow(m_hwnd, SW_SHOW);
	return S_OK;
}

HRESULT CVideoRender::CopyToSurface()
{
	HRESULT hr;

	uint8_t *Buffer;
	if (m_SelectionBuffer)
	{
		memcpy(m_SelectionBuffer, m_VideoBuffer, m_SrcRect.right * m_SrcRect.bottom * 4);
		gray_32bpp(m_SelectionBuffer,
			m_SelectionRect.left, m_SelectionRect.top, 
			m_SelectionRect.right, m_SelectionRect.bottom,
			m_SrcRect.right, m_SrcRect.bottom);
		Buffer = m_SelectionBuffer;
	}
	else
	{
		Buffer = m_VideoBuffer;
	}

	D3DLOCKED_RECT LockedRect;
	hr = m_Surface->LockRect(&LockedRect, NULL, D3DLOCK_DISCARD); assert(hr == S_OK);

	uint8_t* p1 = (uint8_t *)LockedRect.pBits;
	uint8_t* p2 = (uint8_t *)Buffer;
	int32_t w1 = LockedRect.Pitch;
	int32_t w2 = m_SrcRect.right * 4;
	for(int i = 0; i < m_SrcRect.bottom; i++)
	{
		memcpy(p1, p2, w2);
		p1 += w1;
		p2 += w2;
	}
	hr = m_Surface->UnlockRect();

	return hr;
}


HRESULT CVideoRender::Repaint()
{
	HRESULT hr;

	m_pDevice->BeginScene();
	m_pDevice->Clear(1, (D3DRECT *)&m_ViewRect, D3DCLEAR_TARGET, BORDER_COLOR, 0, 0);

	hr = m_pDevice->StretchRect(m_Surface, &m_SrcRect, m_pRenderTarget, &m_DstRect, D3DTEXF_LINEAR);

	if (m_SelectionBuffer)
	{
		m_pDevice->SetFVF( D3DFVF_BOX_VERTEX );
		m_pDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, 16, &m_Vertex, sizeof(SELECT_BOX_VERTEX));
	}

	m_pDevice->EndScene();
	hr = m_pDevice->Present(&m_ViewRect, NULL, NULL, NULL); assert(hr == S_OK);

	return S_OK;
}

void CVideoRender::MapBoxVertexCoord()
{
	float left = (float)(m_SelectionRect.left);
	float top = (float)(m_SelectionRect.top);
	float right = (float)(m_SelectionRect.right);
	float bottom = (float)(m_SelectionRect.bottom);
	float x = (float)(m_DstRect.right);
	float xx = (float)(m_SrcRect.right);
	float y = (float)(m_DstRect.bottom);
	float yy = (float)(m_SrcRect.bottom);

	float x0 = left * x / xx;
	float x1 = (left * 3.0f + right) * x / (xx * 4.0f);
	float x2 = (left + right) * x / (xx * 2.0f);
	float x3 = (left + right * 3.0f) * x / (xx * 4.0f);
	float x4 = right * x / xx;

	float y0 = top * y / yy;
	float y1 = (top * 3.0f + bottom) * y / (yy * 4.0f);
	float y2 = (top + bottom) * y / (yy * 2.0f);
	float y3 = (top + bottom * 3.0f) * y / (yy * 4.0f);
	float y4 = bottom * y / yy;

	m_BoxRect.left = (int32_t)x0;
	m_BoxRect.right = (int32_t)x4;
	m_BoxRect.top = (int32_t)y0;
	m_BoxRect.bottom = (int32_t)y4;
	SetBoxVertexCoord(x0, x1, x2, x3, x4, y0, y1, y2, y3, y4);
}

void CVideoRender::SetBoxVertexCoord(float x0, float x1, float x2, float x3, float x4, float y0, float y1, float y2, float y3, float y4)
{
	m_Vertex[0].x = x0;
	m_Vertex[0].y = y0;

	m_Vertex[1].x = x1; m_Vertex[2].x = x2; m_Vertex[3].x = x3; m_Vertex[4].x = x4;
	m_Vertex[1].y = m_Vertex[2].y = m_Vertex[3].y = m_Vertex[4].y = y0;

	m_Vertex[5].x = m_Vertex[6].x = m_Vertex[7].x = m_Vertex[8].x = x4;
	m_Vertex[5].y = y1; m_Vertex[6].y = y2;	m_Vertex[7].y = y3;	m_Vertex[8].y = y4;

	m_Vertex[9].x = x3; m_Vertex[10].x = x2; m_Vertex[11].x = x1; m_Vertex[12].x = x0;
	m_Vertex[9].y = m_Vertex[10].y = m_Vertex[11].y = m_Vertex[12].y = y4;

	m_Vertex[13].x = m_Vertex[14].x = m_Vertex[15].x = m_Vertex[16].x = x0;
	m_Vertex[13].y = y3; m_Vertex[14].y = y2; m_Vertex[15].y = y1; m_Vertex[16].y = y0;
}

void CVideoRender::SetBoxVertexColor()
{
	for(int i = 0; i < 17; i++)
	{
		m_Vertex[i].color = BoxColors[(m_BoxFlip + i) % 4];
	}
}

void CVideoRender::OnBoxTimer()
{

	if (m_SelectionBuffer == NULL) return ;

	m_BoxFlip ++;
	if (m_BoxFlip > 10000) m_BoxFlip = 0;
	SetBoxVertexColor();

	m_pDevice->BeginScene();
	m_pDevice->SetFVF( D3DFVF_BOX_VERTEX );
	m_pDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, 16, &m_Vertex, sizeof(SELECT_BOX_VERTEX));
	m_pDevice->EndScene();
	m_pDevice->Present(&m_ViewRect, NULL, NULL, NULL);
}

void CVideoRender::SetSelectionRect(Rect *rect)
{

	if (rect->X < 0) rect->X = 0;
	if (rect->Y < 0) rect->Y = 0;
	if (rect->X > m_SrcRect.right - 1) rect->X = 0;
	if (rect->X > m_SrcRect.bottom - 1) rect->X = 0;
	if (rect->Width < 0) rect->Width = 0;
	if (rect->Height < 0) rect->Height = 0;
	if (rect->GetRight() > m_SrcRect.right) rect->Width = 0;
	if (rect->GetBottom() > m_SrcRect.bottom) rect->Height = 0;

	m_SelectionRect.left = rect->X;
	m_SelectionRect.top = rect->Y;
	m_SelectionRect.right = rect->Width ? rect->GetRight() : m_SrcRect.right;
	m_SelectionRect.bottom = rect->Height ? rect->GetBottom() : m_SrcRect.bottom;

	if (m_SelectionBuffer == NULL)
	{
		m_SelectionBuffer = (uint8_t *)MemoryAlloc((m_SrcRect.right + 1) * (m_SrcRect.bottom + 1) * 4);
	}

	MapBoxVertexCoord();
	SetBoxVertexColor();
	CopyToSurface();
	Repaint();
	if (m_BoxTimer == 0)
	{
		m_BoxTimer = SetTimer(m_hwnd, 1, 300, NULL);
	}
}

void CVideoRender::RemoveSelectionRect()
{
	if (m_SelectionBuffer == NULL) return ;
	KillTimer(m_hwnd, m_BoxTimer);
	SAFE_FREE(m_SelectionBuffer);
	CopyToSurface();
	Repaint();
}

uint8_t* CVideoRender::GetBuffer()
{
	return m_VideoBuffer;
}

void CVideoRender::Process(uint8_t *source, int64_t Pts)
{
	if (source != m_VideoBuffer)
	{
		CopyMemory(m_VideoBuffer, source, m_SrcRect.right * m_SrcRect.bottom * 4);
	}
	CopyToSurface();
	Repaint();
}

void CVideoRender::NewSegment(int64_t tStart, int64_t tStop)
{
}

void CVideoRender::EndOfStream()
{
}

void CVideoRender::PutSourceSize(int32_t width, int32_t height)
{
	assert(width == m_SrcRect.right);
	assert(height == m_SrcRect.bottom);
}

HRESULT CVideoRender::SaveToFile(wchar_t *wszFile)
{
	return SaveToImageFile(wszFile, m_VideoBuffer, m_SrcRect.right, m_SrcRect.bottom);
}


#define CORNER_WIDTH 5

#define DRAG_MOUSE_NONE				0
#define DRAG_MOUSE_LEFT				0x1
#define DRAG_MOUSE_TOP				0x2
#define DRAG_MOUSE_RIGHT			0x4
#define DRAG_MOUSE_BOTTOM			0x8
#define DRAG_MOUSE_CENTER			0x10
#define DRAG_MOUSE_MOVE				0x20
#define DRAG_MOUSE_CROP				0x40

void CVideoRender::OnMouseEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CAPTURECHANGED:
		if ((m_DragFlag) && (GetCapture() != m_hwnd))
		{
			OnMouseDraggedStop();
		}
		break;

	case WM_CANCELMODE:
	case WM_LBUTTONUP:
		if (m_DragFlag)
		{
			OnMouseDraggedStop();
		}
		break;

	case WM_LBUTTONDOWN:
		{
			int32_t xPos = GET_X_LPARAM(lParam); 
			int32_t yPos = GET_Y_LPARAM(lParam); 
			OnMouseDown(xPos, yPos);
		}
		break;

	case WM_MOUSEMOVE:
		if (m_DragFlag)
		{
			if (GetCapture() != hWnd)
			{
				OnMouseDraggedStop();
			}
			else
			{
				int32_t xPos = GET_X_LPARAM(lParam); 
				int32_t yPos = GET_Y_LPARAM(lParam); 
				OnMouseDragged(xPos, yPos);
			}
		}
		else
		{
			int32_t xPos = GET_X_LPARAM(lParam); 
			int32_t yPos = GET_Y_LPARAM(lParam); 
			OnMouseMoved(xPos, yPos);
		}
		break;
	}
}

void CVideoRender::OnMouseDraggedStop()
{
	m_DragFlag = 0;
	if (GetCapture() == m_hwnd) ReleaseCapture();
	SetCursor(hcur_arrow);

	BOOL bRedraw = FALSE;
	if (m_BoxRect.left == m_BoxRect.right)
	{
		m_SelectionRect.left = 0;
		m_SelectionRect.right = m_SrcRect.right;
		bRedraw = TRUE;
	}
	if (m_BoxRect.top == m_BoxRect.bottom)
	{
		m_SelectionRect.top = 0;
		m_SelectionRect.bottom = m_SrcRect.bottom;
		bRedraw = TRUE;
	}
	if (bRedraw == FALSE) return ;

	MapBoxVertexCoord();
	CopyToSurface();
	Repaint();

	DWORD wParam = m_SelectionRect.left | (m_SelectionRect.top << 16);
	DWORD lParam = (m_SelectionRect.right - m_SelectionRect.left) | ((m_SelectionRect.bottom - m_SelectionRect.top) << 16);
	PostMessage(m_hwndMessage, VC_VIDEO_PLAYER_CROP, wParam, lParam);
}

void CVideoRender::OnMouseDown(const int32_t xPos, const int32_t yPos)
{
	if (m_SelectionBuffer == NULL) return;

	m_DragFlag = HintTest(xPos, yPos);
	if (m_DragFlag == 0) return ;

	if (m_DragFlag & DRAG_MOUSE_LEFT)
	{
		m_xDelta = m_BoxRect.left - xPos;
		m_xMin = 0;
		m_xMax = m_BoxRect.right - CORNER_WIDTH;
	}

	if (m_DragFlag & DRAG_MOUSE_TOP)
	{
		m_yDelta = m_BoxRect.top - yPos;
		m_yMin = 0;
		m_yMax = m_BoxRect.bottom - CORNER_WIDTH;
	}

	if (m_DragFlag & DRAG_MOUSE_RIGHT)
	{
		m_xDelta = m_BoxRect.right - xPos;
		m_xMin = m_BoxRect.left + CORNER_WIDTH;
		m_xMax = m_DstRect.right;
	}

	if (m_DragFlag & DRAG_MOUSE_BOTTOM)
	{
		m_yDelta = m_BoxRect.bottom - yPos;
		m_yMin = m_BoxRect.top + CORNER_WIDTH;
		m_yMax = m_DstRect.bottom;
	}

	if (m_DragFlag & DRAG_MOUSE_CENTER)
	{
		m_xDelta = -xPos;
		m_yDelta = -yPos;
		m_xMin = -m_BoxRect.left;
		m_xMax = m_DstRect.right - m_BoxRect.right;
		m_yMin = -m_BoxRect.top;
		m_yMax = m_DstRect.bottom - m_BoxRect.bottom;
		m_rcStart = m_BoxRect;
		m_DragFlag = DRAG_MOUSE_MOVE;
	}

	if (m_DragFlag & DRAG_MOUSE_CROP)
	{
		m_xDelta = 0;
		m_yDelta = 0;
		m_xStart = xPos;
		m_yStart = yPos;
		m_xMin = 0;
		m_xMax = m_DstRect.right;
		m_yMin = 0;
		m_yMax = m_DstRect.bottom;
	}

	m_xLast = xPos;
	m_yLast = yPos;
	SetCurrorType(m_DragFlag);
	SetCapture(m_hwnd);
}

void CVideoRender::OnMouseMoved(const int32_t xPos, const int32_t yPos)
{
	if (m_SelectionBuffer == NULL) return;
	if ((m_xLast == xPos) && (m_yLast == yPos)) return ;

	m_xLast = xPos;
	m_yLast = yPos;

	if (m_DragFlag)
	{
		OnMouseDragged(xPos, yPos);
	}
	else
	{
		SetCurrorType(HintTest(xPos, yPos));		
	}
}

#define CheckDelta(v) \
	if (v##Cur < m_##v##Min) v##Cur = m_##v##Min; \
	if (v##Cur > m_##v##Max) v##Cur = m_##v##Max;

void CVideoRender::OnMouseDragged(const int32_t xPos, const int32_t yPos)
{
	int32_t xCur = xPos + m_xDelta; CheckDelta(x);
	int32_t yCur = yPos + m_yDelta; CheckDelta(y);

	if (m_DragFlag & DRAG_MOUSE_LEFT)
	{
		m_BoxRect.left = xCur;
	}

	if (m_DragFlag & DRAG_MOUSE_TOP)
	{
		m_BoxRect.top = yCur;
	}

	if (m_DragFlag & DRAG_MOUSE_RIGHT)
	{
		m_BoxRect.right = xCur;
	}

	if (m_DragFlag & DRAG_MOUSE_BOTTOM)
	{
		m_BoxRect.bottom = yCur;
	}

	if (m_DragFlag & DRAG_MOUSE_MOVE)
	{
		m_BoxRect.left = m_rcStart.left + xCur;
		m_BoxRect.right = m_rcStart.right + xCur;
		m_BoxRect.top = m_rcStart.top + yCur;
		m_BoxRect.bottom = m_rcStart.bottom + yCur;
	}

	if (m_DragFlag & DRAG_MOUSE_CROP)
	{
		if (xCur < m_xStart)
		{
			m_BoxRect.left = xCur;
			m_BoxRect.right = m_xStart;
		}
		else
		{
			m_BoxRect.left = m_xStart;
			m_BoxRect.right = xCur;
		}
		if (yCur < m_yStart)
		{
			m_BoxRect.top = yCur;
			m_BoxRect.bottom = m_yStart;
		}
		else
		{
			m_BoxRect.top = m_yStart;
			m_BoxRect.bottom = yCur;
		}
	}

	float left = (float)(m_BoxRect.left);
	float top = (float)(m_BoxRect.top);
	float right = (float)(m_BoxRect.right);
	float bottom = (float)(m_BoxRect.bottom);

	float x = (float)(m_SrcRect.right);
	float xx = (float)(m_DstRect.right);
	float y = (float)(m_SrcRect.bottom);
	float yy = (float)(m_DstRect.bottom);

	m_SelectionRect.left = (int32_t)(left * x / xx);
	m_SelectionRect.top = (int32_t)(top * y / yy);
	m_SelectionRect.right = (int32_t)(right * x / xx);
	m_SelectionRect.bottom = (int32_t)(bottom * y / yy);

	float x0 = left;
	float x1 = (left * 3.0f + right) / 4.0f;
	float x2 = (left + right) / 2.0f;
	float x3 = (left + right * 3.0f) / 4.0f;
	float x4 = right;

	float y0 = top ;
	float y1 = (top * 3.0f + bottom) / (4.0f);
	float y2 = (top + bottom) / (2.0f);
	float y3 = (top + bottom * 3.0f) / (4.0f);
	float y4 = bottom;

	SetBoxVertexCoord(x0, x1, x2, x3, x4, y0, y1, y2, y3, y4);
	CopyToSurface();
	Repaint();

	DWORD wParam = m_SelectionRect.left | (m_SelectionRect.top << 16);
	DWORD lParam = (m_SelectionRect.right - m_SelectionRect.left) | ((m_SelectionRect.bottom - m_SelectionRect.top) << 16);
	PostMessage(m_hwndMessage, VC_VIDEO_PLAYER_CROP, wParam, lParam);
}

int32_t CVideoRender::HintTest(const int32_t xPos, const int32_t yPos)
{
	if (InSelectionRect(xPos, yPos) == FALSE)
	{
		return 0;
	}

	int32_t left = m_BoxRect.left + CORNER_WIDTH;
	int32_t right = m_BoxRect.right - CORNER_WIDTH;
	int32_t top = m_BoxRect.top + CORNER_WIDTH;
	int32_t bottom = m_BoxRect.bottom - CORNER_WIDTH;
	int32_t flag = 0;

	if (xPos < left)
	{
		flag |= DRAG_MOUSE_LEFT;
	}
	else if (xPos >= right)
	{
		flag |= DRAG_MOUSE_RIGHT;
	}

	if (yPos < top)
	{
		flag |= DRAG_MOUSE_TOP;
	}
	else if (yPos >= bottom)
	{
		flag |= DRAG_MOUSE_BOTTOM;
	}

	if (flag == 0)
	{
		flag = memcmp(&m_SrcRect, &m_SelectionRect, sizeof(RECT)) == 0 ? DRAG_MOUSE_CROP : DRAG_MOUSE_CENTER;
	}

	return flag;
}

void CVideoRender::SetCurrorType(const int32_t type)
{
	HCURSOR aCursor;
	switch(type)
	{
	case DRAG_MOUSE_LEFT | DRAG_MOUSE_TOP:
	case DRAG_MOUSE_RIGHT | DRAG_MOUSE_BOTTOM:
		aCursor = hcur_sizenwse;
		break;

	case DRAG_MOUSE_RIGHT | DRAG_MOUSE_TOP:
	case DRAG_MOUSE_LEFT | DRAG_MOUSE_BOTTOM:
		aCursor = hcur_sizenesw;
		break;

	case DRAG_MOUSE_LEFT:
	case DRAG_MOUSE_RIGHT:
		aCursor = hcur_sizewe;
		break;

	case DRAG_MOUSE_TOP:
	case DRAG_MOUSE_BOTTOM:
		aCursor = hcur_sizens;
		break;

	case DRAG_MOUSE_CENTER:
		aCursor = hcur_select;
		break;

	case DRAG_MOUSE_MOVE:
		aCursor = hcur_sizeall;
		break;

	case DRAG_MOUSE_CROP:
		aCursor = hcur_crop;
		break;

	default:
		aCursor = hcur_arrow;
		break;
	}

	SetCursor(aCursor);
}

BOOL CVideoRender::InSelectionRect(const int32_t xPos, const int32_t yPos)
{
	return	(xPos >= m_BoxRect.left) && (xPos <= m_BoxRect.right) && (yPos >= m_BoxRect.top) && (yPos <= m_BoxRect.bottom);
}
