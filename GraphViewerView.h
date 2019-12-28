#ifndef GRAPHVIEWERVIEW_H
#define GRAPHVIEWERVIEW_H

#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <limits>
#include "GraphViewerDoc.h"
#include "FParser.h"
#include "MainFrm.h"
//------------------------------------------------------------------------------------------//
#define  ZOOMSTEP 100
#define  ISOP(X) (X=='+' || X == '-' || X == '*' || X == '/' || X == '^' || X == '(')
#define  N (((m_curFunc->params.to) - (m_curFunc->params.from))/(m_curFunc->params.delta) + 1)
//------------------------------------------------------------------------------------------//
using namespace std;

class CGraph
{
public:
	//---------------------------------------------------------------------------------------------//
	CGraph(RECT rc, Function *pFunc, DWORD col) : m_x0(pFunc->params.from), m_xn(pFunc->params.to), m_gridColor(col)
	{
		m_gridPen = CreatePen(PS_SOLID, 1, m_gridColor);
		m_axisesPen= CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
		m_graphPen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
		UpdateRect(rc, pFunc);
	}
	//---------------------------------------------------------------------------------------------//
	virtual ~CGraph()
	{
		DeleteObject(m_gridPen);
		DeleteObject(m_axisesPen);
		DeleteObject(m_graphPen);
	}
	//---------------------------------------------------------------------------------------------//
	virtual BOOL GetFuncValue(double arg, double *fValue, Function *pFunc, FParser *pParser)
	{
		if(!pParser || !pFunc || arg < m_x0 - pFunc->params.delta || arg > m_xn + pFunc->params.delta) 
			return FALSE; //failed

		*fValue = pParser->Execute(arg, &(pFunc->ops));
		return TRUE;
	}
	//---------------------------------------------------------------------------------------------//
	virtual void DrawGraph(HDC hDC, USHORT style, Function *pFunc, 
		FParser *pParser, RECT &ClientRect, CPoint &scrlpos,
		int &GridDelta, HWND hWnd)
		// style == 0 --> no grid, style == 1 --> simple grid, 
	{
		POINT ptFrom, ptTo;
		ptTo.x = ClientRect.left + scrlpos.x;

		double arg;
		double fVal;

		int YposOfOx = FValToScreenY(0, pFunc);
		int XposOfOy = ArgToScreenX(0);

		//draw simple grid 
		if(style == 1)
		{
			SelectObject(hDC, m_gridPen);
			//vertical lines;
			int xPos = XposOfOy - GridDelta;
			if(XposOfOy > ClientRect.right + scrlpos.x)
				xPos = XposOfOy - ((XposOfOy - (ClientRect.right + scrlpos.x))/GridDelta + 1)*GridDelta;
			while(xPos >= ClientRect.left + scrlpos.x)
			{
				Line(hDC, xPos, ClientRect.top + scrlpos.y, xPos, ClientRect.bottom + scrlpos.y);
				xPos -= GridDelta;
			}
			xPos = XposOfOy + GridDelta;
			if(XposOfOy < ClientRect.left + scrlpos.x)
				xPos = XposOfOy + ((ClientRect.left + scrlpos.x - XposOfOy)/GridDelta)*GridDelta;
			while(xPos <= ClientRect.right + scrlpos.x)
			{
				Line(hDC, xPos, ClientRect.top + scrlpos.y, xPos, ClientRect.bottom + scrlpos.y);
				xPos += GridDelta;
			}
			//horizontal lines
			int yPos = YposOfOx - GridDelta;
			if(YposOfOx > ClientRect.bottom + scrlpos.y)
				yPos = YposOfOx - ((YposOfOx - (ClientRect.bottom + scrlpos.y))/GridDelta + 1)*GridDelta;
			while(yPos >= ClientRect.top + scrlpos.y)
			{
				Line(hDC, ClientRect.left + scrlpos.x, yPos, ClientRect.right + scrlpos.x, yPos);
				yPos -= GridDelta;
			}
			yPos = YposOfOx + GridDelta;
			if(YposOfOx < ClientRect.top + scrlpos.y)
				yPos = YposOfOx + ((ClientRect.top + scrlpos.y - YposOfOx)/GridDelta)*GridDelta;
			while(yPos <= ClientRect.bottom + scrlpos.y)
			{
				Line(hDC, ClientRect.left + scrlpos.x, yPos, ClientRect.right + scrlpos.x, yPos);
				yPos += GridDelta;
			}
		}
		//draw axises
		SelectObject(hDC, m_axisesPen);
		//Drawing Ox
		if((YposOfOx >= ClientRect.top + scrlpos.y) && 
			(YposOfOx <= ClientRect.bottom + scrlpos.y))
			Line(hDC, ClientRect.left + scrlpos.x, YposOfOx, ClientRect.right + scrlpos.x, YposOfOx);
		//Drawing Oy
		if((XposOfOy >= ClientRect.left + scrlpos.x) && 
			(XposOfOy <= ClientRect.right + scrlpos.x))
			Line(hDC, XposOfOy, ClientRect.top + scrlpos.y, XposOfOy, ClientRect.bottom + scrlpos.y);	 
		//draw graph
		SelectObject(hDC, m_graphPen);
		CString strCheck;
		int count = 0;
		while(ptTo.x <= ClientRect.right + scrlpos.x && ptTo.x <= m_rect.right)
		{
			arg = ScreenXToArg(ptTo.x);
			if(!GetFuncValue(arg, &fVal, pFunc, pParser))
				break;
			strCheck.Format(TEXT("%lf"), fVal);
			if(strCheck.GetAt(3) == '#') //NaN (checking by _isnan() doesn't work!?)
			{
				(ptTo.x)++;
				continue;
			}
			if(fVal > pFunc->max)
			{
				pFunc->max = fVal;
			}
			if(fVal < pFunc->min)
			{
				pFunc->min = fVal;
			}
			ptTo.y = FValToScreenY(fVal, pFunc);
			if(count) Line(hDC, ptFrom, ptTo);

			ptFrom = ptTo;
			(ptTo.x)++;
			count++;
		}
	}
	//---------------------------------------------------------------------------------------------//
	void UpdateRect(RECT &rc, Function *pFunc)
	{
		m_rect.left = rc.left;
		m_rect.top = rc.top;
		m_rect.right = rc.right;
		m_rect.bottom = rc.bottom;
		m_x0 = pFunc->params.from;
		m_xn = pFunc->params.to;
		UpdateRatios(pFunc);
	}
	//---------------------------------------------------------------------------------------------//
	static void Line(HDC hDC, int x0, int y0, int x1, int y1)
	{
		MoveToEx(hDC, x0, y0, NULL);
		LineTo(hDC, x1, y1);
	}
	//---------------------------------------------------------------------------------------------//
	static void Line(HDC hDC, POINT from, POINT to)
	{
		Line(hDC, from.x, from.y, to.x, to.y);
	}
	//---------------------------------------------------------------------------------------------//
	int FValToScreenY(double fval, Function *pFunc)
	{
		if(pFunc) return int(m_rect.bottom - (fval - pFunc->min)*m_ratioY);
		return -1;
	}
	//---------------------------------------------------------------------------------------------//
	int ArgToScreenX(double arg)
	{
		return int(m_rect.right - m_ratioX*(m_xn - arg));
	}
	//---------------------------------------------------------------------------------------------//
	double ScreenYToFVal(int scrY, Function *pFunc)
	{
		return (pFunc->min + double(m_rect.bottom - scrY)/m_ratioY);
	}
	//---------------------------------------------------------------------------------------------//
	double ScreenXToArg(int scrX)
	{
		return (m_xn - double(m_rect.right - scrX)/m_ratioX);
	}
	//---------------------------------------------------------------------------------------------//
private:
	void UpdateRatios(Function *pFunc)
	{
		m_ratioX = double(m_rect.right - m_rect.left)/(m_xn - m_x0);
		m_ratioY = double(m_rect.bottom - m_rect.top)/(pFunc->max - pFunc->min);	 
	}

