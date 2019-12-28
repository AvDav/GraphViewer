#include "stdafx.h"
#include "SaveDlg.h"
//------------------------------------------------------------------------------------------//
CSaveDlg::CSaveDlg(CWnd* pParent, FuncArray &funcs, FuncArray &newfncs)
	: CDialog(CSaveDlg::IDD, pParent), m_refAr(funcs), m_refChoosed(newfncs)
{
	//{{AFX_DATA_INIT(CSaveDlg)
	//}}AFX_DATA_INIT
}
//------------------------------------------------------------------------------------------//
void CSaveDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSaveDlg)

	//}}AFX_DATA_MAP
}
//------------------------------------------------------------------------------------------//
BEGIN_MESSAGE_MAP(CSaveDlg, CDialog)
	//{{AFX_MSG_MAP(CSaveDlg)
	ON_BN_CLICKED(ID_ALL, OnAll)
	ON_BN_CLICKED(ID_NONE, OnNone)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
//------------------------------------------------------------------------------------------//
BOOL CSaveDlg::OnInitDialog() 
{
	int i;
	SetIcon(AfxGetApp()->LoadIcon(IDR_GRAPHVTYPE), TRUE);
	CDialog::OnInitDialog();
	if(!m_refAr.empty())
	{
		CListCtrl *pList = (CListCtrl *)GetDlgItem(IDC_LIST1);
		pList->SetExtendedStyle(pList->GetExtendedStyle() |
			LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
		const char* strs[4] = {"Function", "Initial value", "Final value", "Delta value"};
		int HeaderWidthArr[4];
		HDC hdcHeader = (pList->GetHeaderCtrl())->GetDC()->m_hDC;
		CSize sz;

		for(i = 0; i < 4; i++)
		{ 
			GetTextExtentPoint32(hdcHeader, strs[i], strlen(strs[i]), &sz);
			HeaderWidthArr[i] = sz.cx;
			pList->InsertColumn(i, strs[i]);
		}
		for(FuncArray::iterator iter = m_refAr.begin(); iter < m_refAr.end(); iter++)
		{
			int index = iter - m_refAr.begin();
			pList->InsertItem(index, iter->params.fName);

			CString from;
			CString to;
			CString delta;

			from.Format("%lf", iter->params.from);
			to.Format("%lf", iter->params.to);
			delta.Format("%lf", iter->params.delta);

			CString strings[4] = {iter->params.fName, from, to, delta};
			for(i = 0; i < 4; i++)
			{
				GetTextExtentPoint32(hdcHeader, strings[i], strings[i].GetLength(), &sz);
				if(sz.cx > HeaderWidthArr[i])
					HeaderWidthArr[i] = sz.cx;

				pList->SetItemText(index, i, strings[i]);
			}
		} 
		for(i = 0; i < 4; i++)
			pList->SetColumnWidth(i, HeaderWidthArr[i]);

		OnAll();
	}

	return TRUE;
}
//------------------------------------------------------------------------------------------//
void CSaveDlg::OnAll() 
{
	CListCtrl *pList = (CListCtrl *)GetDlgItem(IDC_LIST1);
	for(int i = 0; i < pList->GetItemCount(); i++)
		pList->SetCheck(i);
}
//------------------------------------------------------------------------------------------//
void CSaveDlg::OnNone() 
{
	CListCtrl *pList = (CListCtrl *)GetDlgItem(IDC_LIST1);
	for(int i = 0; i < pList->GetItemCount(); i++)
		pList->SetCheck(i, FALSE);	
}
//------------------------------------------------------------------------------------------//
void CSaveDlg::GetFuncsToSave(FuncArray &fAr)
{
	fAr.clear();
	CListCtrl *pList = (CListCtrl *)GetDlgItem(IDC_LIST1);
	for(int i = 0; i < pList->GetItemCount(); i++)
		if(pList->GetCheck(i))
		{
			CString str = pList->GetItemText(i, 0);
			for(FuncArray::iterator iter = m_refAr.begin(); iter < m_refAr.end(); iter++)
				if(iter->params.fName == str)
				{
					fAr.push_back(*iter);
					break;
				}
		}
}
//------------------------------------------------------------------------------------------//
void CSaveDlg::OnOK() 
{
	GetFuncsToSave(m_refChoosed);
	CDialog::OnOK();
}
//------------------------------------------------------------------------------------------//