#include "stdafx.h"

#include "GraphViewer.h"
#include "GridPropDlg.h"

//------------------------------------------------------------------------------------------//
GridPropDlg::GridPropDlg(CWnd* pParent, COLORREF &color, int &interval, BOOL checkAdvanceGrid)
	: CDialog(GridPropDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(GridPropDlg)
	m_checked = checkAdvanceGrid;
	//}}AFX_DATA_INIT
	m_gridcol = color;
	m_spinpos = interval;
}
//------------------------------------------------------------------------------------------//
void GridPropDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(GridPropDlg)
	DDX_Control(pDX, IDC_SPIN, m_spin);
	DDX_Check(pDX, IDC_CHECK1, m_checked);
	//}}AFX_DATA_MAP
}
//------------------------------------------------------------------------------------------//
BEGIN_MESSAGE_MAP(GridPropDlg, CDialog)
	//{{AFX_MSG_MAP(GridPropDlg)
	ON_BN_CLICKED(IDC_BUTTON, OnGridColor)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
//------------------------------------------------------------------------------------------//
BOOL GridPropDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	SetIcon(AfxGetApp()->LoadIcon(IDR_GRAPHVTYPE), TRUE);

	m_spin.SetRange(GRIDINTERVAL, 100);
	m_spin.SetPos(m_spinpos);

	return TRUE;
}
//------------------------------------------------------------------------------------------//
void GridPropDlg::OnGridColor() 
{
	CColorDialog dlg(m_gridcol, CC_FULLOPEN);

	if(dlg.DoModal()==IDOK)
		m_gridcol = dlg.GetColor();
}
//------------------------------------------------------------------------------------------//
void GridPropDlg::OnOK() 
{	
	m_spinpos = m_spin.GetPos();
	CDialog::OnOK();
}
//------------------------------------------------------------------------------------------//

