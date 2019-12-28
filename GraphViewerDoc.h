#ifndef GRAPHVIEWERDOC_H
#define GRAPHVIEWERDOC_H

#include "FParser.h"
class CGraphViewerDoc : public CDocument
{
protected:
	CGraphViewerDoc();
	DECLARE_DYNCREATE(CGraphViewerDoc)

	//{{AFX_MSG(CGraphViewerDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	//{{AFX_VIRTUAL(CGraphViewerDoc)
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

	int           m_parsed;
	int           m_interval;
	COLORREF      m_gridCol;
	FuncArray     m_fAr;
	int           m_fCount;

	virtual ~CGraphViewerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};

#endif 
