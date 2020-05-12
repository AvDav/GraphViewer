#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

typedef BOOL (WINAPI *GradPfn)(HDC, PTRIVERTEX, ULONG, PVOID, ULONG, ULONG);

class ProgressBar : public CWnd
{
public:
	ProgressBar();
	ProgressBar(LOGFONT*, int, USHORT [], COLORREF);
	BOOL Create(const RECT&, UINT, CWnd * = NULL);
	virtual ~ProgressBar() { DeleteObject(m_brush); FreeLibrary(m_handle); DeleteObject(m_font);}
	BOOL DoStep();
	void SetGradientColors(USHORT []);
	void SetPercentTextColor(COLORREF col = RGB(0, 0, 0)) {m_color = col;}

private:
	RECT	  m_rect;
	RECT	  m_varRect;
	CFont	  m_font;
	CString	  m_percent;
	int		  m_length;
	TRIVERTEX m_vert[2];
	COLORREF  m_color;
	int       m_curProg;
	GradPfn   m_func;
	HINSTANCE m_handle;
	HBRUSH    m_brush;

	void	DrawProgress(CDC *, RECT &);
	void	DrawProgress(CDC *, int, int);
	void    SetLength(int len) {m_length = len;}
	void    SetFont(LOGFONT* = NULL);
	void    InitMembers();
	afx_msg void OnPaint();

	DECLARE_MESSAGE_MAP()
};

#endif