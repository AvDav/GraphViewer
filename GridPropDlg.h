#ifndef GRIDPROPDLG_H
#define GRIDPROPDLG_H

class GridPropDlg : public CDialog
{
public:
	GridPropDlg(CWnd* pParent, COLORREF &, int &, BOOL);   

	COLORREF GetColor() const { return m_gridcol; }
	int GetInterval() const { return m_spinpos; }
	BOOL GetGridState() const { return m_checked; }

	//{{AFX_VIRTUAL(GridPropDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    
	//}}AFX_VIRTUAL

protected:
	//{{AFX_DATA(GridPropDlg)
	enum { IDD = IDD_GRID };
	CSpinButtonCtrl	m_spin;
	BOOL m_checked;
	//}}AFX_DATA

	//{{AFX_MSG(GridPropDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnGridColor();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	COLORREF  m_gridcol;
	int       m_spinpos;
};

#endif 
