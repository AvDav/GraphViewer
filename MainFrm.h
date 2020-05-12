#ifndef MAINFRM_H
#define MAINFRM_H

#define FUNCCOMBO		201
#define INITEDIT		202
#define FINEDIT			203
#define DELTAEDIT		204
#define RUNBUTTON		205
#define GOTOEDIT		206
#define GOTOBUT			207
#define MINSTEPCOUNT	5

class CMainFrame : public CFrameWnd
{

protected: 
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnUpdatePane(CCmdUI *pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CComboBoxEx        m_FuncCombo;
	CEdit              m_editFrom;
	CEdit              m_editTo;
	CEdit              m_editDelta;
	CButton			   m_DrawButton;

	CButton			   m_GotoButton;
	CEdit			   m_editGoto;

	CStatusBar         m_wndStatusBar;
	CToolBar		   m_wndToolBar;
	CDialogBar		   m_wndDlgBar;

	void ShowTime();
public:
	CWnd* GetControl(int);
	void SetGotoReadOnly(BOOL set = TRUE) { m_editGoto.SetReadOnly(set); }
	void GetGotoContent(CString &str) { m_editGoto.GetWindowText(str); }
	//{{AFX_VIRTUAL(CMainFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL
	void SetStatusText(CString = "", CString = "");
	void ClearFields();

	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};

#endif 
