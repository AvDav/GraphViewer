#include "stdafx.h"
#include "ProgressBar.h"
#include <cmath>
//----------------------------------------------------------------------------------------------//
BEGIN_MESSAGE_MAP(ProgressBar, CWnd)
	ON_WM_PAINT()
END_MESSAGE_MAP()
//----------------------------------------------------------------------------------------------//
void ProgressBar::InitMembers()
{
	memset(&m_rect, 0, sizeof(RECT));
	memset(&m_varRect, 0, sizeof(RECT));
	m_handle = LoadLibrary("Msimg32.dll");
	m_func = (GradPfn)GetProcAddress(m_handle, "GradientFill");
	m_curProg = 1;
	m_brush = CreateSolidBrush(RGB(0, 0, 0));
}
//----------------------------------------------------------------------------------------------//
void ProgressBar::SetGradientColors(USHORT ar[])
{ 
	if(ar)
	{ 
		ASSERT((sizeof(ar)/sizeof(ar[0]))!=6);

		m_vert[0].Red = ar[0];
		m_vert[0].Green = ar[1];
		m_vert[0].Blue = ar[2];

		m_vert[1].Red = ar[3];
		m_vert[1].Green = ar[4];
		m_vert[1].Blue = ar[5];
	}
	else
	{
		m_vert[0].Red = 0;
		m_vert[0].Green = 0xffff;
		m_vert[0].Blue = 0xffff;

		m_vert[1].Red = 0;
		m_vert[1].Green = 0;
		m_vert[1].Blue = 0;
	}
}
//----------------------------------------------------------------------------------------------//
ProgressBar::ProgressBar(LOGFONT *lfont, int len, USHORT ar[], COLORREF col)
{
	InitMembers();
	SetFont(lfont);
	SetLength(len);
	SetGradientColors(ar);
	SetPercentTextColor(col);
}
//----------------------------------------------------------------------------------------------//
ProgressBar::ProgressBar()
{
	InitMembers();
	SetFont();
	SetPercentTextColor();
}
//----------------------------------------------------------------------------------------------//
void ProgressBar::SetFont(LOGFONT *lfont)
{
	LOGFONT fnt;
	memset(&fnt, 0, sizeof(LOGFONT));

	if(!lfont)
	{
		fnt.lfHeight = 20;
		fnt.lfQuality = ANTIALIASED_QUALITY;
		fnt.lfWeight = FW_BOLD;
		strcpy_s(fnt.lfFaceName, TEXT("Arial"));
	}
	else
		fnt = *lfont;

	m_font.CreateFontIndirect(&fnt);
}
//----------------------------------------------------------------------------------------------//
BOOL ProgressBar::Create(const RECT &rect, UINT id, CWnd *pParent)
{
	BOOL res = CWnd::CreateEx(WS_EX_STATICEDGE | WS_EX_TOPMOST, AfxRegisterWndClass(CS_HREDRAW |
		CS_VREDRAW | CS_DBLCLKS, ::LoadCursor(NULL, IDC_ARROW), m_brush, NULL), 
		"ProgressBar", WS_VISIBLE | WS_CHILD | WS_TABSTOP,
		rect, pParent, id, NULL);

	m_rect.right = rect.right - rect.left;
	m_rect.bottom = rect.bottom - rect.top;
	m_rect.left = m_rect.top = 0;
	m_varRect.bottom = m_rect.bottom;
	m_varRect.left = m_rect.left;
	m_varRect.top = m_rect.top;

	SetTimer(1, 10, NULL);

	return res;
}
//----------------------------------------------------------------------------------------------//
void ProgressBar::OnPaint()
{
	CPaintDC dc(this);
	dc.SelectObject(m_font);
	DrawProgress(&dc, m_varRect);
	CWnd::OnPaint();
}
//----------------------------------------------------------------------------------------------//
void ProgressBar::DrawProgress(CDC *pDC, RECT &rect)
{
	DrawProgress(pDC, rect.right, rect.bottom);
}
//----------------------------------------------------------------------------------------------//
void ProgressBar::DrawProgress(CDC *pDC, int width, int height)
{
	m_vert[0].x = 0;
	m_vert[0].y = 0;
	m_vert[1].x = width;
	m_vert[1].y = height;

	GRADIENT_RECT Rect;

	Rect.UpperLeft = 0;
	Rect.LowerRight = 1;

	if(!m_func(pDC->GetSafeHdc(), m_vert, 2, &Rect, 1, GRADIENT_FILL_RECT_V))
		MessageBox("Could not find GradientFill() function.", "message", MB_ICONERROR | MB_OK);

	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(m_color);
	pDC->DrawText(m_percent, &m_rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
}
//----------------------------------------------------------------------------------------------//
BOOL ProgressBar::DoStep()
{
	BOOL ret;
	if(m_curProg > m_length)
		return FALSE;

	if((ret = m_varRect.right < m_rect.right))
	{
		int prevRc = m_varRect.right;
		m_varRect.right = (int)floor(m_rect.right*((double)m_curProg/(double)m_length));

		RECT rc = m_varRect;
		if(m_varRect.right != prevRc)
		{
			rc.left = prevRc;   //Update delta region
			InvalidateRect(&rc);
		}
		static int percent = 0;
		int perc = (int)floor(((double) m_curProg/(double)m_length)*100);
		if(perc==percent)
		{
			m_curProg++;
			return TRUE;
		}
		percent = perc;
		CString str;

		str.Format("%d", percent);
		str += "%";

		m_curProg++;
		if(str!=m_percent)
		{
			m_percent = str;
			CDC *pDC = GetDC();
			pDC->SelectObject(m_font);
			CSize szText = pDC->GetTextExtent(m_percent);
			rc.left = m_rect.right/2 - szText.cx/2;
			rc.top = m_rect.top/2 + szText.cy/2;
			rc.right = m_rect.right/2 + szText.cx/2;
			rc.bottom = m_rect.bottom/2 + szText.cy/2;
			InvalidateRect(&rc);
		}
	}
	UpdateWindow();
	return ret;
}
//----------------------------------------------------------------------------------------------//