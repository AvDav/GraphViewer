#include "stdafx.h"
#include "GraphViewer.h"
#include "MainFrm.h"

//----------------------------------------------------------------------------------------------//
CMainFrame::CMainFrame()
{

}
CMainFrame::~CMainFrame()
{

}
//----------------------------------------------------------------------------------------------//
IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)
	//------------------------------------------------------------------------------------------//
BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_SCRL, OnUpdatePane)
	//}}AFX_MSG_MAP
	END_MESSAGE_MAP()
	//------------------------------------------------------------------------------------------//
	static UINT indicators[] =
	{
		ID_SEPARATOR,       
		ID_INDICATOR_SCRL,
		ID_INDICATOR_SCRL,
		ID_INDICATOR_SCRL
	};
	//------------------------------------------------------------------------------------------//
	int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
		int i;
		LOGFONT lf;
		static CFont font;
		memset(&lf, 0, sizeof(lf));
		lf.lfQuality = ANTIALIASED_QUALITY;
		lf.lfHeight = 16;
		lf.lfWeight = FW_BOLD;
		strcpy_s(lf.lfFaceName, TEXT("Tahoma"));
		font.CreateFontIndirect(&lf);

		if(CFrameWnd::OnCreate(lpCreateStruct) == -1)
			return -1;

		if(!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP |
			CBRS_SIZE_DYNAMIC) ||
			!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
			return -1; 

		if(!m_wndStatusBar.Create(this) || !m_wndStatusBar.SetIndicators(indicators,
			sizeof(indicators)/sizeof(UINT)))
			return -1;

		if (!m_wndDlgBar.Create(this, IDD_DIALOGBAR,
			CBRS_TOP|CBRS_TOOLTIPS|CBRS_FLYBY, IDD_DIALOGBAR))
		{
			TRACE0("Failed to create DlgBar\n");
			return -1;      // fail to create
		}
		CWnd *pDlgBar = FromHandle(m_wndDlgBar.m_hWnd);
		if(!m_DrawButton.Create(NULL, WS_CHILD | WS_VISIBLE | BS_ICON | WS_TABSTOP | WS_GROUP, 
			CRect(1, 10, 40, 45), pDlgBar, RUNBUTTON))
			return -1;
		m_DrawButton.SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME));

		static CStatic statCombo;
		statCombo.Create("Current function", SS_LEFT | WS_CHILD | WS_VISIBLE, 
			CRect(57, 3, 200, 19), pDlgBar);
		statCombo.SetFont(&font);
		if(!m_FuncCombo.Create(CBS_DROPDOWN | WS_CHILD | WS_TABSTOP | WS_VISIBLE, 
			CRect(57, 20, 557, 190), pDlgBar, FUNCCOMBO))
			return -1;
		m_FuncCombo.SetFont(&font);

		static CStatic stats[3];
		UINT nIDs[] = {INITEDIT, FINEDIT, DELTAEDIT};
		char *names[] = {"Initial value", "Final value", "Delta value"};
		CRect bounds[] = {CRect(577,20,707,42), CRect(727,20,857,42), CRect(877,20,1007,42)};
		CEdit *pArEdits[] = {&m_editFrom, &m_editTo, &m_editDelta};

		for(i = 0; i < 3; i++)
		{
			if(!pArEdits[i]->Create(ES_CENTER | WS_VISIBLE | WS_CHILD | WS_BORDER |
				WS_TABSTOP | WS_GROUP | ES_AUTOHSCROLL, bounds[i], pDlgBar, nIDs[i]))
				return -1;
			if(!stats[i].Create(names[i], SS_LEFT | WS_CHILD | WS_VISIBLE, 
				CRect(bounds[i].left, 3, bounds[i].left + 150, 19), pDlgBar))
				return -1;
			pArEdits[i]->SetFont(&font);
			stats[i].SetFont(&font);
		}
		for(i = 1; i < ARRSIZE(indicators); i++)
			m_wndStatusBar.SetPaneInfo(i, ID_INDICATOR_SCRL, 
			i ==3 ? SBPS_NORMAL : SBPS_POPOUT, i==3 ? 130 : 150);

		m_wndStatusBar.GetStatusBarCtrl().SetIcon(1, AfxGetApp()->LoadIcon(IDI_X));
		m_wndStatusBar.GetStatusBarCtrl().SetIcon(2, AfxGetApp()->LoadIcon(IDI_FX));

		SetStatusText();
		SetTimer(1, 1000, NULL);
		ShowTime();

		DWORD width = m_wndToolBar.GetToolBarCtrl().GetButtonSize();
		UINT offset = width*(m_wndToolBar.GetToolBarCtrl().GetButtonCount() - 1)-20;
		m_GotoButton.Create("Go to", BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE, 
			CRect(offset, 0, offset + 60, 23), FromHandle(m_wndToolBar.m_hWnd), GOTOBUT);
		m_GotoButton.SetFont(&font);
		m_editGoto.Create(ES_CENTER | WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 
			CRect(offset + 60, 0, offset + 200, 23), FromHandle(m_wndToolBar.m_hWnd), GOTOEDIT);
		m_editGoto.SetFont(&font);
		return 0;
	}
	//------------------------------------------------------------------------------------------//
	BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
	{
		if(!CFrameWnd::PreCreateWindow(cs))
			return FALSE;
		return TRUE;
	}
	//------------------------------------------------------------------------------------------//
#ifdef _DEBUG
	void CMainFrame::AssertValid() const
	{
		CFrameWnd::AssertValid();
	}
	//------------------------------------------------------------------------------------------//
	void CMainFrame::Dump(CDumpContext& dc) const
	{
		CFrameWnd::Dump(dc);
	}
#endif 
	//------------------------------------------------------------------------------------------//
	void CMainFrame::OnUpdatePane(CCmdUI *pCmdUI)
	{
		pCmdUI->Enable();
	}
	//------------------------------------------------------------------------------------------//
	void CMainFrame::SetStatusText(CString arg, CString func)
	{
		m_wndStatusBar.SetPaneText(1, arg);
		m_wndStatusBar.SetPaneText(2, func);
	}
	//------------------------------------------------------------------------------------------//
	void CMainFrame::ShowTime()
	{
		TCHAR buf[26];
		time_t tm;
		time(&tm);
		ctime_s(buf, sizeof buf, &tm);
		m_wndStatusBar.SetPaneText(3, buf);
	}
	//------------------------------------------------------------------------------------------//
	void CMainFrame::OnTimer(UINT nIDEvent) 
	{
		ShowTime();
		CFrameWnd::OnTimer(nIDEvent);
	}
	//------------------------------------------------------------------------------------------//
	CWnd* CMainFrame::GetControl(int id)
	{
		switch(id)
		{
		case 0 : return &m_FuncCombo;
		case 1 : return &m_editFrom;
		case 2 : return &m_editTo;
		case 3 : return &m_editDelta;
		default : return NULL;
		}
	}
	//------------------------------------------------------------------------------------------//
	void CMainFrame::ClearFields()
	{
		//m_FuncCombo.ResetContent();
		for(int i = 0; i < 4; i++)
			GetControl(i)->SetWindowText("");
	}

