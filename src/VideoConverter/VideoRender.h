#pragma once
#include "VideoFilters.h"
#include "VideoFilter.h"

#define D3DFVF_BOX_VERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)
struct SELECT_BOX_VERTEX 
{
    FLOAT x, y, z, rhw; // The transformed position for the vertex
    DWORD color;        // The vertex color
};

class CVideoRender:
	public CVideoFilterBase
{
private:
	D3DPRESENT_PARAMETERS	m_d3dPP;
	IDirect3D9*				m_pd3d;
	IDirect3DDevice9*		m_pDevice;
	IDirect3DSurface9*		m_pRenderTarget;
	IDirect3DSurface9*		m_Surface;

	RECT					m_SrcRect;
	RECT					m_DstRect;
	RECT					m_ViewRect;
	POINT					m_WindowPoint;

private:
	HWND					m_hwndMessage;
	HWND					m_hwndParent;
	HWND					m_hwnd;

private:
	uint8_t*				m_VideoBuffer;
	uint8_t*				m_SelectionBuffer;
	RECT					m_SelectionRect;
	uint32_t				m_BoxFlip;
	uint32_t				m_BoxTimer;
	SELECT_BOX_VERTEX		m_Vertex[17];
	void MapBoxVertexCoord();
	void SetBoxVertexCoord(float x0, float x1, float x2, float x3, float x4, float y0, float y1, float y2, float y3, float y4);
	void SetBoxVertexColor();

private:
	RECT					m_BoxRect;
	int32_t						m_DragFlag;
	int32_t						m_xDelta, m_yDelta;
	int32_t						m_xMin, m_yMin;
	int32_t						m_xMax, m_yMax;
	int32_t						m_xLast, m_yLast;
	int32_t						m_xStart, m_yStart;
	RECT					m_rcStart;

private:
	void OnMouseDown(const int32_t xPos, const int32_t yPos);
	void OnMouseMoved(const int32_t xPos, const int32_t yPos);
	void OnMouseDragged(const int32_t xPos, const int32_t yPos);
	void OnMouseDraggedStop();
	int32_t HintTest(const int32_t xPos, const int32_t yPos);
	void SetCurrorType(const int32_t type);
	BOOL InSelectionRect(const int32_t xPos, const int32_t yPos);

public:
	void OnMouseEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

public:
	void OnBoxTimer();

public:
	CVideoRender(HWND hwndParent, HWND hwndMessage, int32_t width, int32_t height);
	~CVideoRender(void);
	virtual void FilterRelease();

	HRESULT Init(void);

private:
	void CalcWindowPos();
	HRESULT RecreateDevice();
	HRESULT CopyToSurface();
	HRESULT Repaint();

public:
	virtual void NewSegment(int64_t tStart, int64_t tStop); 
	virtual void EndOfStream(); 
	virtual void PutSourceSize(int32_t width, int32_t height);
	virtual uint8_t* GetBuffer();
	virtual void Process(uint8_t *source, int64_t Pts);

public:
	void SetSelectionRect(Rect *rect);
	void RemoveSelectionRect();

public:
	void RenderWindowPaint();

public:
	void Resize();
	HRESULT SaveToFile(wchar_t *wszFile);

};

extern void InitVideoRender();
