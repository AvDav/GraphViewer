#include "stdafx.h"

#include "GraphViewer.h"
#include "GraphViewerDoc.h"
#include "GraphViewerView.h"
#include "FParser.h"
#include "MainFrm.h"
//------------------------------------------------------------------------------------------//
IMPLEMENT_DYNCREATE(CGraphViewerDoc, CDocument)
	//------------------------------------------------------------------------------------------//
	BEGIN_MESSAGE_MAP(CGraphViewerDoc, CDocument)
		//{{AFX_MSG_MAP(CGraphViewerDoc)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()
	//------------------------------------------------------------------------------------------//
	CGraphViewerDoc::CGraphViewerDoc()
	{
		m_gridCol = DEFAULTCOLOR;
		m_interval = GRIDINTERVAL;
		m_parsed = 0;
		m_fCount = 0;
		m_fAr.clear();
	}
	//------------------------------------------------------------------------------------------//
	CGraphViewerDoc::~CGraphViewerDoc()
	{

	}
	//------------------------------------------------------------------------------------------//
	BOOL CGraphViewerDoc::OnNewDocument()
	{
		if(!CDocument::OnNewDocument())
			return FALSE;

		m_gridCol = DEFAULTCOLOR;
		m_interval = GRIDINTERVAL;
		m_parsed = 0;
		m_fCount = 0;
		m_fAr.clear();
		/*((CMainFrame*)AfxGetMainWnd())->CMainFrame::ClearFields();
		POSITION pos = GetFirstViewPosition();
		((CGraphViewerView*)GetNextView(pos))->Reset();*/
		return TRUE;
	}
	//------------------------------------------------------------------------------------------//
	void CGraphViewerDoc::Serialize(CArchive& ar)
	{
		if (ar.IsStoring())
		{
			ar << m_fCount;
			ar << m_interval;
			ar << m_gridCol;
			for(Function *i = &m_fAr.front(); i <= &m_fAr.back(); i++)
				ar << *i;
		}
		else
		{
			ar >> m_fCount;
			ar >> m_interval;
			ar >> m_gridCol;
			m_fAr.clear();
			for(int i = 0; i < m_fCount; i++)
			{
				Function f;
				ar >> f;
				m_fAr.push_back(f);
				FParser::Initialize((m_fAr.end() - 1)->ops);
			}
			m_parsed = 1; 
		}
	}
	//------------------------------------------------------------------------------------------//
#ifdef _DEBUG
	void CGraphViewerDoc::AssertValid() const
	{
		CDocument::AssertValid();
	}
	//------------------------------------------------------------------------------------------//
	void CGraphViewerDoc::Dump(CDumpContext& dc) const
	{
		CDocument::Dump(dc);
	}
#endif 
	//------------------------------------------------------------------------------------------//
	BOOL CGraphViewerDoc::OnOpenDocument(LPCTSTR lpszPathName) 
	{
		if (!CDocument::OnOpenDocument(lpszPathName))
			return FALSE;

		SetTitle(lpszPathName);
		return TRUE;
	}
	//------------------------------------------------------------------------------------------//
