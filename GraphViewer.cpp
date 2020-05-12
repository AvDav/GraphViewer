#include "stdafx.h"
#include "GraphViewer.h"

//------------------------------------------------------------------------------------------//
BEGIN_MESSAGE_MAP(CGraphViewerApp, CWinApp)
	//{{AFX_MSG_MAP(CGraphViewerApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()
//------------------------------------------------------------------------------------------//
void ErrorMsg(int code, DWORD flags)
{
	char *buff = new char[MAX_PATH];
	if(code == ERROR_SUCCESS)
		return;
	FormatMessage(flags, NULL, code, 0, buff, MAX_PATH, NULL);
	MessageBox(NULL, buff, "Error Message", MB_OK | MB_ICONERROR);
	delete[] buff;
}
//------------------------------------------------------------------------------------------//
CGraphViewerApp::CGraphViewerApp()
{

}
//------------------------------------------------------------------------------------------//
CGraphViewerApp theApp;
//------------------------------------------------------------------------------------------//
BOOL CGraphViewerApp::InitInstance()
{
	AfxEnableControlContainer();
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CGraphViewerDoc),
		RUNTIME_CLASS(CMainFrame),       
		RUNTIME_CLASS(CGraphViewerView));

	AddDocTemplate(pDocTemplate);

	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	if(!ProcessShellCommand(cmdInfo))
		return FALSE;

	m_pMainWnd->ShowWindow(SW_SHOWMAXIMIZED);
	m_pMainWnd->UpdateWindow(); 
	m_pMainWnd->DragAcceptFiles();

	return TRUE;
}
//------------------------------------------------------------------------------------------//

//------------------------------------------------------------------------------------------//
CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}
//------------------------------------------------------------------------------------------//
void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}
//------------------------------------------------------------------------------------------//
BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
//------------------------------------------------------------------------------------------//
void CGraphViewerApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}