	double    m_x0;
	double    m_xn;
	double    m_ratioX;
	double    m_ratioY;

	RECT      m_rect;
	HPEN	  m_gridPen, m_axisesPen, m_graphPen;
	DWORD	  m_gridColor;
};

class CGraphViewerView : public CFormView
{
protected: 
	CGraphViewerView();
	DECLARE_DYNCREATE(CGraphViewerView)
	//{{AFX_MSG(CGraphViewerView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnZoomIn();
	afx_msg void OnZoomOut();
	afx_msg void OnRun();
	afx_msg void OnSavetxt();
	afx_msg void OnZoomInAlongOx();
	afx_msg void OnZoomOutAlongOx();
	afx_msg void OnZoominAlongOy();
	afx_msg void OnZoomoutAlongOy();
	afx_msg void OnCloseupFuncCombo();
	afx_msg void OnShowgrid();
	afx_msg void OnUpdateSavetxt(CCmdUI* pCmdUI);
	afx_msg void OnUpdateZoomin(CCmdUI* pCmdUI);
	afx_msg void OnUpdateZoominAlongOx(CCmdUI* pCmdUI);
	afx_msg void OnUpdateZoominAlongOy(CCmdUI* pCmdUI);
	afx_msg void OnUpdateZoomout(CCmdUI* pCmdUI);
	afx_msg void OnUpdateZoomoutAlongOx(CCmdUI* pCmdUI);
	afx_msg void OnUpdateZoomoutAlongOy(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileSaveAs(CCmdUI* pCmdUI);
	afx_msg void OnUpdateGotoButton(CCmdUI* pCmdUI);
	afx_msg void OnBuild();
	afx_msg void OnGoto();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnUpdateShowgrid(CCmdUI* pCmdUI);
	afx_msg void OnFitToWindow();
	afx_msg void OnUpdateFitToWindow(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	void Reset();
	//{{AFX_DATA(CGraphViewerView)
	enum{ IDD = IDD_GRAPHVIEWER_FORM };
	//}}AFX_DATA

	virtual ~CGraphViewerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	CGraphViewerDoc* GetDocument();

	//{{AFX_VIRTUAL(CGraphViewerView)
protected:
	virtual void OnInitialUpdate(); 
	virtual void OnDraw(CDC* pDC);
	//}}AFX_VIRTUAL

private:
	inline void UpdateScrollPos() { m_scrlpos = GetDeviceScrollPosition(); }
	inline void UpdateDraw(LPRECT lpRc = NULL) { InvalidateRect(lpRc); UpdateWindow(); }
	inline void UpdateSize() { GetClientRect(&m_clRect); }
	inline void ResetZoom() 
	{ 
		m_GraphRect = m_clRect;
		if(GetDocument()->m_parsed && m_clRect.right < N)
			m_GraphRect.right = (LONG)N;
	}

	void GetAbsMaxValue();
	void ShowPoint(CDC*);
	void UpdateGraph(USHORT);
	void DoZoomInAlongOx();
	void DoZoomOutAlongOx();
	void DoZoomInAlongOy();
	void DoZoomOutAlongOy();

	CString GetError(funcParams &, CString []);
	CString CheckFunctionName(funcParams &params);
	CString CheckString(CString &);

	CPoint      m_scrlpos;
	RECT        m_clRect;
	RECT        m_GraphRect;
	Function   *m_curFunc;
	CGraph     *m_graph;
	double      m_clArg;
	int         m_interval;
	bool        m_showPt;
	bool        m_showGrid;
	bool		m_disableGoto;
};

#ifndef _DEBUG 
inline CGraphViewerDoc* CGraphViewerView::GetDocument()
{ return (CGraphViewerDoc*)m_pDocument; }
#endif

#endif 
