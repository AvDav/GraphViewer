#ifndef SAVEDLG_H
#define SAVEDLG_H

#include "FParser.h"
#include "GraphViewer.h"
class CSaveDlg : public CDialog
{
public:
	CSaveDlg(CWnd* pParent, FuncArray &, FuncArray &);
	//{{AFX_DATA(CSaveDlg)
	enum { IDD = IDD_SAVE };
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CSaveDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL
protected:
	void GetFuncsToSave(FuncArray &);
	//{{AFX_MSG(CSaveDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnAll();
	afx_msg void OnNone();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	FuncArray &m_refAr; // complete array ref
	FuncArray &m_refChoosed; // checked funcs array ref
};

#endif
