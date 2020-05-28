#ifndef GRAPHVIEWER_H
#define GRAPHVIEWER_H

#include "resource.h"
#include "MainFrm.h"
#include "GraphViewerDoc.h"
#include "GraphViewerView.h"

#define ARRSIZE(x) ((sizeof(x))/(sizeof(x[(0)])))
#define GRIDINTERVAL 20
#define DEFAULTCOLOR RGB(0, 0, 0xff)

void ErrorMsg(int = GetLastError(), DWORD = FORMAT_MESSAGE_FROM_SYSTEM);
class CGraphViewerApp : public CWinApp
{
public:
	CGraphViewerApp();

	//{{AFX_VIRTUAL(CGraphViewerApp)
public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CGraphViewerApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CAboutDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);   
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CAboutDlg)
//	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif 
